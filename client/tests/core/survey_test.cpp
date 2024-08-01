#include <QTest>

#include "../core/survey_test.hpp"

#include "core/survey.hpp"

void SurveyTest::testListFromByteArrayForEmptyArray()
{
    const auto data = QString("[]").toUtf8();
    const auto surveysParsingResult = Survey::listFromByteArray(data);
    Q_ASSERT(surveysParsingResult.isSuccess());
    QCOMPARE(surveysParsingResult.getValue().count(), 0);
}

void SurveyTest::testListFromByteArrayForSingleSurvey()
{
    const auto data = QString(R"([{"id": "1234" ,"name": "test"}])").toUtf8();
    const auto surveysParsingResult = Survey::listFromByteArray(data);
    Q_ASSERT(surveysParsingResult.isSuccess());
    QCOMPARE(surveysParsingResult.getValue().count(), 1);
    QCOMPARE(surveysParsingResult.getValue().first()->id, "1234");
    QCOMPARE(surveysParsingResult.getValue().first()->name, "test");
}

void SurveyTest::testFromByteArrayForSingleSurveyWithQuery()
{
    const auto data = QString("{\"id\": \"1234\", \"name\": \"test\", "
                              "\"queries\": [{\"id\": \"1\", "
                              "\"data_key\": \"timestamp\", \"cohorts\": "
                              "[\"1\", \"2\"], \"discrete\": true}]}")
                          .toUtf8();
    const auto surveyParsingResult = Survey::fromByteArray(data);
    Q_ASSERT(surveyParsingResult.isSuccess());
    const auto& survey = surveyParsingResult.getValue();

    QCOMPARE(survey->id, "1234");
    QCOMPARE(survey->name, "test");
    QCOMPARE(survey->queries.count(), 1);
    QCOMPARE(survey->queries.first()->id, "1");
    QCOMPARE(survey->queries.first()->discrete, true);
    QCOMPARE(survey->queries.first()->dataKey, "timestamp");
}

void SurveyTest::testListFromByteArrayForSingleSurveyWithQuery()
{
    const auto data = QString("[{\"id\": \"1234\", \"name\": \"test\", "
                              "\"queries\": [{\"id\": \"1\", "
                              "\"data_key\": \"timestamp\", \"cohorts\": "
                              "[\"1\", \"2\"], \"discrete\": true}]}]")
                          .toUtf8();
    const auto surveysParsingResult = Survey::listFromByteArray(data);
    Q_ASSERT(surveysParsingResult.isSuccess());
    const auto& surveys = surveysParsingResult.getValue();

    QCOMPARE(surveys.count(), 1);
    const auto survey = surveys.first();

    QCOMPARE(survey->id, "1234");
    QCOMPARE(survey->name, "test");
    QCOMPARE(survey->queries.count(), 1);
    QCOMPARE(survey->queries.first()->id, "1");
    QCOMPARE(survey->queries.first()->discrete, true);
    QCOMPARE(survey->queries.first()->dataKey, "timestamp");
}

void SurveyTest::testToByteArrayForSingleSurveyWithQuery()
{
    const QList<QString> cohorts = { "1", "2", "3" };
    Survey survey("1234", "test");
    const auto query
        = QSharedPointer<Query>::create("1111", "testKey", cohorts, true);
    survey.queries.append(query);

    const auto jsonSurvey
        = QJsonDocument::fromJson(survey.toByteArray()).object();

    QCOMPARE(jsonSurvey["id"], survey.id);
    QCOMPARE(jsonSurvey["name"], survey.name);

    const auto jsonQuery = jsonSurvey["queries"].toArray().first();

    QCOMPARE(jsonQuery["id"], query->id);
    QCOMPARE(jsonQuery["data_key"], query->dataKey);
    QCOMPARE(jsonQuery["discrete"], query->discrete);

    const auto jsonCohorts = jsonQuery["cohorts"].toArray();

    for (int i = 0; i < cohorts.length(); i++) {
        QCOMPARE(jsonCohorts[i], cohorts[i]);
    }
}

void SurveyTest::testToByteArrayAndBackWorks()
{
    const QList<QString> cohorts = { "1", "2", "3" };
    Survey survey("1234", "test");
    const auto query
        = QSharedPointer<Query>::create("1111", "testKey", cohorts, true);
    survey.queries.append(query);

    const auto reimportedSurveyResult
        = Survey::fromByteArray(survey.toByteArray());
    Q_ASSERT(reimportedSurveyResult.isSuccess());

    const auto& reimportedSurvey = reimportedSurveyResult.getValue();

    const auto reimportedQuery = reimportedSurvey->queries.first();

    QCOMPARE(survey.id, reimportedSurvey->id);
    QCOMPARE(survey.name, reimportedSurvey->name);

    QCOMPARE(query->id, reimportedQuery->id);
    QCOMPARE(query->dataKey, reimportedQuery->dataKey);
    QCOMPARE(query->cohorts, reimportedQuery->cohorts);
    QCOMPARE(query->discrete, reimportedQuery->discrete);
}

QTEST_MAIN(SurveyTest)
