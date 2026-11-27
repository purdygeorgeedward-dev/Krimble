/*
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_adjustment_layer.h"

#include <klocalizedstring.h>
#include "kis_debug.h"

#include <KoIcon.h>
#include <kis_icon.h>
#include <KoCompositeOpRegistry.h>

#include "kis_image.h"
#include "kis_selection.h"
#include "filter/kis_filter_configuration.h"
#include "filter/kis_filter_registry.h"
#include "filter/kis_filter.h"
#include "kis_node_visitor.h"
#include "kis_processing_visitor.h"

KisAdjustmentLayer::KisAdjustmentLayer(KisImageWSP image,
                                       const QString &name,
                                       KisFilterConfigurationSP kfc,
                                       KisSelectionSP selection)
    : KisSelectionBasedLayer(image.data(), name, selection, kfc)
{

    setCompositeOpId(COMPOSITE_COPY);
    setUseSelectionInProjection(false);
}

KisAdjustmentLayer::KisAdjustmentLayer(const KisAdjustmentLayer& rhs)
        : KisSelectionBasedLayer(rhs)
{
}

KisAdjustmentLayer::~KisAdjustmentLayer()
{
}

void KisAdjustmentLayer::setFilter(KisFilterConfigurationSP filterConfig, bool checkCompareConfig)
{
    filterConfig->setChannelFlags(channelFlags());
    KisSelectionBasedLayer::setFilter(filterConfig, checkCompareConfig);
}

QRect KisAdjustmentLayer::incomingChangeRect(const QRect &rect) const
{
    KisFilterConfigurationSP filterConfig = filter();

    QRect filteredRect = rect;

    if (filterConfig) {
        KisFilterSP filter = KisFilterRegistry::instance()->value(filterConfig->name());
        filteredRect = filter->changedRect(rect, filterConfig.data(), projection()->defaultBounds()->currentLevelOfDetail());
    }

    return filteredRect;
}

QRect KisAdjustmentLayer::needRect(const QRect& rect, PositionToFilthy pos) const
{
    Q_UNUSED(pos);

    KisFilterConfigurationSP filterConfig = filter();
    if (!filterConfig) return rect;
    KisFilterSP filter = KisFilterRegistry::instance()->value(filterConfig->name());

    QRect needRect = rect;
    needRect |= needRectForOriginal(needRect);
    needRect = filter->neededRect(needRect, filterConfig.data(), projection()->defaultBounds()->currentLevelOfDetail());
    return needRect;
}

bool KisAdjustmentLayer::accept(KisNodeVisitor & v)
{
    return v.visit(this);
}

void KisAdjustmentLayer::accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter)
{
    return visitor.visit(this, undoAdapter);
}

QIcon KisAdjustmentLayer::icon() const
{
    return KisIconUtils::loadIcon("filterLayer");
}

KisBaseNode::PropertyList KisAdjustmentLayer::sectionModelProperties() const
{
    KisFilterConfigurationSP filterConfig = filter();
    KisBaseNode::PropertyList l = KisLayer::sectionModelProperties();
    if (filterConfig)
        l << KisBaseNode::Property(KoID("filter", i18nc("property of a filter layer, noun", "Filter")), KisFilterRegistry::instance()->value(filterConfig->name())->name());

    return l;
}

void KisAdjustmentLayer::setChannelFlags(const QBitArray & channelFlags)
{
    KisFilterConfigurationSP filterConfig = filter();

    if (filterConfig) {
        filterConfig->setChannelFlags(channelFlags);
    }
    KisLayer::setChannelFlags(channelFlags);
}

