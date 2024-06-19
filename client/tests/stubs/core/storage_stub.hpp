#pragma once

#include <QtCore>
#include <core/storage.hpp>

class StorageStub : public Storage {
private:
    QList<QPair<QString, QString>> dataPoints;
    QList<SurveyResponseRecord> surveyResponses;
    QList<SurveySignup> surveySignups;

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

    QList<SurveySignup> listSurveySignups() const { return surveySignups; }

    QList<SurveySignup> listSurveySignupsForState(const QString& state) const
    {
        QList<SurveySignup> signups;
        for (auto signup : listSurveySignups())
            if (signup.state == state)
                signups.append(signup);
        return signups;
    }

    void addSurveySignup(const Survey& survey, const QString& state,
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
        // TODO: The only thing that can currently be changed is the state.
        for (auto signup : surveySignups) {
            if (signup.clientId != signup.clientId)
                continue;
            signup.state = signup.state;
            break;
        }
    }
};
