/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <klocalizedstring.h>
#include "kis_node.h"
#include "kis_layer.h"
#include "kis_image.h"
#include "kis_selection_mask.h"
#include "kis_paint_layer.h"
#include "commands/kis_node_property_list_command.h"
#include "kis_undo_adapter.h"
#include "kis_layer_properties_icons.h"
#include "kis_command_ids.h"

#include <memory>

#include "kis_simple_stroke_strategy.h"
#include "kis_abstract_projection_plane.h"

namespace {

QSet<QString> changedProperties(const KisBaseNode::PropertyList &before,
                                const KisBaseNode::PropertyList &after)
{
    QSet<QString> changedIds;

    auto valueForId = [] (const QString &id, const KisBaseNode::PropertyList &list) {
        QVariant value;
        Q_FOREACH (const KisBaseNode::Property &prop, list) {
            if (prop.id == id) {
                value = prop.state;
                break;
            }

        }
        return value;
    };

    const KisBaseNode::PropertyList &list1 = before.size() >= after.size() ? before : after;
    const KisBaseNode::PropertyList &list2 = before.size() >= after.size() ? after : before;

    Q_FOREACH (const KisBaseNode::Property &prop, list1) {
        if (prop.state != valueForId(prop.id, list2)) {
            changedIds.insert(prop.id);
        }
    }

    return changedIds;
}

}

KisNodePropertyListCommand::KisNodePropertyListCommand(KisNodeSP node, KisBaseNode::PropertyList newPropertyList)
    : KisNodeCommand(kundo2_i18n("Property Changes"), node),
      m_newPropertyList(newPropertyList),
      m_oldPropertyList(node->sectionModelProperties())
{
}

void KisNodePropertyListCommand::redo()
{
    const KisBaseNode::PropertyList propsBefore = m_node->sectionModelProperties();
    const QSet<QString> changed = changedProperties(propsBefore, m_newPropertyList);
    if (changed.isEmpty()) return;

    const QRect oldExtent = m_node->projectionPlane()->tightUserVisibleBounds();
    m_node->setSectionModelProperties(m_newPropertyList);

    if (!propsWithNoUpdates().contains(changed)) {
        doUpdate(propsBefore, m_node->sectionModelProperties(), oldExtent | m_node->projectionPlane()->tightUserVisibleBounds());
    }
}

void KisNodePropertyListCommand::undo()
{
    const KisBaseNode::PropertyList propsBefore = m_node->sectionModelProperties();
    const QSet<QString> changed = changedProperties(propsBefore, m_oldPropertyList);
    if (changed.isEmpty()) return;

    const QRect oldExtent = m_node->projectionPlane()->tightUserVisibleBounds();
    m_node->setSectionModelProperties(m_oldPropertyList);

    if (!propsWithNoUpdates().contains(changed)) {
        doUpdate(propsBefore, m_node->sectionModelProperties(), oldExtent | m_node->projectionPlane()->tightUserVisibleBounds());
    }
}

int KisNodePropertyListCommand::id() const
{
    return KisCommandUtils::NodePropertyListCommandId;
}

bool KisNodePropertyListCommand::mergeWith(const KUndo2Command *command)
{
    const KisNodePropertyListCommand *other =
        dynamic_cast<const KisNodePropertyListCommand*>(command);

    if (other && other->m_node == m_node &&
        (changedProperties(m_oldPropertyList, m_newPropertyList).isEmpty() ||
         changedProperties(m_oldPropertyList, m_newPropertyList) ==
             changedProperties(other->m_oldPropertyList, other->m_newPropertyList))) {

        const QSet<QString> changedInTheMeantime =
            changedProperties(m_newPropertyList, other->m_oldPropertyList);

        KIS_SAFE_ASSERT_RECOVER_NOOP(changedInTheMeantime.isEmpty() ||
                                     (changedInTheMeantime.size() == 1 &&
                                      *changedInTheMeantime.begin() == KisLayerPropertiesIcons::colorOverlay.id()));

        m_newPropertyList = other->m_newPropertyList;
        return true;
    }

    return false;
}

bool KisNodePropertyListCommand::canMergeWith(const KUndo2Command *command) const
{
    const KisNodePropertyListCommand *other =
        dynamic_cast<const KisNodePropertyListCommand*>(command);

    return other && other->m_node == m_node &&
        (changedProperties(m_oldPropertyList, m_newPropertyList).isEmpty() ||
         changedProperties(m_oldPropertyList, m_newPropertyList) ==
             changedProperties(other->m_oldPropertyList, other->m_newPropertyList));
}

