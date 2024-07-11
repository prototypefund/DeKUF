#include <QtSql>

#include "core/storage.hpp"
#include "core/survey.hpp"
#include "survey_response.hpp"

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
    const auto result = query.exec();
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

    query.prepare("CREATE TABLE IF NOT EXISTS survey_response_record("
                  "    id INTEGER PRIMARY KEY,"
                  "    data TEXT,"
                  "    survey_id VARCHAR(255),"
                  "    created_at DATETIME"
                  ")");
    execQuery(query);

    query.prepare("CREATE TABLE IF NOT EXISTS survey_record("
                  "    id VARCHAR(255) PRIMARY KEY,"
                  "    survey_id VARCHAR(255),"
                  "    survey_data TEXT,"
                  "    client_id VARCHAR(255),"
                  "    delegate_id VARCHAR(255),"
                  "    group_size INT"
                  ")");
    execQuery(query);
}

template <typename T>
QVariant optionalToQVariant(const std::optional<T>& optional)
{
    return optional.has_value() ? optional.value() : QVariant();
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

QList<DataPoint> SqliteStorage::listDataPoints(const QString& key) const
{
    QList<DataPoint> dataPoints;
    QSqlQuery query;
    if (key.isEmpty())
        query.prepare("SELECT key, value, created_at FROM data_point");
    else {
        query.prepare(
            "SELECT key, value, created_at FROM data_point WHERE key = :key");
        query.bindValue(":key", key);
    }
    if (!execQuery(query))
        return dataPoints;

    while (query.next())
        dataPoints.push_back({ .key = query.value(0).toString(),
            .value = query.value(1).toString(),
            .createdAt = query.value(2).toDateTime() });
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

QList<SurveyResponseRecord> SqliteStorage::listSurveyResponses() const
{
    QList<SurveyResponseRecord> responses;
    QSqlQuery query;
    query.prepare(
        "SELECT data, survey_id, created_at FROM survey_response_record");
    if (!execQuery(query))
        return responses;

    while (query.next()) {
        const auto data = query.value(0).toByteArray();
        QSharedPointer<SurveyResponse> response(
            SurveyResponse::fromJsonByteArray(data));

        const auto surveyId = query.value(1).toString();
        const auto surveyRecord = findSurveyRecordById(surveyId);

        const auto createdAt = query.value(2).toDateTime();

        responses.push_back({ .response = response,
            .surveyRecord = surveyRecord,
            .createdAt = createdAt });
    }

    return responses;
}

void SqliteStorage::addSurveyResponse(
    const SurveyResponse& response, const Survey& survey)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO survey_response_record (data, survey_id, created_at)"
        "values (:data, :survey_id, CURRENT_TIMESTAMP)");
    query.bindValue(":data", response.toJsonByteArray());
    query.bindValue(":survey_id", survey.id);
    execQuery(query);
}

QList<SurveyRecord> SqliteStorage::listSurveyRecords() const
{
    QList<SurveyRecord> survey_records;
    QSqlQuery query;
    query.prepare("SELECT survey_data, client_id, delegate_id, group_size "
                  "FROM survey_record");
    if (!execQuery(query))
        return survey_records;

    while (query.next()) {
        const auto survey = Survey::fromByteArray(query.value(0).toByteArray());
        const auto clientId = query.value(1).toString();
        const auto delegateId = query.value(2).toString();
        const auto groupSize = query.value(3).toInt();
        survey_records.push_back(
            SurveyRecord(survey, clientId, delegateId, groupSize));
    }

    return survey_records;
}

void SqliteStorage::addSurveyRecord(const Survey& survey,
    const QString& clientId, const QString& delegateId,
    const std::optional<int>& groupSize)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO survey_record (
            survey_id, survey_data, client_id, delegate_id, group_size
        )
        VALUES (
            :survey_id, :survey_data, :client_id, :delegate_id, :group_size
        )
    )");
    query.bindValue(":survey_id", survey.id);
    query.bindValue(":survey_data", survey.toByteArray());
    query.bindValue(":client_id", clientId);
    query.bindValue(":delegate_id", delegateId);
    query.bindValue(":group_size", optionalToQVariant(groupSize));
    execQuery(query);
}

void SqliteStorage::saveSurveyRecord(const SurveyRecord& record)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE survey_record
        SET client_id = :client_id,
            delegate_id = :delegate_id,
            group_size = :group_size
        WHERE survey_id = :survey_id
    )");
    query.bindValue(":survey_id", record.survey->id);
    query.bindValue(":client_id", record.clientId);
    query.bindValue(":delegate_id", record.delegateId);
    query.bindValue(":group_size", optionalToQVariant(record.groupSize));
    execQuery(query);
}

QSharedPointer<SurveyRecord> SqliteStorage::findSurveyRecordById(
    const QString& surveyId) const
{
    QSqlQuery query;
    query.prepare(R"(SELECT survey_data, client_id, delegate_id, group_size
                     FROM survey_record
                     WHERE survey_id = :survey_id)");
    query.bindValue(":survey_id", surveyId);
    if (!execQuery(query)) {
        return nullptr;
    }

    if (!query.next())
        return nullptr;

    const auto survey = Survey::fromByteArray(query.value(0).toByteArray());
    const auto clientId = query.value(1).toString();
    const auto delegateId = query.value(2).toString();
    const auto groupSize = query.value(3).toInt();
    return QSharedPointer<SurveyRecord>::create(
        survey, clientId, delegateId, groupSize);
}
