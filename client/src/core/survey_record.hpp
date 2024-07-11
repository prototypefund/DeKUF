#pragma once

enum SurveyState { Initial, Processing, Done };

struct SurveyRecord {
    QSharedPointer<Survey> survey;
    // TODO: add response reference or method to get it

    QString clientId;
    QString delegateId;
    std::optional<int> groupSize;

    SurveyRecord(const QSharedPointer<Survey>& survey, const QString& clientId,
        const QString& delegateId, const std::optional<int>& groupSize)
        : survey(survey)
        , clientId(clientId)
        , delegateId(delegateId)
        , groupSize(groupSize)
    {
    }

    SurveyState getState() const
    {
        if (delegateId.isEmpty())
            return Initial;
        // TODO: Add response case since we need to check if there or not to
        // determine status
        return Done;
    }
};
