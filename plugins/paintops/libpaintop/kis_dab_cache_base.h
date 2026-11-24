/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_DAB_CACHE_BASE_H
#define __KIS_DAB_CACHE_BASE_H

#include "kritapaintop_export.h"
#include "kis_brush.h"

#include "KisDabCacheUtils.h"

class KisColorSource;
class KisSharpnessOption;
class KisTextureOption;
class KisMirrorOption;
class KisPrecisionOption;
struct MirrorProperties;

class PAINTOP_EXPORT KisDabCacheBase
{
public:
    KisDabCacheBase();
    ~KisDabCacheBase();

    void setMirrorPostprocessing(KisMirrorOption *option);
    void setPrecisionOption(KisPrecisionOption *option);

    void disableSubpixelPrecision();

    bool needSeparateOriginal(KisTextureOption *textureOption,
                              KisSharpnessOption *sharpnessOption) const;

protected:
    void fetchDabGenerationInfo(bool hasDabInCache,
                                KisDabCacheUtils::DabRenderingResources *resources,
                                const KisDabCacheUtils::DabRequestInfo &request,
                                KisDabCacheUtils::DabGenerationInfo *di,
                                bool *shouldUseCache);

private:
    struct SavedDabParameters;
    struct DabPosition;
private:
    inline SavedDabParameters getDabParameters(KisBrushSP brush, const KoColor& color,
                                               KisDabShape const&,
                                               const KisPaintInformation& info,
                                               double subPixelX, double subPixelY,
                                               qreal softnessFactor,
                                               qreal lightnessStrength,
                                               MirrorProperties mirrorProperties);

    inline KisDabCacheBase::DabPosition
    calculateDabRect(KisBrushSP brush, const QPointF &cursorPoint,
                     KisDabShape,
                     const KisPaintInformation& info,
                     const MirrorProperties &mirrorProperties, KisSharpnessOption *sharpnessOption);

private:
    struct Private;
    Private * const m_d;
};

#endif
