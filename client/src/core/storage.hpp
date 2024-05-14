#pragma once

#include <QtCore>

class SurveyResponse;

struct DataPoint {
    QString key;
    QString value;
    QDateTime createdAt;
};

struct SurveyResponseRecord {
    QSharedPointer<SurveyResponse> response;
    QDateTime createdAt;
    QString commissionerName;
};

class Storage {
public:
    virtual ~Storage() {};
    virtual QList<DataPoint> listDataPoints(const QString& key = "") const = 0;
    virtual void addDataPoint(const QString& key, const QString& value) = 0;
    virtual QList<SurveyResponseRecord> listSurveyResponses() const = 0;
    virtual void addSurveyResponse(const SurveyResponse& response) = 0;
};
