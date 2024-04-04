#include <QTest>

#include "../src/sqlite_storage.hpp"
#include "../src/survey_response.hpp"

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
    const auto dataPoints = storage->listDataPoints("timestamp");
    QCOMPARE(dataPoints.count(), 0);
}

void SqliteStorageTest::testAddAndListDataPoints()
{
    storage->addDataPoint("timestamp", "1337");
    const auto dataPoints = storage->listDataPoints("timestamp");
    QCOMPARE(dataPoints.count(), 1);
    const auto first = dataPoints.first();
    QCOMPARE(first.key, "timestamp");
    QCOMPARE(first.value, "1337");
    QCOMPARE(first.createdAt.date(), QDate::currentDate());
}

void SqliteStorageTest::testAddAndListSurveyResponses()
{
    QCOMPARE(storage->listSurveyResponses().count(), 0);

    auto survey = QSharedPointer<Survey>::create("id", "name");
    survey->commissioners.append(QSharedPointer<Commissioner>::create("KDE"));
    auto response = SurveyResponse::create(survey, *storage);
    storage->addSurveyResponse(*response);

    QCOMPARE(storage->listSurveyResponses().count(), 1);
}

QTEST_MAIN(SqliteStorageTest)
