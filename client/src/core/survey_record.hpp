#pragma once

enum SurveyState { Initial, Processing, Done };

class SurveyRecord {
public:
    QSharedPointer<Survey> survey;

    QString clientId;
    QString publicKey;
    QString delegatePublicKey;
    std::optional<QString> aggregationPublicKey;
    std::optional<int> groupSize;

    SurveyRecord(const QSharedPointer<Survey>& survey, const QString& clientId,
        const QString& publicKey, const QString& delegatePublicKey,
        const std::optional<QString>& aggregationPublicKey,
        const std::optional<int>& groupSize, const bool hasResponse = false)
        : survey(survey)
        , clientId(clientId)
        , publicKey(publicKey)
        , delegatePublicKey(delegatePublicKey)
        , aggregationPublicKey(aggregationPublicKey)
        , groupSize(groupSize)
        , hasResponse(hasResponse)
    {
    }

    SurveyState getState() const
    {
        if (hasResponse)
            return Done;

        if (!delegatePublicKey.isEmpty() && publicKey == delegatePublicKey)
            return Processing;

        return Initial;
    }

private:
    bool hasResponse;
};
