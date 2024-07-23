#pragma once

#include <QtCore>

class Network {
public:
    virtual ~Network() = default;

    virtual QFuture<QByteArray> listSurveys() const = 0;
    virtual QFuture<QByteArray> surveySignup(
        const QString& surveyId, const QString& publicKey)
        = 0;
    virtual QFuture<QByteArray> getSignupState(
        const QString& clientId) const = 0;
    virtual QFuture<bool> postMessageToDelegate(
        const QString& delegatePublicKey, const QString& message) const = 0;
    virtual QFuture<QByteArray> getMessagesForDelegate(
        const QString& delegateId) const = 0;
    virtual QFuture<bool> postAggregationResult(
        const QString& delegateId, const QByteArray& data)
        = 0;
};
