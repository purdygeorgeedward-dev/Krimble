/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2008 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSTARSHAPE_H
#define KOSTARSHAPE_H

#include <array>
#include <KoParameterShape.h>

#define StarShapeId "StarShape"

class StarShape : public KoParameterShape
{
public:
    StarShape();
    ~StarShape() override;

    KoShape* cloneShape() const override;

    void setCornerCount(uint cornerCount);

    uint cornerCount() const;

    void setBaseRadius(qreal baseRadius);

    qreal baseRadius() const;

    void setTipRadius(qreal tipRadius);

    qreal tipRadius() const;

    void setBaseRoundness(qreal baseRoundness);

    void setTipRoundness(qreal tipRoundness);

    void setConvex(bool convex);

    bool convex() const;

    QPointF starCenter() const;

    void setSize(const QSizeF &newSize) override;
    QString pathShapeId() const override;

protected:
    StarShape(const StarShape &rhs);

    void moveHandleAction(int handleId, const QPointF &point, Qt::KeyboardModifiers modifiers = Qt::NoModifier) override;
    void updatePath(const QSizeF &size) override;
    void createPoints(int requiredPointCount);

private:
    QPointF computeCenter() const;

    double defaultAngleRadian() const;

    enum Handles { tip = 0, base = 1 };

    uint m_cornerCount;
    std::array<qreal, 2> m_radius;
    std::array<qreal, 2> m_angles;
    qreal m_zoomX;
    qreal m_zoomY;
    std::array<qreal, 2> m_roundness;
    QPointF m_center;
    bool m_convex;
};

#endif

