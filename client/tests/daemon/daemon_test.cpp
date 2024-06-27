#include <QTest>

#include <core/survey_response.hpp>
#include <daemon/daemon.hpp>

#include "../stubs/core/storage_stub.hpp"
#include "../stubs/daemon/network_stub.hpp"

#include "daemon_test.hpp"

void DaemonTest::testProcessSurveysIgnoresErrors()
{
    auto storage = QSharedPointer<StorageStub>::create();
    Daemon daemon(nullptr, storage, QSharedPointer<NetworkStub>::create());
    auto future = daemon.processSurveys();
    future.waitForFinished();
    QCOMPARE(storage->listSurveySignups().count(), 0);
}

void DaemonTest::testProcessSurveysSignsUpForRightCommissioner()
{
    auto storage = QSharedPointer<StorageStub>::create();
    auto network = QSharedPointer<NetworkStub>::create();
    Daemon daemon(nullptr, storage, network);

    Survey survey("testId", "testName");
    survey.commissioner = QSharedPointer<Commissioner>::create("KDE");
    network->listSurveysResponse
        = QByteArray("[\n" + survey.toByteArray() + "\n]");

    auto future = daemon.processSurveys();
    future.waitForFinished();
    auto first = storage->listSurveySignups().first();
    QCOMPARE(first.survey->id, survey.id);
}

void DaemonTest::testProcessSurveyDoesNotSignUpForWrongCommissioner()
{
    auto storage = QSharedPointer<StorageStub>::create();
    auto network = QSharedPointer<NetworkStub>::create();
    Daemon daemon(nullptr, storage, network);

    Survey survey("testId", "testName");
    survey.commissioner = QSharedPointer<Commissioner>::create("Wrong");
    network->listSurveysResponse
        = QByteArray("[\n" + survey.toByteArray() + "\n]");

    auto future = daemon.processSurveys();
    future.waitForFinished();
    QCOMPARE(storage->listSurveySignups().count(), 0);
}

void DaemonTest::testCreateSurveyResponseSucceedsForIntervals()
{
    auto storage = QSharedPointer<StorageStub>::create();

    Survey survey("testId", "testName");
    QList<QString> cohorts = { "[8, 16)", "[16, 32)", "[32, 64)" };
    survey.queries.append(
        QSharedPointer<Query>::create("1", "testKey", cohorts, false));
    survey.commissioner = QSharedPointer<Commissioner>::create("KDE");
    storage->addDataPoint("testKey", "8");
    storage->addDataPoint("testKey", "16");
    storage->addDataPoint("testKey", "31");

    Daemon daemon(nullptr, storage, QSharedPointer<NetworkStub>::create());
    const auto surveyResponse = daemon.createSurveyResponse(survey);

    QMap<QString, int> testCohortData
        = { { "[8, 16)", 1 }, { "[16, 32)", 2 }, { "[32, 64)", 0 } };

    QVERIFY(!surveyResponse.isNull());
    QCOMPARE(surveyResponse->queryResponses.first()->queryId, "1");
    QCOMPARE(
        surveyResponse->queryResponses.first()->cohortData, testCohortData);
}

void DaemonTest::testCreateSurveyResponseSucceedsForIntervalsWithInfinity()
{
    auto storage = QSharedPointer<StorageStub>::create();

    Survey survey("testId", "testName");
    QList<QString> cohorts = { "(-inf, 16)", "[16, 32)", "[32, inf)" };
    survey.queries.append(
        QSharedPointer<Query>::create("1", "testKey", cohorts, false));
    survey.commissioner = QSharedPointer<Commissioner>::create("KDE");
    storage->addDataPoint("testKey", "8");
    storage->addDataPoint("testKey", "31");
    storage->addDataPoint("testKey", "32");
    storage->addDataPoint("testKey", "1000");
    storage->addDataPoint("testKey", "10000.23");

    Daemon daemon(nullptr, storage, QSharedPointer<NetworkStub>::create());
    const auto surveyResponse = daemon.createSurveyResponse(survey);

    QMap<QString, int> testCohortData
        = { { "(-inf, 16)", 1 }, { "[16, 32)", 1 }, { "[32, inf)", 3 } };

    QVERIFY(!surveyResponse.isNull());
    QCOMPARE(surveyResponse->queryResponses.first()->queryId, "1");
    QCOMPARE(
        surveyResponse->queryResponses.first()->cohortData, testCohortData);
}

QTEST_MAIN(DaemonTest)
