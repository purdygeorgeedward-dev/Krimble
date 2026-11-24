/*
 *  SPDX-FileCopyrightText: 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *  SPDX-FileCopyrightText: 2010 Ricardo Cabello <hello@mrdoob.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_sketch_paintop.h"
#include "kis_sketch_paintop_settings.h"

#include <cmath>
#include <QRect>

#include <KoColor.h>
#include <KoColorSpace.h>

#include <kis_image.h>
#include <kis_debug.h>

#include <kis_global.h>
#include <kis_paint_device.h>
#include <kis_painter.h>
#include <kis_types.h>
#include <kis_paintop_plugin_utils.h>
#include <brushengine/kis_paintop.h>
#include <brushengine/kis_paint_information.h>
#include <kis_fixed_paint_device.h>

#include <kis_dab_cache.h>
#include "kis_lod_transform.h"
#include <KoResourceLoadResult.h>

#include <QtGlobal>

KisSketchPaintOp::KisSketchPaintOp(const KisPaintOpSettingsSP settings, KisPainter *painter, KisNodeSP node, KisImageSP image)
    : KisPaintOp(painter)
    , m_opacityOption(settings.data(), node)
    , m_sizeOption(settings.data())
    , m_rotationOption(settings.data())
    , m_rateOption(settings.data())
    , m_densityOption(settings.data())
    , m_lineWidthOption(settings.data())
    , m_offsetScaleOption(settings.data())
{
    Q_UNUSED(image);
    Q_UNUSED(node);

    m_airbrushOption.read(settings.data());
    m_sketchProperties.read(settings.data());
    m_brushOption.readOptionSetting(settings, settings->resourcesInterface(), settings->canvasResourcesInterface());

    m_brush = m_brushOption.brush();
    m_dabCache = new KisDabCache(m_brush);

    m_painter = 0;
    m_count = 0;
}

KisSketchPaintOp::~KisSketchPaintOp()
{
    delete m_painter;
    delete m_dabCache;
}

QList<KoResourceLoadResult> KisSketchPaintOp::prepareLinkedResources(const KisPaintOpSettingsSP settings, KisResourcesInterfaceSP resourcesInterface)
{
    KisBrushOptionProperties brushOption;
    return brushOption.prepareLinkedResources(settings, resourcesInterface);
}

void KisSketchPaintOp::drawConnection(const QPointF& start, const QPointF& end, double lineWidth)
{
    if (m_sketchProperties.antiAliasing) {
        if (lineWidth == 1.0) {
            m_painter->drawWuLine(start, end);
        }
        else {
            m_painter->drawLine(start, end, lineWidth, true);
        }
    }
    else {
        if (lineWidth == 1.0) {
            m_painter->drawDDALine(start, end);
        }
        else {
            m_painter->drawLine(start, end, lineWidth, false);
        }
    }
}

void KisSketchPaintOp::updateBrushMask(const KisPaintInformation& info, qreal scale, qreal rotation)
{
    QRect dstRect;
    m_maskDab = m_dabCache->fetchDab(m_dab->colorSpace(),
                                     painter()->paintColor(),
                                     info.pos(),
                                     KisDabShape(scale, 1.0, rotation),
                                     info, 1.0,
                                     &dstRect);

    m_brushBoundingBox = dstRect;
    m_hotSpot = QPointF(0.5 * m_brushBoundingBox.width(),
                        0.5 * m_brushBoundingBox.height());
}

void KisSketchPaintOp::paintLine(const KisPaintInformation &pi1, const KisPaintInformation &pi2,
                                 KisDistanceInformation *currentDistance)
{
    if (pi1.pos() == pi2.pos()) {
        KisPaintOp::paintLine(pi1, pi2, currentDistance);
    }
    else {
        doPaintLine(pi1, pi2);
    }
}
void KisSketchPaintOp::doPaintLine(const KisPaintInformation &pi1, const KisPaintInformation &pi2)
{
    if (!m_brush || !painter()) return;

    if (!m_dab) {
        m_dab = source()->createCompositionSourceDevice();
        m_painter = new KisPainter(m_dab);
        m_painter->setPaintColor(painter()->paintColor());
    }
    else {
        m_dab->clear();
    }

    QPointF prevMouse = pi1.pos();
    QPointF mousePosition = pi2.pos();
    m_points.append(mousePosition);

    const qreal lodAdditionalScale = KisLodTransform::lodToScale(painter()->device());
    const qreal scale = lodAdditionalScale * m_sizeOption.apply(pi2);
    if ((scale * m_brush->width()) <= 0.01 || (scale * m_brush->height()) <= 0.01) return;

    const qreal currentLineWidth = qMax(0.9, lodAdditionalScale * m_lineWidthOption.apply(pi2) * m_sketchProperties.lineWidth);

    const qreal currentOffsetScale = m_offsetScaleOption.apply(pi2) * m_sketchProperties.offset * 0.01;
    const double rotation = m_rotationOption.apply(pi2);
    const double currentProbability = m_densityOption.apply(pi2) * m_sketchProperties.probability;

    if (m_sketchProperties.makeConnection) {
        drawConnection(prevMouse, mousePosition, currentLineWidth);
    }

    qreal thresholdDistance = 0.0;

    if (m_count == 0 && m_sketchProperties.simpleMode) {
        updateBrushMask(pi2, 1.0, 0.0);
        m_radius = 0.5 * qMax(m_brush->width(), m_brush->height());
    }

    if (!m_sketchProperties.simpleMode) {
        updateBrushMask(pi2, scale, rotation);
        m_radius = qMax(m_maskDab->bounds().width(), m_maskDab->bounds().height()) * 0.5;
        thresholdDistance = pow(m_radius, 2);
    }

    if (m_sketchProperties.simpleMode) {
        thresholdDistance = pow(m_radius * scale, 2);
    }

    const qreal density = thresholdDistance * currentProbability;

    qreal probability = 1.0 - currentProbability;

    QColor painterColor = painter()->paintColor().toQColor();
    QColor randomColor;
    KoColor color(m_dab->colorSpace());

    int w = m_maskDab->bounds().width();
    quint8 opacityU8 = 0;
    quint8 * pixel;
    qreal distance;
    QPoint  positionInMask;
    QPointF diff;

    int size = m_points.size();
    for (int i = 0; i < size; i++) {
        diff = m_points.at(i) - mousePosition;
        distance = diff.x() * diff.x() + diff.y() * diff.y();

        bool makeConnection = false;
        if (m_sketchProperties.simpleMode) {
            if (distance < thresholdDistance) {
                makeConnection = true;
            }
        }
        else {
            if (m_brushBoundingBox.contains(m_points.at(i))) {
                positionInMask = (diff + m_hotSpot).toPoint();
                uint pos = ((positionInMask.y() * w + positionInMask.x()) * m_maskDab->pixelSize());
                if (pos < m_maskDab->allocatedPixels() * m_maskDab->pixelSize()) {
                    pixel = m_maskDab->data() + pos;
                    opacityU8 = m_maskDab->colorSpace()->opacityU8(pixel);
                    if (opacityU8 != 0) {
                        makeConnection = true;
                    }
                }
            }

        }

        if (!makeConnection) {
            continue;
        }

        if (m_sketchProperties.distanceDensity) {
            probability =  distance / density;
        }

        KisRandomSourceSP randomSource = pi2.randomSource();

        if (randomSource->generateNormalized() >= probability) {
            QPointF offsetPt = diff * currentOffsetScale;

            if (m_sketchProperties.randomRGB) {
                qreal r1 = randomSource->generateNormalized();
                qreal r2 = randomSource->generateNormalized();
                qreal r3 = randomSource->generateNormalized();

                randomColor.setRgbF(r1 * painterColor.redF(),
                                    r2 * painterColor.greenF(),
                                    r3 * painterColor.blueF());
                color.fromQColor(randomColor);
                m_painter->setPaintColor(color);
            }

            qreal opacity = OPACITY_OPAQUE_F;
            if (m_sketchProperties.distanceOpacity) {
                opacity *= qRound((1.0 - (distance / thresholdDistance)));
            }

            if (m_sketchProperties.randomOpacity) {
                opacity *= randomSource->generateNormalized();
            }

            m_painter->setOpacityF(opacity);

            if (m_sketchProperties.magnetify) {
                drawConnection(mousePosition + offsetPt, m_points.at(i) - offsetPt, currentLineWidth);
            }
            else {
                drawConnection(mousePosition + offsetPt, mousePosition - offsetPt, currentLineWidth);
            }

        }
    }

    m_count++;

    QRect rc = m_dab->extent();
    m_opacityOption.apply(painter(), pi2);

    painter()->bitBlt(rc.x(), rc.y(), m_dab, rc.x(), rc.y(), rc.width(), rc.height());
    painter()->renderMirrorMask(rc, m_dab);
}

KisSpacingInformation KisSketchPaintOp::paintAt(const KisPaintInformation& info)
{
    doPaintLine(info, info);
    return updateSpacingImpl(info);
}

KisSpacingInformation KisSketchPaintOp::updateSpacingImpl(const KisPaintInformation &info) const
{
    return KisPaintOpPluginUtils::effectiveSpacing(0.0, 0.0, true, 0.0, false, 0.0, false, 0.0,
                                                   KisLodTransform::lodToScale(painter()->device()),
                                                   &m_airbrushOption, nullptr, info);
}

KisTimingInformation KisSketchPaintOp::updateTimingImpl(const KisPaintInformation &info) const
{
    return KisPaintOpPluginUtils::effectiveTiming(&m_airbrushOption, &m_rateOption, info);
}
