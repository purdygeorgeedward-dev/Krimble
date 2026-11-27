/*
 *  SPDX-FileCopyrightText: 2004 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_PAINT_INFORMATION_
#define _KIS_PAINT_INFORMATION_

#include <kis_debug.h>
#include <QTime>

#include "kis_global.h"
#include "kritaimage_export.h"
#include <kis_distance_information.h>
#include "kis_random_source.h"
#include "KisPerStrokeRandomSource.h"
#include "kis_spacing_information.h"
#include "kis_timing_information.h"

class QDomDocument;
class QDomElement;
class KisDistanceInformation;

class KRITAIMAGE_EXPORT KisPaintInformation
{
public:
    class KRITAIMAGE_EXPORT DistanceInformationRegistrar
    {
    public:
        DistanceInformationRegistrar(KisPaintInformation *_p, KisDistanceInformation *distanceInfo);
        DistanceInformationRegistrar(const DistanceInformationRegistrar &rhs) = delete;
        DistanceInformationRegistrar(DistanceInformationRegistrar &&rhs);
        ~DistanceInformationRegistrar();
    private:
        KisPaintInformation *p;
    };

public:

    KisPaintInformation(const QPointF & pos,
                        qreal pressure,
                        qreal xTilt,
                        qreal yTilt,
                        qreal rotation,
                        qreal tangentialPressure,
                        qreal perspective,
                        qreal time,
                        qreal speed);

    KisPaintInformation(const QPointF & pos,
                        qreal pressure,
                        qreal xTilt,
                        qreal yTilt,
                        qreal rotation);

    KisPaintInformation(const QPointF & pos = QPointF(),
                        qreal pressure = PRESSURE_DEFAULT);

    KisPaintInformation(const KisPaintInformation& rhs);

    void operator=(const KisPaintInformation& rhs);

    ~KisPaintInformation();

    template <class PaintOp>
    void paintAt(PaintOp &op, KisDistanceInformation *distanceInfo) {
        KisSpacingInformation spacingInfo;
        KisTimingInformation timingInfo;
        {
            DistanceInformationRegistrar r = registerDistanceInformation(distanceInfo);
            spacingInfo = op.paintAt(*this);
            timingInfo = op.updateTimingImpl(*this);

            if (!this->isHoveringMode()) {
                distanceInfo->lockCurrentDrawingAngle(*this);
            }
        }

        distanceInfo->registerPaintedDab(*this, spacingInfo, timingInfo);
    }

    const QPointF& pos() const;
    void setPos(const QPointF& p);

    qreal pressure() const;

    void setPressure(qreal p);

    qreal xTilt() const;

    qreal yTilt() const;

    void overrideDrawingAngle(qreal angle);

    qreal drawingAngleSafe(const KisDistanceInformation &distance) const;

    DistanceInformationRegistrar registerDistanceInformation(KisDistanceInformation *distance);

    qreal drawingAngle(bool considerLockedAngle = false) const;

    QPointF drawingDirectionVector() const;

    qreal drawingSpeed() const;

    qreal drawingDistance() const;

    qreal maxPressure() const;

    qreal rotation() const;

    qreal tangentialPressure() const;

    qreal perspective() const;

    qreal currentTime() const;
    void setCurrentTime(qreal time) const;

    int currentDabSeqNo() const;

    qreal totalStrokeLength() const;

    KisRandomSourceSP randomSource() const;

    void setRandomSource(KisRandomSourceSP value);

    KisPerStrokeRandomSourceSP perStrokeRandomSource() const;

    void setPerStrokeRandomSource(KisPerStrokeRandomSourceSP value);

    void setLevelOfDetail(int levelOfDetail);

    bool isHoveringMode() const;

    static KisPaintInformation createHoveringModeInfo(const QPointF &pos,
            qreal pressure = PRESSURE_DEFAULT,
            qreal xTilt = 0.0, qreal yTilt = 0.0,
            qreal rotation = 0.0,
            qreal tangentialPressure = 0.0,
            qreal perspective = 1.0,
	        qreal speed = 0.0,
            qreal canvasrotation = 0.0,
            bool canvasMirroredH = false,
            bool canvasMirroredV = false,
            qreal tiltDirectionOffset = 0.0);

    qreal canvasRotation() const;

    void setCanvasRotation(qreal rotation);

    bool canvasMirroredH() const;

    void setCanvasMirroredH(bool value);

    bool canvasMirroredV() const;

    void setCanvasMirroredV(bool value);

    qreal tiltDirectionOffset() const;

    void setTiltDirectionOffset(qreal angle);

    void toXML(QDomDocument&, QDomElement&) const;

    static KisPaintInformation fromXML(const QDomElement&);

    static KisPaintInformation mixOnlyPosition(qreal t, const KisPaintInformation& mixedPi, const KisPaintInformation& basePi);
    static KisPaintInformation mix(const QPointF& p, qreal t, const KisPaintInformation& p1, const KisPaintInformation& p2);
    static KisPaintInformation mix(qreal t, const KisPaintInformation& pi1, const KisPaintInformation& pi2);
    static KisPaintInformation mixWithoutTime(const QPointF &p, qreal t, const KisPaintInformation &p1, const KisPaintInformation &p2);
    static KisPaintInformation mixWithoutTime(qreal t, const KisPaintInformation &pi1, const KisPaintInformation &pi2);
    void mixOtherOnlyPosition(qreal t, const KisPaintInformation& other);
    void mixOtherWithoutTime(qreal t, const KisPaintInformation& other);
    static qreal tiltDirection(const KisPaintInformation& info, bool normalize = true);
    static qreal tiltElevation(const KisPaintInformation& info, qreal maxTiltX = 60.0, qreal maxTiltY = 60.0, bool normalize = true);

private:
    static KisPaintInformation mixImpl(const QPointF &p, qreal t, const KisPaintInformation &p1, const KisPaintInformation &p2, bool posOnly, bool mixTime);
    void mixOtherImpl(const QPointF &p, qreal t, const KisPaintInformation &other, bool posOnly, bool mixTime);

private:
    struct Private;
    Private* const d;
};

KRITAIMAGE_EXPORT QDebug operator<<(QDebug debug, const KisPaintInformation& info);

#endif
