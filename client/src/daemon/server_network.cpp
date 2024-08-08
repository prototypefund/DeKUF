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

QByteArray ServerNetwork::getSignupState(const QString& clientId) const
{
    const auto url
        = QString("http://localhost:8000/api/signup-state/%1/").arg(clientId);
    auto reply = getRequestSync(url);
    if (reply->error() != QNetworkReply::NoError) {
        qCritical() << "Error:" << reply->errorString();
        return {};
    }
    return reply->readAll();
}

bool ServerNetwork::postMessageToDelegate(
    const QString& delegatePublicKey, const QString& message) const
{
    QJsonObject jsonObjData;
    jsonObjData["message"] = message;
    jsonObjData["public_key"] = delegatePublicKey;
    const QJsonDocument jsonDocData(jsonObjData);
    auto url = QString("http://localhost:8000/api/message-to-delegate/");
    auto reply = postRequestSync(url, jsonDocData.toJson());
    if (reply->error() != QNetworkReply::NoError) {
        qCritical() << "Error:" << reply->errorString();
        return false;
    }
    return true;
}

QByteArray ServerNetwork::getMessagesForDelegate(
    const QString& delegateId) const
{
    const auto url
        = QString("http://localhost:8000/api/messages-for-delegate/%1/")
              .arg(delegateId);
    auto reply = getRequestSync(url);
    if (reply->error() != QNetworkReply::NoError) {
        qCritical() << "Error:" << reply->errorString();
        return {};
    }

    auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (status != 200) {
        return {};
    }

    return reply->readAll();
}

// TODO: May be better to return a proper result type with success and result
// instead of a bool
bool ServerNetwork::postAggregationResult(
    const QString& delegateId, const QByteArray& data)
{
    auto url = QString("http://localhost:8000/api/post-aggregation-result/%1/")
                   .arg(delegateId);
    auto reply = postRequestSync(url, data);
    if (reply->error() != QNetworkReply::NoError) {
        qCritical() << "Error:" << reply->errorString();
        return false;
    }
    return true;
}

QFuture<QNetworkReply*> ServerNetwork::getRequest(const QString& url) const
{
    QNetworkRequest request(url);
    auto reply = manager->get(request);
    auto future = QtFuture::connect(reply, &QNetworkReply::finished);
    return future.then([reply] { return reply; });
}

QNetworkReply* ServerNetwork::getRequestSync(const QString& url) const
{
    QNetworkRequest request(url);
    auto reply = manager->get(request);
    QEventLoop eventLoop;
    QObject::connect(
        reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
    return reply;
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

QNetworkReply* ServerNetwork::postRequestSync(
    const QString& url, const QByteArray& data) const
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    auto reply = manager->post(request, data);
    QEventLoop eventLoop;
    QObject::connect(
        reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
    return reply;
}
