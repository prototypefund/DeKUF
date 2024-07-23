#include <QTest>

#include <core/survey_response.hpp>

#include "survey_response_test.hpp"

QJsonArray readQueriesFromSurveyJsonObject(QJsonObject jsonSurvey)
{
    const auto jsonQueriesValue = jsonSurvey["query_responses"];
    return jsonQueriesValue.toArray();
}

void SurveyResponseTest::testToByteArrayForEmptyResponse()
{
    SurveyResponse surveyResponse("1");

    const auto jsonSurveyResponse
        = QJsonDocument::fromJson(surveyResponse.toJsonByteArray()).object();

    QCOMPARE(readQueriesFromSurveyJsonObject(jsonSurveyResponse).count(), 0);
}

void SurveyResponseTest::testToByteArrayForResponse()
{
    SurveyResponse surveyResponse("1");

    QMap<QString, int> cohortTestData
        = { { "8", 1 }, { "16", 0 }, { "32", 0 } };

    QJsonObject cohortTestDataJsonObject;
    for (auto it = cohortTestData.constBegin(); it != cohortTestData.constEnd();
         ++it) {
        cohortTestDataJsonObject.insert(it.key(), QJsonValue(it.value()));
    }

    surveyResponse.queryResponses.append(
        QSharedPointer<QueryResponse>::create("1", cohortTestData));

    const auto jsonSurvey
        = QJsonDocument::fromJson(surveyResponse.toJsonByteArray()).object();
    const auto queryResponseJsonObject
        = readQueriesFromSurveyJsonObject(jsonSurvey);

    QCOMPARE(queryResponseJsonObject.first()["query_id"].toString(), "1");
    QCOMPARE(queryResponseJsonObject.first()["data"],
        QJsonValue(cohortTestDataJsonObject));
}

void SurveyResponseTest::testToAndFromByteArray()
{
    SurveyResponse response("1");

    QMap<QString, int> cohortTestData
        = { { "8", 1 }, { "16", 0 }, { "32", 0 } };

    response.queryResponses.append(
        QSharedPointer<QueryResponse>::create("timestamp", cohortTestData));
    const auto json = response.toJsonByteArray();
    const auto deserialized = SurveyResponse::fromJsonByteArray(json);
    QCOMPARE(response.queryResponses.count(), 1);
    QCOMPARE(*response.queryResponses.first(),
        *deserialized->queryResponses.first());
}

void SurveyResponseTest::testAggregationWithOneQuery()
{
    auto response = QSharedPointer<SurveyResponse>::create("1");

    QMap<QString, int> cohortTestData
        = { { "8", 1 }, { "16", 0 }, { "32", 0 } };

    response->queryResponses.append(
        QSharedPointer<QueryResponse>::create("test", cohortTestData));

    auto response2 = QSharedPointer<SurveyResponse>::create("1");

    QMap<QString, int> cohortTestData2
        = { { "8", 0 }, { "16", 1 }, { "32", 0 } };

    response2->queryResponses.append(
        QSharedPointer<QueryResponse>::create("test", cohortTestData2));

    const QList<QSharedPointer<SurveyResponse>> responses { response,
        response2 };

    auto aggregatedResponse
        = SurveyResponse::aggregateSurveyResponses(responses);

    QMap<QString, int> aggregatedCohortData
        = { { "8", 1 }, { "16", 1 }, { "32", 0 } };

    QCOMPARE(aggregatedResponse->queryResponses.first()->cohortData,
        aggregatedCohortData);
}

void SurveyResponseTest::testAggregationWithMultipleQueries()
{
    auto response = QSharedPointer<SurveyResponse>::create("1");

    QMap<QString, int> cohortTestData
        = { { "8", 1 }, { "16", 1 }, { "32", 0 } };

    response->queryResponses.append(
        QSharedPointer<QueryResponse>::create("test", cohortTestData));

    response->queryResponses.append(
        QSharedPointer<QueryResponse>::create("test2", cohortTestData));

    auto response2 = QSharedPointer<SurveyResponse>::create("1");

    QMap<QString, int> cohortTestData2
        = { { "8", 0 }, { "16", 1 }, { "32", 0 } };

    response2->queryResponses.append(
        QSharedPointer<QueryResponse>::create("test", cohortTestData2));

    response->queryResponses.append(
        QSharedPointer<QueryResponse>::create("test2", cohortTestData2));

    const QList<QSharedPointer<SurveyResponse>> responses { response,
        response2 };

    auto aggregatedResponse
        = SurveyResponse::aggregateSurveyResponses(responses);

    QMap<QString, int> aggregatedCohortData
        = { { "8", 1 }, { "16", 2 }, { "32", 0 } };

    QCOMPARE(aggregatedResponse->queryResponses.first()->cohortData,
        aggregatedCohortData);

    QCOMPARE(aggregatedResponse->queryResponses.last()->cohortData,
        aggregatedCohortData);
}

void SurveyResponseTest::
    testAggregationThrowsRuntimeExceptionWhenSurveyIdDiffers()
{
    auto response = QSharedPointer<SurveyResponse>::create("1");

    QMap<QString, int> cohortTestData
        = { { "8", 1 }, { "16", 0 }, { "32", 0 } };

    response->queryResponses.append(
        QSharedPointer<QueryResponse>::create("test", cohortTestData));

    auto response2 = QSharedPointer<SurveyResponse>::create("not1");

    QMap<QString, int> cohortTestData2
        = { { "8", 0 }, { "16", 1 }, { "32", 0 } };

    response2->queryResponses.append(
        QSharedPointer<QueryResponse>::create("test", cohortTestData2));

    const QList<QSharedPointer<SurveyResponse>> responses { response,
        response2 };

    QVERIFY_THROWS_EXCEPTION(std::runtime_error,
        SurveyResponse::aggregateSurveyResponses(responses));
}

QTEST_MAIN(SurveyResponseTest)
