#include <QTest>

#include "../src/survey.hpp"

#include "survey_test.hpp"

void SurveyTest::testListFromByteArrayForEmptyArray()
{
    auto data = QString("[]").toUtf8();
    auto surveys = Survey::listFromByteArray(data);
    QCOMPARE(surveys.count(), 0);
}

void SurveyTest::testListFromByteArrayForSingleSurvey()
{
    auto data = QString("[{\"id\": \"1234\" ,\"name\": \"test\"}]").toUtf8();
    auto surveys = Survey::listFromByteArray(data);
    QCOMPARE(surveys.count(), 1);
    QCOMPARE(surveys.first()->id, "1234");
    QCOMPARE(surveys.first()->name, "test");
}
