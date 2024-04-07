#include <QTest>

#include <core/survey_response.hpp>
#include <daemon/client.hpp>

#include "../stubs/core/storage_stub.hpp"

#include "client_test.hpp"

void ClientTest::testCreateSurveyResponseSucceedsForRightCommissioners()
{
    auto storage = QSharedPointer<StorageStub>::create();

    Survey survey("testId", "testName");
    survey.queries.append(QSharedPointer<Query>::create("testKey"));
    survey.commissioners.append(QSharedPointer<Commissioner>::create("KDE"));
    storage->addDataPoint("testKey", "testValue");

    Client client(nullptr, storage);
    const auto surveyResponse = client.createSurveyResponse(survey);

    QVERIFY(!surveyResponse.isNull());
    QCOMPARE(surveyResponse->queryResponses.first()->dataKey, "testKey");
    QCOMPARE(surveyResponse->queryResponses.first()->data, "testValue");
    QCOMPARE(surveyResponse->commissioners.first()->name, "KDE");
}

void ClientTest::testCreateSurveyResponseNullForWrongCommissioners()
{
    auto storage = QSharedPointer<StorageStub>::create();

    Survey survey("testId", "testName");
    survey.queries.append(QSharedPointer<Query>::create("testKey"));
    survey.commissioners.append(QSharedPointer<Commissioner>::create("Wrong"));
    storage->addDataPoint("testKey", "testValue");

    Client client(nullptr, storage);
    const auto surveyResponse = client.createSurveyResponse(survey);

    QVERIFY(surveyResponse.isNull());
}

QTEST_MAIN(ClientTest)
