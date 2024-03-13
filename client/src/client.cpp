#include <QDateTime>
#include <QTextStream>
#include <QtNetwork>

#include "client.hpp"
#include "survey.hpp"

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
            } else
                qCritical() << "Error:" << reply->errorString();
            emit finished();
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
    cout << "Retrieved " << surveys.count() << " surveys:" << endl;
    for (auto survey : surveys)
        cout << "- " << survey->name << endl;
}
