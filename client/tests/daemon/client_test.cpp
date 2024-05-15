#include <QTest>

#include <core/survey_response.hpp>
#include <daemon/client.hpp>

#include "../stubs/core/storage_stub.hpp"

#include "client_test.hpp"

void ClientTest::testCreateSurveyResponseSucceedsForRightCommissioner()
{
    auto storage = QSharedPointer<StorageStub>::create();

    Survey survey("testId", "testName");
    QList<QString> cohorts = { "8", "16", "32" };
    survey.queries.append(
        QSharedPointer<Query>::create("1", "testKey", cohorts, true));
    survey.commissioner = QSharedPointer<Commissioner>::create("KDE");
    storage->addDataPoint("testKey", "8");

    Client client(nullptr, storage);
    const auto surveyResponse = client.createSurveyResponse(survey);

    QMap<QString, int> testCohortData
        = { { "8", 1 }, { "16", 0 }, { "32", 0 } };

    QVERIFY(!surveyResponse.isNull());
    QCOMPARE(surveyResponse->queryResponses.first()->queryId, "1");
    QCOMPARE(
        surveyResponse->queryResponses.first()->cohortData, testCohortData);
}

void ClientTest::testCreateSurveyResponseSucceedsForIntervals()
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

    Client client(nullptr, storage);
    const auto surveyResponse = client.createSurveyResponse(survey);

    QMap<QString, int> testCohortData
        = { { "[8, 16)", 1 }, { "[16, 32)", 2 }, { "[32, 64)", 0 } };

    QVERIFY(!surveyResponse.isNull());
    QCOMPARE(surveyResponse->queryResponses.first()->queryId, "1");
    QCOMPARE(
        surveyResponse->queryResponses.first()->cohortData, testCohortData);
}

void ClientTest::testCreateSurveyResponseSucceedsForIntervalsWithInfinity()
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

    Client client(nullptr, storage);
    const auto surveyResponse = client.createSurveyResponse(survey);

    QMap<QString, int> testCohortData
        = { { "(-inf, 16)", 1 }, { "[16, 32)", 1 }, { "[32, inf)", 3 } };

    QVERIFY(!surveyResponse.isNull());
    QCOMPARE(surveyResponse->queryResponses.first()->queryId, "1");
    QCOMPARE(
        surveyResponse->queryResponses.first()->cohortData, testCohortData);
}

void ClientTest::testCreateSurveyResponseNullForWrongCommissioner()
{
    auto storage = QSharedPointer<StorageStub>::create();

    Survey survey("testId", "testName");
    QList<QString> cohorts = { "8", "16", "32" };
    survey.queries.append(
        QSharedPointer<Query>::create("1", "testKey", cohorts, true));
    survey.commissioner = QSharedPointer<Commissioner>::create("Wrong");
    storage->addDataPoint("testKey", "8");

    Client client(nullptr, storage);
    const auto surveyResponse = client.createSurveyResponse(survey);

    qPrintable(survey.name);

    QVERIFY(surveyResponse.isNull());
}

QTEST_MAIN(ClientTest)
