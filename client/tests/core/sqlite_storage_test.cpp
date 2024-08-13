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
    storage->addSurveyRecord(survey, "", "", "", std::nullopt, std::nullopt);

    SurveyResponse response("1");
    storage->addSurveyResponse(response, survey);

    QCOMPARE(storage->listSurveyResponses().count(), 1);

    auto surveyResponse = storage->listSurveyResponses().first();
    QCOMPARE(surveyResponse.response->surveyId, "1");

    auto surveyRecord = surveyResponse.surveyRecord;
    QVERIFY(!surveyRecord.isNull());

    auto storedSurvey = surveyRecord->survey;
    QCOMPARE(storedSurvey->id, "1");
    QCOMPARE(storedSurvey->commissioner->name, "testCommissioner");
    QCOMPARE(storedSurvey->name, "testName");
}

void SqliteStorageTest::testAddAndListSurveyRecords()
{
    QCOMPARE(storage->listSurveyRecords().count(), 0);

    Survey survey("1", "testName");
    QString state("foo");
    QString clientId("bar");
    QString publicKey("");
    QString delegatePublicKey("");
    storage->addSurveyRecord(survey, clientId, publicKey, delegatePublicKey,
        std::nullopt, std::nullopt);

    QCOMPARE(storage->listSurveyRecords().count(), 1);
}

void SqliteStorageTest::testSaveSurveyRecord()
{
    storage->addSurveyRecord(
        Survey("1", "1"), "1", "", "", std::nullopt, std::nullopt);
    auto modifiedRecord = storage->listSurveyRecords().first();
    modifiedRecord.delegatePublicKey = "2";
    modifiedRecord.groupSize = 1337;
    storage->saveSurveyRecord(modifiedRecord);
    const auto retrievedRecord = storage->listSurveyRecords().first();
    QCOMPARE(retrievedRecord.getState(), modifiedRecord.getState());
    QCOMPARE(
        retrievedRecord.delegatePublicKey, modifiedRecord.delegatePublicKey);
    QCOMPARE(retrievedRecord.groupSize, modifiedRecord.groupSize);
}

void SqliteStorageTest::testAddSurveyWorksWithValuesPresent()
{
    Survey testSurvey("123", "test");
    testSurvey.queries.append(QSharedPointer<Query>::create(
        "12345", "testDataKey", QList<QString> { "1", "2", "3" }, true));
    storage->addSurveyRecord(
        testSurvey, "", "", "", std::nullopt, std::nullopt);

    auto retrievedSurvey = storage->findSurveyRecordById(testSurvey.id);
    QVERIFY(!retrievedSurvey.isNull());

    auto retrievedSurveyValue = retrievedSurvey->survey;
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

void SqliteStorageTest::testAddSurveyWorksWithReturningNullWhenNotFound()
{
    QVERIFY(storage->findSurveyRecordById("123").isNull());
}

QTEST_MAIN(SqliteStorageTest)
