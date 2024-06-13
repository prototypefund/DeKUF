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

void Client::fetchSurveys()
{
    qDebug() << "Fetching surveys ...";
    getRequest("http://localhost:8000/api/surveys", [&](QNetworkReply* reply) {
        if (reply->error() != QNetworkReply::NoError) {
            qCritical() << "Error:" << reply->errorString();
            emit finished();
            return;
        }
        const auto responseData = reply->readAll();
        handleSurveysResponse(responseData);
    });
}

void Client::run()
{
    qDebug() << "Stored datapoints:";
    for (const auto& dataPoint : storage->listDataPoints())
        qDebug() << "-" << dataPoint.value;

    qDebug() << "Survey signups:";
    for (const auto& signup : storage->listSurveySignups())
        qDebug() << "-" << signup.survey->id << "as" << signup.clientId;

    fetchSurveys();
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

    // TODO: In principle, we should wait for all survey signups to finish.
    emit finished();
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
        storage->addSurveySignup(*survey, "TODO", clientId, "");
    });
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
