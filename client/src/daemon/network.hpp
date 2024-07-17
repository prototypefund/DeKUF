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
    virtual QFuture<QByteArray> getMessagesForDelegate(
        const QString& delegateId) const = 0;
    virtual QFuture<void> postAggregationResult(
        const QString& delegateId, const QByteArray& data)
        = 0;
};
