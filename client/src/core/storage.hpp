#pragma once

#include <QtCore>
#include <core/survey.hpp>

#include "survey_record.hpp"

class SurveyResponse;

struct DataPoint {
    QString key;
    QString value;
    QDateTime createdAt;
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
    virtual std::optional<SurveyResponseRecord> findSurveyResponseFor(
        const QString& surveyId) const = 0;
    virtual QList<SurveyRecord> listSurveyRecords() const = 0;
    virtual void addSurveyRecord(const Survey& survey, const QString& clientId,
        const QString& delegateId, const std::optional<int>& groupSize)
        = 0;
    virtual void saveSurveyRecord(const SurveyRecord& record) = 0;
    virtual QSharedPointer<SurveyRecord> findSurveyRecordById(
        const QString& survey_id) const = 0;
};
