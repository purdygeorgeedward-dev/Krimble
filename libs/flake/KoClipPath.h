/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCLIPPATH_H
#define KOCLIPPATH_H

#include "kritaflake_export.h"

#include <QList>
#include <QSharedDataPointer>
#include <qnamespace.h>
#include <KoFlakeCoordinateSystem.h>

class KoShape;
class KoPathShape;
class QPainter;
class QTransform;
class QPainterPath;
class QSizeF;

class KRITAFLAKE_EXPORT KoClipPath
{
public:

    KoClipPath(QList<KoShape*> clipShapes, KoFlake::CoordinateSystem coordinates);
    ~KoClipPath();

    KoClipPath(const KoClipPath &);
    KoClipPath &operator=(const KoClipPath &);

    KoClipPath *clone() const;

    KoFlake::CoordinateSystem coordinates() const;

    void setClipRule(Qt::FillRule clipRule);

    Qt::FillRule clipRule() const;

    QPainterPath path() const;

    QPainterPath pathForSize(const QSizeF &size) const;

    QList<KoPathShape*> clipPathShapes() const;

    QList<KoShape*> clipShapes() const;

    QTransform clipDataTransformation(KoShape *clippedShape) const;

    static void applyClipping(KoShape *clippedShape, QPainter &painter);

private:
    class Private;
    QSharedDataPointer<Private> d;
};

#endif
