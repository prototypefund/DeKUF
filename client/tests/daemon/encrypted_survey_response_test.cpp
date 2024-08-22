#include <QTest>

#include <core/encrypted_survey_response.hpp>

#include "../stubs/daemon/homomorphic_encryptor_stub.hpp"
#include "encrypted_survey_response_test.hpp"

QJsonArray readQueriesFromSurveyJsonObject(QJsonObject jsonSurvey)
{
    const auto jsonQueriesValue = jsonSurvey["query_responses"];
    return jsonQueriesValue.toArray();
}

void EncryptedSurveyResponseTest::testToByteArrayForEmptyResponse()
{
    EncryptedSurveyResponse encryptedSurveyResponse("1");

    const auto jsonEncryptedSurveyResponse
        = QJsonDocument::fromJson(encryptedSurveyResponse.toJsonByteArray())
              .object();

    QCOMPARE(
        readQueriesFromSurveyJsonObject(jsonEncryptedSurveyResponse).count(),
        0);
}

void EncryptedSurveyResponseTest::testToByteArrayForResponse()
{
    EncryptedSurveyResponse encryptedSurveyResponse("1");

    QMap<QString, mpz_class> cohortTestData = { { "8", mpz_class(1) },
        { "16", mpz_class(0) }, { "32", mpz_class(0) } };

    QJsonObject cohortTestDataJsonObject;
    for (auto it = cohortTestData.constBegin(); it != cohortTestData.constEnd();
         ++it) {
        cohortTestDataJsonObject.insert(
            it.key(), QJsonValue(QString::fromStdString(it.value().get_str())));
    }

    encryptedSurveyResponse.encryptedQueryResponses.append(
        QSharedPointer<EncryptedQueryResponse>::create("1", cohortTestData));

    const auto jsonSurvey
        = QJsonDocument::fromJson(encryptedSurveyResponse.toJsonByteArray())
              .object();
    const auto encryptedQueryResponseJsonObject
        = readQueriesFromSurveyJsonObject(jsonSurvey);

    QCOMPARE(
        encryptedQueryResponseJsonObject.first()["query_id"].toString(), "1");
    QCOMPARE(encryptedQueryResponseJsonObject.first()["data"],
        QJsonValue(cohortTestDataJsonObject));
}

void EncryptedSurveyResponseTest::testToAndFromByteArray()
{
    EncryptedSurveyResponse response("1");

    QMap<QString, mpz_class> cohortTestData = { { "8", mpz_class(1) },
        { "16", mpz_class(0) }, { "32", mpz_class(0) } };

    response.encryptedQueryResponses.append(
        QSharedPointer<EncryptedQueryResponse>::create(
            "timestamp", cohortTestData));
    const auto json = response.toJsonByteArray();
    const auto deserializedResult
        = EncryptedSurveyResponse::fromJsonByteArray(json);
    Q_ASSERT(deserializedResult.isSuccess());
    QCOMPARE(response.encryptedQueryResponses.count(), 1);
    QCOMPARE(*response.encryptedQueryResponses.first(),
        *deserializedResult.getValue()->encryptedQueryResponses.first());
}

void EncryptedSurveyResponseTest::testAggregationWithOneQuery()
{
    auto response = QSharedPointer<EncryptedSurveyResponse>::create("1");

    QMap<QString, mpz_class> cohortTestData = { { "8", mpz_class(1) },
        { "16", mpz_class(1) }, { "32", mpz_class(0) } };

    response->encryptedQueryResponses.append(
        QSharedPointer<EncryptedQueryResponse>::create("test", cohortTestData));

    auto response2 = QSharedPointer<EncryptedSurveyResponse>::create("1");

    QMap<QString, mpz_class> cohortTestData2 = { { "8", mpz_class(1) },
        { "16", mpz_class(1) }, { "32", mpz_class(0) } };

    response2->encryptedQueryResponses.append(
        QSharedPointer<EncryptedQueryResponse>::create(
            "test", cohortTestData2));

    const QList<QSharedPointer<EncryptedSurveyResponse>> responses { response,
        response2 };

    auto aggregatedResult
        = EncryptedSurveyResponse::aggregateEncryptedSurveyResponses(responses,
            QSharedPointer<HomomorphicEncryptorStub>::create(
                HomomorphicEncryptorStub()));

    QMap<QString, mpz_class> aggregatedCohortData = { { "8", mpz_class(2) },
        { "16", mpz_class(2) }, { "32", mpz_class(0) } };

    Q_ASSERT(aggregatedResult.isSuccess() && aggregatedResult.hasValue());

    QCOMPARE(aggregatedResult.getValue()
                 ->encryptedQueryResponses.first()
                 ->cohortData,
        aggregatedCohortData);
}

