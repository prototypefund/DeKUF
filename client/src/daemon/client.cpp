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
    QUrl url("http://localhost:8000/api/surveys");
    QNetworkRequest request(url);
    manager->get(request);
    connect(
        manager, &QNetworkAccessManager::finished, [&](QNetworkReply* reply) {
            if (reply->error() == QNetworkReply::NoError) {
                const auto responseData = reply->readAll();
                handleSurveysResponse(responseData);
            } else {
                qCritical() << "Error:" << reply->errorString();
                emit finished();
            }
            reply->deleteLater();
        });
}

void Client::run()
{
    storage->addDataPoint("num-data-points",
        QVariant(storage->listDataPoints().size()).toString());
    for (const auto& dataPoint : storage->listDataPoints())
        qDebug() << "-" << dataPoint.value;
    fetchSurveys();
}

void Client::handleSurveysResponse(const QByteArray& data)
{
    using Qt::endl;

    QTextStream cout(stdout);
    const auto surveys = Survey::listFromByteArray(data);
    qDebug() << "Fetched surveys:" << surveys.count();
    for (const auto& survey : surveys) {
        const auto surveyResponse = createSurveyResponse(*survey);
        if (surveyResponse == nullptr
            || surveyResponse->queryResponses.isEmpty())
            continue;
        postSurveyResponse(surveyResponse, survey);
    }
}

QSharedPointer<SurveyResponse> Client::createSurveyResponse(
    const Survey& survey) const
{
    // Only KDE allowed as commissioner
    QString kdeName("KDE");
    if (survey.commissioner->name != kdeName)
        return {};

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

void Client::postSurveyResponse(QSharedPointer<SurveyResponse> surveyResponse,
    QSharedPointer<Survey> survey)
{
    QUrl url("http://localhost:8000/api/survey-response/");
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    manager->post(request, surveyResponse->toJsonByteArray());

    connect(manager, &QNetworkAccessManager::finished,
        [&, surveyResponse, survey](QNetworkReply* reply) {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                storage->addSurveyResponse(*surveyResponse, *survey);
            } else {
                QByteArray response = reply->readAll();
                QString errorString = reply->errorString();
                qCritical() << "Response error:" << response;
                qCritical() << "Error string:" << reply->errorString();
            }
            reply->deleteLater();
            emit finished();
        });
}
