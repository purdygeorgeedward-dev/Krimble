/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_roundmarkerop.h"

#include <cmath>
#include <memory>
#include <QRect>

#include <KoColorSpaceRegistry.h>
#include <KoColor.h>
#include <KoColorProfile.h>
#include <KoCompositeOpRegistry.h>

#include <kis_brush.h>
#include <kis_global.h>
#include <kis_paint_device.h>
#include <kis_painter.h>
#include <kis_image.h>
#include <kis_selection.h>
#include <kis_brush_based_paintop_settings.h>
#include <kis_cross_device_color_sampler.h>
#include <kis_fixed_paint_device.h>
#include <kis_lod_transform.h>
#include <kis_spacing_information.h>
#include "kis_marker_painter.h"
#include "kis_paintop_utils.h"

KisRoundMarkerOp::KisRoundMarkerOp(KisPaintOpSettingsSP settings, KisPainter* painter, KisNodeSP node, KisImageSP )
    : KisPaintOp(painter)
    , m_sizeOption(settings.data())
    , m_spacingOption(settings.data())
{
    Q_UNUSED(node);

    Q_ASSERT(settings);
    Q_ASSERT(painter);

    m_firstRun = true;
    m_lastRadius = 1.0;

    m_markerOption.read(settings.data());
}

KisRoundMarkerOp::~KisRoundMarkerOp()
{
}

KisSpacingInformation KisRoundMarkerOp::paintAt(const KisPaintInformation& info)
{
    if (!painter()->device()) {
        return KisSpacingInformation(1.0);
    }

    const qreal lodScale = KisLodTransform::lodToScale(painter()->device());
    const qreal scale = m_sizeOption.apply(info) * lodScale;
    const qreal rotation = 0;

    const qreal diameter = m_markerOption.diameter * scale;
    qreal radius = 0.5 * diameter;

    if (KisPaintOpUtils::checkSizeTooSmall(scale, diameter, diameter)) return KisSpacingInformation();
    KisDabShape shape(scale, 1.0, rotation);

    QPointF pos = info.pos();

    KisMarkerPainter gc(painter()->device(), painter()->paintColor());

    if (m_firstRun) {
        const QVector<QPointF> points =
            painter()->calculateAllMirroredPoints(pos);

        Q_FOREACH(const QPointF &pt, points) {
            gc.fillFullCircle(pt - QPointF(0.5, 0.5), radius);
        }
    } else {
        const QVector<QPair<QPointF, QPointF>> pairs =
            painter()->calculateAllMirroredPoints(qMakePair(m_lastPaintPos, pos));

        Q_FOREACH(const auto &pair, pairs) {
            gc.fillCirclesDiff(pair.first - QPointF(0.5, 0.5), m_lastRadius,
                               pair.second - QPointF(0.5, 0.5), radius);
        }
    }

    m_firstRun = false;
    m_lastPaintPos = pos;
    m_lastRadius = radius;

    QRectF dirtyRect(pos.x() - radius, pos.y() - radius,
                     2 * radius, 2 * radius);
    dirtyRect = kisGrowRect(dirtyRect, 1);

    const QVector<QRect> allDirtyRects =
        painter()->calculateAllMirroredRects(dirtyRect.toAlignedRect());

    painter()->addDirtyRects(allDirtyRects);

    KisSpacingInformation spacingInfo = computeSpacing(info, diameter);

    if (m_firstRun) {
        m_firstRun = false;
        return spacingInfo;
    }

    return spacingInfo;
}

KisSpacingInformation KisRoundMarkerOp::updateSpacingImpl(const KisPaintInformation &info) const
{
    const qreal lodScale = KisLodTransform::lodToScale(painter()->device());
    const qreal diameter = m_markerOption.diameter * m_sizeOption.apply(info) * lodScale;

    return computeSpacing(info, diameter);
}

KisSpacingInformation KisRoundMarkerOp::computeSpacing(const KisPaintInformation &info,
                                                       qreal diameter) const
{
    const qreal rotation = 0;
    const bool axesFlipped = false;

    qreal extraSpacingScale = 1.0;
    if (m_spacingOption.isChecked()) {
        extraSpacingScale = m_spacingOption.apply(info);
    }

    return KisPaintOpUtils::effectiveSpacing(diameter, diameter,
                                             extraSpacingScale, true, true, rotation, axesFlipped,
                                             m_markerOption.spacing,
                                             m_markerOption.useAutoSpacing,
                                             m_markerOption.autoSpacingCoeff,
                                             KisLodTransform::lodToScale(painter()->device()));
}
