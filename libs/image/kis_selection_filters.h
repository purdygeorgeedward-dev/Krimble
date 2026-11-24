/*
 *  SPDX-FileCopyrightText: 2005 Michael Thaler
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_SELECTION_FILTERS_H
#define KIS_SELECTION_FILTERS_H

#include "kis_types.h"
#include "kritaimage_export.h"
#include "kis_default_bounds_base.h"

#include <QRect>

class KUndo2MagicString;
class KoColorSpace;

class KRITAIMAGE_EXPORT KisSelectionFilter
{
public:
    virtual ~KisSelectionFilter();

    virtual void process(KisPixelSelectionSP pixelSelection,
                         const QRect &rect) = 0;

    virtual KUndo2MagicString name();
    virtual QRect changeRect(const QRect &rect, KisDefaultBoundsBaseSP defaultBounds);

protected:
    void computeBorder(qint32  *circ, qint32  xradius, qint32  yradius);

    void rotatePointers(quint8  **p, quint32 n);

    void computeTransition(quint8* transition, quint8** buf, qint32 width);
};

class KRITAIMAGE_EXPORT KisErodeSelectionFilter : public KisSelectionFilter
{
public:
    KUndo2MagicString name() override;

    QRect changeRect(const QRect &rect, KisDefaultBoundsBaseSP defaultBounds) override;

    void process(KisPixelSelectionSP pixelSelection, const QRect &rect) override;
};

class KRITAIMAGE_EXPORT KisDilateSelectionFilter : public KisSelectionFilter
{
public:
    KUndo2MagicString name() override;

    QRect changeRect(const QRect &rect, KisDefaultBoundsBaseSP defaultBounds) override;

    void process(KisPixelSelectionSP pixelSelection, const QRect &rect) override;
};

class KRITAIMAGE_EXPORT KisBorderSelectionFilter : public KisSelectionFilter
{
public:
    KisBorderSelectionFilter(qint32 xRadius, qint32 yRadius, bool fade);

    KUndo2MagicString name() override;

    QRect changeRect(const QRect &rect, KisDefaultBoundsBaseSP defaultBounds) override;

    void process(KisPixelSelectionSP pixelSelection, const QRect &rect) override;

private:
    qint32 m_xRadius;
    qint32 m_yRadius;
    bool m_antialiasing;
};

class KRITAIMAGE_EXPORT KisFeatherSelectionFilter : public KisSelectionFilter
{
public:
    KisFeatherSelectionFilter(qint32 radius);

    KUndo2MagicString name() override;

    QRect changeRect(const QRect &rect, KisDefaultBoundsBaseSP defaultBounds) override;

    void process(KisPixelSelectionSP pixelSelection, const QRect &rect) override;
private:
    qint32 m_radius;
};

class KRITAIMAGE_EXPORT KisGrowSelectionFilter : public KisSelectionFilter
{
public:
    KisGrowSelectionFilter(qint32 xRadius, qint32 yRadius);

    KUndo2MagicString name() override;

    QRect changeRect(const QRect &rect, KisDefaultBoundsBaseSP defaultBounds) override;

    void process(KisPixelSelectionSP pixelSelection, const QRect &rect) override;

private:
    qint32 m_xRadius;
    qint32 m_yRadius;
};

class KRITAIMAGE_EXPORT KisShrinkSelectionFilter : public KisSelectionFilter
{
public:
    KisShrinkSelectionFilter(qint32 xRadius, qint32 yRadius, bool edgeLock);

    KUndo2MagicString name() override;

    QRect changeRect(const QRect &rect, KisDefaultBoundsBaseSP defaultBounds) override;

    void process(KisPixelSelectionSP pixelSelection, const QRect &rect) override;

private:
    qint32 m_xRadius;
    qint32 m_yRadius;
    qint32 m_edgeLock;
};

class KRITAIMAGE_EXPORT KisSmoothSelectionFilter : public KisSelectionFilter
{
public:
    KUndo2MagicString name() override;

    QRect changeRect(const QRect &rect, KisDefaultBoundsBaseSP defaultBounds) override;

    void process(KisPixelSelectionSP pixelSelection, const QRect &rect) override;
};

class KRITAIMAGE_EXPORT KisInvertSelectionFilter : public KisSelectionFilter
{
public:
    KUndo2MagicString name() override;

    QRect changeRect(const QRect &rect, KisDefaultBoundsBaseSP defaultBounds) override;

    void process(KisPixelSelectionSP pixelSelection, const QRect &rect) override;
};

class KRITAIMAGE_EXPORT KisAntiAliasSelectionFilter : public KisSelectionFilter
{
public:
    KUndo2MagicString name() override;
    void process(KisPixelSelectionSP pixelSelection, const QRect &rect) override;

private:
    static constexpr qint32 edgeThreshold {4};
    static constexpr qint32 numSteps {30};
    static constexpr qint32 offsets[numSteps] {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2
    };
    static constexpr qint32 horizontalBorderSize {2};
    static constexpr qint32 verticalBorderSize {40};
    static constexpr qint32 numberOfScanlines {2 * verticalBorderSize + 1};
    static constexpr qint32 currentScanlineIndex {verticalBorderSize};
    bool getInterpolationValue(qint32 negativeSpanEndDistance, qint32 positiveSpanEndDistance,
                               qint32 negativePixelDiff, qint32 positivePixelDiff, qint32 currentPixelDiff,
                               bool negativeSpanExtremeValid, bool positiveSpanExtremeValid,
                               qint32 *interpolationValue) const;
    void findSpanExtreme(quint8 **scanlines, qint32 x, qint32 pixelOffset,
                         qint32 rowMultiplier, qint32 colMultiplier, qint32 direction,
                         qint32 pixelAvg, qint32 scaledGradient, qint32 currentPixelDiff,
                         qint32 *spanEndDistance, qint32 *pixelDiff, bool *spanExtremeValidType) const;
    void findSpanExtremes(quint8 **scanlines, qint32 x, qint32 pixelOffset,
                          qint32 rowMultiplier, qint32 colMultiplier,
                          qint32 pixelAvg, qint32 scaledGradient, qint32 currentPixelDiff,
                          qint32 *negativeSpanEndDistance, qint32 *positiveSpanEndDistance,
                          qint32 *negativePixelDiff, qint32 *positivePixelDiff,
                          bool *negativeSpanExtremeValid, bool *positiveSpanExtremeValid) const;
};

class KRITAIMAGE_EXPORT KisGrowUntilDarkestPixelSelectionFilter : public KisSelectionFilter
{
public:
    KisGrowUntilDarkestPixelSelectionFilter(qint32 radius, KisPaintDeviceSP referenceDevice);

    KUndo2MagicString name() override;
    QRect changeRect(const QRect &rect, KisDefaultBoundsBaseSP defaultBounds) override;
    void process(KisPixelSelectionSP pixelSelection, const QRect &rect) override;

private:
    qint32 m_radius;
    KisPaintDeviceSP m_referenceDevice;
};

#endif
