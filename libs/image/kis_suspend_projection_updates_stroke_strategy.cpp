/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include <QMutex>
#include <QMutexLocker>

#include "kis_suspend_projection_updates_stroke_strategy.h"

#include <kis_image.h>
#include <krita_utils.h>
#include <kis_projection_updates_filter.h>
#include "kis_image_signal_router.h"
#include "kis_image_animation_interface.h"

#include "kundo2command.h"
#include "KisRunnableStrokeJobDataBase.h"
#include "KisRunnableStrokeJobsInterface.h"
#include "kis_paintop_utils.h"

#include <memory>

inline uint qHash(const QRect &rc) {
    return rc.x() +
        (rc.y() << 16) +
        (rc.width() << 8) +
        (rc.height() << 24);
}

struct KisSuspendProjectionUpdatesStrokeStrategy::Private
{
    KisImageWSP image;
    bool suspend;
    QVector<QRect> accumulatedDirtyRects;
    bool sanityResumingFinished = false;
    int updatesEpoch = 0;
    bool haveDisabledGUILodSync = false;
    SharedDataSP sharedData;

    void tryFetchUsedUpdatesFilter(KisImageSP image);
    void tryIssueRecordedDirtyRequests(KisImageSP image);

    class SuspendLod0Updates : public SuspendUpdatesFilterInterface
    {

        struct Request {
            Request() : flags(KisProjectionUpdateFlag::None) {}
            Request(const QRect &_rect, KisProjectionUpdateFlags _flags)
                : rect(_rect), flags(_flags)
            {
            }

            QRect rect;
            KisProjectionUpdateFlags flags;
        };

        struct FullRefreshRequest {
            FullRefreshRequest() {}
            FullRefreshRequest(const QRect &_rect, const QRect &_cropRect, KisProjectionUpdateFlags _flags)
                : rect(_rect), cropRect(_cropRect), flags(_flags)
            {
            }

            QRect rect;
            QRect cropRect;
            KisProjectionUpdateFlags flags;
        };

        typedef QHash<KisNodeSP, QVector<Request> > UpdatesHash;
        typedef QHash<KisNodeSP, QVector<FullRefreshRequest> > RefreshesHash;
    public:
        SuspendLod0Updates()
        {
        }

        void addExplicitUIUpdateRect(const QRect &rc) override
        {
            m_explicitUIUpdateRequest |= rc;
        }

        QRect explicitUIUpdateRequest() const {
            return m_explicitUIUpdateRequest;
        }

        bool filter(KisImage *image, KisNode *node, const QVector<QRect> &rects,  KisProjectionUpdateFlags flags) override {
            if (image->currentLevelOfDetail() > 0) return false;

            QMutexLocker l(&m_mutex);

            Q_FOREACH(const QRect &rc, rects) {
                m_requestsHash[KisNodeSP(node)].append(Request(rc, flags));
            }

            return true;
        }

        bool filterRefreshGraph(KisImage *image, KisNode *node, const QVector<QRect> &rects, const QRect &cropRect, KisProjectionUpdateFlags flags) override {
            if (image->currentLevelOfDetail() > 0) return false;

            QMutexLocker l(&m_mutex);

            Q_FOREACH(const QRect &rc, rects) {
                m_refreshesHash[KisNodeSP(node)].append(FullRefreshRequest(rc, cropRect, flags));
            }

            return true;
        }

