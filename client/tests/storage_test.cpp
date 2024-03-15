#include <QTest>

#include "../src/storage.hpp"

#include "storage_test.hpp"

#define TEST_DATA_POINT "timestamp"

void StorageTest::init()
{
    QFile dbFile(STORAGE_DATABASE_PATH);
    dbFile.remove();
}

void StorageTest::testListDataPointsInitiallyEmpty()
{
    Storage storage;
    auto dataPoints = storage.listDataPoints("timestamp");
    QCOMPARE(dataPoints.count(), 0);
}

void StorageTest::testAddAndListDataPoints()
{
    Storage storage;
    storage.addDataPoint("timestamp", "1337");
    auto dataPoints = storage.listDataPoints("timestamp");
    QCOMPARE(dataPoints.count(), 1);
    QCOMPARE(dataPoints.first(), "1337");
}

QTEST_MAIN(StorageTest)
