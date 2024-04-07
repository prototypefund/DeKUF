#include <QTest>

#include <core/survey_response.hpp>

#include "survey_response_test.hpp"

QJsonArray readQueriesFromSurveyJsonObject(QJsonObject jsonSurvey)
{
    const auto jsonQueriesValue = jsonSurvey["queryResponses"];
    return jsonQueriesValue.toArray();
}

void SurveyResponseTest::testToByteArrayForEmptyResponse()
{
    SurveyResponse surveyResponse;

    const auto jsonSurvey
        = QJsonDocument::fromJson(surveyResponse.toJsonByteArray()).object();

    QCOMPARE(readQueriesFromSurveyJsonObject(jsonSurvey).count(), 0);
}

void SurveyResponseTest::testToByteArrayForResponse()
{
    SurveyResponse surveyResponse;

    surveyResponse.queryResponses.append(
        QSharedPointer<QueryResponse>::create("timestamp", "123456789"));

    const auto jsonSurvey
        = QJsonDocument::fromJson(surveyResponse.toJsonByteArray()).object();
    const auto queryResponseJsonObject
        = readQueriesFromSurveyJsonObject(jsonSurvey);

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
    const auto json = response.toJsonByteArray();
    const auto deserialized = SurveyResponse::fromJsonByteArray(json);
    QCOMPARE(response.commissioners.count(), 1);
    QCOMPARE(
        *response.commissioners.first(), *deserialized->commissioners.first());
    QCOMPARE(response.queryResponses.count(), 1);
    QCOMPARE(*response.queryResponses.first(),
        *deserialized->queryResponses.first());
}

QTEST_MAIN(SurveyResponseTest)
