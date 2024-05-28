#pragma once

#include <QtCore>
#include <core/survey.hpp>

class SurveyResponse;

struct DataPoint {
    QString key;
    QString value;
    QDateTime createdAt;
};

struct SurveyResponseRecord {
    QSharedPointer<SurveyResponse> response;
    QSharedPointer<Survey> survey;
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
};
