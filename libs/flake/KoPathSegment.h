/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHSEGMENT_H
#define KOPATHSEGMENT_H

#include "kritaflake_export.h"
#include <QList>
#include <QPair>

class KoPathPoint;
class QTransform;
class QPointF;
class QRectF;

class KRITAFLAKE_EXPORT KoPathSegment
{
public:
    explicit KoPathSegment(KoPathPoint * first = 0, KoPathPoint * second = 0);

    KoPathSegment(const KoPathSegment &segment);

    KoPathSegment(const QPointF &p0, const QPointF &p1);
    KoPathSegment(const QPointF &p0, const QPointF &p1, const QPointF &p2);
    KoPathSegment(const QPointF &p0, const QPointF &p1, const QPointF &p2, const QPointF &p3);

    KoPathSegment& operator=(const KoPathSegment &other);

    ~KoPathSegment();

    KoPathPoint *first() const;

    void setFirst(KoPathPoint *first);

    KoPathPoint *second() const;

    void setSecond(KoPathPoint *second);

    bool isValid() const;

    bool operator==(const KoPathSegment &other) const;

    int degree() const;

    QList<QPointF> intersections(const KoPathSegment &segment) const;

    QList<QPointF> convexHull() const;

    QPair<KoPathSegment, KoPathSegment> splitAt(qreal t) const;

    QPointF pointAt(qreal t) const;

    QRectF boundingRect() const;

    QRectF controlPointRect() const;

    KoPathSegment mapped(const QTransform &matrix) const;

    KoPathSegment toCubic() const;

    qreal length(qreal error = 0.005) const;

    qreal lengthAt(qreal t, qreal error = 0.005) const;

    qreal paramAtLength(qreal length, qreal tolerance = 0.001) const;

    bool isFlat(qreal tolerance = 0.01) const;

    qreal nearestPoint(const QPointF &point) const;

    QList<QPointF> controlPoints() const;

    static KoPathSegment interpolate(const QPointF &p0, const QPointF &p1, const QPointF &p2, qreal t);

    QPointF angleVectorAtParam(qreal t);

private:
    class Private;
    Private * const d;
};

#endif
