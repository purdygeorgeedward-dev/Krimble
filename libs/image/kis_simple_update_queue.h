/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_SIMPLE_UPDATE_QUEUE_H
#define __KIS_SIMPLE_UPDATE_QUEUE_H

#include <QMutex>
#include "kis_updater_context.h"
#include <KisProjectionUpdateFlags.h>

typedef QList<KisBaseRectsWalkerSP> KisWalkersList;
typedef QListIterator<KisBaseRectsWalkerSP> KisWalkersListIterator;
typedef QMutableListIterator<KisBaseRectsWalkerSP> KisMutableWalkersListIterator;

typedef QList<KisSpontaneousJob*> KisSpontaneousJobsList;
typedef QListIterator<KisSpontaneousJob*> KisSpontaneousJobsListIterator;
typedef QMutableListIterator<KisSpontaneousJob*> KisMutableSpontaneousJobsListIterator;

class KRITAIMAGE_EXPORT KisSimpleUpdateQueue
{
public:
    KisSimpleUpdateQueue();
    virtual ~KisSimpleUpdateQueue();

    void processQueue(KisUpdaterContext &updaterContext);

    void addUpdateJob(KisNodeSP node, const QVector<QRect> &rects, const QRect& cropRect, int levelOfDetail, KisProjectionUpdateFlags flags);
    void addFullRefreshJob(KisNodeSP node, const QVector<QRect> &rects, const QRect& cropRect, int levelOfDetail, KisProjectionUpdateFlags flags);

    void addUpdateJob(KisNodeSP node, const QRect &rc, const QRect& cropRect, int levelOfDetail);

    void addFullRefreshJob(KisNodeSP node, const QRect &rc, const QRect& cropRect, int levelOfDetail);

    void addSpontaneousJob(KisSpontaneousJob *spontaneousJob);

    void optimize();

    bool isEmpty() const;

    bool isIdle() const;

    qint32 sizeMetric() const;

    void updateSettings();

    int overrideLevelOfDetail() const;

protected:
    void addJob(KisNodeSP node, const QVector<QRect> &rects, const QRect& cropRect, int levelOfDetail, KisBaseRectsWalker::UpdateType type, bool dontInvalidateFrames);

    bool processOneJob(KisUpdaterContext &updaterContext);

    bool trySplitJob(KisNodeSP node, const QRect& rc, const QRect& cropRect, int levelOfDetail, KisBaseRectsWalker::UpdateType type, bool dontInvalidateFrames);
    bool tryMergeJob(KisNodeSP node, const QRect& rc, const QRect& cropRect, int levelOfDetail, KisBaseRectsWalker::UpdateType type, bool dontInvalidateFrames);

    void collectJobs(KisBaseRectsWalkerSP &baseWalker, QRect baseRect,
                     const qreal maxAlpha);
    bool joinRects(QRect& baseRect, const QRect& newRect, qreal maxAlpha);

protected:

    mutable QMutex m_lock;
    KisWalkersList m_updatesList;
    KisSpontaneousJobsList m_spontaneousJobsList;

    qint32 m_patchWidth;
    qint32 m_patchHeight;

    qreal m_maxCollectAlpha;

    qreal m_maxMergeAlpha;

    qreal m_maxMergeCollectAlpha;

    int m_overrideLevelOfDetail;
};

class KRITAIMAGE_EXPORT KisTestableSimpleUpdateQueue : public KisSimpleUpdateQueue
{
public:
    KisWalkersList& getWalkersList();
    KisSpontaneousJobsList& getSpontaneousJobsList();
};

#endif

