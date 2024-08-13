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
                  "    public_key TEXT,"
                  "    delegate_public_key VARCHAR(255),"
                  "    aggregation_public_key TEXT,"
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

bool SqliteStorage::checkIfDataPointPresent(const QString& key) const
{
    QSqlQuery query;
    query.prepare("SELECT EXISTS(SELECT 1 FROM data_point WHERE key LIKE "
                  ":key)");
    query.bindValue(":key", key);
    if (query.exec() && query.next()) {
        return query.value(0).toBool();
    }

    return false;
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
        const auto surveyId = query.value(1).toString();
        const auto createdAt = query.value(2).toDateTime();
        responses.push_back(
            createSurveyResponseRecord(data, surveyId, createdAt));
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

std::optional<SurveyResponseRecord> SqliteStorage::findSurveyResponseFor(
    const QString& surveyId) const
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT data, created_at
        FROM survey_response_record
        WHERE survey_id = :survey_id
    )");
    query.bindValue(":survey_id", surveyId);
    if (!execQuery(query) || !query.next())
        return std::nullopt;

    const auto data = query.value(0).toByteArray();
    const auto createdAt = query.value(1).toDateTime();
    return createSurveyResponseRecord(data, surveyId, createdAt);
}

QList<SurveyRecord> SqliteStorage::listSurveyRecords() const
{
    QList<SurveyRecord> survey_records;
    QSqlQuery query;
    query.prepare("SELECT survey_data, client_id, public_key, "
                  "delegate_public_key, aggregation_public_key, group_size "
                  "FROM survey_record");
    if (!execQuery(query))
        return survey_records;

    while (query.next()) {
        const auto surveyResult
            = Survey::fromByteArray(query.value(0).toByteArray());
        Q_ASSERT(surveyResult.isSuccess());
        const auto clientId = query.value(1).toString();
        const auto publicKey = query.value(2).toString();
        const auto delegatePublicKey = query.value(3).toString();
        std::optional<QString> aggregationPublicKey;
        if (const QVariant value = query.value(4); !value.isNull()) {
            aggregationPublicKey = value.toString();
        }
        const auto groupSize = query.value(5).toInt();
        const auto hasResponse
            = findSurveyResponseFor(surveyResult.getValue()->id).has_value();
        survey_records.push_back(SurveyRecord(surveyResult.getValue(), clientId,
            publicKey, delegatePublicKey, aggregationPublicKey, groupSize,
            hasResponse));
    }

    return survey_records;
}

void SqliteStorage::addSurveyRecord(const Survey& survey,
    const QString& clientId, const QString& publicKey,
    const QString& delegatePublicKey,
    const std::optional<QString>& aggregationPublicKey,
    const std::optional<int>& groupSize)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO survey_record (
            survey_id,
            survey_data,
            client_id,
            public_key,
            delegate_public_key,
            aggregation_public_key,
            group_size
        )
        VALUES (
            :survey_id,
            :survey_data,
            :client_id,
            :public_key,
            :delegate_public_key,
            :aggregation_public_key,
            :group_size
        )
    )");
    query.bindValue(":survey_id", survey.id);
    query.bindValue(":survey_data", survey.toByteArray());
    query.bindValue(":client_id", clientId);
    query.bindValue(":public_key", publicKey);
    query.bindValue(":delegate_public_key", delegatePublicKey);
    query.bindValue(
        ":aggregation_public_key", optionalToQVariant(aggregationPublicKey));
    query.bindValue(":group_size", optionalToQVariant(groupSize));
    execQuery(query);
}

void SqliteStorage::saveSurveyRecord(const SurveyRecord& record)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE survey_record
        SET client_id = :client_id,
            delegate_public_key = :delegate_public_key,
            aggregation_public_key = :aggregation_public_key,
            group_size = :group_size
        WHERE survey_id = :survey_id
    )");
    query.bindValue(":survey_id", record.survey->id);
    query.bindValue(":client_id", record.clientId);
    query.bindValue(":delegate_public_key", record.delegatePublicKey);
    query.bindValue(":aggregation_public_key",
        optionalToQVariant(record.aggregationPublicKey));
    query.bindValue(":group_size", optionalToQVariant(record.groupSize));
    execQuery(query);
}

QSharedPointer<SurveyRecord> SqliteStorage::findSurveyRecordById(
    const QString& surveyId) const
{
    QSqlQuery query;
    query.prepare(
        R"(SELECT survey_data,
                        client_id,
                        public_key,
                        delegate_public_key,
                        aggregation_public_key,
                        group_size
                     FROM survey_record
                     WHERE survey_id = :survey_id)");
    query.bindValue(":survey_id", surveyId);
    if (!execQuery(query)) {
        return nullptr;
    }

    if (!query.next())
        return nullptr;

    const auto surveyParsingResult
        = Survey::fromByteArray(query.value(0).toByteArray());
    Q_ASSERT(surveyParsingResult.isSuccess());
    const auto clientId = query.value(1).toString();
    const auto publicKey = query.value(2).toString();
    const auto delegatePublicKey = query.value(3).toString();
    std::optional<QString> aggregationPublicKey;
    if (const QVariant value = query.value(4); !value.isNull()) {
        aggregationPublicKey = value.toString();
    }
    const auto groupSize = query.value(5).toInt();
    return QSharedPointer<SurveyRecord>::create(surveyParsingResult.getValue(),
        clientId, publicKey, delegatePublicKey, aggregationPublicKey,
        groupSize);
}

SurveyResponseRecord SqliteStorage::createSurveyResponseRecord(
    const QByteArray& data, const QString& surveyId,
    const QDateTime& createdAt) const
{
    auto responseResult = SurveyResponse::fromJsonByteArray(data);
    qDebug() << responseResult.getErrorMessage();
    Q_ASSERT(responseResult.isSuccess());
    const auto surveyRecord = findSurveyRecordById(surveyId);
    return { .response = responseResult.getValue(),
        .surveyRecord = surveyRecord,
        .createdAt = createdAt };
}
