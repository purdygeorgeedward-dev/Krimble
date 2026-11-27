/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_VECTORLAYER_H
#define LIBKIS_VECTORLAYER_H

#include <QObject>

#include <kis_types.h>

#include "kritalibkis_export.h"
#include "libkis.h"

#include <KoShapeControllerBase.h>

#include "Node.h"
#include "Shape.h"

class KRITALIBKIS_EXPORT VectorLayer : public Node
{
    Q_OBJECT
    Q_DISABLE_COPY(VectorLayer)

public:
    explicit VectorLayer(KoShapeControllerBase* shapeController, KisImageSP image, QString name, QObject *parent = 0);
    explicit VectorLayer(KisShapeLayerSP layer, QObject *parent = 0);
    ~VectorLayer() override;
public Q_SLOTS:

    virtual QString type() const override;

    QList<Shape *> shapes() const;

    QString toSvg();

    QList<Shape *> addShapesFromSvg(const QString &svg);

    Shape* shapeAtPosition(const QPointF &position) const;

    QList<Shape *> shapesInRect(const QRectF &rect, bool omitHiddenShapes = true, bool containedMode = false) const;

    Shape* createGroupShape(const QString &name, QList<Shape *> shapes) const;

    bool isAntialiased() const;

    void setAntialiased(const bool antialiased);
};

#endif

