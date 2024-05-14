#include <QTest>

#include <core/survey_response.hpp>

#include "core/commissioner.hpp"
#include "survey_response_test.hpp"

QJsonArray readQueriesFromSurveyJsonObject(QJsonObject jsonSurvey)
{
    const auto jsonQueriesValue = jsonSurvey["query_responses"];
    return jsonQueriesValue.toArray();
}

void SurveyResponseTest::testToByteArrayForEmptyResponse()
{
    SurveyResponse surveyResponse;
    surveyResponse.commissioner = QSharedPointer<Commissioner>::create("KDE");

    const auto jsonSurveyResponse
        = QJsonDocument::fromJson(surveyResponse.toJsonByteArray()).object();

    QCOMPARE(readQueriesFromSurveyJsonObject(jsonSurveyResponse).count(), 0);
}

void SurveyResponseTest::testToByteArrayForResponse()
{
    SurveyResponse surveyResponse;
    surveyResponse.commissioner = QSharedPointer<Commissioner>::create("KDE");

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
    SurveyResponse response;
    response.commissioner = QSharedPointer<Commissioner>::create("KDE");

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

QTEST_MAIN(SurveyResponseTest)
