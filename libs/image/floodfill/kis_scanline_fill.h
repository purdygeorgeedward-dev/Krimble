/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_SCANLINE_FILL_H
#define __KIS_SCANLINE_FILL_H

#include <QScopedPointer>

#include <kritaimage_export.h>
#include <kis_types.h>
#include <kis_paint_device.h>

class KisFillInterval;
class KisFillIntervalMap;

class KRITAIMAGE_EXPORT KisScanlineFill
{
public:
    KisScanlineFill(KisPaintDeviceSP device, const QPoint &startPoint, const QRect &boundingRect);
    ~KisScanlineFill();

    void fill(const KoColor &fillColor);

    void fillUntilColor(const KoColor &fillColor, const KoColor &boundaryColor);

    void fill(const KoColor &fillColor, KisPaintDeviceSP externalDevice);

    void fillUntilColor(const KoColor &fillColor, const KoColor &boundaryColor, KisPaintDeviceSP externalDevice);

    void fillSelection(KisPixelSelectionSP pixelSelection, KisPaintDeviceSP boundarySelection);

    void fillSelection(KisPixelSelectionSP pixelSelection);

    void fillSelectionUntilColor(KisPixelSelectionSP pixelSelection, const KoColor &boundaryColor, KisPaintDeviceSP boundarySelection);

    void fillSelectionUntilColor(KisPixelSelectionSP pixelSelection, const KoColor &boundaryColor);

    void fillSelectionUntilColorOrTransparent(KisPixelSelectionSP pixelSelection, const KoColor &boundaryColor, KisPaintDeviceSP boundarySelection);

    void fillSelectionUntilColorOrTransparent(KisPixelSelectionSP pixelSelection, const KoColor &boundaryColor);

    void clearNonZeroComponent();

    void fillContiguousGroup(KisPaintDeviceSP groupMapDevice, qint32 groupIndex);

    void setThreshold(int threshold);

    void setOpacitySpread(int opacitySpread);

    void setCloseGap(int closeGap);

    QRect fillExtent() const;

private:
    friend class KisScanlineFillTest;
    Q_DISABLE_COPY(KisScanlineFill)

    template <typename DifferencePolicy, typename SelectionPolicy, typename PixelAccessPolicy>
    void processLine(KisFillInterval interval, const int rowIncrement,
                     DifferencePolicy &differencePolicy,
                     SelectionPolicy &selectionPolicy,
                     PixelAccessPolicy &pixelAccessPolicy);

    template <typename DifferencePolicy, typename SelectionPolicy, typename PixelAccessPolicy>
    void extendedPass(KisFillInterval *currentInterval, int srcRow, bool extendRight,
                      DifferencePolicy &differencePolicy,
                      SelectionPolicy &selectionPolicy,
                      PixelAccessPolicy &pixelAccessPolicy);

    template <typename DifferencePolicy, typename SelectionPolicy, typename PixelAccessPolicy>
    void runImpl(DifferencePolicy &differencePolicy,
                 SelectionPolicy &selectionPolicy,
                 PixelAccessPolicy &pixelAccessPolicy);

    template <template <typename SrcPixelType> typename OptimizedDifferencePolicy,
              typename SlowDifferencePolicy,
              typename SelectionPolicy, typename PixelAccessPolicy>
    void selectDifferencePolicyAndRun(const KoColor &srcColor,
                                      SelectionPolicy &selectionPolicy,
                                      PixelAccessPolicy &pixelAccessPolicy);

    template <typename DifferencePolicy, typename SelectionPolicy, typename PixelAccessPolicy>
    KisFillInterval closeGapPass(DifferencePolicy &differencePolicy,
                                 SelectionPolicy &selectionPolicy,
                                 PixelAccessPolicy &pixelAccessPolicy);

    template <typename DifferencePolicy, typename SelectionPolicy, typename PixelAccessPolicy>
    bool fillOpacity(DifferencePolicy &differencePolicy,
                     SelectionPolicy &selectionPolicy,
                     PixelAccessPolicy &pixelAccessPolicy,
                     KisPaintDevice* const devicePtr,
                     const QRect& rect) const;

    inline bool tryPushingCloseGapSeed(int x, int y, bool allowExpand);

private:
    void testingProcessLine(const KisFillInterval &processInterval);
    QVector<KisFillInterval> testingGetForwardIntervals() const;
    KisFillIntervalMap* testingGetBackwardIntervals() const;
private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
