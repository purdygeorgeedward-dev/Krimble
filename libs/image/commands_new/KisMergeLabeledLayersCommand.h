/*
 *  SPDX-FileCopyrightText: 2020 Agata Cacko <cacko.azh@gmail.com>
 *  SPDX-FileCopyrightText: 2022 Deif Lou <ginoba@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_MERGE_LABELED_LAYERS_H
#define __KIS_MERGE_LABELED_LAYERS_H

#include <QList>
#include <QString>
#include <QSharedPointer>

#include "kundo2command.h"
#include "kritaimage_export.h"
#include "kis_types.h"
#include "kis_image.h"

class KisUpdatesFacade;

class KRITAIMAGE_EXPORT KisMergeLabeledLayersCommand : public KUndo2Command
{
public:
    enum GroupSelectionPolicy
    {
        GroupSelectionPolicy_SelectAlways,
        GroupSelectionPolicy_SelectIfColorLabeled,
        GroupSelectionPolicy_NeverSelect
    };

    struct ReferenceNodeInfo
    {
        QUuid nodeId;
        int sequenceNumber;
        int opacity;

        bool operator==(const ReferenceNodeInfo &other) const
        {
            return nodeId == other.nodeId && sequenceNumber == other.sequenceNumber && opacity == other.opacity;
        }
    };

    using ReferenceNodeInfoList = QList<ReferenceNodeInfo>;
    using ReferenceNodeInfoListSP = QSharedPointer<ReferenceNodeInfoList>;

    KisMergeLabeledLayersCommand(KisImageSP image,
                                 KisPaintDeviceSP newRefPaintDevice,
                                 QList<int> selectedLabels,
                                 GroupSelectionPolicy groupSelectionPolicy = GroupSelectionPolicy_SelectAlways);

    KisMergeLabeledLayersCommand(KisImageSP image,
                                 ReferenceNodeInfoListSP prevRefNodeInfoList,
                                 ReferenceNodeInfoListSP newRefNodeInfoList,
                                 KisPaintDeviceSP prevRefPaintDevice,
                                 KisPaintDeviceSP newRefPaintDevice,
                                 QList<int> selectedLabels,
                                 GroupSelectionPolicy groupSelectionPolicy = GroupSelectionPolicy_SelectAlways,
                                 bool forceRegeneration = false,
                                 KisNodeSP activeNode = nullptr);

    ~KisMergeLabeledLayersCommand() override;

    void undo() override;
    void redo() override;

    static KisPaintDeviceSP createRefPaintDevice(KisImageSP originalImage, QString name = "Merge Labeled Layers Reference Paint Device");

private:
    void mergeLabeledLayers();
    bool hasToCheckForChangesInNodes() const;
    QPair<KisNodeSP, QPair<bool, bool>> collectNode(KisNodeSP node) const;
    bool collectNodes(KisNodeSP node, QList<KisNodeSP> &nodeList, ReferenceNodeInfoList &nodeInfoList) const;

private:
    KisImageSP m_refImage;
    ReferenceNodeInfoListSP m_prevRefNodeInfoList;
    ReferenceNodeInfoListSP m_newRefNodeInfoList;
    KisPaintDeviceSP m_prevRefPaintDevice;
    KisPaintDeviceSP m_newRefPaintDevice;
    KisNodeSP m_currentRoot;
    QList<int> m_selectedLabels;
    GroupSelectionPolicy m_groupSelectionPolicy;
    bool m_forceRegeneration;
    KisNodeSP m_activeNode;
};

#endif