        void notifyUpdates(KisImageSP image) {
            const int step = 64;

            auto processRefreshes = [&] (bool noFilthyUpdates) {
                RefreshesHash::const_iterator it = m_refreshesHash.constBegin();
                RefreshesHash::const_iterator end = m_refreshesHash.constEnd();

                for (; it != end; ++it) {
                    KisNodeSP node = it.key();

                    QHash<QRect, QVector<QRect>> fullRefreshRequests;

                    bool invalidateFrames = false;

                    Q_FOREACH (const FullRefreshRequest &req, it.value()) {
                        if (req.flags.testFlag(KisProjectionUpdateFlag::NoFilthy) == noFilthyUpdates) {
                            fullRefreshRequests[req.cropRect] += req.rect;
                            invalidateFrames |= !req.flags.testFlag(KisProjectionUpdateFlag::DontInvalidateFrames);
                        }
                    }

                    KisProjectionUpdateFlags finalFlags(KisProjectionUpdateFlag::None);
                    finalFlags.setFlag(KisProjectionUpdateFlag::DontInvalidateFrames, !invalidateFrames);
                    finalFlags.setFlag(KisProjectionUpdateFlag::NoFilthy, noFilthyUpdates);

                    auto reqIt = fullRefreshRequests.begin();
                    for (; reqIt != fullRefreshRequests.end(); ++reqIt) {
                        const QVector<QRect> simplifiedRects = KisRegion::fromOverlappingRects(reqIt.value(), step).rects();
                        image->refreshGraphAsync(node, simplifiedRects, reqIt.key(), finalFlags);
                    }
                }
            };

            processRefreshes(false);
            processRefreshes(true);

            auto processUpdates = [&] (bool noFilthyUpdates) {
                UpdatesHash::const_iterator it = m_requestsHash.constBegin();
                UpdatesHash::const_iterator end = m_requestsHash.constEnd();

                for (; it != end; ++it) {
                    KisNodeSP node = it.key();

                    QVector<QRect> dirtyRects;

                    bool invalidateFrames = false;

                    Q_FOREACH (const Request &req, it.value()) {
                        if (req.flags.testFlag(KisProjectionUpdateFlag::NoFilthy) == noFilthyUpdates) {
                            dirtyRects += req.rect;
                            invalidateFrames |= !req.flags.testFlag(KisProjectionUpdateFlag::DontInvalidateFrames);
                        }
                    }

                    KisProjectionUpdateFlags finalFlags(KisProjectionUpdateFlag::None);
                    finalFlags.setFlag(KisProjectionUpdateFlag::DontInvalidateFrames, !invalidateFrames);
                    finalFlags.setFlag(KisProjectionUpdateFlag::NoFilthy, noFilthyUpdates);

                    const QVector<QRect> simplifiedRects = KisRegion::fromOverlappingRects(dirtyRects, step).rects();

                    image->requestProjectionUpdate(const_cast<KisNode*>(node.data()), simplifiedRects, finalFlags);
                }
            };

            processUpdates(false);
            processUpdates(true);
        }

    private:
        UpdatesHash m_requestsHash;
        RefreshesHash m_refreshesHash;
        QRect m_explicitUIUpdateRequest;
        QMutex m_mutex;
    };

    QVector<QSharedPointer<SuspendLod0Updates>> usedFilters;

    struct StrokeJobCommand : public KUndo2Command
    {
        StrokeJobCommand(KisStrokeJobData::Sequentiality sequentiality = KisStrokeJobData::SEQUENTIAL,
                         KisStrokeJobData::Exclusivity exclusivity = KisStrokeJobData::NORMAL)
            : m_sequentiality(sequentiality),
              m_exclusivity(exclusivity)
        {}

        KisStrokeJobData::Sequentiality m_sequentiality;
        KisStrokeJobData::Exclusivity m_exclusivity;
    };

    struct UndoableData : public KisRunnableStrokeJobDataBase
    {
        UndoableData(StrokeJobCommand *command)
            : KisRunnableStrokeJobDataBase(command->m_sequentiality, command->m_exclusivity),
              m_command(command)
        {
        }

        void run() override {
            KIS_SAFE_ASSERT_RECOVER_RETURN(m_command);
            m_command->redo();
        }

        std::unique_ptr<StrokeJobCommand> m_command;
    };

    struct SuspendUpdatesCommand : public StrokeJobCommand
    {
        SuspendUpdatesCommand(Private *d)
            : StrokeJobCommand(KisStrokeJobData::BARRIER),
              m_d(d) {}

        void redo() override {
            KisImageSP image = m_d->image.toStrongRef();
            KIS_SAFE_ASSERT_RECOVER_RETURN(image);
            KIS_SAFE_ASSERT_RECOVER_RETURN(!image->currentProjectionUpdatesFilter());
            KIS_SAFE_ASSERT_RECOVER_RETURN(!m_d->sharedData->installedFilterCookie);

            m_d->sharedData->installedFilterCookie = image->addProjectionUpdatesFilter(
                toQShared(new Private::SuspendLod0Updates()));
        }

