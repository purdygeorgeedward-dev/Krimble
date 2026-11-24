/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_PIXEL_SELECTION_H_
#define KIS_PIXEL_SELECTION_H_

#include <QRect>
#include <QPainterPath>

#include "kis_types.h"
#include "kis_global.h"
#include "kis_paint_device.h"
#include "kis_selection_component.h"
#include "kis_selection.h"
#include <kritaimage_export.h>

class KRITAIMAGE_EXPORT KisPixelSelection : public KisPaintDevice, public KisSelectionComponent
{

public:

    KisPixelSelection(KisDefaultBoundsBaseSP defaultBounds = KisDefaultBoundsBaseSP(), KisSelectionWSP parentSelection = KisSelectionWSP());

    KisPixelSelection(const KisPixelSelection& rhs, KritaUtils::DeviceCopyMode copyMode = KritaUtils::CopySnapshot);

    KisPixelSelection(const KisPaintDeviceSP copySource, KritaUtils::DeviceCopyMode copyMode = KritaUtils::CopySnapshot, KisSelectionWSP parentSelection = KisSelectionWSP());

    ~KisPixelSelection() override;

    KisSelectionComponent* clone(KisSelection*) override;

    const KoColorSpace* compositionSourceColorSpace() const override;

    bool read(QIODevice *stream);

    void select(const QRect & r, quint8 selectedness = MAX_SELECTED);

    void invert();

    void clear(const QRect & r);

    void clear() override;

    void copyAlphaFrom(KisPaintDeviceSP src, const QRect &processRect);

    void applySelection(KisPixelSelectionSP selection, SelectionAction action);

    bool isTotallyUnselected(const QRect & r) const;

    QRect selectedRect() const;

    QRect selectedExactRect() const;

    QVector<QPolygon> outline() const;

    void moveTo(const QPoint& pt) override;
    using KisPaintDevice::moveTo;

    bool isEmpty() const override;
    QPainterPath outlineCache() const override;
    bool outlineCacheValid() const override;
    void recalculateOutlineCache() override;

    void setOutlineCache(const QPainterPath &cache);
    void invalidateOutlineCache();

    bool thumbnailImageValid() const;
    QImage thumbnailImage() const;
    QTransform thumbnailImageTransform() const;
    void recalculateThumbnailImage(const QColor &maskColor);

    void setParentSelection(KisSelectionWSP selection);
    KisSelectionWSP parentSelection() const;

    void renderToProjection(KisPaintDeviceSP projection) override;
    void renderToProjection(KisPaintDeviceSP projection, const QRect& r) override;

private:
    void addSelection(KisPixelSelectionSP selection);

    void subtractSelection(KisPixelSelectionSP selection);

    void intersectSelection(KisPixelSelectionSP selection);

    void symmetricdifferenceSelection(KisPixelSelectionSP selection);

private:
    using KisPaintDevice::extent;
    using KisPaintDevice::exactBounds;

private:

    struct Private;
    Private * const m_d;
};

#endif
