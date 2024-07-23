#include <QDateTime>
#include <QRegularExpression>
#include <QTextStream>

#include "core/interval.hpp"
#include "core/storage.hpp"
#include "core/survey_response.hpp"
#include "daemon.hpp"

#include "encryption.hpp"

namespace {
/**
 * Returns a future that resolves once all provided futures resolved.
 *
 * Replace with QtPromise::whenAll once we upgrade to Qt >= 6.3.
 */
QFuture<void> whenAll(const QList<QFuture<void>>& futures)
{
    auto promise = QSharedPointer<QPromise<void>>::create();
    auto pendingFutures = QSharedPointer<int>::create(0);
    for (auto future : futures)
        if (!future.isFinished())
            *pendingFutures = *pendingFutures + 1;
    if (*pendingFutures == 0) {
        promise->finish();
        return promise->future();
    }

    for (auto future : futures)
        if (!future.isFinished()) {
            future.then([=] {
                *pendingFutures = *pendingFutures - 1;
                if (*pendingFutures == 0)
                    promise->finish();
            });
        }
    return promise->future();
}

/**
 * Chains a callback that returns a subsequent future once the provided future
 * resolves, for consecutive execution.
 *
 * This is seemingly not the behaviour of QFuture::next() - though one might
 * argue it should be, or that there should be a more elegant way to do this
 * kind of thing.
 */
template <typename T>
QFuture<void> chain(QFuture<T> future, std::function<QFuture<void>(T)> next)
{
    auto promise = QSharedPointer<QPromise<void>>::create();
    future.then(
        [=](T value) { next(value).then([promise] { promise->finish(); }); });
    return promise->future();
}
};

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

    // TODO: Consider rewriting this to use signals/slots.
    qDebug() << "Processing surveys ...";
    processSurveys().then([&] {
        qDebug() << "Processing signups ...";
        processSignups().then([&] {
            qDebug() << "Processing finished.";
            emit finished();
        });
    });
}

QFuture<void> Daemon::handleSurveysResponse(const QByteArray& data)
{
    using Qt::endl;

    QTextStream cout(stdout);
    const auto surveys = Survey::listFromByteArray(data);

    qDebug() << "Fetched surveys:" << surveys.count();

    QSet<QString> signedUpSurveys;
    for (const auto& record : storage->listSurveyRecords())
        signedUpSurveys.insert(record.survey->id);

    QList<QFuture<void>> futures;
    for (const auto& survey : surveys) {
        if (signedUpSurveys.contains(survey->id))
            continue;

        // Only KDE allowed as commissioner
        QString kdeName("KDE");
        if (survey->commissioner->name != kdeName)
            continue;

        // TODO: Only sign up for surveys if we have the data points they
        //       request.
        futures.append(signUpForSurvey(survey));
    }
    return whenAll(futures);
}

QFuture<void> Daemon::processSurveys()
{
    return chain<QByteArray>(network->listSurveys(),
        [this](QByteArray data) { return handleSurveysResponse(data); });
}

QFuture<void> Daemon::signUpForSurvey(const QSharedPointer<const Survey> survey)
{
    auto publicKey = encryption->generateKeyPair();
    return network->surveySignup(survey->id, publicKey)
        .then([&, survey, publicKey](QByteArray data) {
            const auto responseObject = QJsonDocument::fromJson(data).object();
            const auto clientId = responseObject["client_id"].toString();
            storage->addSurveyRecord(
                *survey, clientId, publicKey, "", std::nullopt);
        });
}

