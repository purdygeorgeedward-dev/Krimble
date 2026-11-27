/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_AUTO_LEVELS_H
#define KIS_AUTO_LEVELS_H

#include <QVector>

#include <KoColor.h>
#include <kis_paint_device.h>
#include <KisLevelsCurve.h>
#include <kritaimage_export.h>

class KisHistogram;

namespace KisAutoLevels
{

enum ShadowsAndHighlightsAdjustmentMethod {
    ShadowsAndHighlightsAdjustmentMethod_MonochromaticContrast,
    ShadowsAndHighlightsAdjustmentMethod_PerChannelContrast
};

enum MidtonesAdjustmentMethod {
    MidtonesAdjustmentMethod_None,
    MidtonesAdjustmentMethod_UseMedian,
    MidtonesAdjustmentMethod_UseMean
};

struct KRITAIMAGE_EXPORT ChannelHistogram
{
    KisHistogram *histogram{nullptr};
    int channel{0};
};

QPair<qreal, qreal> KRITAIMAGE_EXPORT getInputBlackAndWhitePoints(ChannelHistogram histogram,
                                                                  qreal shadowsClipping,
                                                                  qreal highlightsClipping);

QPair<KoColor, KoColor> KRITAIMAGE_EXPORT getDarkestAndWhitestColors(const KisPaintDeviceSP device,
                                                                     qreal shadowsClipping,
                                                                     qreal highlightsClipping);

qreal KRITAIMAGE_EXPORT getGamma(qreal blackPoint,
                                 qreal whitePoint,
                                 qreal inputIntensity,
                                 qreal outputIntensity);

QVector<KisLevelsCurve> KRITAIMAGE_EXPORT adjustMonochromaticContrast(ChannelHistogram lightnessHistogram,
                                                                      QVector<ChannelHistogram> &channelsHistograms,
                                                                      qreal shadowsClipping,
                                                                      qreal highlightsClipping,
                                                                      qreal maximumInputBlackAndWhiteOffset,
                                                                      MidtonesAdjustmentMethod midtonesAdjustmentMethod,
                                                                      qreal midtonesAdjustmentAmount,
                                                                      const QVector<qreal> &outputBlackPoints,
                                                                      const QVector<qreal> &outputWhitePoints,
                                                                      const QVector<qreal> &outputMidtones);

QVector<KisLevelsCurve> KRITAIMAGE_EXPORT adjustPerChannelContrast(QVector<ChannelHistogram> &channelsHistograms,
                                                                   qreal shadowsClipping,
                                                                   qreal highlightsClipping,
                                                                   qreal maximumInputBlackAndWhiteOffset,
                                                                   MidtonesAdjustmentMethod midtonesAdjustmentMethod,
                                                                   qreal midtonesAdjustmentAmount,
                                                                   const QVector<qreal> &outputBlackPoints,
                                                                   const QVector<qreal> &outputWhitePoints,
                                                                   const QVector<qreal> &outputMidtones);

}

#endif
