/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006, 2011 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006-2008 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPATHSHAPE_H
#define KOPATHSHAPE_H

#include "kritaflake_export.h"

#include <QMetaType>
#include <QTransform>

#include "KoShape.h"

#define KoPathShapeId "KoPathShape"

class KoPathSegment;
class KoPathPoint;
class KoPathShapePrivate;
class KoMarker;
class KisHandlePainterHelper;

typedef QPair<int, int> KoPathPointIndex;

typedef QList<KoPathPoint *> KoSubpath;
typedef QList<KoSubpath *> KoSubpathList;
class KRITAFLAKE_EXPORT KoPathShape : public KoShape
{
public:
    KoPathShape();

    ~KoPathShape() override;

    KoShape *cloneShape() const override;

    void paint(QPainter &painter) const override;
    virtual void paintPoints(KisHandlePainterHelper &handlesHelper);

    QRectF outlineRect() const override;
    QPainterPath outline() const override;
    QRectF boundingRect() const override;
    QSizeF size() const override;

    QPainterPath pathStroke(const QPen &pen) const;
    void setSize(const QSizeF &size) override;

    bool hitTest(const QPointF &position) const override;

    void clear();
    KoPathPoint *moveTo(const QPointF &p);

    KoPathPoint *lineTo(const QPointF &p);

    KoPathPoint *curveTo(const QPointF &c1, const QPointF &c2, const QPointF &p);

    KoPathPoint *curveTo(const QPointF &c, const QPointF &p);

    KoPathPoint *arcTo(qreal rx, qreal ry, qreal startAngle, qreal sweepAngle);

    void close();

    void closeMerge();

    virtual QPointF normalize();

    QList<KoPathPoint*> pointsAt(const QRectF &rect, const bool useControlPoints = false) const;

    QList<KoPathSegment> segmentsAt(const QRectF &rect) const;

    KoPathPointIndex pathPointIndex(const KoPathPoint *point) const;

    KoPathPoint *pointByIndex(const KoPathPointIndex &pointIndex) const;

    KoPathSegment segmentByIndex(const KoPathPointIndex &pointIndex) const;

    int pointCount() const;

    int subpathCount() const;

    int subpathPointCount(int subpathIndex) const;

    bool isClosedSubpath(int subpathIndex) const;

    bool insertPoint(KoPathPoint *point, const KoPathPointIndex &pointIndex);

    KoPathPoint *removePoint(const KoPathPointIndex &pointIndex);

    bool breakAfter(const KoPathPointIndex &pointIndex);

    bool join(int subpathIndex);

    bool moveSubpath(int oldSubpathIndex, int newSubpathIndex);

    KoPathPointIndex openSubpath(const KoPathPointIndex &pointIndex);

    KoPathPointIndex closeSubpath(const KoPathPointIndex &pointIndex);

    bool reverseSubpath(int subpathIndex);

    KoSubpath *removeSubpath(int subpathIndex);

    bool addSubpath(KoSubpath *subpath, int subpathIndex);

    int combine(KoPathShape *path);

    bool separate(QList<KoPathShape*> &separatedPaths);

    virtual QString pathShapeId() const;

    QString toString(const QTransform &matrix = QTransform()) const;

    QString nodeTypes() const;

    void loadNodeTypes(const QString &nodeTypes);

    Qt::FillRule fillRule() const;

    void setFillRule(Qt::FillRule fillRule);

    static KoPathShape *createShapeFromPainterPath(const QPainterPath &path);

    void setMarker(KoMarker *marker, KoFlake::MarkerPosition pos);
    KoMarker* marker(KoFlake::MarkerPosition pos) const;
    bool hasMarkers() const;

    bool autoFillMarkers() const;
    void setAutoFillMarkers(bool value);

    KoPathSegment segmentAtPoint(const QPointF &point, const QRectF &grabRoi) const;

public:
    struct KRITAFLAKE_EXPORT PointSelectionChangeListener : public ShapeChangeListener {
        void notifyShapeChanged(ChangeType type, KoShape *shape) override;
        virtual void recommendPointSelectionChange(KoPathShape *shape, const QList<KoPathPointIndex> &newSelection) = 0;
        virtual void notifyPathPointsChanged(KoPathShape *shape) = 0;
    };

    void recommendPointSelectionChange(const QList<KoPathPointIndex> &newSelection);

protected:
    void notifyPointsChanged();

protected:
    KoPathShape(const KoPathShape &rhs);

protected:

    int arcToCurve(qreal rx, qreal ry, qreal startAngle, qreal sweepAngle, const QPointF &offset, QPointF *curvePoints) const;

    QTransform resizeMatrix( const QSizeF &newSize ) const;

private:
    void closeMergeSubpathPriv(KoSubpath *subpath);
    void closeSubpathPriv(KoSubpath *subpath);
    void updateLastPriv(KoPathPoint **lastPoint);

protected:
    const KoSubpathList &subpaths() const;
    KoSubpathList &subpaths();
    void map(const QTransform &matrix);

private:
    class Private;
    QScopedPointer<Private> d;
};

Q_DECLARE_METATYPE(KoPathShape*)

#endif
