/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_UPDATER_CONTEXT_H
#define __KIS_UPDATER_CONTEXT_H

#include <QMutex>
#include <QReadWriteLock>
#include <QThreadPool>
#include <QWaitCondition>

#include "kis_base_rects_walker.h"
#include "kis_async_merger.h"
#include "kis_lock_free_lod_counter.h"

#include "KisUpdaterContextSnapshotEx.h"
#include "kis_update_scheduler.h"

class KisUpdateJobItem;
class KisSpontaneousJob;
class KisStrokeJob;
class KisUpdateScheduler;

class KRITAIMAGE_EXPORT KisUpdaterContext
{
public:
    static const int useIdealThreadCountTag;

public:
    KisUpdaterContext(qint32 threadCount = useIdealThreadCountTag, KisUpdateScheduler *parent = 0);
    ~KisUpdaterContext();

    void getJobsSnapshot(qint32 &numMergeJobs, qint32 &numStrokeJobs);

    KisUpdaterContextSnapshotEx getContextSnapshotEx() const;

    int currentLevelOfDetail() const;

    bool hasSpareThread();

    bool isJobAllowed(KisBaseRectsWalkerSP walker);

    void addMergeJob(KisBaseRectsWalkerSP walker);

    void addStrokeJob(KisStrokeJob *strokeJob);

    void addSpontaneousJob(KisSpontaneousJob *spontaneousJob);

    void waitForDone();

    bool isIdle() const;

    void lock();

    void unlock();

    void setThreadsLimit(int value);

    int threadsLimit() const;

    void continueUpdate(const QRect& rc);
    void doSomeUsefulWork();
    void jobFinished();
    void jobThreadExited();

    void setTestingMode(bool value);

protected:
    static bool walkerIntersectsJob(KisBaseRectsWalkerSP walker,
                                    const KisUpdateJobItem* job);
    qint32 findSpareThread();

protected:
    QReadWriteLock m_exclusiveJobLock;

    QMutex m_lock;
    mutable QMutex m_runningThreadsMutex;
    int m_numRunningThreads = 0;
    QWaitCondition m_waitForDoneCondition;
    QVector<KisUpdateJobItem*> m_jobs;
    QThreadPool m_threadPool;
    KisLockFreeLodCounter m_lodCounter;
    KisUpdateScheduler *m_scheduler;
    bool m_testingMode = false;

private:

    friend class KisUpdaterContextTest;
    friend class KisUpdateSchedulerTest;
    friend class KisStrokesQueueTest;
    friend class KisSimpleUpdateQueueTest;
    friend class KisUpdateJobItem;

    const QVector<KisUpdateJobItem*> getJobs();
    void clear();

    void startThread(int index);

};

class KRITAIMAGE_EXPORT KisTestableUpdaterContext : public KisUpdaterContext
{
public:
    KisTestableUpdaterContext(qint32 threadCount);
};

#endif

