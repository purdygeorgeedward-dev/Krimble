/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_UPDATE_JOB_ITEM_H
#define __KIS_UPDATE_JOB_ITEM_H

#include <atomic>

#include <QRunnable>
#include <QReadWriteLock>

#include "kis_stroke_job.h"
#include "kis_spontaneous_job.h"
#include "kis_base_rects_walker.h"
#include "kis_async_merger.h"
#include "kis_updater_context.h"
#include <KoAlwaysInline.h>

class KRITAIMAGE_EXPORT KisUpdateJobItem : public QObject, public QRunnable
{
    Q_OBJECT
public:
    enum class Type : int {
        EMPTY = 0,
        WAITING,
        MERGE,
        STROKE,
        SPONTANEOUS
    };

public:
    KisUpdateJobItem(KisUpdaterContext *updaterContext)
        : m_updaterContext(updaterContext)
    {
        setAutoDelete(false);
        KIS_SAFE_ASSERT_RECOVER_NOOP(m_atomicType.is_lock_free());
    }
    ~KisUpdateJobItem() override
    {
        delete m_runnableJob;
    }

    void run() override {
        runImpl();

        m_updaterContext->jobThreadExited();
    }

private:

    ALWAYS_INLINE void runImpl() {
        if (!isRunning()) return;

        while (1) {
            KIS_SAFE_ASSERT_RECOVER_RETURN(isRunning());

            if(m_exclusive) {
                m_updaterContext->m_exclusiveJobLock.lockForWrite();
            } else {
                m_updaterContext->m_exclusiveJobLock.lockForRead();
            }

            if(m_atomicType == Type::MERGE) {
                runMergeJob();
            } else {
                KIS_ASSERT(m_atomicType == Type::STROKE ||
                           m_atomicType == Type::SPONTANEOUS);

                if (m_runnableJob) {
#ifdef DEBUG_JOBS_SEQUENCE
                    if (m_atomicType == Type::STROKE) {
                        qDebug() << "running: stroke" << m_runnableJob->debugName();
                    } else if (m_atomicType == Type::SPONTANEOUS) {
                        qDebug() << "running: spont " << m_runnableJob->debugName();
                    } else {
                        qDebug() << "running: unkn. " << m_runnableJob->debugName();
                    }
#endif

                    m_runnableJob->run();
                }
            }

            setDone();

            m_updaterContext->doSomeUsefulWork();

            m_updaterContext->jobFinished();

            m_updaterContext->m_exclusiveJobLock.unlock();

            Type expectedValue = Type::WAITING;
            if (m_atomicType.compare_exchange_strong(expectedValue, Type::EMPTY)) {
                break;
            }
        }
    }

public:

    inline void runMergeJob() {
        KIS_SAFE_ASSERT_RECOVER_RETURN(m_atomicType == Type::MERGE);
        KIS_SAFE_ASSERT_RECOVER_RETURN(m_walker);

#ifdef DEBUG_JOBS_SEQUENCE
        qDebug() << "running: merge " << m_walker->startNode() << m_walker->changeRect();

#endif

        m_merger.startMerge(*m_walker);

        QRect changeRect = m_walker->changeRect();
        m_updaterContext->continueUpdate(changeRect);
    }

    inline bool setWalker(KisBaseRectsWalkerSP walker) {
        KIS_ASSERT(m_atomicType <= Type::WAITING);

        m_accessRect = walker->accessRect();
        m_changeRect = walker->changeRect();
        m_walker = walker;

        m_exclusive = false;
        m_runnableJob = 0;

        const Type oldState = m_atomicType.exchange(Type::MERGE);
        return oldState == Type::EMPTY;
    }

    inline bool setStrokeJob(KisStrokeJob *strokeJob) {
        KIS_ASSERT(m_atomicType <= Type::WAITING);

        m_runnableJob = strokeJob;
        m_strokeJobSequentiality = strokeJob->sequentiality();

        m_exclusive = strokeJob->isExclusive();
        m_walker = 0;
        m_accessRect = m_changeRect = QRect();

        const Type oldState = m_atomicType.exchange(Type::STROKE);
        return oldState == Type::EMPTY;
    }

    inline bool setSpontaneousJob(KisSpontaneousJob *spontaneousJob) {
        KIS_ASSERT(m_atomicType <= Type::WAITING);

        m_runnableJob = spontaneousJob;

        m_exclusive = spontaneousJob->isExclusive();
        m_walker = 0;
        m_accessRect = m_changeRect = QRect();

        const Type oldState = m_atomicType.exchange(Type::SPONTANEOUS);
        return oldState == Type::EMPTY;
    }

    inline void setDone() {
        m_walker = 0;
        delete m_runnableJob;
        m_runnableJob = 0;
        m_atomicType = Type::WAITING;
    }

    inline bool isRunning() const {
        return m_atomicType >= Type::MERGE;
    }

    inline Type type() const {
        return m_atomicType;
    }

    inline const QRect& accessRect() const {
        return m_accessRect;
    }

    inline const QRect& changeRect() const {
        return m_changeRect;
    }

    inline KisStrokeJobData::Sequentiality strokeJobSequentiality() const {
        return m_strokeJobSequentiality;
    }

private:
    friend class KisTestableUpdaterContext;
    friend class KisSimpleUpdateQueueTest;
    friend class KisStrokesQueueTest;
    friend class KisUpdateSchedulerTest;
    friend class KisUpdaterContext;

    inline KisBaseRectsWalkerSP walker() const {
        return m_walker;
    }

    inline KisStrokeJob* strokeJob() const {
        KisStrokeJob *job = dynamic_cast<KisStrokeJob*>(m_runnableJob);
        Q_ASSERT(job);

        return job;
    }

    inline void testingSetDone() {
        setDone();
    }

private:
    KisUpdaterContext *m_updaterContext {0};
    bool m_exclusive {false};
    std::atomic<Type> m_atomicType {Type::EMPTY};
    volatile KisStrokeJobData::Sequentiality m_strokeJobSequentiality {KisStrokeJobData::SEQUENTIAL};

    KisRunnableWithDebugName *m_runnableJob {0};

    KisBaseRectsWalkerSP m_walker;
    KisAsyncMerger m_merger;

    QRect m_accessRect;
    QRect m_changeRect;
};

#endif
