#pragma once

#include <QtCore>
#include <core/survey.hpp>

class SurveyResponse;

struct DataPoint {
    QString key;
    QString value;
    QDateTime createdAt;
};

enum SurveyState { Initial, Processing, Done };

// TODO: Move this to it's own file
struct SurveyRecord {
    QSharedPointer<Survey> survey;
    // TODO: add response reference or method to get it

    QString clientId;
    QString delegateId;
    std::optional<int> groupSize;

    // TODO: Double check that we need all these constructors

    SurveyRecord() = default;

    explicit SurveyRecord(const QSharedPointer<Survey>& survey)
        : survey(survey)
    {
    }

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

struct SurveyResponseRecord {
    QSharedPointer<SurveyResponse> response;
    QSharedPointer<SurveyRecord> surveyRecord;
    QDateTime createdAt;
};

class Storage {
public:
    virtual ~Storage() {};
    virtual QList<DataPoint> listDataPoints(const QString& key = "") const = 0;
    virtual void addDataPoint(const QString& key, const QString& value) = 0;
    virtual QList<SurveyResponseRecord> listSurveyResponses() const = 0;
    virtual void addSurveyResponse(
        const SurveyResponse& response, const Survey& survey)
        = 0;
    virtual QList<SurveyRecord> listSurveyRecords() const = 0;
    virtual void addSurveyRecord(const Survey& survey, const QString& clientId,
        const QString& delegateId, const std::optional<int>& groupSize)
        = 0;
    virtual void saveSurveyRecord(const SurveyRecord& record) = 0;
    virtual QSharedPointer<SurveyRecord> findSurveyRecordById(
        const QString& survey_id) const = 0;
};
