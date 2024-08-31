#include <QDateTime>
#include <QRegularExpression>
#include <QTextStream>

#include "core/interval.hpp"
#include "core/storage.hpp"
#include "core/survey_response.hpp"
#include "daemon.hpp"

#include "encryption.hpp"

Daemon::Daemon(QObject* parent, QSharedPointer<Storage> storage,
    QSharedPointer<Network> network, QSharedPointer<Encryption> encryption)
    : QObject(parent)
    , storage(storage)
    , network(network)
    , encryption(encryption)
    , dbusService(storage)
{
    if (auto object = dynamic_cast<QObject*>(network.get()))
        object->setParent(this);
}

void Daemon::run()
{
    qDebug() << "Processing started.";

    qDebug() << "Stored datapoints:";
    for (const auto& dataPoint : storage->listDataPoints())
        qDebug() << "-" << dataPoint.value;

    qDebug() << "Survey signups:";
    for (const auto& record : storage->listSurveyRecords())
        qDebug() << "-" << record.survey->id << "as" << record.clientId
                 << "state:" << record.getState();

    qDebug() << "Processing surveys ...";
    processSurveys();
    qDebug() << "Processing signups ...";
    processSignups();
    qDebug() << "Processing finished.";
    emit finished();
}

bool Daemon::checkIfAllDataKeysArePresent(
    const QSharedPointer<Survey>& survey) const
{
    for (auto query : survey->queries) {
        if (!storage->checkIfDataPointPresent(query->dataKey))
            return false;
    }
    return true;
}

void Daemon::handleSurveysResponse(const QByteArray& data)
{
    using Qt::endl;

    QTextStream cout(stdout);
    const auto surveysParsingResult = Survey::listFromByteArray(data);

    if (!surveysParsingResult.isSuccess()) {
        qWarning() << "Error occured while parsing surveys:"
                   << surveysParsingResult.getErrorMessage();
        return;
    }

    auto surveys = surveysParsingResult.getValue();

    qDebug() << "Fetched surveys:" << surveys.count();

    QSet<QString> signedUpSurveys;
    for (const auto& record : storage->listSurveyRecords())
        signedUpSurveys.insert(record.survey->id);

    for (const auto& survey : surveys) {
        if (signedUpSurveys.contains(survey->id))
            continue;

        // Only KDE allowed as commissioner
        QString kdeName("KDE");
        if (survey->commissioner->name != kdeName)
            continue;

        if (!checkIfAllDataKeysArePresent(survey))
            continue;

        signUpForSurvey(survey);
    }
}

void Daemon::processSurveys()
{
    auto data = network->listSurveys();
    handleSurveysResponse(data);
}

void Daemon::signUpForSurvey(const QSharedPointer<const Survey> survey)
{
    auto publicKey = encryption->generateKeyPair();
    auto data = network->surveySignup(survey->id, publicKey);
    const auto responseObject = QJsonDocument::fromJson(data).object();
    const auto clientId = responseObject["client_id"].toString();
    storage->addSurveyRecord(
        *survey, clientId, publicKey, "", std::nullopt, std::nullopt);
}

void Daemon::processInitialSignup(SurveyRecord& record)
{
    auto data = network->getSignupState(record.clientId);
    const auto responseDocument = QJsonDocument::fromJson(data);
    const auto responseObject = responseDocument.object();
    if (!responseObject["aggregation_started"].toBool()) {
        return;
    }

    record.delegatePublicKey = responseObject["delegate_public_key"].toString();
    record.aggregationPublicKey
        = responseObject["aggregation_public_key_n"].toString();

    if (record.publicKey == record.delegatePublicKey) {
        record.groupSize = responseObject["group_size"].toInt();
        qDebug() << "Client acts as delegate";

        // test edge case
        if (record.groupSize == 1) {
            qDebug() << "Directly posting data to server as groupSize "
                        "is 1";
            auto surveyResponse = createSurveyResponse(record.survey);
            auto success = network->postAggregationResult(
                record.clientId, surveyResponse->toJsonByteArray());
            if (success) {
                storage->addSurveyResponse(*surveyResponse, *record.survey);
                storage->saveSurveyRecord(record);
            }
            return;
        }

        processMessagesForDelegate(record);
    } else {
        postMessageToDelegate(record);
    }

    storage->saveSurveyRecord(record);
}

void Daemon::processSignups()
{
    auto surveyRecords = storage->listSurveyRecords();
    for (auto surveyRecord : surveyRecords) {
        if (surveyRecord.getState() == Initial) {
            processInitialSignup(surveyRecord);
            continue;
        }
        if (surveyRecord.getState() != Processing
            || surveyRecord.publicKey != surveyRecord.delegatePublicKey)
            continue;
        qDebug() << surveyRecord.getState() << surveyRecord.clientId
                 << surveyRecord.delegatePublicKey;
        processMessagesForDelegate(surveyRecord);
    }
}

