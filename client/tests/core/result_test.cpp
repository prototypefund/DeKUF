#include <QTest>

#include <core/result.hpp>

#include "result_test.hpp"

void ResultTest::testVoidResultSuccess()
{
    Result<void> result;
    QVERIFY(result.isSuccess());
    QVERIFY(result.getErrorMessage().isEmpty());
}

void ResultTest::testVoidResultFailure()
{
    QString errorMsg = "An error occurred";
    Result<void> result = Result<void>::Failure(errorMsg);
    QVERIFY(!result.isSuccess());
    QCOMPARE(result.getErrorMessage(), errorMsg);
}

void ResultTest::testIntResultSuccess()
{
    Result<int> result(42);
    QVERIFY(result.isSuccess());
    QVERIFY(result.getErrorMessage().isEmpty());
    QVERIFY(result.hasValue());
    QCOMPARE(result.getValue(), 42);
}

void ResultTest::testIntResultFailure()
{
    QString errorMsg = "An error occurred";
    Result<int> result = Result<int>::Failure(errorMsg);
    QVERIFY(!result.isSuccess());
    QCOMPARE(result.getErrorMessage(), errorMsg);
    QVERIFY(!result.hasValue());
}

void ResultTest::testQStringResultSuccess()
{
    Result<QString> result("Hello, World!");
    QVERIFY(result.isSuccess());
    QVERIFY(result.getErrorMessage().isEmpty());
    QVERIFY(result.hasValue());
    QCOMPARE(result.getValue(), QString("Hello, World!"));
}

void ResultTest::testQStringResultFailure()
{
    QString errorMsg = "An error occurred";
    Result<QString> result = Result<QString>::Failure(errorMsg);
    QVERIFY(!result.isSuccess());
    QCOMPARE(result.getErrorMessage(), errorMsg);
    QVERIFY(!result.hasValue());
}

void ResultTest::testCustomTypeResultSuccess()
{
    struct CustomType {
        int value;
    };
    CustomType ct { 123 };
    Result<CustomType> result(ct);
    QVERIFY(result.isSuccess());
    QVERIFY(result.getErrorMessage().isEmpty());
    QVERIFY(result.hasValue());
    QCOMPARE(result.getValue().value, 123);
}

void ResultTest::testCustomTypeResultFailure()
{
    struct CustomType {
        int value;
    };
    QString errorMsg = "An error occurred";
    Result<CustomType> result = Result<CustomType>::Failure(errorMsg);
    QVERIFY(!result.isSuccess());
    QCOMPARE(result.getErrorMessage(), errorMsg);
    QVERIFY(!result.hasValue());
}

QTEST_MAIN(ResultTest);