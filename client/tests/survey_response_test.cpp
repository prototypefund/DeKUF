#include <QTest>

#include "../src/survey_response.hpp"

#include "stubs/storage_stub.hpp"
#include "survey_response_test.hpp"

QJsonArray readQueriesFromSurveyJsonObject(QJsonObject jsonSurvey)
{
    auto jsonQueriesValue = jsonSurvey["queryResponses"];
    return jsonQueriesValue.toArray();
}

void SurveyResponseTest::testToByteArrayForEmptyResponse()
{
    SurveyResponse surveyResponse;

    auto jsonSurvey
        = QJsonDocument::fromJson(surveyResponse.toJsonByteArray()).object();

    QCOMPARE(readQueriesFromSurveyJsonObject(jsonSurvey).count(), 0);
}

void SurveyResponseTest::testToByteArrayForResponse()
{
    SurveyResponse surveyResponse;

    surveyResponse.queryResponses.append(
        QSharedPointer<QueryResponse>::create("timestamp", "123456789"));

    auto jsonSurvey
        = QJsonDocument::fromJson(surveyResponse.toJsonByteArray()).object();
    auto queryResponseJsonObject = readQueriesFromSurveyJsonObject(jsonSurvey);

    QCOMPARE(
        queryResponseJsonObject.first()["dataKey"].toString(), "timestamp");
    QCOMPARE(queryResponseJsonObject.first()["data"].toString(), "123456789");
}

void SurveyResponseTest::testToAndFromByteArray()
{
    SurveyResponse response;
    response.commissioners.append(QSharedPointer<Commissioner>::create("Foo"));
    response.queryResponses.append(
        QSharedPointer<QueryResponse>::create("timestamp", "123456789"));
    auto json = response.toJsonByteArray();
    auto deserialized = SurveyResponse::fromJsonByteArray(json);
    QCOMPARE(response.commissioners.count(), 1);
    QCOMPARE(
        *response.commissioners.first(), *deserialized->commissioners.first());
    QCOMPARE(response.queryResponses.count(), 1);
    QCOMPARE(*response.queryResponses.first(),
        *deserialized->queryResponses.first());
}

void SurveyResponseTest::testCreateSurveyResponseSucceedsForRightCommissioners()
{
    StorageStub storage;

    Survey survey("testId", "testName");
    survey.queries.append(QSharedPointer<Query>::create("testKey"));
    survey.commissioners.append(QSharedPointer<Commissioner>::create("KDE"));
    storage.addDataPoint("testKey", "testValue");

    auto surveyResponse = SurveyResponse::create(
        QSharedPointer<Survey>::create(survey), storage);

    QVERIFY(!surveyResponse.isNull());
    QCOMPARE(surveyResponse->queryResponses.first()->dataKey, "testKey");
    QCOMPARE(surveyResponse->queryResponses.first()->data, "testValue");
    QCOMPARE(surveyResponse->commissioners.first()->name, "KDE");
}

void SurveyResponseTest::testNoSurveyResponseCreatedForWrongCommissioners()
{
    StorageStub storage;

    Survey survey("testId", "testName");
    survey.queries.append(QSharedPointer<Query>::create("testKey"));
    survey.commissioners.append(QSharedPointer<Commissioner>::create("Wrong"));
    storage.addDataPoint("testKey", "testValue");

    auto surveyResponse = SurveyResponse::create(
        QSharedPointer<Survey>::create(survey), storage);

    QVERIFY(surveyResponse.isNull());
}

QTEST_MAIN(SurveyResponseTest)
