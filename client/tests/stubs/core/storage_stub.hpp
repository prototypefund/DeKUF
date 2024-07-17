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

    std::optional<SurveyResponseRecord> findSurveyResponseFor(
        const QString& surveyId) const
    {
        for (const auto& response : surveyResponses)
            if (response.surveyRecord->survey->id == surveyId)
                return response;
        return std::nullopt;
    }

    QList<SurveyRecord> listSurveyRecords() const { return surveyRecords; }

    void addSurveyRecord(const Survey& survey, const QString& clientId,
        const QString& publicKey, const QString& delegatePublicKey,
        const std::optional<int>& groupSize)
    {
        surveyRecords.push_back(
            SurveyRecord(QSharedPointer<Survey>::create(survey), clientId,
                publicKey, delegatePublicKey, groupSize));
    }

    void saveSurveyRecord(const SurveyRecord& record)
    {
        for (auto& existingSurvey : surveyRecords) {
            if (existingSurvey.clientId != record.clientId)
                continue;
            existingSurvey.delegatePublicKey = record.delegatePublicKey;
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
