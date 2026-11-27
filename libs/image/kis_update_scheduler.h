/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_UPDATE_SCHEDULER_H
#define __KIS_UPDATE_SCHEDULER_H

#include <QObject>
#include "kritaimage_export.h"
#include "kis_types.h"

#include "kis_image_interfaces.h"
#include "kis_stroke_strategy_factory.h"
#include "kis_strokes_queue_undo_result.h"
#include "KisLodPreferences.h"
#include "KisProjectionUpdateFlags.h"

class QRect;
class KoProgressProxy;
class KisProjectionUpdateListener;
class KisSpontaneousJob;
class KisPostExecutionUndoAdapter;

class KRITAIMAGE_EXPORT KisUpdateScheduler : public QObject, public KisStrokesFacade
{
    Q_OBJECT

public:
    KisUpdateScheduler(KisProjectionUpdateListener *projectionUpdateListener, QObject *parent = 0);
    ~KisUpdateScheduler() override;

    void setThreadsLimit(int value);

    int threadsLimit() const;

    void setProgressProxy(KoProgressProxy *progressProxy);

    void immediateLockForReadOnly();

    void unlock(bool resetLodLevels = true);

    void waitForDone();

    void barrierLock();

    bool tryBarrierLock();

    bool isIdle();

    void blockUpdates();

    void unblockUpdates();

    void updateProjection(KisNodeSP node, const QVector<QRect> &rects, const QRect &cropRect, KisProjectionUpdateFlags flags);
    void fullRefreshAsync(KisNodeSP root, const QVector<QRect>& rects, const QRect &cropRect, KisProjectionUpdateFlags flags);

    void updateProjection(KisNodeSP node, const QRect &rc, const QRect &cropRect);

    void fullRefreshAsync(KisNodeSP root, const QRect &rc, const QRect &cropRect);

    void addSpontaneousJob(KisSpontaneousJob *spontaneousJob);

    bool hasUpdatesRunning() const;

    KisStrokeId startStroke(KisStrokeStrategy *strokeStrategy) override;
    void addJob(KisStrokeId id, KisStrokeJobData *data) override;
    void endStroke(KisStrokeId id) override;
    bool cancelStroke(KisStrokeId id) override;

    void setLodPreferences(const KisLodPreferences &value);

    KisLodPreferences lodPreferences() const;

    void explicitRegenerateLevelOfDetail();

    void setLod0ToNStrokeStrategyFactory(const KisLodSyncStrokeStrategyFactory &factory);

    void setSuspendResumeUpdatesStrokeStrategyFactory(const KisSuspendResumeStrategyPairFactory &factory);

    void setPurgeRedoStateCallback(const std::function<void()> &callback);

    void setPostSyncLod0GUIPlaneRequestForResumeCallback(const std::function<void()> &callback);

    KisPostExecutionUndoAdapter* lodNPostExecutionUndoAdapter() const;

    bool tryCancelCurrentStrokeAsync();

    UndoResult tryUndoLastStrokeAsync();

    bool wrapAroundModeSupported() const;
    int currentLevelOfDetail() const;

    void continueUpdate(const QRect &rect);
    void doSomeUsefulWork();
    void spareThreadAppeared();

protected:
    KisUpdateScheduler();
    void connectSignals();
    void processQueues();

protected Q_SLOTS:
    void updateSettings();

private:
    friend class UpdatesBlockTester;
    bool haveUpdatesRunning();
    void tryProcessUpdatesQueue();
    void wakeUpWaitingThreads();

    void progressUpdate();

protected:
    struct Private;
    Private * const m_d;
};

class KisTestableSimpleUpdateQueue;
class KisUpdaterContext;

class KRITAIMAGE_EXPORT KisTestableUpdateScheduler : public KisUpdateScheduler
{
public:
    KisTestableUpdateScheduler(KisProjectionUpdateListener *projectionUpdateListener,
                               qint32 threadCount);

    KisUpdaterContext* updaterContext();
    using KisUpdateScheduler::processQueues;
};

#endif

