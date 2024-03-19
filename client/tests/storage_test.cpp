#include <QTest>

#include "../src/sqlite_storage.hpp"

#include "storage_test.hpp"

void StorageTest::init()
{
    QFile dbFile(STORAGE_DATABASE_PATH);
    dbFile.remove();
}

void StorageTest::testListDataPointsInitiallyEmpty()
{
    SqliteStorage storage;
    auto dataPoints = storage.listDataPoints("timestamp");
    QCOMPARE(dataPoints.count(), 0);
}

void StorageTest::testAddAndListDataPoints()
{
    SqliteStorage storage;
    storage.addDataPoint("timestamp", "1337");
    auto dataPoints = storage.listDataPoints("timestamp");
    QCOMPARE(dataPoints.count(), 1);
    QCOMPARE(dataPoints.first(), "1337");
}

QTEST_MAIN(StorageTest)
