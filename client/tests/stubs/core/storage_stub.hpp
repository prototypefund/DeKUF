#pragma once

#include <QtCore>
#include <core/storage.hpp>

class StorageStub : public Storage {
private:
    QList<QPair<QString, QString>> dataPoints;
    QList<SurveyResponseRecord> surveyResponses;

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

    QList<SurveyResponseRecord> listSurveyResponses() const
    {
        return surveyResponses;
    }

    void addSurveyResponse(const SurveyResponse& response, const Survey& survey)
    {
        surveyResponses.push_back(
            { .response = QSharedPointer<SurveyResponse>::create(response),
                .createdAt = QDateTime::currentDateTime() });
    }
};
