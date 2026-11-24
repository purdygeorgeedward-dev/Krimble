/*
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "kis_time_span_test.h"
#include "kis_time_span.h"

void KisTimeSpanTest::testCreationMethods()
{
    {
        const int start = 20;
        const int end = 100;
        KisTimeSpan span = KisTimeSpan::fromTimeToTime(start, end);
        QVERIFY(span.start() == start);
        QVERIFY(span.end() == end);
    }

    {
        const int start = 20;
        const int end = 150;
        const int length = (end - start) + 1;
        KisTimeSpan span = KisTimeSpan::fromTimeWithDuration(start, length);
        QVERIFY(span.start() == start);
        QVERIFY(span.end() == end);
    }

    {

        const int start = 20;
        KisTimeSpan span = KisTimeSpan::infinite(start);
        QVERIFY(span.isInfinite());
        QVERIFY(span.start() == start);
    }

    {

        QVERIFY(KisTimeSpan::fromTimeToTime(10, 5).isValid() == false);

        QVERIFY(KisTimeSpan::fromTimeWithDuration(10, -5).isValid() == false);
    }

}

void KisTimeSpanTest::testTimeSpanOperators()
{
    const KisTimeSpan spanA = KisTimeSpan::fromTimeToTime(0, 25);
    const KisTimeSpan spanB = KisTimeSpan::fromTimeToTime(15, 30);

    {
        KisTimeSpan spanABU = spanA | spanB;
        QCOMPARE(spanABU.start(), spanA.start());
        QCOMPARE(spanABU.end(), spanB.end());

        const KisTimeSpan spanC = KisTimeSpan::fromTimeToTime(20, 60);
        spanABU |= spanC;

        QCOMPARE(spanABU.start(), spanA.start());
        QCOMPARE(spanABU.end(), spanC.end());
    }

    {
        KisTimeSpan spanABI = spanA & spanB;

        QCOMPARE(spanABI.start(), spanB.start());
        QCOMPARE(spanABI.end(), spanA.end());

        const KisTimeSpan spanC = KisTimeSpan::fromTimeToTime(0, 20);
        spanABI &= spanC;

        QCOMPARE(spanABI.start(), spanB.start());
        QCOMPARE(spanABI.end(), spanC.end());
    }
}

SIMPLE_TEST_MAIN(KisTimeSpanTest)
