#pragma once

#include "storage.hpp"
#include <QSqlDatabase>
#include <QtCore>

class SqliteStorage : public Storage {
public:
    explicit SqliteStorage(const QString& databasePath);
    SqliteStorage();

    QList<DataPoint> listDataPoints(const QString& key = "") const;
    void addDataPoint(const QString& key, const QString& value);
    bool checkIfDataPointPresent(const QString& key) const;
    QList<SurveyResponseRecord> listSurveyResponses() const;
    void addSurveyResponse(
        const SurveyResponse& response, const Survey& survey);
    std::optional<SurveyResponseRecord> findSurveyResponseFor(
        const QString& surveyId) const;
    QList<SurveyRecord> listSurveyRecords() const;
    void addSurveyRecord(const Survey& survey, const QString& clientId,
        const QString& publicKey, const QString& delegatePublicKey,
        const std::optional<QString>& dataPublicKey,
        const std::optional<int>& groupSize);
    void saveSurveyRecord(const SurveyRecord& record);
    QSharedPointer<SurveyRecord> findSurveyRecordById(
        const QString& surveyId) const;

private:
    QSqlDatabase db;
    SurveyResponseRecord createSurveyResponseRecord(const QByteArray& data,
        const QString& surveyId, const QDateTime& createdAt) const;
};