void Daemon::postMessageToDelegate(SurveyRecord& record) const
{
    if (!record.aggregationPublicKey.has_value()) {
        qWarning() << "AggregationKey is null, posting message failed.";
        return;
    }
    const auto encryptorResult = PaillierEncryptor::createPaillierEncryptor(
        record.aggregationPublicKey.value());
    qDebug() << record.aggregationPublicKey.value();
    if (!encryptorResult.isSuccess()) {
        qWarning() << "Posting message failed:"
                   << encryptorResult.getErrorMessage();
        return;
    }
    const auto response = createSurveyResponse(record.survey);
    // TODO: Improve naming of dual encryption
    const auto dataEncryptedResponse
        = response->encrypt(encryptorResult.getValue());

    // TODO: unnecessary back and forth conversion maybe just implement
    // toJsonString method
    auto responseString = QString::fromLatin1(
        dataEncryptedResponse->toJsonByteArray().toBase64());
    auto encryptedResponseString
        = encryption->encrypt(responseString, record.delegatePublicKey);
    auto success = network->postMessageToDelegate(
        record.delegatePublicKey, encryptedResponseString);
    // TODO do we need to copy everything?
    if (!success)
        return;

    // implicitely this will set the state to __Done__
    storage->addSurveyResponse(*response, *record.survey);
}

void Daemon::processMessagesForDelegate(SurveyRecord& record)
{
    qDebug() << "ClientId:" << record.clientId;
    auto data = network->getMessagesForDelegate(record.clientId);
    if (!record.groupSize.has_value()) {
        return;
    }

    const auto responsesParsingResult
        = parseResponseMessages(data, record.groupSize.value());

    if (!responsesParsingResult.isSuccess()) {
        qWarning() << "Error parsing other clients responses"
                   << responsesParsingResult.getErrorMessage();
        return;
    }

    auto responses = responsesParsingResult.getValue();

    if (responses.count() < (record.groupSize.value() - 1)) {
        qDebug() << "Insufficient messages available";
        return;
    }

    if (!record.aggregationPublicKey.has_value()) {
        qWarning() << "AggregationKey is null, processing messages failed.";
        return;
    }
    const auto encryptorResult = PaillierEncryptor::createPaillierEncryptor(
        record.aggregationPublicKey.value());
    if (!encryptorResult.isSuccess()) {
        qWarning() << "Posting message failed:"
                   << encryptorResult.getErrorMessage();
        return;
    }

    auto personalResponse = createSurveyResponse(record.survey);
    auto encryptedPersonalResponse
        = personalResponse->encrypt(encryptorResult.getValue());
    responses.append(encryptedPersonalResponse);

    qDebug() << "Personal response:" << personalResponse->toJsonByteArray();
    auto aggregationResult
        = EncryptedSurveyResponse::aggregateEncryptedSurveyResponses(
            responses, encryptorResult.getValue());

    if (!aggregationResult.isSuccess()) {
        qDebug() << "Aggregation unsuccessful:"
                 << aggregationResult.errorMessage;
    }

    const auto& aggregatedResponse = aggregationResult.getValue();

    qDebug() << "Aggregated Response:" << aggregatedResponse->toJsonByteArray();
    auto success = network->postAggregationResult(
        record.clientId, aggregatedResponse->toJsonByteArray());
    if (!success)
        return;
    storage->addSurveyResponse(*personalResponse, *record.survey);
    storage->saveSurveyRecord(record);
}

Result<QList<QSharedPointer<EncryptedSurveyResponse>>>
Daemon::parseResponseMessages(const QByteArray& data, int groupSize) const
{
    QList<QSharedPointer<EncryptedSurveyResponse>> responses {};
    auto jsonDoc = QJsonDocument::fromJson(data);

    auto jsonObj = jsonDoc.object();
    auto jsonArray = jsonObj["messages"].toArray();
    if (jsonArray.count() < groupSize - 1)
        return {};

    for (const QJsonValue& value : jsonArray) {
        auto encryptedString = value.toString();
        // TODO: We need the proper private key here
        QString decryptedResponseString
            = encryption->decrypt(encryptedString, "");
        QByteArray jsonByteArray
            = QByteArray::fromBase64(decryptedResponseString.toLatin1());
        auto parsingResult
            = EncryptedSurveyResponse::fromJsonByteArray(jsonByteArray);
        if (!parsingResult.isSuccess()) {
            return Result<QList<QSharedPointer<EncryptedSurveyResponse>>>::
                Failure(parsingResult.getErrorMessage());
        }
        responses.append(parsingResult.getValue());
    }
    return Result(responses);
}

QSharedPointer<SurveyResponse> Daemon::createSurveyResponse(
    const QSharedPointer<Survey>& survey) const
{
    auto surveyResponse = QSharedPointer<SurveyResponse>::create(survey->id);

    for (const auto& query : survey->queries) {
        auto queryResponse = createQueryResponse(query);
        if (queryResponse == nullptr)
            continue;
        surveyResponse->queryResponses.append(queryResponse);
    }
    return surveyResponse;
}

QSharedPointer<QueryResponse> Daemon::createQueryResponse(
    const QSharedPointer<Query>& query) const
{
    const auto dataPoints = storage->listDataPoints(query->dataKey);

    qDebug() << "Datakey" << query->dataKey;

    if (dataPoints.count() == 0)
        return nullptr;

    qDebug() << "Found dataPoints:" << dataPoints.count();

    QMap<QString, int> cohortData;

    for (const QString& cohort : query->cohorts) {
        cohortData[cohort] = 0;

        for (const DataPoint& dataPoint : dataPoints) {
            if (query->discrete) {
                if (cohort == dataPoint.value) {
                    cohortData[cohort]++;
                }
                continue;
            }
            try {
                auto interval = Interval(cohort);

                if (interval.isInInterval(dataPoint.value.toDouble())) {
                    cohortData[cohort]++;
                }
            } catch (std::invalid_argument) {
                // Error handling here
            }
        }
    }

    return QSharedPointer<QueryResponse>::create(query->id, cohortData);
}
