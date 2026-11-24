/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_MARKER_PAINTER_H
#define __KIS_MARKER_PAINTER_H

#include <QScopedPointer>

#include "kis_types.h"
#include "kritaimage_export.h"

class KoColor;

class KRITAIMAGE_EXPORT KisMarkerPainter
{
public:
    static const qint32 ValidNumberRangeValue = 2140000000;

    KisMarkerPainter(KisPaintDeviceSP device, const KoColor &color);
    ~KisMarkerPainter();

    void fillFullCircle(const QPointF &center, qreal radius);
    void fillHalfBrushDiff(const QPointF &p1, const QPointF &p2, const QPointF &p3,
                           const QPointF &center, qreal radius);

    void fillCirclesDiff(const QPointF &c1, qreal r1,
                         const QPointF &c2, qreal r2);

private:
    struct Private;
    const QScopedPointer<Private> m_d;

    bool isNumberInValidRange(qint32 number);

    bool isRectInValidRange(const QRect &rect);
};

#endif
