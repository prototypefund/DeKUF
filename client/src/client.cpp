#include <QTextStream>
#include <QtNetwork>

#include "client.hpp"

Client::Client(QObject* parent)
    : QObject(parent)
{
}

void Client::run()
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

void Client::handleSurveysResponse(const QByteArray& data)
{
    using Qt::endl;

    QTextStream cout(stdout);
    auto document = QJsonDocument::fromJson(data);
    auto surveys = document.array();
    cout << "Retrieved " << surveys.count() << " surveys:" << endl;
    for (auto survey : surveys) {
        auto surveyObject = survey.toObject();
        cout << "- " << surveyObject["name"].toString() << endl;
    }
}
