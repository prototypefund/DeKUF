#pragma once

#include <QtCore>
#include <core/storage.hpp>

class StorageStub : public Storage {
private:
    QList<QPair<QString, QString>> dataPoints;
    QList<SurveyResponseRecord> surveyResponses;
    QList<SurveyRecord> surveyRecords;
    QList<QSharedPointer<Survey>> surveys;

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

    QList<SurveyRecord> listSurveyRecords() const { return surveyRecords; }

    void addSurveyRecord(const Survey& survey, const QString& clientId,
        const QString& delegateId, const std::optional<int>& groupSize)
    {
// TODO
#if 0
        surveyRecords.push_back(
            { .survey = QSharedPointer<Survey>::create(survey),
                .clientId = clientId,
                .delegateId = delegateId,
                .groupSize = groupSize });
#else
        surveyRecords.push_back(
            SurveyRecord(QSharedPointer<Survey>::create(survey), clientId,
                delegateId, groupSize));
#endif
    }

    void saveSurveyRecord(const SurveyRecord& record)
    {
        for (auto& existingSurvey : surveyRecords) {
            if (existingSurvey.clientId != record.clientId)
                continue;
            existingSurvey.delegateId = record.delegateId;
            existingSurvey.groupSize = record.groupSize;
            break;
        }
    }

    void addSurvey(const Survey& survey)
    {
        surveys.push_back(QSharedPointer<Survey>::create(survey));
    }

    std::optional<QSharedPointer<Survey>> findSurveyById(
        const QString& survey_id) const
    {
        for (auto survey : surveys) {
            if (survey->id == survey_id)
                return survey;
        }
        return std::nullopt;
    }

    QSharedPointer<SurveyRecord> findSurveyRecordById(
        const QString& survey_id) const
    {
        return nullptr;
    }
};
