/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_DAB_CACHE_H
#define __KIS_DAB_CACHE_H

#include "kritapaintop_export.h"

#include "kis_dab_cache_base.h"

#include "kis_brush.h"

class KisColorSource;
class KisTextureOption;
class KisPrecisionOption;
struct MirrorProperties;

class PAINTOP_EXPORT KisDabCache : public KisDabCacheBase
{
public:
    KisDabCache(KisBrushSP brush);
    ~KisDabCache();

    KisFixedPaintDeviceSP fetchDab(const KoColorSpace *cs,
                                   KisColorSource *colorSource,
                                   const QPointF &cursorPoint,
                                   KisDabShape const&,
                                   const KisPaintInformation& info,
                                   qreal softnessFactor,
                                   QRect *dstDabRect,
                                   qreal lightnessStrength = 1.0);

    KisFixedPaintDeviceSP fetchDab(const KoColorSpace *cs,
                                   const KoColor& color,
                                   const QPointF &cursorPoint,
                                   KisDabShape const&,
                                   const KisPaintInformation& info,
                                   qreal softnessFactor,
                                   QRect *dstDabRect,
                                   qreal lightnessStrength = 1.0);

    KisFixedPaintDeviceSP fetchNormalizedImageDab(const KoColorSpace *cs,
                                                  const QPointF &cursorPoint,
                                                  KisDabShape const& shape,
                                                  const KisPaintInformation& info,
                                                  qreal softnessFactor,
                                                  QRect *dstDabRect);

    void setSharpnessPostprocessing(KisSharpnessOption *option);
    void setTexturePostprocessing(KisTextureOption *option);

    bool needSeparateOriginal() const;

private:

    inline KisFixedPaintDeviceSP fetchFromCache(KisDabCacheUtils::DabRenderingResources *resources, const KisPaintInformation& info,
                                                QRect *dstDabRect);

    inline KisFixedPaintDeviceSP fetchDabCommon(const KoColorSpace *cs,
            KisColorSource *colorSource,
            const KoColor& color,
            const QPointF &cursorPoint,
            KisDabShape,
            const KisPaintInformation& info,
            qreal softnessFactor,
            QRect *dstDabRect,
            qreal lightnessStrength = 1.0,
            bool forceNormalizedRGBAImageStamp = false);

private:

    struct Private;
    Private * const m_d;

};

#endif
