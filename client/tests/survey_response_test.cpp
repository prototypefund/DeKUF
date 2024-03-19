#include <QTest>

#include "../src/survey_response.hpp"

#include "survey_response_test.hpp"

QJsonArray ReadQueriesFromSurveyJsonObject(QJsonObject jsonSurvey)
{
    auto jsonQueriesValue = jsonSurvey["queries"];
    return jsonQueriesValue.toArray();
}

void SurveyResponseTest::testToByteArrayForEmptyResponse()
{
    SurveyResponse surveyResponse;

    auto jsonSurvey
        = QJsonDocument::fromJson(surveyResponse.toJsonByteArray()).object();

    QCOMPARE(ReadQueriesFromSurveyJsonObject(jsonSurvey).count(), 0);
}

void SurveyResponseTest::testToByteArrayForResponse()
{
    SurveyResponse surveyResponse;

    surveyResponse.queryResponses.append(
        QSharedPointer<QueryResponse>::create("timestamp", "123456789"));

    auto jsonSurvey
        = QJsonDocument::fromJson(surveyResponse.toJsonByteArray()).object();

    auto queryResponseJsonObject = ReadQueriesFromSurveyJsonObject(jsonSurvey);

    QCOMPARE(
        queryResponseJsonObject.first()["dataKey"].toString(), "timestamp");
    QCOMPARE(queryResponseJsonObject.first()["data"].toString(), "123456789");
}

void SurveyResponseTest::testCreateSurveyResponse()
{
    SurveyResponse surveyResponse;

    surveyResponse.queryResponses.append(
        QSharedPointer<QueryResponse>::create("timestamp", "123456789"));

    auto jsonSurvey
        = QJsonDocument::fromJson(surveyResponse.toJsonByteArray()).object();

    auto queryResponseJsonObject = ReadQueriesFromSurveyJsonObject(jsonSurvey);

    QCOMPARE(
        queryResponseJsonObject.first()["dataKey"].toString(), "timestamp");
    QCOMPARE(queryResponseJsonObject.first()["data"].toString(), "123456789");
}

QTEST_MAIN(SurveyResponseTest)