        void undo() override {
            KisImageSP image = m_d->image.toStrongRef();
            KIS_SAFE_ASSERT_RECOVER_RETURN(image);
            KIS_SAFE_ASSERT_RECOVER_RETURN(image->currentProjectionUpdatesFilter());
            KIS_SAFE_ASSERT_RECOVER_RETURN(image->currentProjectionUpdatesFilter() == m_d->sharedData->installedFilterCookie);

            m_d->tryFetchUsedUpdatesFilter(image);
        }

        Private *m_d;
    };

    struct ResumeAndIssueGraphUpdatesCommand : public StrokeJobCommand
    {
        ResumeAndIssueGraphUpdatesCommand(Private *d)
            : StrokeJobCommand(KisStrokeJobData::BARRIER),
              m_d(d) {}

        void redo() override {
            KisImageSP image = m_d->image.toStrongRef();
            KIS_SAFE_ASSERT_RECOVER_RETURN(image);
            KIS_SAFE_ASSERT_RECOVER_RETURN(image->currentProjectionUpdatesFilter());
            KIS_SAFE_ASSERT_RECOVER_RETURN(image->currentProjectionUpdatesFilter() == m_d->sharedData->installedFilterCookie);

            image->disableUIUpdates();
            m_d->tryFetchUsedUpdatesFilter(image);
            m_d->tryIssueRecordedDirtyRequests(image);
        }

        void undo() override {
            KisImageSP image = m_d->image.toStrongRef();
            KIS_SAFE_ASSERT_RECOVER_RETURN(image);
            KIS_SAFE_ASSERT_RECOVER_RETURN(!image->currentProjectionUpdatesFilter());
            KIS_SAFE_ASSERT_RECOVER_RETURN(!m_d->sharedData->installedFilterCookie);

            m_d->sharedData->installedFilterCookie = image->addProjectionUpdatesFilter(
                toQShared(new Private::SuspendLod0Updates()));
            image->enableUIUpdates();
        }

        Private *m_d;
    };

    struct UploadDataToUIData : public KisRunnableStrokeJobDataBase
    {
        UploadDataToUIData(const QRect &rc, int updateEpoch, KisSuspendProjectionUpdatesStrokeStrategy *strategy)
            : KisRunnableStrokeJobDataBase(KisStrokeJobData::CONCURRENT),
              m_strategy(strategy),
              m_rc(rc),
              m_updateEpoch(updateEpoch)
        {
        }

        void run() override {
            if (m_strategy->m_d->updatesEpoch > m_updateEpoch) {
                return;
            }

            KisImageSP image = m_strategy->m_d->image.toStrongRef();
            KIS_SAFE_ASSERT_RECOVER_RETURN(image);

            image->notifyProjectionUpdated(m_rc);
        }

        KisSuspendProjectionUpdatesStrokeStrategy *m_strategy;
        QRect m_rc;
        int m_updateEpoch;
    };

    struct BlockUILodSync : public KisRunnableStrokeJobDataBase
    {
        BlockUILodSync(bool block, KisSuspendProjectionUpdatesStrokeStrategy *strategy)
            : KisRunnableStrokeJobDataBase(KisStrokeJobData::BARRIER),
              m_strategy(strategy),
              m_block(block)
        {}

        void run() override {
            KisImageSP image = m_strategy->m_d->image.toStrongRef();
            KIS_SAFE_ASSERT_RECOVER_RETURN(image);

            image->signalRouter()->emitRequestLodPlanesSyncBlocked(m_block);
            m_strategy->m_d->haveDisabledGUILodSync = m_block;
        }

        KisSuspendProjectionUpdatesStrokeStrategy *m_strategy;
        bool m_block;
    };

    struct StartBatchUIUpdatesCommand : public StrokeJobCommand
    {
        StartBatchUIUpdatesCommand(KisSuspendProjectionUpdatesStrokeStrategy *strategy)
            : StrokeJobCommand(KisStrokeJobData::BARRIER),
              m_strategy(strategy) {}

