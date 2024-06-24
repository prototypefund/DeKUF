#pragma once

#include "storage.hpp"
#include <QSqlDatabase>
#include <QtCore>

class SqliteStorage : public Storage {
public:
    explicit SqliteStorage(const QString& databasePath);
    SqliteStorage();

    QList<DataPoint> listDataPoints(const QString& key) const;
    void addDataPoint(const QString& key, const QString& value);
    QList<SurveyResponseRecord> listSurveyResponses() const;
    void addSurveyResponse(
        const SurveyResponse& response, const Survey& survey);
    QList<SurveySignup> listSurveySignups() const;
    QList<SurveySignup> listSurveySignupsForState(const QString& state) const;
    QList<SurveySignup> listActiveDelegateSurveySignups() const;
    void addSurveySignup(const Survey& survey, const QString& state,
        const QString& clientId, const QString& delegateId);
    void saveSurveySignup(const SurveySignup& signup);

private:
    QSqlDatabase db;
};
