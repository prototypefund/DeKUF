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

struct SurveySignup {
    QSharedPointer<Survey> survey;

    // TODO: This probably deserves an enum and better named/documented states.
    /// Possible states: initial, processing, done
    QString state;

    QString clientId;
    QString delegateId;
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
    virtual QList<SurveySignup> listSurveySignups() const = 0;
    virtual QList<SurveySignup> listSurveySignupsForState(
        const QString& state) const = 0;
    virtual void addSurveySignup(const Survey& survey, const QString& state,
        const QString& clientId, const QString& delegateId)
        = 0;
    virtual void saveSurveySignup(const SurveySignup& signup) = 0;
};
