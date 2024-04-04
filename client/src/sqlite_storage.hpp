#pragma once

#include "storage.hpp"
#include <QSqlDatabase>
#include <QtCore>

class SqliteStorage : public Storage {
public:
    explicit SqliteStorage(const QString& databasePath);
    SqliteStorage();

    QList<DataPoint> listDataPoints(const QString& key);
    void addDataPoint(const QString& key, const QString& value);
    QList<QSharedPointer<SurveyResponse>> listSurveyResponses();
    void addSurveyResponse(const SurveyResponse& response);

private:
    QSqlDatabase db;
};
