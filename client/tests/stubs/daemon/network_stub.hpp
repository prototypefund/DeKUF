#include <daemon/network.hpp>

class NetworkStub : public Network {
public:
    QByteArray listSurveysResponse;
    QByteArray getSignupStateResponse;

    QByteArray listSurveys() const { return listSurveysResponse; }

    QByteArray surveySignup(const QString& surveyId, const QString& publicKey)
    {
        return {};
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
};
