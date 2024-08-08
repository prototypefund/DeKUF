#include <daemon/network.hpp>

class NetworkStub : public Network {
public:
    QByteArray listSurveysResponse;
    QByteArray getSignupStateResponse;

    QFuture<QByteArray> listSurveys() const
    {
        return dummyFuture(listSurveysResponse);
    }

    QFuture<QByteArray> surveySignup(
        const QString& surveyId, const QString& publicKey)
    {
        return dummyFuture(QByteArray());
    }

    QByteArray getSignupState(const QString& clientId) const
    {
        return getSignupStateResponse;
    }

    QByteArray getMessagesForDelegate(const QString& delegatePublicKey) const
    {
        return {};
    }

    bool postMessageToDelegate(
        const QString& delegatePublicKey, const QString& message) const
    {
        return true;
    }

    bool postAggregationResult(
        const QString& delegatePublicKey, const QByteArray& data)
    {
        return true;
    }

private:
    QTimer timer;

    template <typename T> QFuture<T> dummyFuture(T value) const
    {
        const auto t = const_cast<QTimer* const>(&timer); // nolint
        auto future = QtFuture::connect(t, &QTimer::timeout).then([=] {
            return value;
        });
        t->start(1);
        return future;
    }

    QFuture<void> dummyFuture() const
    {
        return dummyFuture(true).then([](bool b) {});
    }
};
