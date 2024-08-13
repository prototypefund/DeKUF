#pragma once

#include "network.hpp"

class QNetworkAccessManager;
class QNetworkReply;

class ServerNetwork : public QObject, public Network {
    Q_OBJECT

public:
    ServerNetwork();

    QFuture<QByteArray> listSurveys() const;
    QFuture<QByteArray> surveySignup(
        const QString& surveyId, const QString& publicKey);
    QByteArray getSignupState(const QString& clientId) const;
    bool postMessageToDelegate(
        const QString& delegatePublicKey, const QString& message) const;
    QByteArray getMessagesForDelegate(const QString& delegateId) const;
    bool postAggregationResult(
        const QString& delegateId, const QByteArray& data);

private:
    QNetworkAccessManager* manager;

    QFuture<QNetworkReply*> getRequest(const QString& url) const;
    QNetworkReply* getRequestSync(const QString& url) const;
    QFuture<QNetworkReply*> postRequest(
        const QString& url, const QByteArray& data) const;
    QNetworkReply* postRequestSync(
        const QString& url, const QByteArray& data) const;
};
