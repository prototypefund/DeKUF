#include <QTest>

#include <core/sqlite_storage.hpp>
#include <core/survey.hpp>
#include <core/survey_response.hpp>

#include "sqlite_storage_test.hpp"

namespace {
const QString databasePath = "test-db.sqlite3";
};

void SqliteStorageTest::init() { storage = new SqliteStorage(databasePath); }

void SqliteStorageTest::cleanup()
{
    delete storage;
    QFile dbFile(databasePath);
    dbFile.remove();
}

void SqliteStorageTest::testListDataPointsInitiallyEmpty()
{
    const auto dataPoints = storage->listDataPoints();
    QCOMPARE(dataPoints.count(), 0);
}

void SqliteStorageTest::testAddAndListDataPoints()
{
    storage->addDataPoint("timestamp", "1337");
    const auto dataPoints = storage->listDataPoints();
    QCOMPARE(dataPoints.count(), 1);
    const auto first = dataPoints.first();
    QCOMPARE(first.key, "timestamp");
    QCOMPARE(first.value, "1337");
    QCOMPARE(first.createdAt.date(), QDate::currentDate());
}

void SqliteStorageTest::testListDataPointsByName()
{
    storage->addDataPoint("a", "a");
    storage->addDataPoint("b", "b");
    QCOMPARE(storage->listDataPoints().size(), 2);
    QCOMPARE(storage->listDataPoints("a").size(), 1);
}

void SqliteStorageTest::testAddAndListSurveyResponses()
{
    QCOMPARE(storage->listSurveyResponses().count(), 0);

    Survey survey("1", "testName");
    SurveyResponse response("1");
    storage->addSurveyResponse(response, survey);

    QCOMPARE(storage->listSurveyResponses().count(), 1);
}

void SqliteStorageTest::testAddAndListSurveyResponseWithSurvey()
{
    QCOMPARE(storage->listSurveyResponses().count(), 0);

    auto commissioner
        = QSharedPointer<Commissioner>::create("testCommissioner");
    Survey survey("1", "testName");
    survey.commissioner = commissioner;
    SurveyResponse response("1");
    storage->addSurveyResponse(response, survey);

    QCOMPARE(storage->listSurveyResponses().count(), 1);

    auto surveyResponse = storage->listSurveyResponses().first();
    QCOMPARE(surveyResponse.response->surveyId, "1");

    auto storedSurvey = surveyResponse.surveyRecord->survey;
    QCOMPARE(storedSurvey->id, "1");
    QCOMPARE(storedSurvey->commissioner->name, "testCommissioner");
    QCOMPARE(storedSurvey->name, "testName");
}

void SqliteStorageTest::testAddAndListSurveySignups()
{
    QCOMPARE(storage->listSurveyRecords().count(), 0);

    Survey survey("1", "testName");
    QString state("foo");
    QString clientId("bar");
    QString delegateId("");
    storage->addSurveyRecord(survey, clientId, delegateId, std::nullopt);

    QCOMPARE(storage->listSurveyRecords().count(), 1);
}

void SqliteStorageTest::testSaveSurveySignup()
{
    storage->addSurveyRecord(Survey("1", "1"), "1", "", std::nullopt);
    auto signup = storage->listSurveyRecords().first();
    signup.delegateId = "2";
    signup.groupSize = 1337;
    storage->saveSurveySignup(signup);
    auto retrievedSignup = storage->listSurveyRecords().first();
    QCOMPARE(retrievedSignup.getState(), signup.getState());
    QCOMPARE(retrievedSignup.delegateId, signup.delegateId);
    QCOMPARE(retrievedSignup.groupSize, signup.groupSize);
}

// TODO:
/*
void SqliteStorageTest::testSaveSurveyWorksWithValuesPresent()
{
    Survey testSurvey("123", "test");
    testSurvey.queries.append(QSharedPointer<Query>::create(
        "12345", "testDataKey", QList<QString> { "1", "2", "3" }, true));

    storage->addSurveyRecord(testSurvey);
    auto retrievedSurvey = storage->findSurveyById(testSurvey.id);

    QVERIFY(retrievedSurvey.has_value());
    if (!retrievedSurvey.has_value()) {
        return;
    }

    auto retrievedSurveyValue = retrievedSurvey.value();

    QCOMPARE(retrievedSurveyValue->id, testSurvey.id);
    QCOMPARE(retrievedSurveyValue->name, testSurvey.name);
    QCOMPARE(retrievedSurveyValue->queries.first()->id,
        testSurvey.queries.first()->id);
    QCOMPARE(retrievedSurveyValue->queries.first()->dataKey,
        testSurvey.queries.first()->dataKey);
    QCOMPARE(retrievedSurveyValue->queries.first()->cohorts,
        testSurvey.queries.first()->cohorts);
    QCOMPARE(retrievedSurveyValue->queries.first()->discrete,
        testSurvey.queries.first()->discrete);
}

void SqliteStorageTest::testSaveSurveyWorksWithReturningNullWhenNotFound()
{
    QVERIFY(!storage->findSurveyById("123").has_value());
}
*/

QTEST_MAIN(SqliteStorageTest)
