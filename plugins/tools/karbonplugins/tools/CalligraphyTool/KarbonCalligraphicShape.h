/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Fela Winkelmolen <fela.kde@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KARBONCALLIGRAPHICSHAPE_H
#define KARBONCALLIGRAPHICSHAPE_H

#include <KoParameterShape.h>
#include <QDebug>

#define KarbonCalligraphicShapeId "KarbonCalligraphicShape"

class KarbonCalligraphicPoint
{
public:
    KarbonCalligraphicPoint(const QPointF &point, qreal angle, qreal width)
        : m_point(point), m_angle(angle), m_width(width) {}

    KarbonCalligraphicPoint(const KarbonCalligraphicPoint &rhs) = default;
    KarbonCalligraphicPoint() = default;

    bool operator==(const KarbonCalligraphicPoint &rhs) const {
        return m_point == rhs.m_point &&
            qFuzzyCompare(m_angle, rhs.m_angle) &&
            qFuzzyCompare(m_width, rhs.m_width);
    }

    QPointF point() const
    {
        return m_point;
    }
    qreal angle() const
    {
        return m_angle;
    }
    qreal width() const
    {
        return m_width;
    }

    void setPoint(const QPointF &point)
    {
        m_point = point;
    }
    void setAngle(qreal angle)
    {
        m_angle = angle;
    }

private:
    QPointF m_point;
    qreal m_angle = 0.0;
    qreal m_width = 0.0;
};

inline QDebug operator<<(QDebug dbg, const KarbonCalligraphicPoint &pt)
{
    dbg.nospace() << "(" << pt.point() << ", a: " << pt.angle() << ", w: " << pt.width() << ")";
    return dbg.space();
}

class KarbonCalligraphicShape : public KoParameterShape
{
public:
    explicit KarbonCalligraphicShape(qreal caps = 0.0);
    ~KarbonCalligraphicShape() override;

    KoShape* cloneShape() const override;

    void appendPoint(const QPointF &p1, qreal angle, qreal width);
    void appendPointToPath(const KarbonCalligraphicPoint &p);

    const QRectF lastPieceBoundingRect();

    void setSize(const QSizeF &newSize) override;

    QPointF normalize() override;

    void simplifyPath();

    void simplifyGuidePath();

    QString pathShapeId() const override;

protected:
    void moveHandleAction(int handleId,
                          const QPointF &point,
                          Qt::KeyboardModifiers modifiers = Qt::NoModifier) override;

    void updatePath(const QSizeF &size) override;

private:
    KarbonCalligraphicShape(const KarbonCalligraphicShape &rhs);

    void appendPointsToPathAux(const QPointF &p1, const QPointF &p2);

    bool flipDetected(const QPointF &p1, const QPointF &p2);

    void smoothLastPoints();
    void smoothPoint(const int index);

    static int ccw(const QPointF &p1, const QPointF &p2, const QPointF &p3);

    void addCap(int index1, int index2, int pointIndex, bool inverted = false);

    struct Private;
    QSharedDataPointer<Private> s;
};

#endif

