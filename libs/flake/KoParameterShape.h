/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPARAMETERSHAPE_H
#define KOPARAMETERSHAPE_H

#include "KoPathShape.h"
#include "kritaflake_export.h"

class KoParameterShapePrivate;
class KisHandlePainterHelper;

class KRITAFLAKE_EXPORT KoParameterShape : public KoPathShape
{
public:
    KoParameterShape();
    ~KoParameterShape() override;

    void moveHandle(int handleId, const QPointF &point, Qt::KeyboardModifiers modifiers = Qt::NoModifier);

    int handleIdAt(const QRectF &rect) const;

    QPointF handlePosition(int handleId) const;

    void paintHandles(KisHandlePainterHelper &handlesHelper);

    void paintHandle(KisHandlePainterHelper &handlesHelper, int handleId);

    void setSize(const QSizeF &size) override;

    bool isParametricShape() const;

    void setParametricShape(bool parametric);

    QPointF normalize() override;

    int handleCount() const;

protected:
    QList<QPointF> handles() const;

    void setHandles(const QList<QPointF> &handles);

    KoParameterShape(const KoParameterShape &rhs);

    virtual void moveHandleAction(int handleId, const QPointF & point, Qt::KeyboardModifiers modifiers = Qt::NoModifier) = 0;

    virtual void updatePath(const QSizeF &size) = 0;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

#endif
