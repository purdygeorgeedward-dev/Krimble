/*
 *  SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2013 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_DISTANCE_INFORMATION_H_
#define _KIS_DISTANCE_INFORMATION_H_

#include <QPointF>
#include <QVector2D>
#include <QDomDocument>
#include <QDomElement>
#include "kritaimage_export.h"
#include <boost/optional.hpp>
#include <boost/operators.hpp>

class KisPaintInformation;
class KisSpacingInformation;
class KisTimingInformation;
class KisDistanceInformation;

class KRITAIMAGE_EXPORT KisDistanceInitInfo : boost::equality_comparable<KisDistanceInitInfo> {

public:

    explicit KisDistanceInitInfo();

    explicit KisDistanceInitInfo(qreal spacingUpdateInterval, qreal timingUpdateInterval, int currentDabSeqNo);

    explicit KisDistanceInitInfo(const QPointF &lastPosition, qreal lastAngle, int currentDabSeqNo);

    explicit KisDistanceInitInfo(const QPointF &lastPosition, qreal lastAngle,
                                 qreal spacingUpdateInterval, qreal timingUpdateInterval, int currentDabSeqNo);

    KisDistanceInitInfo(const KisDistanceInitInfo &rhs);

    ~KisDistanceInitInfo();

    bool operator==(const KisDistanceInitInfo &other) const;

    KisDistanceInitInfo &operator=(const KisDistanceInitInfo &rhs);

    KisDistanceInformation makeDistInfo();

    void toXML(QDomDocument &doc, QDomElement &elt) const;

    static KisDistanceInitInfo fromXML(const QDomElement &elt);

private:
    struct Private;
    Private * const m_d;
};

class KRITAIMAGE_EXPORT KisDistanceInformation {
public:
    KisDistanceInformation();
    KisDistanceInformation(qreal spacingUpdateInterval, qreal timingUpdateInterval, int currentDabSeqNo = 0);
    KisDistanceInformation(const QPointF &lastPosition, qreal lastAngle);
    KisDistanceInformation(const QPointF &lastPosition, qreal lastAngle,
                           qreal spacingUpdateInterval, qreal timingUpdateInterval, int currentDabSeqNo);
    KisDistanceInformation(const KisDistanceInformation &rhs);
    KisDistanceInformation(const KisDistanceInformation &rhs, int levelOfDetail);
    KisDistanceInformation& operator=(const KisDistanceInformation &rhs);

    ~KisDistanceInformation();

    const KisSpacingInformation& currentSpacing() const;
    void updateSpacing(const KisSpacingInformation &spacing);
    bool needsSpacingUpdate() const;

    const KisTimingInformation &currentTiming() const;
    void updateTiming(const KisTimingInformation &timing);
    bool needsTimingUpdate() const;

    bool hasLastDabInformation() const;
    QPointF lastPosition() const;
    qreal lastDrawingAngle() const;

    bool hasLastPaintInformation() const;
    const KisPaintInformation& lastPaintInformation() const;

    int currentDabSeqNo() const;

    qreal maxPressure() const;

    void registerPaintedDab(const KisPaintInformation &info,
                            const KisSpacingInformation &spacing,
                            const KisTimingInformation &timing);

    qreal getNextPointPosition(const QPointF &start,
                               const QPointF &end,
                               qreal startTime,
                               qreal endTime);

    qreal getSpacingInterval() const;
    qreal getTimingUpdateInterval() const;

    bool isStarted() const;

    boost::optional<qreal> lockedDrawingAngleOptional() const;

    void lockCurrentDrawingAngle(const KisPaintInformation &info) const;

    qreal scalarDistanceApprox() const;

    void overrideLastValues(const QPointF &lastPosition, qreal lastAngle);

private:
    qreal getNextPointPositionIsotropic(const QPointF &start,
                                        const QPointF &end);
    qreal getNextPointPositionAnisotropic(const QPointF &start,
                                          const QPointF &end);
    qreal getNextPointPositionTimed(qreal startTime,
                                    qreal endTime);
    void resetAccumulators();

private:
    struct Private;
    Private * const m_d;
};

#endif
