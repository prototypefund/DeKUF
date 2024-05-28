#include <QTest>

#include "../core/survey_test.hpp"

#include "core/survey.hpp"

void SurveyTest::testListFromByteArrayForEmptyArray()
{
    const auto data = QString("[]").toUtf8();
    const auto surveys = Survey::listFromByteArray(data);
    QCOMPARE(surveys.count(), 0);
}

void SurveyTest::testListFromByteArrayForSingleSurvey()
{
    const auto data = QString(R"([{"id": "1234" ,"name": "test"}])").toUtf8();
    const auto surveys = Survey::listFromByteArray(data);
    QCOMPARE(surveys.count(), 1);
    QCOMPARE(surveys.first()->id, "1234");
    QCOMPARE(surveys.first()->name, "test");
}

void SurveyTest::testFromByteArrayForSingleSurveyWithQuery()
{
    const auto data = QString("{\"id\": \"1234\", \"name\": \"test\", "
                              "\"queries\": [{\"id\": \"1\", "
                              "\"data_key\": \"timestamp\", \"cohorts\": "
                              "[\"1\", \"2\"], \"discrete\": true}]}")
                          .toUtf8();
    const auto survey = Survey::fromByteArray(data);

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
    const auto surveys = Survey::listFromByteArray(data);

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

    const auto reimportedSurvey = Survey::fromByteArray(survey.toByteArray());
    const auto reimportedQuery = reimportedSurvey->queries.first();

    QCOMPARE(survey.id, reimportedSurvey->id);
    QCOMPARE(survey.name, reimportedSurvey->name);

    QCOMPARE(query->id, reimportedQuery->id);
    QCOMPARE(query->dataKey, reimportedQuery->dataKey);
    QCOMPARE(query->cohorts, reimportedQuery->cohorts);
    QCOMPARE(query->discrete, reimportedQuery->discrete);
}

QTEST_MAIN(SurveyTest)
