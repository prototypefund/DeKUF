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

    query.prepare("CREATE TABLE IF NOT EXISTS survey_response("
                  "    id INTEGER PRIMARY KEY,"
                  "    data TEXT,"
                  "    survey TEXT,"
                  "    created_at DATETIME"
                  ")");
    execQuery(query);

    query.prepare("CREATE TABLE IF NOT EXISTS survey_signup("
                  "    id INTEGER PRIMARY KEY,"
                  "    survey TEXT,"
                  "    state VARCHAR(255),"
                  "    client_id VARCHAR(255),"
                  "    delegate_id VARCHAR(255)"
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
    query.prepare("SELECT data, survey, created_at FROM survey_response");
    if (!execQuery(query))
        return responses;

    while (query.next()) {
        const auto data = query.value(0).toByteArray();
        const auto surveyData = query.value(1).toByteArray();
        QSharedPointer<SurveyResponse> response(
            SurveyResponse::fromJsonByteArray(data));
        QSharedPointer<Survey> survey(Survey::fromByteArray(surveyData));
        const auto createdAt = query.value(2).toDateTime();
        responses.push_back(
            { .response = response, .survey = survey, .createdAt = createdAt });
    }

    return responses;
}

void SqliteStorage::addSurveyResponse(
    const SurveyResponse& response, const Survey& survey)
{
    QSqlQuery query;
    query.prepare("INSERT INTO survey_response (data, survey, created_at)"
                  "values (:data, :survey, CURRENT_TIMESTAMP)");
    query.bindValue(":data", response.toJsonByteArray());
    query.bindValue(":survey", survey.toByteArray());
    execQuery(query);
}

QList<SurveySignup> SqliteStorage::listSurveySignups() const
{
    QList<SurveySignup> signups;
    QSqlQuery query;
    query.prepare(
        "SELECT survey, state, client_id, delegate_id FROM survey_signup");
    if (!execQuery(query))
        return signups;

    while (query.next()) {
        const auto surveyData = query.value(0).toByteArray();
        QSharedPointer<Survey> survey(Survey::fromByteArray(surveyData));
        const auto state = query.value(1).toString();
        const auto clientId = query.value(2).toString();
        const auto delegateId = query.value(3).toString();
        signups.push_back({ .survey = survey,
            .state = state,
            .clientId = clientId,
            .delegateId = delegateId });
    }

    return signups;
}

QList<SurveySignup> SqliteStorage::listSurveySignupsForState(
    const QString& state) const
{
    QList<SurveySignup> signups;
    for (auto signup : listSurveySignups())
        if (signup.state == state)
            signups.append(signup);
    return signups;
}

QList<SurveySignup> SqliteStorage::listActiveDelegateSurveySignups() const
{
    QList<SurveySignup> signups;
    for (auto signup : listSurveySignups())
        if (signup.state == "processing"
            && signup.clientId == signup.delegateId)
            signups.append(signup);
    return signups;
}

void SqliteStorage::addSurveySignup(const Survey& survey, const QString& state,
    const QString& clientId, const QString& delegateId)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO survey_signup (survey, state, client_id, delegate_id)"
        "values (:survey, :state, :client_id, :delegate_id)");
    query.bindValue(":survey", survey.toByteArray());
    query.bindValue(":state", state);
    query.bindValue(":client_id", clientId);
    query.bindValue(":delegate_id", delegateId);
    execQuery(query);
}

void SqliteStorage::saveSurveySignup(const SurveySignup& signup)
{
    // TODO: The only things that can currently be changed is the state and the
    //       delegate ID.
    QSqlQuery query;
    query.prepare(
        R"(UPDATE survey_signup
           SET state = :state,
               delegate_id = :delegate_id
           WHERE client_id = :client_id)");
    query.bindValue(":state", signup.state);
    query.bindValue(":delegate_id", signup.delegateId);
    query.bindValue(":client_id", signup.clientId);
    execQuery(query);
}
