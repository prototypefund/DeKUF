#include <QtSql>

#include "sqlite_storage.hpp"

namespace {
const QString userDir = ".dekuf";
const QString dbFileName = "db.sqlite3";

QSqlDatabase createDb(const QString& path)
{
    if (QSqlDatabase::connectionNames().count() > 0)
        return QSqlDatabase::database();

    QFileInfo(path).dir().mkpath(".");
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    return db;
}

bool execQuery(QSqlQuery& query)
{
    auto result = query.exec();
    // TODO: Exception instead of return code.
    if (!result)
        qDebug() << query.lastError();
    return result;
}

void migrate()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS data_point("
                  "    id INTEGER PRIMARY KEY,"
                  "    key TEXT,"
                  "    value TEXT,"
                  "    created_at DATETIME"
                  ")");
    execQuery(query);
}
}

SqliteStorage::SqliteStorage(const QString& databasePath)
    : db(createDb(databasePath))
{
    // TODO: Exception instead of return code.
    if (!db.open())
        qDebug() << "Error: Unable to open database";
    migrate();
}

SqliteStorage::SqliteStorage()
    : SqliteStorage(QDir::homePath() + QDir::separator() + userDir
        + QDir::separator() + dbFileName)
{
}

QList<DataPoint> SqliteStorage::listDataPoints(const QString& key)
{
    QList<DataPoint> dataPoints;
    QSqlQuery query;
    query.prepare("SELECT value, created_at FROM data_point WHERE key = :key");
    query.bindValue(":key", key);
    if (!execQuery(query))
        return dataPoints;

    while (query.next())
        dataPoints.push_back({ .key = key,
            .value = query.value(0).toString(),
            .createdAt = query.value(1).toDateTime() });
    return dataPoints;
}

void SqliteStorage::addDataPoint(const QString& key, const QString& value)
{
    QSqlQuery query;
    query.prepare("INSERT INTO data_point (key, value, created_at)"
                  "    values (:key, :value, CURRENT_TIMESTAMP)");
    query.bindValue(":key", key);
    query.bindValue(":value", value);
    execQuery(query);
}
