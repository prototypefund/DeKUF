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
    QCOMPARE(surveyResponse.survey->id, "1");
    QCOMPARE(surveyResponse.survey->commissioner->name, "testCommissioner");
    QCOMPARE(surveyResponse.survey->name, "testName");
}

void SqliteStorageTest::testAddAndListSurveySignups()
{
    QCOMPARE(storage->listSurveySignups().count(), 0);

    Survey survey("1", "testName");
    QString state("foo");
    QString clientId("bar");
    QString delegateId("");
    storage->addSurveySignup(survey, state, clientId, delegateId);

    QCOMPARE(storage->listSurveySignups().count(), 1);
}

void SqliteStorageTest::testListSurveySignupForState()
{
    storage->addSurveySignup(Survey("1", "1"), "foo", "1", "");
    storage->addSurveySignup(Survey("2", "2"), "bar", "1", "");
    QCOMPARE(storage->listSurveySignupsForState("foo").count(), 1);
}

void SqliteStorageTest::testSaveSurveySignup()
{
    storage->addSurveySignup(Survey("1", "1"), "foo", "1", "");
    auto signup = storage->listSurveySignups().first();
    signup.state = "bar";
    signup.delegateId = "2";
    storage->saveSurveySignup(signup);
    auto retrievedSignup = storage->listSurveySignups().first();
    QCOMPARE(retrievedSignup.state, signup.state);
    QCOMPARE(retrievedSignup.delegateId, signup.delegateId);
}

QTEST_MAIN(SqliteStorageTest)
