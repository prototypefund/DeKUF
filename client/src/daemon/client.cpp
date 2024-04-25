#include <QDateTime>
#include <QTextStream>
#include <QtNetwork>

#include "client.hpp"

Client::Client(QObject* parent, QSharedPointer<Storage> storage)
    : QObject(parent)
    , storage(storage)
    , manager(new QNetworkAccessManager(this))
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
    qDebug() << "Stored data points: ";
    for (const auto& dataPoint : storage->listDataPoints())
        qDebug() << "-" << dataPoint.value;
    fetchSurveys();
}

void Client::handleSurveysResponse(const QByteArray& data)
{
    using Qt::endl;

    QTextStream cout(stdout);
    const auto surveys = Survey::listFromByteArray(data);
    for (const auto& survey : surveys) {
        const auto surveyResponse = createSurveyResponse(*survey);
        if (surveyResponse == nullptr
            || surveyResponse->queryResponses.isEmpty())
            continue;
        postSurveyResponse(surveyResponse);
    }
}

QSharedPointer<SurveyResponse> Client::createSurveyResponse(
    const Survey& survey) const
{
    // Only KDE allowed as commissioner
    QString kdeName("KDE");
    if (!std::any_of(survey.commissioners.begin(), survey.commissioners.end(),
            [&](const QSharedPointer<Commissioner>& commissioner) {
                return commissioner->name == kdeName;
            }))
        return {};

    auto surveyResponse = QSharedPointer<SurveyResponse>::create();
    surveyResponse->commissioners.append(
        QSharedPointer<Commissioner>::create(kdeName));

    for (const auto& query : survey.queries) {
        const auto dataPoints = storage->listDataPoints(query->dataKey);
        if (dataPoints.count() == 0)
            continue;
        surveyResponse->queryResponses.append(
            QSharedPointer<QueryResponse>::create(
                query->dataKey, dataPoints.first().value));
    }
    return surveyResponse;
}

void Client::postSurveyResponse(QSharedPointer<SurveyResponse> surveyResponse)
{
    QUrl url("http://localhost:8000/api/survey-response/");
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    manager->post(request, surveyResponse->toJsonByteArray());

    connect(
        manager, &QNetworkAccessManager::finished, [&](QNetworkReply* reply) {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                qDebug() << "Response:" << response;
                storage->addSurveyResponse(*surveyResponse);
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
