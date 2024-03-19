#pragma once

#include "storage.hpp"
#include <QtCore>
#include <QtSql>

#define STORAGE_DATABASE_PATH "db.sqlite3"

class SqliteStorage : public Storage {
public:
    SqliteStorage()
        : db(QSqlDatabase::addDatabase("QSQLITE"))
    {
        db.setDatabaseName(STORAGE_DATABASE_PATH);
        if (!db.open())
            qDebug() << "Error: Unable to open database";
        migrate();
    }

    QList<QString> listDataPoints(const QString& key)
    {
        QList<QString> dataPoints;
        QSqlQuery query;
        query.prepare("SELECT value FROM data_point WHERE key = :key");
        query.bindValue(":key", key);
        if (!execQuery(query))
            return dataPoints;

        while (query.next())
            dataPoints.push_back(query.value(0).toString());
        return dataPoints;
    }

    void addDataPoint(const QString& key, const QString& value)
    {
        QSqlQuery query;
        query.prepare(
            "INSERT INTO data_point (key, value) values (:key, :value)");
        query.bindValue(":key", key);
        query.bindValue(":value", value);
        execQuery(query);
    }

private:
    static bool execQuery(QSqlQuery& query)
    {
        auto result = query.exec();
        // TODO: Exception instead of return code.
        if (!result)
            qDebug() << query.lastError();
        return result;
    }

    static void migrate()
    {
        QSqlQuery query;
        query.prepare("CREATE TABLE IF NOT EXISTS data_point("
                      "    id INTEGER PRIMARY KEY,"
                      "    key TEXT,"
                      "    value TEXT"
                      ")");
        execQuery(query);
    }

    QSqlDatabase db;
};
