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

QFuture<QByteArray> ServerNetwork::surveySignup(const QString& surveyId)
{
    auto url
        = QString("http://localhost:8000/api/survey-signup/%1/").arg(surveyId);
    QPromise<QByteArray> promise;
    postRequest(url, "", [&](QNetworkReply* reply) {
        if (reply->error() != QNetworkReply::NoError) {
            qCritical() << "Error:" << reply->errorString();
            promise.addResult(QByteArray());
            promise.finish();
            return;
        }
        promise.addResult(reply->readAll());
        promise.finish();
    });
    return promise.future();
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
    QPromise<void> promise;
    postRequest(url, data, [=, &promise](QNetworkReply* response) {
        // TODO: Error handling.
        promise.finish();
    });
    return promise.future();
}

QFuture<QNetworkReply*> ServerNetwork::getRequest(const QString& url) const
{
    QNetworkRequest request(url);
    auto reply = manager->get(request);
    auto future = QtFuture::connect(reply, &QNetworkReply::finished);
    return future.then([reply] { return reply; });
}

// TODO: Rewrite to return a future.
void ServerNetwork::postRequest(const QString& url, const QByteArray& data,
    std::function<void(QNetworkReply*)> callback) const
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