        void redo() override {
            KisImageSP image = m_strategy->m_d->image.toStrongRef();
            KIS_SAFE_ASSERT_RECOVER_RETURN(image);

            const QVector<QRect> totalDirtyRects =
                image->enableUIUpdates() + m_strategy->m_d->accumulatedDirtyRects;

            const QRect totalRect =
                image->bounds() &
                std::accumulate(totalDirtyRects.begin(), totalDirtyRects.end(), QRect(), std::bit_or<QRect>());

            m_strategy->m_d->accumulatedDirtyRects =
                KisPaintOpUtils::splitAndFilterDabRect(totalRect,
                                                       totalDirtyRects,
                                                       KritaUtils::optimalPatchSize().width());

            image->signalRouter()->emitNotifyBatchUpdateStarted();

            QVector<KisRunnableStrokeJobDataBase*> jobsData;
            Q_FOREACH (const QRect &rc, m_strategy->m_d->accumulatedDirtyRects) {
                jobsData << new Private::UploadDataToUIData(rc, m_strategy->m_d->updatesEpoch, m_strategy);
            }

            m_strategy->runnableJobsInterface()->addRunnableJobs(jobsData);

        }

        void undo() override {
            KisImageSP image = m_strategy->m_d->image.toStrongRef();
            KIS_SAFE_ASSERT_RECOVER_RETURN(image);

            image->signalRouter()->emitNotifyBatchUpdateEnded();
            image->disableUIUpdates();
        }

        KisSuspendProjectionUpdatesStrokeStrategy *m_strategy;
    };

    struct EndBatchUIUpdatesCommand : public StrokeJobCommand
    {
        EndBatchUIUpdatesCommand(KisSuspendProjectionUpdatesStrokeStrategy *strategy)
            : StrokeJobCommand(KisStrokeJobData::BARRIER),
              m_strategy(strategy) {}

        void redo() override {
            KisImageSP image = m_strategy->m_d->image.toStrongRef();
            KIS_SAFE_ASSERT_RECOVER_RETURN(image);

            image->signalRouter()->emitNotifyBatchUpdateEnded();
            m_strategy->m_d->sanityResumingFinished = true;
            m_strategy->m_d->accumulatedDirtyRects.clear();
            KIS_SAFE_ASSERT_RECOVER_NOOP(m_strategy->m_d->usedFilters.isEmpty());
        }

        void undo() override {

            KIS_SAFE_ASSERT_RECOVER_NOOP(m_strategy->m_d->usedFilters.isEmpty());
            KIS_SAFE_ASSERT_RECOVER_NOOP(m_strategy->m_d->accumulatedDirtyRects.isEmpty());

            m_strategy->m_d->sanityResumingFinished = false;

            KisImageSP image = m_strategy->m_d->image.toStrongRef();
            KIS_SAFE_ASSERT_RECOVER_RETURN(image);

            image->signalRouter()->emitNotifyBatchUpdateStarted();
        }

        KisSuspendProjectionUpdatesStrokeStrategy *m_strategy;
    };

    QVector<StrokeJobCommand*> executedCommands;
};

KisSuspendProjectionUpdatesStrokeStrategy::KisSuspendProjectionUpdatesStrokeStrategy(KisImageWSP image, bool suspend, SharedDataSP sharedData)
    : KisRunnableBasedStrokeStrategy(suspend ?
                                     QLatin1String("suspend_stroke_strategy") :
                                     QLatin1String("resume_stroke_strategy")),
      m_d(new Private)
{
    m_d->image = image;
    m_d->suspend = suspend;
    m_d->sharedData = sharedData;

    enableJob(JOB_INIT, true);

    enableJob(JOB_DOSTROKE, true);
    enableJob(JOB_CANCEL, true);

    enableJob(JOB_SUSPEND, true, KisStrokeJobData::BARRIER);
    enableJob(JOB_RESUME, true, KisStrokeJobData::BARRIER);

    setNeedsExplicitCancel(true);
    setClearsRedoOnStart(false);
}

KisSuspendProjectionUpdatesStrokeStrategy::~KisSuspendProjectionUpdatesStrokeStrategy()
{
    qDeleteAll(m_d->executedCommands);
}

void KisSuspendProjectionUpdatesStrokeStrategy::initStrokeCallback()
{
    QVector<KisRunnableStrokeJobDataBase*> jobs;

    if (m_d->suspend) {
        jobs << new Private::UndoableData(new Private::SuspendUpdatesCommand(m_d.data()));
    } else {
        jobs << new Private::UndoableData(new Private::ResumeAndIssueGraphUpdatesCommand(m_d.data()));
        jobs << new Private::BlockUILodSync(true, this);
        jobs << new Private::UndoableData(new Private::StartBatchUIUpdatesCommand(this));
        jobs << new Private::UndoableData(new Private::EndBatchUIUpdatesCommand(this));
        jobs << new Private::BlockUILodSync(false, this);
    }

    runnableJobsInterface()->addRunnableJobs(jobs);
}

