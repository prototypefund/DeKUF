#include <QDateTime>
#include <QTextStream>
#include <QtNetwork>

#include "client.hpp"
#include "survey.hpp"
#include "survey_response.hpp"

Client::Client(QObject* parent)
    : QObject(parent)
{
}

void Client::fetchSurveys()
{
    auto manager = new QNetworkAccessManager(this);
    QUrl url("http://localhost:8000/api/surveys");
    QNetworkRequest request(url);
    manager->get(request);
    connect(
        manager, &QNetworkAccessManager::finished, [&](QNetworkReply* reply) {
            if (reply->error() == QNetworkReply::NoError) {
                auto responseData = reply->readAll();
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
    storage.addDataPoint(
        "timestamp", QString::number(QDateTime::currentSecsSinceEpoch()));
    qDebug() << "Stored timestamps: " << storage.listDataPoints("timestamp");
    fetchSurveys();
}

void Client::handleSurveysResponse(const QByteArray& data)
{
    using Qt::endl;

    QTextStream cout(stdout);
    auto surveys = Survey::listFromByteArray(data);
    for (auto survey : surveys) {
        auto surveyResponse = SurveyResponse::create(survey, storage);
        if (surveyResponse == nullptr
            || surveyResponse->queryResponses.isEmpty())
            continue;
        postSurveyResponse(surveyResponse);
    }
}

void Client::postSurveyResponse(QSharedPointer<SurveyResponse> surveyResponse)
{
    auto manager = new QNetworkAccessManager(this);
    QUrl url("http://localhost:8000/api/survey-response/");
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply
        = manager->post(request, surveyResponse->toJsonByteArray());

    connect(
        manager, &QNetworkAccessManager::finished, [&](QNetworkReply* reply) {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                qDebug() << "Response:" << response;
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
