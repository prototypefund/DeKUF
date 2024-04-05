#include <QTest>

#include "../../src/daemon/survey.hpp"

#include "survey_test.hpp"

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

void SurveyTest::testListFromByteArrayForSingleSurveyWithQuery()
{
    const auto data = QString("[{\"id\": \"1234\", \"name\": \"test\", "
                              "\"queries\": [{\"dataKey\": \"timestamp\"}]}]")
                          .toUtf8();
    const auto surveys = Survey::listFromByteArray(data);

    QCOMPARE(surveys.count(), 1);
    const auto survey = surveys.first();

    QCOMPARE(survey->id, "1234");
    QCOMPARE(survey->name, "test");
    QCOMPARE(survey->queries.count(), 1);
    QCOMPARE(survey->queries.first()->dataKey, "timestamp");
}

QTEST_MAIN(SurveyTest)
