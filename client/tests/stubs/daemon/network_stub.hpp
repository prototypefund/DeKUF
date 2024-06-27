#include <daemon/network.hpp>

class NetworkStub : public Network {
public:
    QByteArray listSurveysResponse;

    QFuture<QByteArray> listSurveys() const
    {
        return dummyFuture(listSurveysResponse);
    }

    QFuture<QByteArray> surveySignup(const QString& surveyId)
    {
        return dummyFuture(QByteArray());
    }

    QFuture<QByteArray> getSignupState(const QString& clientId) const
    {
        return dummyFuture(QByteArray());
    }

    QFuture<QByteArray> getMessagesForDelegate(const QString& delegateId) const
    {
        return dummyFuture(QByteArray()).then([](QByteArray data) {
            return data;
        });
    }

    QFuture<void> postAggregationResult(
        const QString& delegateId, const QByteArray& data)
    {
        return dummyFuture();
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
