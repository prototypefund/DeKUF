#pragma once

#include "../../src/storage.hpp"
#include <QtCore>

class StorageStub : public Storage {
private:
    QList<QPair<QString, QString>> dataPoints;
    QList<QSharedPointer<SurveyResponse>> surveyResponses;

public:
    QList<DataPoint> listDataPoints(const QString& key) const
    {
        QList<DataPoint> matchingValues;
        for (const auto& dataPoint : dataPoints)
            matchingValues.push_back({ .value = dataPoint.second });
        return matchingValues;
    };

    void addDataPoint(const QString& dataKey, const QString& data)
    {
        dataPoints.push_back(QPair<QString, QString>(dataKey, data));
    }

    QList<QSharedPointer<SurveyResponse>> listSurveyResponses() const
    {
        return surveyResponses;
    }

    void addSurveyResponse(const SurveyResponse& response)
    {
        surveyResponses.push_back(
            QSharedPointer<SurveyResponse>::create(response));
    }
};
