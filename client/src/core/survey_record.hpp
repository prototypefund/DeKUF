#pragma once

enum SurveyState { Initial, Processing, Done };

class SurveyRecord {
public:
    QSharedPointer<Survey> survey;

    QString clientId;
    QString delegateId;
    std::optional<int> groupSize;

    SurveyRecord(const QSharedPointer<Survey>& survey, const QString& clientId,
        const QString& delegateId, const std::optional<int>& groupSize,
        const bool hasResponse = false)
        : survey(survey)
        , clientId(clientId)
        , delegateId(delegateId)
        , groupSize(groupSize)
        , hasResponse(hasResponse)
    {
    }

    SurveyState getState() const
    {
        if (hasResponse)
            return Done;

        if (!delegateId.isEmpty() && clientId == delegateId)
            return Processing;

        return Initial;
    }

private:
    bool hasResponse;
};