void KisSuspendProjectionUpdatesStrokeStrategy::doStrokeCallback(KisStrokeJobData *data)
{
    KisRunnableStrokeJobDataBase *runnable = dynamic_cast<KisRunnableStrokeJobDataBase*>(data);
    if (runnable) {
        runnable->run();

        if (Private::UndoableData *undoable = dynamic_cast<Private::UndoableData*>(data)) {
            Private::StrokeJobCommand *command =  undoable->m_command.release();
            m_d->executedCommands.append(command);
        }
    }
}

QList<KisStrokeJobData*> KisSuspendProjectionUpdatesStrokeStrategy::createSuspendJobsData(KisImageWSP )
{
    return QList<KisStrokeJobData*>();
}

QList<KisStrokeJobData*> KisSuspendProjectionUpdatesStrokeStrategy::createResumeJobsData(KisImageWSP )
{
    return QList<KisStrokeJobData*>();
}

KisSuspendProjectionUpdatesStrokeStrategy::SharedDataSP KisSuspendProjectionUpdatesStrokeStrategy::createSharedData()
{
    return toQShared(new SharedData());
}

void KisSuspendProjectionUpdatesStrokeStrategy::Private::tryFetchUsedUpdatesFilter(KisImageSP image)
{
    if (!this->sharedData->installedFilterCookie) return;

    KisProjectionUpdatesFilterSP filter = image->removeProjectionUpdatesFilter(image->currentProjectionUpdatesFilter());
    this->sharedData->installedFilterCookie = KisProjectionUpdatesFilterCookie();

    KIS_SAFE_ASSERT_RECOVER_RETURN(filter);

    QSharedPointer<Private::SuspendLod0Updates> localFilter =
        filter.dynamicCast<Private::SuspendLod0Updates>();

    KIS_SAFE_ASSERT_RECOVER_RETURN(localFilter);

    this->usedFilters.append(localFilter);
}

void KisSuspendProjectionUpdatesStrokeStrategy::Private::tryIssueRecordedDirtyRequests(KisImageSP image)
{
    Q_FOREACH (QSharedPointer<Private::SuspendLod0Updates> filter, usedFilters) {
        filter->notifyUpdates(image.data());

        if (!filter->explicitUIUpdateRequest().isEmpty()) {
            accumulatedDirtyRects.append(filter->explicitUIUpdateRequest());
        }
    }
    usedFilters.clear();
}

void KisSuspendProjectionUpdatesStrokeStrategy::cancelStrokeCallback()
{
    KisImageSP image = m_d->image.toStrongRef();
    if (!image) {
        return;
    }

    for (auto it = m_d->executedCommands.rbegin(); it != m_d->executedCommands.rend(); ++it) {
        (*it)->undo();
    }

    m_d->tryFetchUsedUpdatesFilter(image);

    if (m_d->haveDisabledGUILodSync) {
        image->signalRouter()->emitRequestLodPlanesSyncBlocked(false);
    }

    if (!m_d->suspend) {
        image->refreshGraphAsync();
    }
}

void KisSuspendProjectionUpdatesStrokeStrategy::suspendStrokeCallback()
{

    KIS_SAFE_ASSERT_RECOVER_NOOP(m_d->suspend ||
                                 !m_d->sanityResumingFinished ||
                                 (m_d->sanityResumingFinished &&
                                  m_d->usedFilters.isEmpty() &&
                                  m_d->accumulatedDirtyRects.isEmpty()));

    for (auto it = m_d->executedCommands.rbegin(); it != m_d->executedCommands.rend(); ++it) {
        (*it)->undo();
    }

    m_d->updatesEpoch++;
}

void KisSuspendProjectionUpdatesStrokeStrategy::resumeStrokeCallback()
{
    QVector<KisRunnableStrokeJobDataBase*> jobs;

    Q_FOREACH (Private::StrokeJobCommand *command, m_d->executedCommands) {
        jobs << new Private::UndoableData(command);
    }
    m_d->executedCommands.clear();

    runnableJobsInterface()->addRunnableJobs(jobs);
}
