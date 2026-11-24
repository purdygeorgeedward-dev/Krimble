/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006, 2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSELECTION_H
#define KOSELECTION_H

#include <QObject>

#include "KoShape.h"
#include "KoFlake.h"

#include "kritaflake_export.h"

class KoShapeLayer;
class KoSelectionPrivate;

class KRITAFLAKE_EXPORT KoSelection : public QObject, public KoShape, public KoShape::ShapeChangeListener
{
    Q_OBJECT

public:

    KoSelection(QObject *parent = 0);
    ~KoSelection() override;

    void paint(QPainter &painter) const override;
    void setSize(const QSizeF &size) override;
    QSizeF size() const override;
    QRectF outlineRect() const override;
    QRectF boundingRect() const override;

    void select(KoShape *shape);

    void deselect(KoShape *shape);

    void deselectAll();

    const QList<KoShape*> selectedShapes() const;

    const QList<KoShape*> selectedVisibleShapes() const;

    const QList<KoShape*> selectedEditableShapes() const;

    const QList<KoShape*> selectedEditableShapesAndDelegates() const;

    KoShape *firstSelectedShape() const;

    bool isSelected(const KoShape *shape) const;

    int count() const;

    bool hitTest(const QPointF &position) const override;

    void setActiveLayer(KoShapeLayer *layer);

    KoShapeLayer *activeLayer() const;

    void notifyShapeChanged(ChangeType type, KoShape *shape) override;

Q_SIGNALS:
    void selectionChanged();

    void currentLayerChanged(const KoShapeLayer *layer);

protected:
    KoSelection(const KoSelection &rhs);

private:
    class Private;
    QSharedDataPointer<Private> d;
};

#endif
