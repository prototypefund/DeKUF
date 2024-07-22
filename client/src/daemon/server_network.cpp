#include <QtNetwork>

#include "server_network.hpp"

ServerNetwork::ServerNetwork()
    : manager(new QNetworkAccessManager(this))
{
}

QFuture<QByteArray> ServerNetwork::listSurveys() const
{
    return getRequest("http://localhost:8000/api/surveys/")
        .then([&](QNetworkReply* reply) {
            if (reply->error() != QNetworkReply::NoError) {
                qCritical() << "Error:" << reply->errorString();
                return QByteArray();
            }
            return reply->readAll();
        });
}

QFuture<QByteArray> ServerNetwork::surveySignup(
    const QString& surveyId, const QString& publicKey)
{
    auto url = QString("http://localhost:8000/api/survey-signup/");
    QJsonObject jsonObjData;
    jsonObjData["survey_id"] = surveyId;
    jsonObjData["public_key"] = publicKey;
    const QJsonDocument jsonDocData(jsonObjData);
    return postRequest(url, jsonDocData.toJson())
        .then([](QNetworkReply* reply) {
            if (reply->error() != QNetworkReply::NoError) {
                qCritical() << "Error:" << reply->errorString();
                return QByteArray();
            }
            return reply->readAll();
        });
}

QFuture<QByteArray> ServerNetwork::getSignupState(const QString& clientId) const
{
    const auto url
        = QString("http://localhost:8000/api/signup-state/%1/").arg(clientId);
    return getRequest(url).then([&](QNetworkReply* reply) {
        if (reply->error() != QNetworkReply::NoError) {
            qCritical() << "Error:" << reply->errorString();
            return QByteArray();
        }
        return reply->readAll();
    });
}

QFuture<bool> ServerNetwork::postMessageToDelegate(
    const QString& delegatePublicKey, const QString& message) const
{
    QJsonObject jsonObjData;
    jsonObjData["message"] = message;
    jsonObjData["public_key"] = delegatePublicKey;
    const QJsonDocument jsonDocData(jsonObjData);
    auto url = QString("http://localhost:8000/api/message-to-delegate/");
    return postRequest(url, jsonDocData.toJson())
        .then([](QNetworkReply* reply) {
            if (reply->error() != QNetworkReply::NoError) {
                qCritical() << "Error:" << reply->errorString();
                return false;
            }
            return true;
        });
}

QFuture<QByteArray> ServerNetwork::getMessagesForDelegate(
    const QString& delegateId) const
{
    const auto url
        = QString("http://localhost:8000/api/messages-for-delegate/%1/")
              .arg(delegateId);
    return getRequest(url).then([&](QNetworkReply* reply) {
        if (reply->error() != QNetworkReply::NoError) {
            qCritical() << "Error:" << reply->errorString();
            return QByteArray();
        }

        auto status
            = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (status != 200) {
            return QByteArray();
        }

        return reply->readAll();
    });
}

QFuture<void> ServerNetwork::postAggregationResult(
    const QString& delegateId, const QByteArray& data)
{
    auto url = QString("http://localhost:8000/api/post-aggregation-result/%1/")
                   .arg(delegateId);
    return postRequest(url, data).then([](QNetworkReply* response) {
        // TODO: Error handling.
    });
}

QFuture<QNetworkReply*> ServerNetwork::getRequest(const QString& url) const
{
    QNetworkRequest request(url);
    auto reply = manager->get(request);
    auto future = QtFuture::connect(reply, &QNetworkReply::finished);
    return future.then([reply] { return reply; });
}

QFuture<QNetworkReply*> ServerNetwork::postRequest(
    const QString& url, const QByteArray& data) const
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    auto reply = manager->post(request, data);
    auto future = QtFuture::connect(reply, &QNetworkReply::finished);
    return future.then([reply] { return reply; });
}
