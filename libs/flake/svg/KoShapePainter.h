/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEPAINTER_H
#define KOSHAPEPAINTER_H

#include <QList>
#include <QRectF>
#include <QScopedPointer>
#include "kritaflake_export.h"

class KoShape;
class QPainter;
class QImage;
class KoShapeManager;

class KRITAFLAKE_EXPORT KoShapePainter
{
public:
    explicit KoShapePainter();
    ~KoShapePainter();

    void setShapes(const QList<KoShape*> &shapes);

    void paint(QPainter &painter);

    void paint(QPainter &painter, const QRect &painterRect, const QRectF &documentRect);

    void paint(QImage &image);

    QRectF contentRect() const;

    KoShapeManager *internalShapeManager() const;

    void setUpdateFunction(std::function<void(const QRectF&)> function);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif
