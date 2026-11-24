/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_PAINTOP_UTILS_H
#define __KIS_PAINTOP_UTILS_H

#include "kis_global.h"
#include "kis_paint_information.h"
#include "kis_distance_information.h"
#include "kis_spacing_information.h"
#include "kis_timing_information.h"

#include "kritaimage_export.h"

struct KisRenderedDab;

namespace KisPaintOpUtils {

template <class PaintOp>
bool paintFan(PaintOp &op,
              const KisPaintInformation &pi1,
              const KisPaintInformation &pi2,
              KisDistanceInformation *currentDistance,
              qreal fanCornersStep)
{
    const qreal angleStep = fanCornersStep;
    const qreal initialAngle = currentDistance->lastDrawingAngle();
    const qreal finalAngle = pi2.drawingAngleSafe(*currentDistance);
    const qreal fullDistance = shortestAngularDistance(initialAngle,
                                                       finalAngle);
    qreal lastAngle = initialAngle;

    int i = 0;

    while (shortestAngularDistance(lastAngle, finalAngle) > angleStep) {
        lastAngle = incrementInDirection(lastAngle, angleStep, finalAngle);

        qreal t = angleStep * i++ / fullDistance;

        QPointF pt = pi1.pos() + t * (pi2.pos() - pi1.pos());
        KisPaintInformation pi = KisPaintInformation::mix(pt, t, pi1, pi2);
        pi.overrideDrawingAngle(lastAngle);
        pi.paintAt(op, currentDistance);
    }

    return i;
}

template <class PaintOp>
void paintLine(PaintOp &op,
               const KisPaintInformation &pi1,
               const KisPaintInformation &pi2,
               KisDistanceInformation *currentDistance,
               bool fanCornersEnabled,
               qreal fanCornersStep)
{
    QPointF end = pi2.pos();
    qreal endTime = pi2.currentTime();

    KisPaintInformation pi = pi1;
    qreal t = 0.0;

    while ((t = currentDistance->getNextPointPosition(pi.pos(), end, pi.currentTime(), endTime)) >= 0.0) {
        pi = KisPaintInformation::mix(t, pi, pi2);

        if (fanCornersEnabled &&
            currentDistance->hasLastPaintInformation()) {

            paintFan(op,
                     currentDistance->lastPaintInformation(),
                     pi,
                     currentDistance,
                     fanCornersStep);
        }

        pi.paintAt(op, currentDistance);
    }

    if (currentDistance->needsSpacingUpdate()) {
        op.updateSpacing(pi2, *currentDistance);
    }
    if (currentDistance->needsTimingUpdate()) {
        op.updateTiming(pi2, *currentDistance);
    }
}

class KRITAIMAGE_EXPORT PositionHistory
{
public:
    QPointF pushThroughHistory(const QPointF &pt, qreal zoom) {
        QPointF result;
        const qreal pointSwapThreshold = 7.0 / zoom;

        const qreal distance = qMin(qAbs(pt.x() - m_second.x()), qAbs(pt.y() - m_second.y()));
        const qreal coeff = qMin(1.0, distance / pointSwapThreshold);

        if (coeff > 1.0 - std::numeric_limits<qreal>::epsilon()) {
            result = m_second;
            m_first = m_second;
            m_second = pt;
        } else {
            result = m_first;
        }

        return result;
    }

    void reset(const QPointF &pt)
    {
        m_first = pt;
        m_second = pt;
    }

private:
    QPointF m_first;
    QPointF m_second;
};

inline bool checkSizeTooSmall(qreal scale, qreal width, qreal height)
{
    return scale * width < 0.01 || scale * height < 0.01;
}

inline qreal calcAutoSpacing(qreal value, qreal coeff)
{
    return coeff * (value < 1.0 ? value : sqrt(value));
}

inline QPointF calcAutoSpacing(const QPointF &pt, qreal coeff, qreal lodScale)
{
    const qreal invLodScale = 1.0 / lodScale;
    const QPointF lod0Point = invLodScale * pt;

    return lodScale * QPointF(calcAutoSpacing(lod0Point.x(), coeff), calcAutoSpacing(lod0Point.y(), coeff));
}

KRITAIMAGE_EXPORT
KisSpacingInformation effectiveSpacing(qreal dabWidth,
                                       qreal dabHeight,
                                       qreal extraScale,
                                       bool distanceSpacingEnabled,
                                       bool isotropicSpacing,
                                       qreal rotation,
                                       bool axesFlipped,
                                       qreal spacingVal,
                                       bool autoSpacingActive,
                                       qreal autoSpacingCoeff,
                                       qreal lodScale);

KRITAIMAGE_EXPORT
KisTimingInformation effectiveTiming(bool timingEnabled,
                                     qreal timingInterval,
                                     qreal rateExtraScale);

KRITAIMAGE_EXPORT
QVector<QRect> splitAndFilterDabRect(const QRect &totalRect, const QVector<QRect> &dabRects, int idealPatchSize);

KRITAIMAGE_EXPORT
QVector<QRect> splitDabsIntoRects(const QVector<QRect> &dabRects, int idealNumRects, int diameter, qreal spacing);

}

#endif
