/*
 *  SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_CUBIC_CURVE_H_
#define _KIS_CUBIC_CURVE_H_

#include <boost/operators.hpp>

#include<QList>
#include<QVector>
#include<QPointF>
#include<QVariant>

#include "kis_cubic_curve_spline.h"

#include <kritaimage_export.h>

const QString DEFAULT_CURVE_STRING = "0,0;1,1;";

class KRITAIMAGE_EXPORT KisCubicCurvePoint
{
public:
    KisCubicCurvePoint() = default;
    KisCubicCurvePoint(const KisCubicCurvePoint&) = default;
    KisCubicCurvePoint(const QPointF &position, bool setAsCorner = false);
    KisCubicCurvePoint(qreal x, qreal y, bool setAsCorner = false);
    KisCubicCurvePoint& operator=(const KisCubicCurvePoint&) = default;

    bool operator==(const KisCubicCurvePoint &other) const;

    qreal x() const;
    qreal y() const;
    const QPointF& position() const;
    bool isSetAsCorner() const;

    void setX(qreal newX);
    void setY(qreal newY);
    void setPosition(const QPointF &newPosition);
    void setAsCorner(bool newIsSetAsCorner);

private:
    QPointF m_position;
    bool m_isCorner { false };
};

Q_DECLARE_METATYPE(KisCubicCurvePoint)

class KRITAIMAGE_EXPORT KisCubicCurve : public boost::equality_comparable<KisCubicCurve>
{
public:
    KisCubicCurve();

    KisCubicCurve(const QList<QPointF>& points);
    KisCubicCurve(const QList<KisCubicCurvePoint>& points);

    KisCubicCurve(const QString &curveString);
    KisCubicCurve(const KisCubicCurve& curve);
    ~KisCubicCurve();
    KisCubicCurve& operator=(const KisCubicCurve& curve);
    bool operator==(const KisCubicCurve& curve) const;
public:
    qreal value(qreal x) const;
    Q_DECL_DEPRECATED QList<QPointF> points() const;
    const QList<KisCubicCurvePoint>& curvePoints() const;
    void setPoints(const QList<QPointF>& points);
    void setPoints(const QList<KisCubicCurvePoint>& points);
    void setPoint(int idx, const KisCubicCurvePoint& point);
    void setPoint(int idx, const QPointF& position, bool setAsCorner);
    void setPoint(int idx, const QPointF& position);
    void setPointPosition(int idx, const QPointF& position);
    void setPointAsCorner(int idx, bool setAsCorner);
    int addPoint(const KisCubicCurvePoint& point);
    int addPoint(const QPointF& position, bool setAsCorner);
    int addPoint(const QPointF& position);
    void removePoint(int idx);

    bool isIdentity() const;

    bool isConstant(qreal c) const;

    void setName(const QString& name);
    const QString& name() const;

    static qreal interpolateLinear(qreal normalizedValue, const QVector<qreal> &transfer);

public:
    const QVector<quint16> uint16Transfer(int size = 256) const;
    const QVector<qreal> floatTransfer(int size = 256) const;
public:
    QString toString() const;
    Q_DECL_DEPRECATED void fromString(const QString&);
private:
    struct Data;
    struct Private;
    Private* const d {nullptr};
};

Q_DECLARE_METATYPE(KisCubicCurve)

#endif
