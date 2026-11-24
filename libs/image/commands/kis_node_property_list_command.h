/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_IMAGE_NODE_PROPERTY_LIST_COMMAND_H_
#define KIS_IMAGE_NODE_PROPERTY_LIST_COMMAND_H_

#include "kis_node_command.h"
#include "kis_base_node.h"
#include "commands_new/KisAsynchronouslyMergeableCommandInterface.h"

class KRITAIMAGE_EXPORT KisNodePropertyListCommand : public KisNodeCommand, public KisAsynchronouslyMergeableCommandInterface
{

public:
    KisNodePropertyListCommand(KisNodeSP node, KisBaseNode::PropertyList newPropertyList);

    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;
    bool canMergeWith(const KUndo2Command *command) const override;
    bool canAnnihilateWith(const KUndo2Command *other) const override;

    typedef KisBaseNode::PropertyList PropertyList;
    static void setNodePropertiesAutoUndo(KisNodeSP node, KisImageSP image, PropertyList proplist);

private:
    void doUpdate(const KisBaseNode::PropertyList &oldPropertyList,
                  const KisBaseNode::PropertyList &newPropertyList, const QRect &totalUpdateExtent);
    static const QSet<QString>& propsWithNoUpdates();
private:
    KisBaseNode::PropertyList m_newPropertyList;
    KisBaseNode::PropertyList m_oldPropertyList;
};
#endif