void EncryptedSurveyResponseTest::testAggregationWithMultipleQueries()
{
    auto response = QSharedPointer<EncryptedSurveyResponse>::create("1");

    QMap<QString, mpz_class> cohortTestData = { { "8", mpz_class(1) },
        { "16", mpz_class(1) }, { "32", mpz_class(0) } };

    response->encryptedQueryResponses.append(
        QSharedPointer<EncryptedQueryResponse>::create("test", cohortTestData));

    response->encryptedQueryResponses.append(
        QSharedPointer<EncryptedQueryResponse>::create(
            "test2", cohortTestData));

    auto response2 = QSharedPointer<EncryptedSurveyResponse>::create("1");

    QMap<QString, mpz_class> cohortTestData2 = { { "8", mpz_class(0) },
        { "16", mpz_class(1) }, { "32", mpz_class(0) } };

    response2->encryptedQueryResponses.append(
        QSharedPointer<EncryptedQueryResponse>::create(
            "test", cohortTestData2));

    response->encryptedQueryResponses.append(
        QSharedPointer<EncryptedQueryResponse>::create(
            "test2", cohortTestData2));

    const QList<QSharedPointer<EncryptedSurveyResponse>> responses { response,
        response2 };

    auto aggregatedResult
        = EncryptedSurveyResponse::aggregateEncryptedSurveyResponses(responses,
            QSharedPointer<HomomorphicEncryptorStub>::create(
                HomomorphicEncryptorStub()));

    QMap<QString, mpz_class> aggregatedCohortData = { { "8", mpz_class(1) },
        { "16", mpz_class(2) }, { "32", mpz_class(0) } };

    Q_ASSERT(aggregatedResult.isSuccess() && aggregatedResult.hasValue());

    QCOMPARE(aggregatedResult.getValue()
                 ->encryptedQueryResponses.first()
                 ->cohortData,
        aggregatedCohortData);

    QCOMPARE(
        aggregatedResult.getValue()->encryptedQueryResponses.last()->cohortData,
        aggregatedCohortData);
}

void EncryptedSurveyResponseTest::
    testAggregationReturnsFailureWhenSurveyIdDiffers()
{
    auto response = QSharedPointer<EncryptedSurveyResponse>::create("1");

    QMap<QString, mpz_class> cohortTestData = { { "8", mpz_class(1) },
        { "16", mpz_class(1) }, { "32", mpz_class(0) } };

    response->encryptedQueryResponses.append(
        QSharedPointer<EncryptedQueryResponse>::create("test", cohortTestData));

    auto response2 = QSharedPointer<EncryptedSurveyResponse>::create("not1");

    QMap<QString, mpz_class> cohortTestData2 = { { "8", mpz_class(0) },
        { "16", mpz_class(1) }, { "32", mpz_class(0) } };

    response2->encryptedQueryResponses.append(
        QSharedPointer<EncryptedQueryResponse>::create(
            "test", cohortTestData2));

    const QList<QSharedPointer<EncryptedSurveyResponse>> responses { response,
        response2 };

    auto aggregationResult
        = EncryptedSurveyResponse::aggregateEncryptedSurveyResponses(responses,
            QSharedPointer<HomomorphicEncryptorStub>::create(
                HomomorphicEncryptorStub()));

    Q_ASSERT(!aggregationResult.success && !aggregationResult.hasValue());
    qDebug() << aggregationResult.errorMessage;
    Q_ASSERT(aggregationResult.errorMessage
        == "EncryptedSurveyResponses need to reference same Survey");
}

QTEST_MAIN(EncryptedSurveyResponseTest)
