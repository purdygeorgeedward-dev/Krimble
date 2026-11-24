/*
 *  SPDX-FileCopyrightText: 2024 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_base_rects_walker.h"
#include "kis_clone_layer.h"

void KisBaseRectsWalker::addCloneSourceRegenerationJobs()
{
    QVector<JobItem> cloneLayers;
    std::copy_if(m_mergeTask.begin(), m_mergeTask.end(),
                 std::back_inserter(cloneLayers),
                 [] (const JobItem &item) {
                     return item.m_leaf->node()->inherits("KisCloneLayer") &&
                         (item.m_position & (N_FILTHY | N_ABOVE_FILTHY | N_EXTRA));
                 });

    for (auto it = cloneLayers.begin(); it != cloneLayers.end(); ++it) {
        KisCloneLayer *clone = dynamic_cast<KisCloneLayer*>(it->m_leaf->node().data());

        KisNodeSP source = clone->copyFrom();
        if (!source) continue;

        QVector<QRect> preparedRects;

        if (!m_cropRect.isEmpty()) {
            preparedRects << m_cropRect;
        }

        Q_FOREACH (const JobItem &job, m_mergeTask) {
            if (job.m_leaf->node() == source &&
                ((job.m_position & (N_FILTHY | N_EXTRA)) ||
                 (job.m_leaf->dependsOnLowerNodes() && (job.m_position & N_ABOVE_FILTHY)))) {
                preparedRects << job.m_applyRect;
            }
        }

        const QRect srcRect = it->m_applyRect.translated(-clone->x(), -clone->y());

        QRegion prepareRegion(srcRect);

        const QRect needRectOnSource = clone->needRectOnSourceForMasks(srcRect);
        if (!needRectOnSource.isEmpty()) {
            prepareRegion += needRectOnSource;
        }

        Q_FOREACH (const QRect &rc, preparedRects) {
            prepareRegion -= rc;
        }

        for (auto it = prepareRegion.begin(); it != prepareRegion.end(); ++it) {
            pushJob(source->projectionLeaf(), N_EXTRA, *it, KisRenderPassFlag::NoTransformMaskUpdates);
            visitSubtreeTopToBottom(source->projectionLeaf(),
                                    SkipNonRenderableNodes | DontNotifyClones,
                                    KisRenderPassFlag::NoTransformMaskUpdates,
                                    QRect());
        }
    }
}

void KisBaseRectsWalker::visitSubtreeTopToBottom(KisProjectionLeafSP startWith, SubtreeVisitFlags flags,
                                                 KisRenderPassFlags renderFlags, const QRect &cropRect)
{
    if (flags & SkipNonRenderableNodes && !startWith->shouldBeRendered()) return;

    KisProjectionLeafSP currentLeaf = startWith->lastChild();
    while(currentLeaf) {
        NodePosition pos = (flags & NoFilthyMode ? N_ABOVE_FILTHY : N_FILTHY) |
            calculateNodePosition(currentLeaf);
        registerNeedRect(currentLeaf, pos, renderFlags, cropRect);

        if (!flags.testFlag(DontNotifyClones)) {
            registerCloneNotification(currentLeaf->node(), pos);
        }

        currentLeaf = currentLeaf->prevSibling();
    }

    if (flags & NoFilthyMode) return;

    currentLeaf = startWith->lastChild();
    while(currentLeaf) {
        if(currentLeaf->canHaveChildLayers()) {
            visitSubtreeTopToBottom(currentLeaf, flags, renderFlags, cropRect);
        }
        currentLeaf = currentLeaf->prevSibling();
    }
}
