/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOELLIPSESHAPE_H
#define KOELLIPSESHAPE_H

#include "KoParameterShape.h"
#include <SvgShape.h>

#define EllipseShapeId "EllipseShape"

class EllipseShape : public KoParameterShape, public SvgShape
{
public:
    enum EllipseType {
        Arc = 0,
        Pie = 1,
        Chord = 2
    };

    EllipseShape();
    ~EllipseShape() override;

    KoShape* cloneShape() const override;

    void setSize(const QSizeF &newSize) override;
    QPointF normalize() override;

    void setType(EllipseType type);

    EllipseType type() const;

    void setStartAngle(qreal angle);

    qreal startAngle() const;

    void setEndAngle(qreal angle);

    qreal endAngle() const;

    QString pathShapeId() const override;

    bool saveSvg(SvgSavingContext &context) override;

    bool loadSvg(const QDomElement &element, SvgLoadingContext &context) override;

protected:

    void moveHandleAction(int handleId, const QPointF &point, Qt::KeyboardModifiers modifiers = Qt::NoModifier) override;
    void updatePath(const QSizeF &size) override;
    void createPoints(int requiredPointCount);

private:
    qreal sweepAngle() const;

    void updateKindHandle();
    void updateAngleHandles();

    EllipseShape(const EllipseShape &rhs);

    qreal m_startAngle;
    qreal m_endAngle;
    qreal m_kindAngle;
    QPointF m_center;
    QPointF m_radii;
    EllipseType m_type;
};

#endif

