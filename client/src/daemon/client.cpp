#include <QDateTime>
#include <QRegularExpression>
#include <QTextStream>
#include <QtNetwork>

#include "client.hpp"
#include "core/interval.hpp"
#include "core/storage.hpp"
#include "core/survey_response.hpp"

Client::Client(QObject* parent, QSharedPointer<Storage> storage)
    : QObject(parent)
    , storage(storage)
    , manager(new QNetworkAccessManager(this))
    , dbusService(storage)
{
}

void Client::processSurveys(std::function<void()> callback)
{
    qDebug() << "Processing surveys ...";
    getRequest("http://localhost:8000/api/surveys",
        [&, callback](QNetworkReply* reply) {
            if (reply->error() != QNetworkReply::NoError) {
                qCritical() << "Error:" << reply->errorString();
                callback();
                return;
            }
            const auto responseData = reply->readAll();
            handleSurveysResponse(responseData);
            callback();
        });
}

void Client::run()
{
    qDebug() << "Stored datapoints:";
    for (const auto& dataPoint : storage->listDataPoints())
        qDebug() << "-" << dataPoint.value;

    qDebug() << "Survey signups:";
    for (const auto& signup : storage->listSurveySignups())
        qDebug() << "-" << signup.survey->id << "as" << signup.clientId
                 << "state:" << signup.state;

    // TODO: Use futures or something to get out of callback hell.
    processSurveys([&]() { processSignups(); });

    // TODO: Wait for all requests triggered above to finish.
    emit finished();
}

void Client::handleSurveysResponse(const QByteArray& data)
{
    using Qt::endl;

    QTextStream cout(stdout);
    const auto surveys = Survey::listFromByteArray(data);

    qDebug() << "Fetched surveys:" << surveys.count();

    QSet<QString> signedUpSurveys;
    for (const auto& signup : storage->listSurveySignups())
        signedUpSurveys.insert(signup.survey->id);

    for (const auto& survey : surveys) {
        if (signedUpSurveys.contains(survey->id))
            continue;

        // Only KDE allowed as commissioner
        QString kdeName("KDE");
        if (survey->commissioner->name != kdeName)
            continue;

        signUpForSurvey(survey);
    }
}

void Client::signUpForSurvey(const QSharedPointer<const Survey> survey)
{
    auto url = QString("http://localhost:8000/api/survey-signup/%1/")
                   .arg(survey->id);
    postRequest(url, "", [&, survey](QNetworkReply* reply) {
        if (reply->error() != QNetworkReply::NoError) {
            qCritical() << "Error:" << reply->errorString();
            return;
        }

        const auto responseData = reply->readAll();
        const auto responseObject
            = QJsonDocument::fromJson(responseData).object();
        const auto clientId = responseObject["client_id"].toString();
        storage->addSurveySignup(*survey, "initial", clientId, "");
    });
}

void Client::processSignup(SurveySignup& signup)
{
    qDebug() << "Processing signups ...";
    const auto url = QString("http://localhost:8000/api/signup-state/%1/")
                         .arg(signup.clientId);
    getRequest(url, [&, signup](QNetworkReply* reply) mutable {
        if (reply->error() != QNetworkReply::NoError) {
            qCritical() << "Error:" << reply->errorString();
            return;
        }

        const auto responseData = reply->readAll();
        const auto responseDocument = QJsonDocument::fromJson(responseData);
        const auto responseObject = responseDocument.object();
        if (!responseObject["aggregation_started"].toBool())
            return;

        signup.delegateId = responseObject["delegate_id"].toString();

        if (signup.clientId == signup.delegateId) {
            signup.state = "processing";
            // TODO: Save group_size.
        } else {
            // TODO: Send data to delegate.
            signup.state = "done";
        }

        storage->saveSurveySignup(signup);
    });
}

void Client::processSignups()
{
    auto signups = storage->listSurveySignupsForState("initial");
    for (auto signup : signups)
        processSignup(signup);
}

QSharedPointer<SurveyResponse> Client::createSurveyResponse(
    const Survey& survey) const
{
    auto surveyResponse = QSharedPointer<SurveyResponse>::create(survey.id);

    for (const auto& query : survey.queries) {
        auto queryResponse = createQueryResponse(query);
        if (queryResponse == nullptr)
            continue;
        surveyResponse->queryResponses.append(queryResponse);
    }
    return surveyResponse;
}

QSharedPointer<QueryResponse> Client::createQueryResponse(
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

void Client::getRequest(
    const QString& url, std::function<void(QNetworkReply*)> callback)
{
    QNetworkRequest request(url);
    manager->get(request);
    connect(
        manager, &QNetworkAccessManager::finished, this,
        [&, callback](QNetworkReply* reply) {
            callback(reply);
            reply->deleteLater();
        },
        static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));
}

void Client::postRequest(const QString& url, const QByteArray& data,
    std::function<void(QNetworkReply*)> callback)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    manager->post(request, data);
    connect(
        manager, &QNetworkAccessManager::finished, this,
        [&, callback](QNetworkReply* reply) {
            callback(reply);
            reply->deleteLater();
        },
        static_cast<Qt::ConnectionType>(Qt::SingleShotConnection));
}
