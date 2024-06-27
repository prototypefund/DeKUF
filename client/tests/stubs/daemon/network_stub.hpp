#include <daemon/network.hpp>

class NetworkStub : public Network {
    QFuture<QByteArray> listSurveys() const
    {
        QPromise<QByteArray> promise;
        promise.addResult(QByteArray());
        promise.finish();
        return promise.future();
    }

    QFuture<QByteArray> surveySignup(const QString& surveyId)
    {
        QPromise<QByteArray> promise;
        promise.addResult(QByteArray());
        promise.finish();
        return promise.future();
    }

    QFuture<QByteArray> getSignupState(const QString& clientId) const
    {
        QPromise<QByteArray> promise;
        promise.addResult(QByteArray());
        promise.finish();
        return promise.future();
    }

    QFuture<QByteArray> getMessagesForDelegate(const QString& delegateId) const
    {
        QPromise<QByteArray> promise;
        promise.addResult(QByteArray());
        promise.finish();
        return promise.future();
    }

    QFuture<void> postAggregationResult(
        const QString& delegateId, const QByteArray& data)
    {
        QPromise<void> promise;
        promise.finish();
        return promise.future();
    }
};