QFuture<void> Daemon::processInitialSignup(SurveyRecord& record)
{
    return network->getSignupState(record.clientId)
        .then([&, record](QByteArray data) mutable {
            qDebug() << "we got here i can tell you";
            const auto responseDocument = QJsonDocument::fromJson(data);
            const auto responseObject = responseDocument.object();
            if (!responseObject["aggregation_started"].toBool()) {
                return;
            }

            record.delegatePublicKey
                = responseObject["delegate_public_key"].toString();

            if (record.publicKey == record.delegatePublicKey) {
                record.groupSize = responseObject["group_size"].toInt();
                qDebug() << "Client acts as delegate";

                // test edge case
                if (record.groupSize == 1) {
                    qDebug() << "Directly posting data to server as groupSize "
                                "is 1";
                    auto surveyResponse = createSurveyResponse(record.survey);
                    network
                        ->postAggregationResult(
                            record.clientId, surveyResponse->toJsonByteArray())
                        .then([&, record, surveyResponse](bool success) {
                            if (!success)
                                return;
                            storage->addSurveyResponse(
                                *surveyResponse, *record.survey);
                            storage->saveSurveyRecord(record);
                        });
                    return;
                }

                processMessagesForDelegate(record);
            } else {
                postMessageToDelegate(record);
            }

            storage->saveSurveyRecord(record);
        });
}

QFuture<void> Daemon::processSignups()
{
    auto surveyRecords = storage->listSurveyRecords();
    QList<QFuture<void>> futures;
    for (auto surveyRecord : surveyRecords) {
        if (surveyRecord.getState() == Initial) {
            futures.append(processInitialSignup(surveyRecord));
            continue;
        }
        if (surveyRecord.getState() != Processing
            || surveyRecord.publicKey != surveyRecord.delegatePublicKey)
            continue;
        qDebug() << surveyRecord.getState() << surveyRecord.clientId
                 << surveyRecord.delegatePublicKey;
        futures.append(processMessagesForDelegate(surveyRecord));
    }
    return whenAll(futures);
}

QFuture<void> Daemon::postMessageToDelegate(SurveyRecord& record) const
{
    const auto response = createSurveyResponse(record.survey);
    // TODO: unnecessary back and forth conversion maybe just implement
    // toJsonString method
    auto responseString
        = QString::fromLatin1(response->toJsonByteArray().toBase64());
    auto encryptedResponseString
        = encryption->encrypt(responseString, record.delegatePublicKey);
    return network
        ->postMessageToDelegate(
            record.delegatePublicKey, encryptedResponseString)
        // TODO do we need to copy everything?
        .then([=](bool success) {
            if (!success)
                return;

            // implicitely this will set the state to __Done__
            storage->addSurveyResponse(*response, *record.survey);
        });
}

QFuture<void> Daemon::processMessagesForDelegate(SurveyRecord& record)
{
    return network->getMessagesForDelegate(record.clientId)
        .then([&, record](QByteArray data) mutable {
            try {
                if (!record.groupSize.has_value()) {
                    return;
                }
                auto responsesVariant
                    = parseResponseMessages(data, record.groupSize.value());

                if (!responsesVariant
                         .canConvert<QList<QSharedPointer<SurveyResponse>>>()) {
                    qDebug() << "Insufficient messages available";
                    return;
                }

                auto responses
                    = responsesVariant
                          .value<QList<QSharedPointer<SurveyResponse>>>();
                auto personalResponse = createSurveyResponse(record.survey);
                responses.append(personalResponse);
                qDebug() << responses.first()->toJsonByteArray();
                auto aggregatedResponse
                    = SurveyResponse::aggregateSurveyResponses(responses);
                network
                    ->postAggregationResult(
                        record.clientId, aggregatedResponse->toJsonByteArray())
                    .then([=](bool success) {
                        if (!success)
                            return;
                        storage->addSurveyResponse(
                            *personalResponse, *record.survey);
                        storage->saveSurveyRecord(record);
                    });
                // TODO: Can we introduce some kind of return type those errors
                // are annoying to deal with
            } catch (QJsonParseError error) {
                qDebug() << "we're in the error handling json";
                qWarning() << "Invalid JSON: Messages could not be parsed";
            }
        });
}

QVariant Daemon::parseResponseMessages(QByteArray& data, int groupSize)
{
    QList<QSharedPointer<SurveyResponse>> responses {};
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
        responses.append(SurveyResponse::fromJsonByteArray(jsonByteArray));
    }
    return QVariant::fromValue(responses);
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