bool KisNodePropertyListCommand::canAnnihilateWith(const KUndo2Command *command) const
{
    const KisNodePropertyListCommand *other =
        dynamic_cast<const KisNodePropertyListCommand*>(command);

    return other && other->m_node == m_node &&
            changedProperties(m_oldPropertyList, other->m_newPropertyList).isEmpty();
}

bool checkOnionSkinChanged(const KisBaseNode::PropertyList &oldPropertyList,
                           const KisBaseNode::PropertyList &newPropertyList)
{
    return changedProperties(oldPropertyList, newPropertyList).contains(KisLayerPropertiesIcons::onionSkins.id());
}

void KisNodePropertyListCommand::doUpdate(const KisBaseNode::PropertyList &oldPropertyList,
                                          const KisBaseNode::PropertyList &newPropertyList,
                                          const QRect &totalUpdateExtent)
{
    if (oldPropertyList == newPropertyList) {
        return;
    }

    bool oldPassThroughValue = false;
    bool newPassThroughValue = false;

    bool oldVisibilityValue = false;
    bool newVisibilityValue = false;

    Q_FOREACH (const KisBaseNode::Property &prop, oldPropertyList) {
        if (prop.id == KisLayerPropertiesIcons::passThrough.id()) {
            oldPassThroughValue = prop.state.toBool();
        }
        if (prop.id == KisLayerPropertiesIcons::visible.id()) {
            oldVisibilityValue = prop.state.toBool();
        }
    }

    Q_FOREACH (const KisBaseNode::Property &prop, newPropertyList) {
        if (prop.id == KisLayerPropertiesIcons::passThrough.id()) {
            newPassThroughValue = prop.state.toBool();
        }
        if (prop.id == KisLayerPropertiesIcons::visible.id()) {
            newVisibilityValue = prop.state.toBool();
        }
    }

    if (oldPassThroughValue && !newPassThroughValue) {
        KisLayerSP layer(qobject_cast<KisLayer*>(m_node.data()));
        KisImageSP image = layer->image().toStrongRef();
        if (image) {
            image->refreshGraphAsync(layer);
        }
    } else if ((m_node->parent() && !oldPassThroughValue && newPassThroughValue) ||
               (oldPassThroughValue && newPassThroughValue &&
                !oldVisibilityValue && newVisibilityValue)) {

        KisLayerSP layer(qobject_cast<KisLayer*>(m_node->parent().data()));
        KisImageSP image = layer->image().toStrongRef();
        if (image) {
            image->refreshGraphAsync(layer);
        }
    } else if (checkOnionSkinChanged(oldPropertyList, newPropertyList)) {
        m_node->setDirtyDontResetAnimationCache(totalUpdateExtent);
    } else {
        m_node->setDirty(totalUpdateExtent);
    }
}

const QSet<QString>& KisNodePropertyListCommand::propsWithNoUpdates()
{
    static const QSet<QString> noUpdates {
        KisLayerPropertiesIcons::locked.id(),
        KisLayerPropertiesIcons::alphaLocked.id(),
        KisLayerPropertiesIcons::selectionActive.id(),
        KisLayerPropertiesIcons::colorLabelIndex.id(),
        KisLayerPropertiesIcons::colorizeNeedsUpdate.id(),
        KisLayerPropertiesIcons::openFileLayerFile.id(),
        KisLayerPropertiesIcons::layerError.id()
    };
    return noUpdates;
}

void KisNodePropertyListCommand::setNodePropertiesAutoUndo(KisNodeSP node, KisImageSP image, PropertyList proplist)
{
    const QSet<QString> properties = changedProperties(node->sectionModelProperties(), proplist);

    const bool undo = !properties.isEmpty() &&
        (properties.size() != 1 ||
         !KisLayerPropertiesIcons::isStatelessProperty(*properties.begin()));

    std::unique_ptr<KUndo2Command> cmd(new KisNodePropertyListCommand(node, proplist));

    if (undo) {
        image->undoAdapter()->addCommand(cmd.release());
    }
    else {

        struct SimpleLodResettingStroke : public KisSimpleStrokeStrategy {
            SimpleLodResettingStroke(KUndo2Command *cmd)
                : KisSimpleStrokeStrategy(QLatin1String("SimpleLodResettingStroke")),
                  m_cmd(cmd)
            {
                setClearsRedoOnStart(false);
                this->enableJob(JOB_INIT, true);
            }

            void initStrokeCallback() override {
                m_cmd->redo();
            }

        private:
            QScopedPointer<KUndo2Command> m_cmd;
        };

        KisStrokeId strokeId = image->startStroke(new SimpleLodResettingStroke(cmd.release()));
        image->endStroke(strokeId);
    }

}
