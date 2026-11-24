/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_dab_cache.h"

#include <KoColor.h>
#include "kis_paint_device.h"
#include "kis_brush.h"
#include <kis_fixed_paint_device.h>
#include "kis_color_source.h"
#include "KisSharpnessOption.h"
#include "kis_texture_option.h"

#include <kundo2command.h>

struct KisDabCache::Private {

    Private(KisBrushSP brush)
        : brush(brush)
    {}

    int seqNo = 0;

    KisFixedPaintDeviceSP dab;
    KisFixedPaintDeviceSP dabOriginal;

    KisBrushSP brush;
    KisPaintDeviceSP colorSourceDevice;

    KisSharpnessOption *sharpnessOption = 0;
    KisTextureOption *textureOption = 0;
};

KisDabCache::KisDabCache(KisBrushSP brush)
    : m_d(new Private(brush))
{
}

KisDabCache::~KisDabCache()
{
    delete m_d;
}

void KisDabCache::setSharpnessPostprocessing(KisSharpnessOption *option)
{
    m_d->sharpnessOption = option;
}

void KisDabCache::setTexturePostprocessing(KisTextureOption *option)
{
    m_d->textureOption = option;
}

bool KisDabCache::needSeparateOriginal() const
{
    return KisDabCacheBase::needSeparateOriginal(m_d->textureOption, m_d->sharpnessOption);
}

KisFixedPaintDeviceSP KisDabCache::fetchDab(const KoColorSpace *cs,
        KisColorSource *colorSource,
        const QPointF &cursorPoint,
        KisDabShape const& shape,
        const KisPaintInformation& info,
        qreal softnessFactor,
        QRect *dstDabRect,
        qreal lightnessStrength)
{

    return fetchDabCommon(cs, colorSource, KoColor(),
                          cursorPoint,
                          shape,
                          info,
                          softnessFactor,
                          dstDabRect,
                          lightnessStrength);
}

KisFixedPaintDeviceSP KisDabCache::fetchDab(const KoColorSpace *cs,
        const KoColor& color,
        const QPointF &cursorPoint,
        KisDabShape const& shape,
        const KisPaintInformation& info,
        qreal softnessFactor,
        QRect *dstDabRect,
        qreal lightnessStrength)
{
    return fetchDabCommon(cs, 0, color,
                          cursorPoint,
                          shape,
                          info,
                          softnessFactor,
                          dstDabRect,
                          lightnessStrength);
}

KisFixedPaintDeviceSP KisDabCache::fetchNormalizedImageDab(const KoColorSpace *cs,
                                                           const QPointF &cursorPoint,
                                                           KisDabShape const& shape,
                                                           const KisPaintInformation& info,
                                                           qreal softnessFactor,
                                                           QRect *dstDabRect)
{
    return fetchDabCommon(cs, 0, KoColor(),
                          cursorPoint,
                          shape,
                          info,
                          softnessFactor,
                          dstDabRect,
                          1.0,
                          true);
}

inline
KisFixedPaintDeviceSP KisDabCache::fetchFromCache(KisDabCacheUtils::DabRenderingResources *resources,
                                                  const KisPaintInformation& info,
                                                  QRect *dstDabRect)
{
    if (needSeparateOriginal()) {
        *m_d->dab = *m_d->dabOriginal;
        *dstDabRect = KisDabCacheUtils::correctDabRectWhenFetchedFromCache(*dstDabRect, m_d->dab->bounds().size());
        KisDabCacheUtils::postProcessDab(m_d->dab, dstDabRect->topLeft(), info, resources);
    }
    else {
        *dstDabRect = KisDabCacheUtils::correctDabRectWhenFetchedFromCache(*dstDabRect, m_d->dab->bounds().size());
    }

    return m_d->dab;
}

struct TemporaryResourcesWithoutOwning : public KisDabCacheUtils::DabRenderingResources
{
    ~TemporaryResourcesWithoutOwning() override {
        colorSource.release();
        sharpnessOption.release();
        textureOption.release();
    }
};

inline
KisFixedPaintDeviceSP KisDabCache::fetchDabCommon(const KoColorSpace *cs,
        KisColorSource *colorSource,
        const KoColor& color,
        const QPointF &cursorPoint,
        KisDabShape shape,
        const KisPaintInformation& info,
        qreal softnessFactor,
        QRect *dstDabRect,
        qreal lightnessStrength,
        bool forceNormalizedRGBAImageStamp)
{
    Q_ASSERT(dstDabRect);

    bool hasDabInCache = true;

    if (!m_d->dab || *m_d->dab->colorSpace() != *cs) {
        m_d->dab = new KisFixedPaintDevice(cs);
        hasDabInCache = false;
    }

    using namespace KisDabCacheUtils;

    m_d->brush->prepareForSeqNo(info, m_d->seqNo);
    m_d->seqNo++;

    TemporaryResourcesWithoutOwning resources;
    resources.brush = m_d->brush;
    resources.colorSourceDevice = m_d->colorSourceDevice;

    resources.colorSource.reset(colorSource);
    resources.sharpnessOption.reset(m_d->sharpnessOption);
    resources.textureOption.reset(m_d->textureOption);

    DabGenerationInfo di;
    bool shouldUseCache = false;

    fetchDabGenerationInfo(hasDabInCache,
                           &resources,
                           DabRequestInfo(
                               color,
                               cursorPoint,
                               shape,
                               info,
                               softnessFactor,
                               lightnessStrength),
                           &di,
                           &shouldUseCache);

    *dstDabRect = di.dstDabRect;

    if (shouldUseCache) {
        return fetchFromCache(&resources, info, dstDabRect);
    }

    generateDab(di, &resources, &m_d->dab, forceNormalizedRGBAImageStamp);

    if (di.needsPostprocessing) {
        if (!m_d->dabOriginal || *cs != *m_d->dabOriginal->colorSpace()) {
            m_d->dabOriginal = new KisFixedPaintDevice(cs);
        }

        *m_d->dabOriginal = *m_d->dab;

        postProcessDab(m_d->dab, di.dstDabRect.topLeft(), info, &resources);
    }

    return m_d->dab;
}
