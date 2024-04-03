#include <QTest>

#include "../src/sqlite_storage.hpp"

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
    auto dataPoints = storage->listDataPoints("timestamp");
    QCOMPARE(dataPoints.count(), 0);
}

void SqliteStorageTest::testAddAndListDataPoints()
{
    storage->addDataPoint("timestamp", "1337");
    auto dataPoints = storage->listDataPoints("timestamp");
    QCOMPARE(dataPoints.count(), 1);
    auto first = dataPoints.first();
    QCOMPARE(first.key, "timestamp");
    QCOMPARE(first.value, "1337");
    QCOMPARE(first.createdAt.date(), QDate::currentDate());
}

QTEST_MAIN(SqliteStorageTest)
