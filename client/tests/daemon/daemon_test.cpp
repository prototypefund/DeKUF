#include <QSignalSpy>
#include <QTest>

#include <core/survey_response.hpp>
#include <daemon/daemon.hpp>

#include "../stubs/core/storage_stub.hpp"
#include "../stubs/daemon/network_stub.hpp"

#include "daemon_test.hpp"

namespace {
template <typename T>
void await(const QFuture<T>& future, const int timeout = 250)
{
    QFutureWatcher<T> watcher;
    watcher.setFuture(future);
    QSignalSpy spy(&watcher, &QFutureWatcher<void>::finished);
    QVERIFY2(spy.wait(timeout), "Future never finished");
}
};

void DaemonTest::testProcessSurveysIgnoresErrors()
{
    auto storage = QSharedPointer<StorageStub>::create();
    Daemon daemon(nullptr, storage, QSharedPointer<NetworkStub>::create());
    await(daemon.processSurveys());
    QCOMPARE(storage->listSurveySignups().count(), 0);
}

void DaemonTest::testProcessSurveysSignsUpForRightCommissioner()
{
    QSKIP("Not testable unless we rewrite processSurvey to await all nested "
          "futures.");
    auto storage = QSharedPointer<StorageStub>::create();
    auto network = QSharedPointer<NetworkStub>::create();
    Daemon daemon(nullptr, storage, network);

    Survey survey("testId", "testName");
    survey.commissioner = QSharedPointer<Commissioner>::create("KDE");
    network->listSurveysResponse
        = QByteArray("[\n" + survey.toByteArray() + "\n]");

    await(daemon.processSurveys());
    auto signups = storage->listSurveySignups();
    QCOMPARE(signups.count(), 1);
    auto first = signups.first();
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

    await(daemon.processSurveys());
    QCOMPARE(storage->listSurveySignups().count(), 0);
}

void DaemonTest::testProcessSignupsIgnoresErrors()
{
    auto storage = QSharedPointer<StorageStub>::create();
    auto network = QSharedPointer<NetworkStub>::create();
    Daemon daemon(nullptr, storage, network);

    Survey survey("testId", "testName");
    storage->addSurveySignup(survey, "initial", "1337", "");

    await(daemon.processSignups());
    QCOMPARE(storage->listSurveySignups().first().state, "initial");
}

void DaemonTest::testProcessMessagesForDelegatesIgnoresErrors()
{
    auto storage = QSharedPointer<StorageStub>::create();
    auto network = QSharedPointer<NetworkStub>::create();
    Daemon daemon(nullptr, storage, network);

    Survey survey("testId", "testName");
    storage->addSurveySignup(survey, "processing", "1337", "1337");
    await(daemon.processMessagesForDelegates());
}

// TODO: Instead of testing createSurveyResponse directly, it'd be better to
//       rewrite the following test postAggregationResults.

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
