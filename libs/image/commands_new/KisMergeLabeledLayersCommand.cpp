/*
 *  SPDX-FileCopyrightText: 2020 Agata Cacko <cacko.azh@gmail.com>
 *  SPDX-FileCopyrightText: 2022 Deif Lou <ginoba@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisMergeLabeledLayersCommand.h"

#include "KoCompositeOpRegistry.h"

#include "kis_layer_utils.h"
#include "kis_node.h"
#include "kis_image.h"
#include "kis_painter.h"
#include "kis_layer.h"
#include "kis_clone_layer.h"
#include "kis_paint_layer.h"
#include "kis_assert.h"
#include "KisDeleteLaterWrapper.h"
#include <kis_image_animation_interface.h>

KisMergeLabeledLayersCommand::KisMergeLabeledLayersCommand(KisImageSP image,
                                                           KisPaintDeviceSP newRefPaintDevice,
                                                           QList<int> selectedLabels,
                                                           GroupSelectionPolicy groupSelectionPolicy)
    : KUndo2Command(kundo2_noi18n("MERGE_LABELED_LAYERS"))
    , m_refImage(new KisImage(new KisSurrogateUndoStore(), image->width(), image->height(), image->colorSpace(), "Merge Labeled Layers Reference Image"))
    , m_prevRefNodeInfoList(nullptr)
    , m_newRefNodeInfoList(nullptr)
    , m_prevRefPaintDevice(nullptr)
    , m_newRefPaintDevice(newRefPaintDevice)
    , m_currentRoot(image->root())
    , m_selectedLabels(selectedLabels)
    , m_groupSelectionPolicy(groupSelectionPolicy)
    , m_forceRegeneration(true)
    , m_activeNode(nullptr)
{
    KIS_ASSERT(newRefPaintDevice);
    if (image->animationInterface()->hasAnimation()) {
        m_refImage->animationInterface()->switchCurrentTimeAsync(image->animationInterface()->currentTime());
        m_refImage->waitForDone();
    }
}

KisMergeLabeledLayersCommand::KisMergeLabeledLayersCommand(KisImageSP image,
                                                           ReferenceNodeInfoListSP prevRefNodeInfoList,
                                                           ReferenceNodeInfoListSP newRefNodeInfoList,
                                                           KisPaintDeviceSP prevRefPaintDevice,
                                                           KisPaintDeviceSP newRefPaintDevice,
                                                           QList<int> selectedLabels,
                                                           GroupSelectionPolicy groupSelectionPolicy,
                                                           bool forceRegeneration,
                                                           KisNodeSP activeNode)
    : KUndo2Command(kundo2_noi18n("MERGE_LABELED_LAYERS"))
    , m_refImage(new KisImage(new KisSurrogateUndoStore(), image->width(), image->height(), image->colorSpace(), "Merge Labeled Layers Reference Image"))
    , m_prevRefNodeInfoList(prevRefNodeInfoList)
    , m_newRefNodeInfoList(newRefNodeInfoList)
    , m_prevRefPaintDevice(prevRefPaintDevice)
    , m_newRefPaintDevice(newRefPaintDevice)
    , m_currentRoot(image->root())
    , m_selectedLabels(selectedLabels)
    , m_groupSelectionPolicy(groupSelectionPolicy)
    , m_forceRegeneration(forceRegeneration)
    , m_activeNode(activeNode)
{
    KIS_SAFE_ASSERT_RECOVER_NOOP(prevRefNodeInfoList);
    KIS_SAFE_ASSERT_RECOVER_NOOP(newRefNodeInfoList);
    KIS_SAFE_ASSERT_RECOVER_NOOP(prevRefPaintDevice);
    KIS_ASSERT(newRefPaintDevice);
    if (image->animationInterface()->hasAnimation()) {
        m_refImage->animationInterface()->switchCurrentTimeAsync(image->animationInterface()->currentTime());
        m_refImage->waitForDone();
    }
}

KisMergeLabeledLayersCommand::~KisMergeLabeledLayersCommand()
{}

void KisMergeLabeledLayersCommand::undo()
{
    KUndo2Command::undo();
}

void KisMergeLabeledLayersCommand::redo()
{
    if (m_refImage) {
        mergeLabeledLayers();
    }
    KUndo2Command::redo();
}

KisPaintDeviceSP KisMergeLabeledLayersCommand::createRefPaintDevice(KisImageSP originalImage, QString name)
{
    return KisPaintDeviceSP(new KisPaintDevice(originalImage->colorSpace(), name));
}

QPair<KisNodeSP, QPair<bool, bool>> KisMergeLabeledLayersCommand::collectNode(KisNodeSP node) const
{
    if (!node->parent()) {
        return {nullptr, {false, true}};
    }

    if (node->inherits("KisMask")) {
        return {nullptr, {true, false}};
    }

    if (!node->visible()) {
        return {nullptr, {true, false}};
    }

    if (!m_selectedLabels.contains(node->colorLabelIndex())) {
        if (!(m_activeNode != nullptr && node->uuid() == m_activeNode->uuid())) {
            return {nullptr, {true, node->inherits("KisGroupLayer")}};
        }
    }

    if (node->inherits("KisCloneLayer")) {
        KisCloneLayerSP cloneLayer = dynamic_cast<KisCloneLayer*>(node.data());
        KisNodeSP transformedNode = cloneLayer->reincarnateAsPaintLayer();
        return {transformedNode, {true, false}};
    }

    if (node->inherits("KisAdjustmentLayer")) {
        KisPaintDeviceSP proj = new KisPaintDevice(*(node->projection()));
        KisPaintLayerSP transformedNode = new KisPaintLayer(node->image(), node->name(), node->opacity(), proj);
        transformedNode->setX(transformedNode->x() + node->x());
        transformedNode->setY(transformedNode->y() + node->y());
        transformedNode->mergeNodeProperties(node->nodeProperties());
        return {transformedNode, {false, false}};
    }

    if (node->inherits("KisGroupLayer")) {
        if (m_groupSelectionPolicy == GroupSelectionPolicy_NeverSelect ||
            (m_groupSelectionPolicy == GroupSelectionPolicy_SelectIfColorLabeled && node->colorLabelIndex() == 0)) {
            return {nullptr, {true, true}};
        } else {
            return {node, {true, false}};
        }
    }

    return {node, {true, false}};
}

bool KisMergeLabeledLayersCommand::collectNodes(KisNodeSP node, QList<KisNodeSP> &nodeList, ReferenceNodeInfoList &nodeInfoList) const
{
    QPair<KisNodeSP, QPair<bool, bool>> result = collectNode(node);
    KisNodeSP collectedNode = result.first;
    const bool visitNextSibling = result.second.first;
    const bool visitChildren = result.second.second;

    if (collectedNode) {
        nodeList << collectedNode;
        if (hasToCheckForChangesInNodes()) {
            const QUuid uuid = node->uuid();
            const int sequenceNumber = node->projection()->sequenceNumber();
            const int opacity = node->opacity();
            nodeInfoList.append({uuid, sequenceNumber, opacity});
        }
    }

    if (visitChildren) {
        node = node->lastChild();
        while (node) {
            const bool mustVisitNextSibling = collectNodes(node, nodeList, nodeInfoList);
            if (!mustVisitNextSibling) {
                break;
            }
            node = node->prevSibling();
        }
    }

    return visitNextSibling;
}

void KisMergeLabeledLayersCommand::mergeLabeledLayers()
{
    QList<KisNodeSP> currentNodesList;
    ReferenceNodeInfoList currentNodeInfoList;

    collectNodes(m_currentRoot, currentNodesList, currentNodeInfoList);

    if (hasToCheckForChangesInNodes()) {
        *m_newRefNodeInfoList = currentNodeInfoList;
    }

    if (hasToCheckForChangesInNodes() && !m_forceRegeneration && (currentNodeInfoList == *m_prevRefNodeInfoList)) {
        m_newRefPaintDevice->prepareClone(m_prevRefPaintDevice);
        m_newRefPaintDevice->makeCloneFromRough(m_prevRefPaintDevice, m_prevRefPaintDevice->extent());
    } else {
        QList<KisNodeSP> currentNodesListCopy;
        for (KisNodeSP node : currentNodesList) {
            KisNodeSP copy = node->clone();

            if (copy.isNull()) {
                continue;
            }

            if (copy->inherits("KisLayer")) {
                KisLayer* layerCopy = dynamic_cast<KisLayer*>(copy.data());
                KIS_ASSERT(layerCopy);
                layerCopy->setChannelFlags(QBitArray());
            }

            copy->setCompositeOpId(COMPOSITE_OVER);

            bool success = m_refImage->addNode(copy, m_refImage->root(), 0);

            if (!success) {
                continue;
            }

            currentNodesListCopy << copy;
        }

        currentNodesListCopy = KisLayerUtils::sortAndFilterAnyMergeableNodesSafe(currentNodesListCopy, m_refImage);

        m_refImage->initialRefreshGraph();
        KisLayerUtils::refreshHiddenAreaAsync(m_refImage, m_refImage->root(), m_refImage->bounds());
        m_refImage->waitForDone();

        if (m_refImage->root()->childCount() == 0) {
            return;
        }

        m_refImage->waitForDone();
        KisLayerUtils::mergeMultipleNodes(m_refImage, currentNodesListCopy, 0, KisLayerUtils::SkipMergingFrames);
        m_refImage->waitForDone();

        KisPainter::copyAreaOptimized(m_refImage->projection()->exactBounds().topLeft(), m_refImage->projection(), m_newRefPaintDevice, m_refImage->projection()->exactBounds());
    }

    m_prevRefPaintDevice.clear();
    m_newRefPaintDevice.clear();
    m_currentRoot.clear();
    m_prevRefNodeInfoList.clear();
    m_newRefNodeInfoList.clear();

    makeKisDeleteLaterWrapper(m_refImage)->deleteLater();
    m_refImage.clear();
}

bool KisMergeLabeledLayersCommand::hasToCheckForChangesInNodes() const
{
    return m_prevRefNodeInfoList && m_newRefNodeInfoList && m_prevRefPaintDevice;
}
