/*
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2004 Clarence Dang <dang@kde.org>
 *  SPDX-FileCopyrightText: 2004 Adrian Page <adrian@pagenet.plus.com>
 *  SPDX-FileCopyrightText: 2004, 2010 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_PAINTOP_H_
#define KIS_PAINTOP_H_

#include <kis_distance_information.h>
#include "kis_shared.h"
#include "kis_types.h"

#include <kritaimage_export.h>

class QPointF;
class KoColorSpace;

class KisPainter;
class KisPaintInformation;
class KisRunnableStrokeJobData;

class KRITAIMAGE_EXPORT KisPaintOp : public KisShared
{
    struct Private;
public:

    KisPaintOp(KisPainter * painter);
    virtual ~KisPaintOp();

    void paintAt(const KisPaintInformation& info, KisDistanceInformation *currentDistance);

    void updateSpacing(const KisPaintInformation &info, KisDistanceInformation &currentDistance)
        const;

    void updateTiming(const KisPaintInformation &info, KisDistanceInformation &currentDistance)
        const;

    virtual void paintLine(const KisPaintInformation &pi1,
                           const KisPaintInformation &pi2,
                           KisDistanceInformation *currentDistance);

    virtual void paintBezierCurve(const KisPaintInformation &pi1,
                                  const QPointF &control1,
                                  const QPointF &control2,
                                  const KisPaintInformation &pi2,
                                  KisDistanceInformation *currentDistance
                                  );

    virtual bool canPaint() const {
        return true;
    }

    static void splitCoordinate(qreal coordinate, qint32 *whole, qreal *fraction);

    virtual std::pair<int, bool> doAsynchronousUpdate(QVector<KisRunnableStrokeJobData*> &jobs);

protected:
    friend class KisPaintInformation;
    virtual KisSpacingInformation paintAt(const KisPaintInformation& info) = 0;

    virtual KisSpacingInformation updateSpacingImpl(const KisPaintInformation &info) const = 0;

    virtual KisTimingInformation updateTimingImpl(const KisPaintInformation &info) const;

    KisFixedPaintDeviceSP cachedDab();
    KisFixedPaintDeviceSP cachedDab(const KoColorSpace *cs);

    KisPainter* painter() const;

    KisPaintDeviceSP source() const;

private:
    friend class KisRotationOption;
    void setFanCornersInfo(bool fanCornersEnabled, qreal fanCornersStep);

private:
    Private* const d {nullptr};
};

#endif
