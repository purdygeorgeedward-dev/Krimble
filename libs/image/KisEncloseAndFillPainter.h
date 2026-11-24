/*
 * KDE. Krita Project.
 *
 * SPDX-FileCopyrightText: 2022 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISENCLOSEANDFILLPAINTER_H
#define KISENCLOSEANDFILLPAINTER_H

#include <QPoint>
#include <QScopedPointer>

#include <kis_pixel_selection.h>
#include <kis_paint_device.h>
#include <kis_fill_painter.h>
#include <KoColor.h>
#include <kritaimage_export.h>

class KRITAIMAGE_EXPORT KisEncloseAndFillPainter : public KisFillPainter
{
public:
    enum RegionSelectionMethod
    {
        SelectAllRegions,
        SelectRegionsFilledWithSpecificColor,
        SelectRegionsFilledWithTransparent,
        SelectRegionsFilledWithSpecificColorOrTransparent,
        SelectAllRegionsExceptFilledWithSpecificColor,
        SelectAllRegionsExceptFilledWithTransparent,
        SelectAllRegionsExceptFilledWithSpecificColorOrTransparent,
        SelectRegionsSurroundedBySpecificColor,
        SelectRegionsSurroundedByTransparent,
        SelectRegionsSurroundedBySpecificColorOrTransparent
    };

    KisEncloseAndFillPainter(const QSize &imageSize);
    KisEncloseAndFillPainter(KisPaintDeviceSP device, const QSize &imageSize);

    KisEncloseAndFillPainter(KisPaintDeviceSP device, KisSelectionSP selection, const QSize &imageSize);

    ~KisEncloseAndFillPainter() override;

    void encloseAndFillColor(KisPixelSelectionSP enclosingMask, KisPaintDeviceSP referenceDevice);

    void encloseAndFillPattern(KisPixelSelectionSP enclosingMask,
                               KisPaintDeviceSP referenceDevice,
                               QTransform patternTransform = QTransform());

    KisPixelSelectionSP createEncloseAndFillSelection(KisPixelSelectionSP enclosingMask,
                                                      KisPaintDeviceSP referenceDevice,
                                                      KisPixelSelectionSP existingSelection);

    KisPixelSelectionSP createEncloseAndFillSelection(KisPixelSelectionSP newSelection,
                                                      KisPixelSelectionSP enclosingMask,
                                                      KisPaintDeviceSP referenceDevice,
                                                      KisPixelSelectionSP existingSelection);

    void setRegionSelectionMethod(RegionSelectionMethod regionSelectionMethod);

    RegionSelectionMethod regionSelectionMethod() const;

    void setRegionSelectionColor(const KoColor &color);

    KoColor regionSelectionColor() const;

    void setRegionSelectionInvert(bool invert);

    bool regionSelectionInvert() const;

    void setRegionSelectionIncludeContourRegions(bool include);

    bool regionSelectionIncludeContourRegions() const;

    void setRegionSelectionIncludeSurroundingRegions(bool include);

    bool regionSelectionIncludeSurroundingRegions() const;

protected:
    void genericEncloseAndFillStart(KisPixelSelectionSP enclosingMask, KisPaintDeviceSP referenceDevice);
    void genericEncloseAndFillEnd(KisPaintDeviceSP filled);

private:
    class Private;
    QScopedPointer<Private> m_d;
};

#endif
