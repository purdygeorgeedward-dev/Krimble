/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_stroke_test.h"
#include <simpletest.h>

#include "kis_stroke.h"
#include "scheduler_utils.h"

void KisStrokeTest::testRegularStroke()
{
    KisStroke stroke(new KisTestingStrokeStrategy());
    QQueue<KisStrokeJob*> &queue = stroke.testingGetQueue();

    QCOMPARE(queue.size(), 1);
    SCOMPARE(getJobName(queue[0]), "init");
    QCOMPARE(stroke.isEnded(), false);

    stroke.addJob(0);

    QCOMPARE(queue.size(), 2);
    SCOMPARE(getJobName(queue[0]), "init");
    SCOMPARE(getJobName(queue[1]), "dab");
    QCOMPARE(stroke.isEnded(), false);

    stroke.endStroke();

    QCOMPARE(queue.size(), 3);
    SCOMPARE(getJobName(queue[0]), "init");
    SCOMPARE(getJobName(queue[1]), "dab");
    SCOMPARE(getJobName(queue[2]), "finish");
    QCOMPARE(stroke.isEnded(), true);

    KisStrokeJob* job;

    job = stroke.popOneJob();
    delete job;
    QCOMPARE(queue.size(), 2);
    SCOMPARE(getJobName(queue[0]), "dab");
    SCOMPARE(getJobName(queue[1]), "finish");

    job = stroke.popOneJob();
    delete job;
    QCOMPARE(queue.size(), 1);
    SCOMPARE(getJobName(queue[0]), "finish");

    job = stroke.popOneJob();
    delete job;
    QCOMPARE(queue.size(), 0);

    job = stroke.popOneJob();
    QCOMPARE(job, (KisStrokeJob*)0);
}

void KisStrokeTest::testCancelStrokeCase1()
{
    KisStroke stroke(new KisTestingStrokeStrategy());
    QQueue<KisStrokeJob*> &queue = stroke.testingGetQueue();

    stroke.addJob(0);

    QCOMPARE(queue.size(), 2);
    SCOMPARE(getJobName(queue[0]), "init");
    SCOMPARE(getJobName(queue[1]), "dab");
    QCOMPARE(stroke.isEnded(), false);

    stroke.cancelStroke();

    QCOMPARE(queue.size(), 0);
    QCOMPARE(stroke.isEnded(), true);

    stroke.clearQueueOnCancel();
}

void KisStrokeTest::testCancelStrokeCase2and3()
{
    KisStroke stroke(new KisTestingStrokeStrategy());
    QQueue<KisStrokeJob*> &queue = stroke.testingGetQueue();

    stroke.addJob(0);
    delete stroke.popOneJob();

    QCOMPARE(queue.size(), 1);
    SCOMPARE(getJobName(queue[0]), "dab");
    QCOMPARE(stroke.isEnded(), false);

    stroke.cancelStroke();

    QCOMPARE(queue.size(), 1);
    SCOMPARE(getJobName(queue[0]), "cancel");
    QCOMPARE(stroke.isEnded(), true);

    stroke.clearQueueOnCancel();
}

void KisStrokeTest::testCancelStrokeCase5()
{
    KisStroke stroke(new KisTestingStrokeStrategy());
    QQueue<KisStrokeJob*> &queue = stroke.testingGetQueue();

    stroke.addJob(0);
    delete stroke.popOneJob();
    delete stroke.popOneJob();

    QCOMPARE(stroke.isEnded(), false);

    stroke.cancelStroke();
    QCOMPARE(queue.size(), 1);
    SCOMPARE(getJobName(queue[0]), "cancel");
    QCOMPARE(stroke.isEnded(), true);

    delete stroke.popOneJob();
}

void KisStrokeTest::testCancelStrokeCase4()
{
    KisStroke stroke(new KisTestingStrokeStrategy());
    QQueue<KisStrokeJob*> &queue = stroke.testingGetQueue();

    stroke.addJob(0);
    stroke.endStroke();
    delete stroke.popOneJob();
    delete stroke.popOneJob();
    delete stroke.popOneJob();

    QCOMPARE(stroke.isEnded(), true);

    stroke.cancelStroke();
    QCOMPARE(queue.size(), 0);
    QCOMPARE(stroke.isEnded(), true);
}

void KisStrokeTest::testCancelStrokeCase6()
{
    KisStroke stroke(new KisTestingStrokeStrategy());
    QQueue<KisStrokeJob*> &queue = stroke.testingGetQueue();

    stroke.addJob(0);
    delete stroke.popOneJob();

    QCOMPARE(queue.size(), 1);
    SCOMPARE(getJobName(queue[0]), "dab");
    QCOMPARE(stroke.isEnded(), false);

    stroke.cancelStroke();

    QCOMPARE(queue.size(), 1);
    SCOMPARE(getJobName(queue[0]), "cancel");
    QCOMPARE(stroke.isEnded(), true);

    int seqNo = cancelSeqNo(queue.head());

    stroke.cancelStroke();

    QCOMPARE(queue.size(), 1);
    SCOMPARE(getJobName(queue[0]), "cancel");
    QCOMPARE(stroke.isEnded(), true);
    QCOMPARE(cancelSeqNo(queue.head()), seqNo);

    stroke.clearQueueOnCancel();
}

SIMPLE_TEST_MAIN(KisStrokeTest)
