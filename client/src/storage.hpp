#pragma once

#include <QtCore>

class SurveyResponse;

struct DataPoint {
    QString key;
    QString value;
    QDateTime createdAt;
};

class Storage {
public:
    virtual ~Storage() {};
    virtual QList<DataPoint> listDataPoints(const QString& key) = 0;
    virtual void addDataPoint(const QString& key, const QString& value) = 0;
    virtual QList<QSharedPointer<SurveyResponse>> listSurveyResponses() = 0;
    virtual void addSurveyResponse(const SurveyResponse& response) = 0;
};
