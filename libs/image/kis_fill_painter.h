/*
 *  SPDX-FileCopyrightText: 2004 Adrian Page <adrian@pagenet.plus.com>
 *  SPDX-FileCopyrightText: 2004 Bart Coppens <kde@bartcoppens.be>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_FILL_PAINTER_H_
#define KIS_FILL_PAINTER_H_

#include <QRect>

#include <KoColor.h>
#include <KoColorSpaceRegistry.h>
#include <KoPattern.h>

#include "kis_painter.h"
#include "kis_types.h"
#include "kis_selection.h"

#include <KisRunnableStrokeJobUtils.h>
#include <kis_processing_visitor.h>

#include <kritaimage_export.h>

class KisFilterConfiguration;

class KRITAIMAGE_EXPORT KisFillPainter : public KisPainter
{

public:
    enum RegionFillingMode
    {
        RegionFillingMode_FloodFill,
        RegionFillingMode_BoundaryFill
    };

    KisFillPainter();
    KisFillPainter(KisPaintDeviceSP device);

    KisFillPainter(KisPaintDeviceSP device, KisSelectionSP selection);

private:

    void initFillPainter();

public:
    void fillSelection(const QRect &rc, const KoColor &color);

    void fillRect(qint32 x,
                  qint32 y,
                  qint32 w,
                  qint32 h,
                  const KoColor &c,
                  quint8 opacity);

    inline void fillRect(const QRect &rc, const KoColor &c, quint8 opacity)
    {
        fillRect(rc.x(), rc.y(), rc.width(), rc.height(), c, opacity);
    }

    inline void
    fillRect(qint32 x, qint32 y, qint32 w, qint32 h, const KoColor &c)
    {
        fillRect(x, y, w, h, c, OPACITY_OPAQUE_U8);
    }

    inline void fillRect(const QRect &rc, const KoColor &c)
    {
        fillRect(rc.x(), rc.y(), rc.width(), rc.height(), c, OPACITY_OPAQUE_U8);
    }

    void fillRect(qint32 x1, qint32 y1, qint32 w, qint32 h, const KoPatternSP pattern, const QPoint &offset = QPoint());

    void fillRect(qint32 x1, qint32 y1, qint32 w, qint32 h, const KisPaintDeviceSP device, const QRect& deviceRect);

    void fillRect(const QRect &rc, const KisPaintDeviceSP device, const QRect &deviceRect);

    void fillRect(const QRect& rc, const KoPatternSP pattern, const QPoint &offset = QPoint());

    inline void eraseRect(qint32 x1, qint32 y1, qint32 w, qint32 h)
    {
        const KoColorSpace *cs = KoColorSpaceRegistry::instance()->rgb8();
        KoColor c(Qt::black, cs);
        fillRect(x1, y1, w, h, c, OPACITY_TRANSPARENT_U8);
    }

    inline void eraseRect(const QRect &rc)
    {
        const KoColorSpace *cs = KoColorSpaceRegistry::instance()->rgb8();
        KoColor c(Qt::black, cs);
        fillRect(rc.x(),
                 rc.y(),
                 rc.width(),
                 rc.height(),
                 c,
                 OPACITY_TRANSPARENT_U8);
    }

    void fillRectNoCompose(const QRect& rc, const KoPatternSP pattern, const QTransform transform);

    void fillRectNoCompose(qint32 x1, qint32 y1, qint32 w, qint32 h, const KisPaintDeviceSP device, const QRect& deviceRect, const QTransform transform);

    void fillRect(qint32 x1, qint32 y1, qint32 w, qint32 h, const KisFilterConfigurationSP  generator);

    void fillColor(int startX, int startY, KisPaintDeviceSP sourceDevice);

    void fillPattern(int startX, int startY, KisPaintDeviceSP sourceDevice, QTransform patternTransform = QTransform());

    KisPixelSelectionSP createFloodSelection(int startX, int startY,
                                             KisPaintDeviceSP sourceDevice, KisPaintDeviceSP existingSelection);

    KisPixelSelectionSP createFloodSelection(KisPixelSelectionSP newSelection, int startX, int startY,
                                             KisPaintDeviceSP sourceDevice, KisPaintDeviceSP existingSelection);

    void createSimilarColorsSelection(KisPixelSelectionSP outSelection,
                                      const KoColor &referenceColor,
                                      KisPaintDeviceSP referenceDevice,
                                      const QRect &rect,
                                      KisPixelSelectionSP mask);

    QVector<KisStrokeJobData*> createSimilarColorsSelectionJobs(
        KisPixelSelectionSP outSelection,
        const QSharedPointer<KoColor> referenceColor,
        KisPaintDeviceSP referenceDevice,
        const QRect &rect,
        KisPixelSelectionSP mask,
        QSharedPointer<KisProcessingVisitor::ProgressHelper> progressHelper = nullptr
    );

    inline void setFillThreshold(int threshold)
    {
        m_threshold = threshold;
    }

    int fillThreshold() const {
        return m_threshold;
    }

    void setOpacitySpread(int opacitySpread)
    {
        m_opacitySpread = opacitySpread;
    }

    int opacitySpread() const {
        return m_opacitySpread;
    }

    void setCloseGap(int gap) {
        m_closeGap = gap;
    }

    uint closeGap() const {
        return m_closeGap;
    }

    bool useCompositing() const {
        return m_useCompositing;
    }

    void setUseCompositing(bool useCompositing) {
        m_useCompositing = useCompositing;
    }

    void setWidth(int w) {
        m_width = w;
    }

    void setHeight(int h) {
        m_height = h;
    }

    bool careForSelection() const {
        return m_careForSelection;
    }

    void setCareForSelection(bool set) {
        m_careForSelection = set;
    }

    void setAntiAlias(bool antiAlias) {
        m_antiAlias = antiAlias;
    }

    bool antiAlias() const {
        return m_antiAlias;
    }

    void setSizemod(int sizemod) {
        m_sizemod = sizemod;
    }

    int sizemod() const {
        return m_sizemod;
    }

    void setFeather(int feather) {
        m_feather = feather;
    }

    uint feather() const {
        return m_feather;
    }

    void setUseSelectionAsBoundary(bool useSelectionAsBoundary) {
        m_useSelectionAsBoundary = useSelectionAsBoundary;
    }

    uint useSelectionAsBoundary() const {
        return m_useSelectionAsBoundary;
    }

    void setRegionFillingMode(RegionFillingMode regionFillingMode) {
        m_regionFillingMode = regionFillingMode;
    }

    RegionFillingMode regionFillingMode() const {
        return m_regionFillingMode;
    }

    void setRegionFillingBoundaryColor(const KoColor &regionFillingBoundaryColor) {
        m_regionFillingBoundaryColor = regionFillingBoundaryColor;
    }

    KoColor regionFillingBoundaryColor() const {
        return m_regionFillingBoundaryColor;
    }

    void setStopGrowingAtDarkestPixel(bool stopGrowingAtDarkestPixel) {
        m_stopGrowingAtDarkestPixel = stopGrowingAtDarkestPixel;
    }

    bool stopGrowingAtDarkestPixel() const {
        return m_stopGrowingAtDarkestPixel;
    }

protected:
    void setCurrentFillSelection(KisSelectionSP fillSelection)
    {
        m_fillSelection = fillSelection;
    }

    KisSelectionSP currentFillSelection() const
    {
        return m_fillSelection;
    }

    void genericFillStart(int startX, int startY, KisPaintDeviceSP sourceDevice);
    void genericFillEnd(KisPaintDeviceSP filled);

private:
    KisSelectionSP m_fillSelection;

    int m_feather;
    int m_sizemod;
    bool m_antiAlias;
    int m_threshold;
    int m_opacitySpread;
    int m_closeGap;
    int m_width, m_height;
    QRect m_rect;
    bool m_careForSelection;
    bool m_useCompositing;
    bool m_useSelectionAsBoundary;
    RegionFillingMode m_regionFillingMode;
    KoColor m_regionFillingBoundaryColor;
    bool m_stopGrowingAtDarkestPixel;
};

#endif
