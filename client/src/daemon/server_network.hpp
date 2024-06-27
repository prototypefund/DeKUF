#pragma once

#include "network.hpp"

class QNetworkAccessManager;
class QNetworkReply;

// TODO: Turn into an interface, move our server-specific implementation to a
//       new class.
class ServerNetwork : public QObject, public Network {
    Q_OBJECT

public:
    ServerNetwork();
    // virtual ~Network() = default;

    QFuture<QByteArray> listSurveys() const;
    QFuture<QByteArray> surveySignup(const QString& surveyId);
    QFuture<QByteArray> getSignupState(const QString& clientId) const;
    QFuture<QByteArray> getMessagesForDelegate(const QString& delegateId) const;
    QFuture<void> postAggregationResult(
        const QString& delegateId, const QByteArray& data);

private:
    QNetworkAccessManager* manager;

    QFuture<QNetworkReply*> getRequest(const QString& url) const;
    QFuture<QNetworkReply*> postRequest(
        const QString& url, const QByteArray& data) const;
};
