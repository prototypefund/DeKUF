#include <QTest>

#include <core/sqlite_storage.hpp>
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

    SurveyResponse response;
    response.commissioner = QSharedPointer<Commissioner>::create("Foo");
    storage->addSurveyResponse(response);

    QCOMPARE(storage->listSurveyResponses().count(), 1);
}

QTEST_MAIN(SqliteStorageTest)
