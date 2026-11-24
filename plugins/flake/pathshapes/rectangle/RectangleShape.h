/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KORECTANGLESHAPE_H
#define KORECTANGLESHAPE_H

#include "KoParameterShape.h"
#include <SvgShape.h>

#define RectangleShapeId "RectangleShape"

class RectangleShape : public KoParameterShape, public SvgShape
{
public:
    RectangleShape();
    ~RectangleShape() override;

    KoShape* cloneShape() const override;

    qreal cornerRadiusX() const;

    void setCornerRadiusX(qreal radius);

    qreal cornerRadiusY() const;

    void setCornerRadiusY(qreal radius);
    QString pathShapeId() const override;

    bool saveSvg(SvgSavingContext &context) override;

    bool loadSvg(const QDomElement &element, SvgLoadingContext &context) override;

protected:
    RectangleShape(const RectangleShape &rhs);

    void moveHandleAction(int handleId, const QPointF &point, Qt::KeyboardModifiers modifiers = Qt::NoModifier) override;
    void updatePath(const QSizeF &size) override;
    void createPoints(int requiredPointCount);
    void updateHandles();

private:
    qreal m_cornerRadiusX;
    qreal m_cornerRadiusY;
};

#endif

