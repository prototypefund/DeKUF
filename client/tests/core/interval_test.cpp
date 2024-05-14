#include <QTest>

#include <core/interval.hpp>
#include <qglobal.h>
#include <qtestcase.h>

#include "interval_test.hpp"

void IntervalTest::testOpen()
{
    Interval interval("[0, 2]");
    QCOMPARE(interval.isInInterval(-1), false);
    QCOMPARE(interval.isInInterval(0), true);
    QCOMPARE(interval.isInInterval(1), true);
    QCOMPARE(interval.isInInterval(2), true);
    QCOMPARE(interval.isInInterval(3), false);
}

void IntervalTest::testClosed()
{
    Interval interval("(0, 2)");
    QCOMPARE(interval.isInInterval(-1), false);
    QCOMPARE(interval.isInInterval(0), false);
    QCOMPARE(interval.isInInterval(1), true);
    QCOMPARE(interval.isInInterval(2), false);
    QCOMPARE(interval.isInInterval(3), false);
}

void IntervalTest::testMixed()
{
    Interval first("[0, 2)");
    QCOMPARE(first.isInInterval(-1), false);
    QCOMPARE(first.isInInterval(0), true);
    QCOMPARE(first.isInInterval(1), true);
    QCOMPARE(first.isInInterval(2), false);
    QCOMPARE(first.isInInterval(3), false);

    Interval second("(0, 2]");
    QCOMPARE(second.isInInterval(-1), false);
    QCOMPARE(second.isInInterval(0), false);
    QCOMPARE(second.isInInterval(1), true);
    QCOMPARE(second.isInInterval(2), true);
    QCOMPARE(second.isInInterval(3), false);
}

void IntervalTest::testValid()
{
    (Interval("[1, 2]"));
    (Interval("(1, 2)"));
    (Interval("[1, 2)"));
    (Interval("(1, 2]"));
    (Interval("[1,2]"));
}

void IntervalTest::testInvalid()
{
    QVERIFY_EXCEPTION_THROWN((Interval("")), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN((Interval("1")), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN((Interval("1, 2")), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN((Interval("[1 2]")), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN((Interval("[1, 2")), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN((Interval("1, 2]")), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN((Interval("[ 1, 2]")), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN((Interval("[1, 2 ]")), std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN((Interval("[2, 1]")), std::invalid_argument);
}

QTEST_MAIN(IntervalTest)
