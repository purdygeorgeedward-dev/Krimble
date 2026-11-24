/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_SHAPE_H
#define LIBKIS_SHAPE_H

#include <QObject>
#include <KoShape.h>

#include "kritalibkis_export.h"
#include "libkis.h"
#include <kis_types.h>

class KRITALIBKIS_EXPORT Shape : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Shape)

public:
    explicit Shape(KoShape *shape, QObject *parent = 0);
    ~Shape();

    bool operator==(const Shape &other) const;
    bool operator!=(const Shape &other) const;

public Q_SLOTS:

    QString name() const;

    void setName(const QString &name);

    virtual QString type() const;

    int zIndex() const;

    void setZIndex(int zindex);

    bool selectable() const;

    void setSelectable(bool selectable);

    bool geometryProtected() const;

    void setGeometryProtected(bool protect);

    bool visible() const;

    void setVisible(bool visible);

    QRectF boundingBox() const;

    QPointF position() const;

    void setPosition(QPointF point);

    QTransform transformation() const;

    void setTransformation(QTransform matrix);

    QTransform absoluteTransformation() const;

    bool remove();

    void update();

    void updateAbsolute(QRectF box);

    QString toSvg(bool prependStyles = false, bool stripTextMode = true);

    void select();

    void deselect();

    bool isSelected();

    Shape* parentShape() const;

private:
    friend class GroupShape;
    friend class VectorLayer;

    struct Private;
    Private *const d;

    KoShape *shape();
};

#endif
