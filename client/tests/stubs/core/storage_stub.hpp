#pragma once

#include <QtCore>
#include <core/storage.hpp>

class StorageStub : public Storage {
private:
    QList<QPair<QString, QString>> dataPoints;
    QList<SurveyResponseRecord> surveyResponses;
    QList<SurveySignup> surveySignups;
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

    QList<SurveySignup> listSurveyRecords() const { return surveySignups; }

    void addSurveyRecord(const Survey& survey, const QString& state,
        const QString& clientId, const QString& delegateId)
    {
        surveySignups.push_back(
            { .survey = QSharedPointer<Survey>::create(survey),
                .state = state,
                .clientId = clientId,
                .delegateId = delegateId });
    }

    void saveSurveySignup(const SurveySignup& signup)
    {
        for (auto& existingSignup : surveySignups) {
            if (existingSignup.clientId != signup.clientId)
                continue;
            existingSignup.state = signup.state;
            existingSignup.delegateId = signup.delegateId;
            existingSignup.groupSize = signup.groupSize;
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
};
