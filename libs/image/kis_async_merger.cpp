/* SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "kis_async_merger.h"

#include <kis_debug.h>

#include <KoChannelInfo.h>
#include <KoCompositeOpRegistry.h>

#include "kis_node_visitor.h"
#include "kis_painter.h"
#include "kis_layer.h"
#include "kis_group_layer.h"
#include "kis_adjustment_layer.h"
#include "generator/kis_generator_layer.h"
#include "kis_external_layer_iface.h"
#include "kis_paint_layer.h"
#include "filter/kis_filter.h"
#include "filter/kis_filter_configuration.h"
#include "filter/kis_filter_registry.h"
#include "kis_selection.h"
#include "kis_clone_layer.h"
#include "kis_processing_information.h"
#include "kis_busy_progress_indicator.h"

#include "kis_merge_walker.h"
#include "kis_refresh_subtree_walker.h"

#include "kis_abstract_projection_plane.h"

#ifdef DEBUG_MERGER
#define DEBUG_NODE_ACTION(message, type, leaf, rect)            \
    qDebug() << message << type << ":" << leaf->node()->name() << rect << "rendered:" << leaf->shouldBeRendered()
#else
#define DEBUG_NODE_ACTION(message, type, leaf, rect)
#endif

class KisUpdateOriginalVisitor : public KisNodeVisitor
{
public:
    KisUpdateOriginalVisitor(const QRect &updateRect, KisPaintDeviceSP projection)
        : m_updateRect(updateRect),
          m_projection(projection)
        {
        }

    ~KisUpdateOriginalVisitor() override {
    }

public:
    using KisNodeVisitor::visit;

    bool visit(KisAdjustmentLayer* layer) override {
        if (!layer->visible()) return true;

        if (!m_projection) {
            warnImage << "ObligeChild mechanism has been activated for "
                "an adjustment layer! Do nothing...";
            layer->original()->clear();
            return true;
        }

        const QRect originalUpdateRect =
            layer->projectionPlane()->needRectForOriginal(m_updateRect);

        KisPaintDeviceSP originalDevice = layer->original();
        originalDevice->clear(originalUpdateRect);

        const QRect applyRect = originalUpdateRect & m_projection->extent();

        if(applyRect.isNull()) return true;

        KisFilterConfigurationSP filterConfig = layer->filter();
        if (!filterConfig) {
            KisPainter::copyAreaOptimized(applyRect.topLeft(), m_projection, originalDevice, applyRect);
            return true;
        }

        KisSelectionSP selection = layer->fetchComposedInternalSelection(applyRect);
        const QRect filterRect = selection ? applyRect & selection->selectedRect() : applyRect;

        KisFilterSP filter = KisFilterRegistry::instance()->value(filterConfig->name());
        if (!filter) return false;

        KisPaintDeviceSP dstDevice = originalDevice;

        if (selection) {
            dstDevice = new KisPaintDevice(originalDevice->colorSpace());
        }

        if (!filterRect.isEmpty()) {
            KIS_ASSERT_RECOVER_NOOP(layer->busyProgressIndicator());
            layer->busyProgressIndicator()->update();

            filter->process(m_projection, dstDevice, 0, filterRect, filterConfig.data(), 0);
        }

        if (selection) {
            KisPainter::copyAreaOptimized(applyRect.topLeft(), m_projection, originalDevice, applyRect);
            KisPainter::copyAreaOptimized(filterRect.topLeft(), dstDevice, originalDevice, filterRect, selection);
        }

        return true;
    }

    bool visit(KisExternalLayer*) override {
        return true;
    }

    bool visit(KisGeneratorLayer*) override {
        return true;
    }

    bool visit(KisPaintLayer*) override {
        return true;
    }

    bool visit(KisGroupLayer*) override {
        return true;
    }

    bool visit(KisCloneLayer*) override {
        return true;
    }

    bool visit(KisNode*) override {
        return true;
    }
    bool visit(KisFilterMask*) override {
        return true;
    }
    bool visit(KisTransformMask*) override {
        return true;
    }
    bool visit(KisTransparencyMask*) override {
        return true;
    }
    bool visit(KisSelectionMask*) override {
        return true;
    }
    bool visit(KisColorizeMask*) override {
        return true;
    }

private:
    QRect m_updateRect;
    KisPaintDeviceSP m_projection;
};

void KisAsyncMerger::startMerge(KisBaseRectsWalker &walker, bool notifyClones) {
    KisMergeWalker::LeafStack &leafStack = walker.leafStack();

    const bool useTempProjections = walker.needRectVaries();

    while(!leafStack.isEmpty()) {
        KisMergeWalker::JobItem item = leafStack.pop();
        KisProjectionLeafSP currentLeaf = item.m_leaf;

        KIS_SAFE_ASSERT_RECOVER_RETURN(currentLeaf);
        KIS_SAFE_ASSERT_RECOVER_RETURN(currentLeaf->node());

        KIS_SAFE_ASSERT_RECOVER_RETURN(currentLeaf->isLayer());

        QRect applyRect = item.m_applyRect;

        if (currentLeaf->isRoot()) {
            currentLeaf->projectionPlane()->recalculate(applyRect, walker.startNode(), item.m_renderFlags);
            continue;
        }

        if(item.m_position & KisMergeWalker::N_EXTRA) {

            DEBUG_NODE_ACTION("Updating", "N_EXTRA", currentLeaf, applyRect);
            KisUpdateOriginalVisitor originalVisitor(applyRect,
                                                     m_currentProjection);
            currentLeaf->accept(originalVisitor);
            currentLeaf->projectionPlane()->recalculate(applyRect, currentLeaf->node(), item.m_renderFlags);

            continue;
        }

        if (!m_currentProjection) {
            setupProjection(currentLeaf, applyRect, useTempProjections);
        }

        KisUpdateOriginalVisitor originalVisitor(applyRect,
                                                 m_currentProjection);

        if(item.m_position & KisMergeWalker::N_FILTHY) {
            DEBUG_NODE_ACTION("Updating", "N_FILTHY", currentLeaf, applyRect);
            if (currentLeaf->shouldBeRendered()) {
                currentLeaf->accept(originalVisitor);
                currentLeaf->projectionPlane()->recalculate(applyRect, walker.startNode(), item.m_renderFlags);
            }
        }
        else if(item.m_position & KisMergeWalker::N_ABOVE_FILTHY) {
            DEBUG_NODE_ACTION("Updating", "N_ABOVE_FILTHY", currentLeaf, applyRect);
            if(currentLeaf->dependsOnLowerNodes()) {
                if (currentLeaf->shouldBeRendered()) {
                    currentLeaf->accept(originalVisitor);
                    currentLeaf->projectionPlane()->recalculate(applyRect, currentLeaf->node(), item.m_renderFlags);
                }
            }
        }
        else if(item.m_position & KisMergeWalker::N_FILTHY_PROJECTION) {
            DEBUG_NODE_ACTION("Updating", "N_FILTHY_PROJECTION", currentLeaf, applyRect);
            if (currentLeaf->shouldBeRendered()) {
                currentLeaf->projectionPlane()->recalculate(applyRect, walker.startNode(), item.m_renderFlags);
            }
        }
        else {
            DEBUG_NODE_ACTION("Updating", "N_BELOW_FILTHY", currentLeaf, applyRect);
        }

        compositeWithProjection(currentLeaf, applyRect);

        if(item.m_position & KisMergeWalker::N_TOPMOST) {
            writeProjection(currentLeaf, useTempProjections, applyRect);
            resetProjection();
        }

        Q_ASSERT(currentLeaf->projection()->defaultBounds()->currentLevelOfDetail() ==
                 walker.levelOfDetail());
    }

    if(notifyClones) {
        doNotifyClones(walker);
    }

    if(m_currentProjection) {
        warnImage << "BUG: The walker hasn't reached the root layer!";
        warnImage << "     Start node:" << walker.startNode() << "Requested rect:" << walker.requestedRect();
        warnImage << "     An inconsistency in the walkers occurred!";
        warnImage << "     Please report a bug describing how you got this message.";
        resetProjection();
    }
}

void KisAsyncMerger::resetProjection() {
    m_currentProjection = 0;
    m_finalProjection = 0;
}

void KisAsyncMerger::setupProjection(KisProjectionLeafSP currentLeaf, const QRect& rect, bool useTempProjection) {
    KisPaintDeviceSP parentOriginal = currentLeaf->parent()->lazyDestinationForSubtreeComposition();

    if (parentOriginal) {
        if (useTempProjection) {
            if(!m_cachedPaintDevice)
                m_cachedPaintDevice = new KisPaintDevice(parentOriginal->colorSpace());
            m_currentProjection = m_cachedPaintDevice;
            m_currentProjection->prepareClone(parentOriginal);
            m_finalProjection = parentOriginal;
        }
        else {
            parentOriginal->clear(rect);
            m_finalProjection = m_currentProjection = parentOriginal;
        }
    }
    else {
    }
}

void KisAsyncMerger::writeProjection(KisProjectionLeafSP topmostLeaf, bool useTempProjection, const QRect &rect) {
    Q_UNUSED(useTempProjection);
    Q_UNUSED(topmostLeaf);
    if (!m_currentProjection) return;

    if(m_currentProjection != m_finalProjection) {
        KisPainter::copyAreaOptimized(rect.topLeft(), m_currentProjection, m_finalProjection, rect);
    }
    DEBUG_NODE_ACTION("Writing projection", "", topmostLeaf->parent(), rect);
}

bool KisAsyncMerger::compositeWithProjection(KisProjectionLeafSP leaf, const QRect &rect) {

    if (!m_currentProjection) return true;
    if (!leaf->visible()) return true;

    KisPainter gc(m_currentProjection);
    leaf->projectionPlane()->apply(&gc, rect);

    DEBUG_NODE_ACTION("Compositing projection", "", leaf, rect);
    return true;
}

void KisAsyncMerger::doNotifyClones(KisBaseRectsWalker &walker) {
    KisBaseRectsWalker::CloneNotificationsVector &vector =
        walker.cloneNotifications();

    KisBaseRectsWalker::CloneNotificationsVector::iterator it;

    for(it = vector.begin(); it != vector.end(); ++it) {
        (*it).notify();
    }
}
