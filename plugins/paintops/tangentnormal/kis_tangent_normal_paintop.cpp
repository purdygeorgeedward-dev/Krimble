/*
 *  SPDX-FileCopyrightText: 2015 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tangent_normal_paintop.h"

#include <QRect>

#include <KoColorSpaceRegistry.h>
#include <KoColor.h>

#include <kis_brush.h>
#include <kis_paint_device.h>
#include <kis_painter.h>
#include <kis_node.h>
#include <kis_brush_based_paintop_settings.h>
#include <kis_fixed_paint_device.h>
#include <kis_image.h>
#include <kis_lod_transform.h>
#include <kis_paintop_plugin_utils.h>

KisTangentNormalPaintOp::KisTangentNormalPaintOp(const KisPaintOpSettingsSP settings, KisPainter* painter, KisNodeSP node, KisImageSP image)
    : KisBrushBasedPaintOp(settings, painter)
    , m_tangentTiltOption(settings.data())
    , m_opacityOption(settings.data(), node)
    , m_flowOption(settings.data())
    , m_sizeOption(settings.data())
    , m_spacingOption(settings.data())
    , m_softnessOption(settings.data())
    , m_sharpnessOption(settings.data())
    , m_scatterOption(settings.data())
    , m_rotationOption(settings.data())
    , m_rateOption(settings.data())
    , m_tempDev(painter->device()->createCompositionSourceDevice())

{
    Q_UNUSED(image);
    m_airbrushData.read(settings.data());

    m_dabCache->setSharpnessPostprocessing(&m_sharpnessOption);
    m_rotationOption.applyFanCornersInfo(this);
}

KisTangentNormalPaintOp::~KisTangentNormalPaintOp()
{
}

KisSpacingInformation KisTangentNormalPaintOp::paintAt(const KisPaintInformation& info)
{
    KoColor currentColor = painter()->paintColor();
    QString currentSpace = currentColor.colorSpace()->colorModelId().id();
    const KoColorSpace* rgbColorSpace = KoColorSpaceRegistry::instance()->rgb8();
    if (currentSpace != "RGBA") {
        rgbColorSpace = KoColorSpaceRegistry::instance()->rgb8();
    } else {
        rgbColorSpace = currentColor.colorSpace();
    }
    QVector <float> channelValues(4);
    qreal r, g, b;

    if (currentColor.colorSpace()->colorDepthId().id()=="F16" || currentColor.colorSpace()->colorDepthId().id()=="F32"){
        channelValues[0] = 0.5;
        channelValues[1] = 0.5;
        channelValues[2] = 1.0;
        channelValues[3] = 1.0;

        m_tangentTiltOption.apply(info, &r, &g, &b);

        channelValues[0] = r;
        channelValues[1] = g;
        channelValues[2] = b;
    } else {
        channelValues[0] = 1.0;
        channelValues[1] = 0.5;
        channelValues[2] = 0.5;
        channelValues[3] = 1.0;

        m_tangentTiltOption.apply(info, &r, &g, &b);

        channelValues[0] = b;
        channelValues[1] = g;
        channelValues[2] = r;
    }

    quint8 data[MAX_PIXEL_SIZE];
    rgbColorSpace->fromNormalisedChannelsValue(data, channelValues);
    KoColor color(data, rgbColorSpace);
    KisBrushSP brush = m_brush;

    if (!painter()->device() || !brush || !brush->canPaintFor(info)) {
        return KisSpacingInformation(1.0);
    }

    qreal scale    = m_sizeOption.apply(info);
    scale *= KisLodTransform::lodToScale(painter()->device());
    qreal rotation = m_rotationOption.apply(info);
    if (checkSizeTooSmall(scale)) return KisSpacingInformation();
    KisDabShape shape(scale, 1.0, rotation);

    QPointF cursorPos =
            m_scatterOption.apply(info,
                                  brush->maskWidth(shape, 0, 0, info),
                                  brush->maskHeight(shape, 0, 0, info));

    m_maskDab =
            m_dabCache->fetchDab(rgbColorSpace, color, cursorPos,
                                 shape,
                                 info, m_softnessOption.apply(info),
                                 &m_dstDabRect);

    if (m_dstDabRect.isEmpty()) return KisSpacingInformation(1.0);

    QRect dabRect = m_maskDab->bounds();

    Q_ASSERT(m_dstDabRect.size() == dabRect.size());
    Q_UNUSED(dabRect);

    qreal  oldOpacityF = painter()->opacityF();
    QString oldCompositeOpId = painter()->compositeOpId();

    m_opacityOption.apply(painter(), info);
    painter()->bltFixed(m_dstDabRect.topLeft(), m_maskDab, m_maskDab->bounds());
    painter()->renderMirrorMaskSafe(m_dstDabRect, m_maskDab, !m_dabCache->needSeparateOriginal());

    painter()->setOpacityF(oldOpacityF);
    painter()->setCompositeOpId(oldCompositeOpId);

    return computeSpacing(info, scale, rotation);
}

KisSpacingInformation KisTangentNormalPaintOp::updateSpacingImpl(const KisPaintInformation &info) const
{
    qreal scale = m_sizeOption.apply(info) * KisLodTransform::lodToScale(painter()->device());
    qreal rotation = m_rotationOption.apply(info);
    return computeSpacing(info, scale, rotation);
}

KisTimingInformation KisTangentNormalPaintOp::updateTimingImpl(const KisPaintInformation &info) const
{
    return KisPaintOpPluginUtils::effectiveTiming(&m_airbrushData, &m_rateOption, info);
}

KisSpacingInformation KisTangentNormalPaintOp::computeSpacing(const KisPaintInformation &info,
                                                              qreal scale, qreal rotation) const
{
    return effectiveSpacing(scale, rotation, &m_airbrushData, &m_spacingOption, info);
}

void KisTangentNormalPaintOp::paintLine(const KisPaintInformation& pi1, const KisPaintInformation& pi2, KisDistanceInformation *currentDistance)
{
    if (m_sharpnessOption.isChecked() && m_brush && (m_brush->width() == 1) && (m_brush->height() == 1)) {

        if (!m_lineCacheDevice) {
            m_lineCacheDevice = m_tempDev;
        }
        else {
            m_lineCacheDevice->clear();
        }

        KisPainter p(m_lineCacheDevice);
        KoColor currentColor = painter()->paintColor();
        QString currentSpace = currentColor.colorSpace()->colorModelId().id();
        const KoColorSpace* rgbColorSpace = KoColorSpaceRegistry::instance()->rgb8();
        if (currentSpace != "RGBA") {
            rgbColorSpace = KoColorSpaceRegistry::instance()->rgb8();
        } else {
            rgbColorSpace = currentColor.colorSpace();
        }
        QVector <float> channelValues(4);
        qreal r, g, b;

        if (currentColor.colorSpace()->colorDepthId().id()=="F16" || currentColor.colorSpace()->colorDepthId().id()=="F32"){
            channelValues[0] = 0.5;
            channelValues[1] = 0.5;
            channelValues[2] = 1.0;
            channelValues[3] = 1.0;

            m_tangentTiltOption.apply(pi2, &r, &g, &b);

            channelValues[0] = r;
            channelValues[1] = g;
            channelValues[2] = b;
        } else {
            channelValues[0] = 1.0;
            channelValues[1] = 0.5;
            channelValues[2] = 0.5;
            channelValues[3] = 1.0;

            m_tangentTiltOption.apply(pi2, &r, &g, &b);

            channelValues[0] = b;
            channelValues[1] = g;
            channelValues[2] = r;
        }

        quint8 data[4];
        rgbColorSpace->fromNormalisedChannelsValue(data, channelValues);
        KoColor color(data, rgbColorSpace);
        p.setPaintColor(color);
        p.drawDDALine(pi1.pos(), pi2.pos());

        QRect rc = m_lineCacheDevice->extent();
        painter()->bitBlt(rc.x(), rc.y(), m_lineCacheDevice, rc.x(), rc.y(), rc.width(), rc.height());
        painter()->renderMirrorMask(rc, m_lineCacheDevice);
    }
    else {
        KisPaintOp::paintLine(pi1, pi2, currentDistance);
    }
}
