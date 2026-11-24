/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Rob Buis <buis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSPIRALSHAPE_H
#define KOSPIRALSHAPE_H

#include "KoParameterShape.h"

#define SpiralShapeId "SpiralShape"

class SpiralShape : public KoParameterShape
{
public:
    enum SpiralType {
        Curve = 0,
        Line = 1
    };

    SpiralShape();
    ~SpiralShape() override;

    KoShape* cloneShape() const override;

    void setSize(const QSizeF &newSize) override;
    QPointF normalize() override;

    void setType(SpiralType type);

    SpiralType type() const;

    void setFade(qreal fade);

    qreal fade() const;

    bool clockWise() const;
    void setClockWise(bool clockwise);

    QString pathShapeId() const override;

protected:
    SpiralShape(const SpiralShape &rhs);

    void moveHandleAction(int handleId, const QPointF &point, Qt::KeyboardModifiers modifiers = Qt::NoModifier) override;
    void updatePath(const QSizeF &size) override;
    void createPath(const QSizeF &size);

private:
    void updateKindHandle();
    void updateAngleHandles();

    qreal m_fade;
    qreal m_kindAngle;
    QPointF m_center;
    QPointF m_radii;
    SpiralType m_type;
    bool m_clockwise;

    KoSubpath m_points;
};

#endif

