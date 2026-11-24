/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2008, 2010-2011 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006-2011 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2007-2009 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "KoPathShape.h"
#include "KoPathShape_p.h"

#include "KoPathSegment.h"
#include "KoPathPoint.h"
#include "KoShapeStrokeModel.h"
#include "KoPathShapeLoader.h"
#include "KoShapeSavingContext.h"
#include "KoShapeLoadingContext.h"
#include "KoShapeBackground.h"
#include "KoShapeContainer.h"
#include "KoMarker.h"
#include "KoShapeStroke.h"
#include "KoInsets.h"

#include <KoUnit.h>
#include "KisQPainterStateSaver.h"

#include <FlakeDebug.h>
#include <QPainter>
#include <QPainterPath>

#include "kis_global.h"
#include <kis_algebra_2d.h>

#include <qnumeric.h>
static bool qIsNaNPoint(const QPointF &p) {
    return qIsNaN(p.x()) || qIsNaN(p.y());
}

KoPathShape::Private::Private()
    : fillRule(Qt::OddEvenFill)
    , autoFillMarkers(false)
{
}

KoPathShape::Private::Private(const Private &rhs)
    : fillRule(rhs.fillRule)
    , markersNew(rhs.markersNew)
    , autoFillMarkers(rhs.autoFillMarkers)
{
}

QRectF KoPathShape::Private::handleRect(const QPointF &p, qreal radius) const
{
    return QRectF(p.x() - radius, p.y() - radius, 2*radius, 2*radius);
}

KoPathShape::KoPathShape()
    : KoShape()
    , d(new Private)
{
}

KoPathShape::KoPathShape(const KoPathShape &rhs)
    : KoShape(rhs)
    , d(new Private(*rhs.d))
{
    KoSubpathList subpaths;
    Q_FOREACH (KoSubpath *subPath, rhs.d->subpaths) {
        KoSubpath *clonedSubPath = new KoSubpath();

        Q_FOREACH (KoPathPoint *point, *subPath) {
            *clonedSubPath << new KoPathPoint(*point, this);
        }

        subpaths << clonedSubPath;
    }
    d->subpaths = subpaths;
}

KoPathShape::~KoPathShape()
{
    clear();
}

KoShape *KoPathShape::cloneShape() const
{
    return new KoPathShape(*this);
}

void KoPathShape::clear()
{
    Q_FOREACH (KoSubpath *subpath, d->subpaths) {
        Q_FOREACH (KoPathPoint *point, *subpath)
            delete point;
        delete subpath;
    }
    d->subpaths.clear();

    notifyPointsChanged();
}

void KoPathShape::paint(QPainter &painter) const
{
    KisQPainterStateSaver saver(&painter);
    Q_UNUSED(saver);

    QPainterPath path(outline());
    path.setFillRule(d->fillRule);

    if (background()) {
        background()->paint(painter, path);
    }
}

#ifndef NDEBUG
void KoPathShape::Private::paintDebug(QPainter &painter)
{
    KoSubpathList::const_iterator pathIt(subpaths.constBegin());
    int i = 0;

    QPen pen(Qt::black, 0);
    painter.save();
    painter.setPen(pen);
    for (; pathIt != subpaths.constEnd(); ++pathIt) {
        KoSubpath::const_iterator it((*pathIt)->constBegin());
        for (; it != (*pathIt)->constEnd(); ++it) {
            ++i;
            KoPathPoint *point = (*it);
            QRectF r(point->point(), QSizeF(5, 5));
            r.translate(-2.5, -2.5);
            QPen pen(Qt::black, 0);
            painter.setPen(pen);
            if (point->activeControlPoint1() && point->activeControlPoint2()) {
                QBrush b(Qt::red);
                painter.setBrush(b);
            } else if (point->activeControlPoint1()) {
                QBrush b(Qt::yellow);
                painter.setBrush(b);
            } else if (point->activeControlPoint2()) {
                QBrush b(Qt::darkYellow);
                painter.setBrush(b);
            }
            painter.drawEllipse(r);
        }
    }
    painter.restore();
    debugFlake << "nop =" << i;
}

void KoPathShape::Private::debugPath() const
{
    KoSubpathList::const_iterator pathIt(subpaths.constBegin());
    for (; pathIt != subpaths.constEnd(); ++pathIt) {
        KoSubpath::const_iterator it((*pathIt)->constBegin());
        for (; it != (*pathIt)->constEnd(); ++it) {
            debugFlake << "p:" << (*pathIt) << "," << *it << "," << (*it)->point() << "," << (*it)->properties();
        }
    }
}
#endif

void KoPathShape::paintPoints(KisHandlePainterHelper &handlesHelper)
{
    KoSubpathList::const_iterator pathIt(d->subpaths.constBegin());

    for (; pathIt != d->subpaths.constEnd(); ++pathIt) {
        KoSubpath::const_iterator it((*pathIt)->constBegin());
        for (; it != (*pathIt)->constEnd(); ++it)
            (*it)->paint(handlesHelper, KoPathPoint::Node);
    }
}

QRectF KoPathShape::outlineRect() const
{
    return outline().boundingRect();
}

QPainterPath KoPathShape::outline() const
{
    QPainterPath path;
    for (auto subpathIt = d->subpaths.constBegin(); subpathIt != d->subpaths.constEnd(); ++subpathIt) {
        const KoSubpath * subpath = *subpathIt;
        const KoPathPoint * lastPoint = subpath->constFirst();
        bool activeCP = false;
        for (auto pointIt = subpath->constBegin(); pointIt != subpath->constEnd(); ++pointIt) {
            const KoPathPoint * currPoint = *pointIt;
            KoPathPoint::PointProperties currProperties = currPoint->properties();
            if (currPoint == subpath->constFirst()) {
                if (currProperties & KoPathPoint::StartSubpath) {
                    Q_ASSERT(!qIsNaNPoint(currPoint->point()));
                    path.moveTo(currPoint->point());
                }
            } else if (activeCP && currPoint->activeControlPoint1()) {
                Q_ASSERT(!qIsNaNPoint(lastPoint->controlPoint2()));
                Q_ASSERT(!qIsNaNPoint(currPoint->controlPoint1()));
                Q_ASSERT(!qIsNaNPoint(currPoint->point()));
                path.cubicTo(
                            lastPoint->controlPoint2(),
                            currPoint->controlPoint1(),
                            currPoint->point());
            } else if (activeCP || currPoint->activeControlPoint1()) {
                Q_ASSERT(!qIsNaNPoint(lastPoint->controlPoint2()));
                Q_ASSERT(!qIsNaNPoint(currPoint->controlPoint1()));
                path.quadTo(
                            activeCP ? lastPoint->controlPoint2() : currPoint->controlPoint1(),
                            currPoint->point());
            } else {
                Q_ASSERT(!qIsNaNPoint(currPoint->point()));
                path.lineTo(currPoint->point());
            }
            if (currProperties & KoPathPoint::CloseSubpath && currProperties & KoPathPoint::StopSubpath) {
                KoPathPoint * firstPoint = subpath->first();
                Q_ASSERT(!qIsNaNPoint(firstPoint->point()));
                if (currPoint->activeControlPoint2() && firstPoint->activeControlPoint1()) {
                    path.cubicTo(
                                currPoint->controlPoint2(),
                                firstPoint->controlPoint1(),
                                firstPoint->point());
                }
                else if (currPoint->activeControlPoint2() || firstPoint->activeControlPoint1()) {
                    Q_ASSERT(!qIsNaNPoint(currPoint->point()));
                    Q_ASSERT(!qIsNaNPoint(currPoint->controlPoint1()));
                    path.quadTo(
                                currPoint->activeControlPoint2() ? currPoint->controlPoint2() : firstPoint->controlPoint1(),
                                firstPoint->point());
                }
                path.closeSubpath();
            }

            if (currPoint->activeControlPoint2()) {
                activeCP = true;
            } else {
                activeCP = false;
            }
            lastPoint = currPoint;
        }
    }

    return path;
}

QRectF KoPathShape::boundingRect() const
{
    const QTransform transform = absoluteTransformation();

    qreal outlineSweepWidth = 0;

    const QSharedPointer<KoShapeStroke> lineBorder = qSharedPointerDynamicCast<KoShapeStroke>(stroke());
    if (lineBorder) {
        outlineSweepWidth = lineBorder->lineWidth();
    }

    if (stroke()) {
        KoInsets inset;
        stroke()->strokeInsets(this, inset);
        const qreal maxInset = std::max({inset.left, inset.top, inset.right, inset.bottom});

        outlineSweepWidth = std::max({outlineSweepWidth,
                                      2.0 * maxInset,
                                      2.0 * stroke()->strokeMaxMarkersInset(this)});
    }

#if 0
    QPen pen(Qt::black, outlineSweepWidth);

    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);
    QRectF bb = transform.map(pathStroke(pen)).boundingRect();
#endif

    return transform.mapRect(kisGrowRect(outline().boundingRect(), 1.1 * 0.5 * outlineSweepWidth));
}

QSizeF KoPathShape::size() const
{
    return outlineRect().size();
}

void KoPathShape::setSize(const QSizeF &newSize)
{
    QTransform matrix(resizeMatrix(newSize));

    KoShape::setSize(newSize);
    d->map(matrix);
}

QTransform KoPathShape::resizeMatrix(const QSizeF & newSize) const
{
    QSizeF oldSize = size();
    if (oldSize.width() == 0.0) {
        oldSize.setWidth(0.000001);
    }
    if (oldSize.height() == 0.0) {
        oldSize.setHeight(0.000001);
    }

    QSizeF sizeNew(newSize);
    if (sizeNew.width() == 0.0) {
        sizeNew.setWidth(0.000001);
    }
    if (sizeNew.height() == 0.0) {
        sizeNew.setHeight(0.000001);
    }

    return QTransform(sizeNew.width() / oldSize.width(), 0, 0, sizeNew.height() / oldSize.height(), 0, 0);
}

KoPathPoint * KoPathShape::moveTo(const QPointF &p)
{
    KoPathPoint * point = new KoPathPoint(this, p, KoPathPoint::StartSubpath | KoPathPoint::StopSubpath);
    KoSubpath * path = new KoSubpath;
    path->push_back(point);
    d->subpaths.push_back(path);
    notifyPointsChanged();
    return point;
}

KoPathPoint * KoPathShape::lineTo(const QPointF &p)
{
    if (d->subpaths.empty()) {
        moveTo(QPointF(0, 0));
    }
    KoPathPoint * point = new KoPathPoint(this, p, KoPathPoint::StopSubpath);
    KoPathPoint * lastPoint = d->subpaths.last()->last();
    updateLastPriv(&lastPoint);
    d->subpaths.last()->push_back(point);
    notifyPointsChanged();
    return point;
}

KoPathPoint * KoPathShape::curveTo(const QPointF &c1, const QPointF &c2, const QPointF &p)
{
    if (d->subpaths.empty()) {
        moveTo(QPointF(0, 0));
    }
    KoPathPoint * lastPoint = d->subpaths.last()->last();
    updateLastPriv(&lastPoint);
    lastPoint->setControlPoint2(c1);
    KoPathPoint * point = new KoPathPoint(this, p, KoPathPoint::StopSubpath);
    point->setControlPoint1(c2);
    d->subpaths.last()->push_back(point);
    notifyPointsChanged();
    return point;
}

KoPathPoint * KoPathShape::curveTo(const QPointF &c, const QPointF &p)
{
    if (d->subpaths.empty())
        moveTo(QPointF(0, 0));

    KoPathPoint * lastPoint = d->subpaths.last()->last();
    updateLastPriv(&lastPoint);
    lastPoint->setControlPoint2(c);
    KoPathPoint * point = new KoPathPoint(this, p, KoPathPoint::StopSubpath);
    d->subpaths.last()->push_back(point);
    notifyPointsChanged();
    return point;
}

KoPathPoint * KoPathShape::arcTo(qreal rx, qreal ry, qreal startAngle, qreal sweepAngle)
{
    if (d->subpaths.empty()) {
        moveTo(QPointF(0, 0));
    }

    KoPathPoint * lastPoint = d->subpaths.last()->last();
    if (lastPoint->properties() & KoPathPoint::CloseSubpath) {
        lastPoint = d->subpaths.last()->first();
    }
    QPointF startpoint(lastPoint->point());

    KoPathPoint * newEndPoint = lastPoint;

    QPointF curvePoints[12];
    int pointCnt = arcToCurve(rx, ry, startAngle, sweepAngle, startpoint, curvePoints);
    for (int i = 0; i < pointCnt; i += 3) {
        newEndPoint = curveTo(curvePoints[i], curvePoints[i+1], curvePoints[i+2]);
    }
    return newEndPoint;
}

int KoPathShape::arcToCurve(qreal rx, qreal ry, qreal startAngle, qreal sweepAngle, const QPointF & offset, QPointF * curvePoints) const
{
    int pointCnt = 0;

    if (sweepAngle == 0.0)
        return pointCnt;

    sweepAngle = qBound(-360.0, sweepAngle, 360.0);

    if (rx == 0 || ry == 0) {
    }

    qreal parts = ceil(qAbs(sweepAngle / 90.0));

    qreal sa_rad = startAngle * M_PI / 180.0;
    qreal partangle = sweepAngle / parts;
    qreal endangle = startAngle + partangle;
    qreal se_rad = endangle * M_PI / 180.0;
    qreal sinsa = sin(sa_rad);
    qreal cossa = cos(sa_rad);
    qreal kappa = 4.0 / 3.0 * tan((se_rad - sa_rad) / 4);

    QPointF startpoint(offset);

    QPointF center(startpoint - QPointF(cossa * rx, -sinsa * ry));

    for (int part = 0; part < parts; ++part) {
        curvePoints[pointCnt++] = QPointF(startpoint - QPointF(sinsa * rx * kappa, cossa * ry * kappa));

        qreal sinse = sin(se_rad);
        qreal cosse = cos(se_rad);

        QPointF endpoint(center + QPointF(cosse * rx, -sinse * ry));
        curvePoints[pointCnt++] = QPointF(endpoint - QPointF(-sinse * rx * kappa, -cosse * ry * kappa));
        curvePoints[pointCnt++] = endpoint;

        startpoint = endpoint;
        sinsa = sinse;
        cossa = cosse;
        endangle += partangle;
        se_rad = endangle * M_PI / 180.0;
    }

    return pointCnt;
}

void KoPathShape::close()
{
    if (d->subpaths.empty()) {
        return;
    }
    closeSubpathPriv(d->subpaths.last());
}

void KoPathShape::closeMerge()
{
    if (d->subpaths.empty()) {
        return;
    }
    closeMergeSubpathPriv(d->subpaths.last());
}

QPointF KoPathShape::normalize()
{
    QPointF tl(outline().boundingRect().topLeft());
    QTransform matrix;
    matrix.translate(-tl.x(), -tl.y());
    d->map(matrix);

    applyTransformation(matrix.inverted());
    shapeChangedPriv(ContentChanged);
    return tl;
}

void KoPathShape::Private::map(const QTransform &matrix)
{
    KoSubpathList::const_iterator pathIt(subpaths.constBegin());
    for (; pathIt != subpaths.constEnd(); ++pathIt) {
        KoSubpath::const_iterator it((*pathIt)->constBegin());
        for (; it != (*pathIt)->constEnd(); ++it) {
            if (*it) {
                (*it)->map(matrix);
            }
        }
    }
}

void KoPathShape::updateLastPriv(KoPathPoint **lastPoint)
{
    if ((*lastPoint)->properties() & KoPathPoint::StopSubpath
            && (*lastPoint)->properties() & KoPathPoint::CloseSubpath) {
        KoPathPoint *subpathStart = d->subpaths.last()->first();
        KoPathPoint * newLastPoint = new KoPathPoint(*subpathStart, this);
        newLastPoint->setProperties(KoPathPoint::Normal);
        KoSubpath *path = new KoSubpath;
        path->push_back(newLastPoint);
        d->subpaths.push_back(path);
        *lastPoint = newLastPoint;
    } else {
        (*lastPoint)->unsetProperty(KoPathPoint::StopSubpath);
    }
    (*lastPoint)->unsetProperty(KoPathPoint::CloseSubpath);
}

QList<KoPathPoint*> KoPathShape::pointsAt(const QRectF &r, const bool useControlPoints) const
{
    QList<KoPathPoint*> result;

    KoSubpathList::const_iterator pathIt(d->subpaths.constBegin());
    for (; pathIt != d->subpaths.constEnd(); ++pathIt) {
        KoSubpath::const_iterator it((*pathIt)->constBegin());
        for (; it != (*pathIt)->constEnd(); ++it) {
            if (r.contains((*it)->point()))
                result.append(*it);
            else if (useControlPoints) {
                if ((*it)->activeControlPoint1() && r.contains((*it)->controlPoint1()))
                    result.append(*it);
                else if ((*it)->activeControlPoint2() && r.contains((*it)->controlPoint2()))
                    result.append(*it);
            }
        }
    }
    return result;
}

QList<KoPathSegment> KoPathShape::segmentsAt(const QRectF &r) const
{
    QList<KoPathSegment> segments;
    int subpathCount = d->subpaths.count();
    for (int subpathIndex = 0; subpathIndex < subpathCount; ++subpathIndex) {
        KoSubpath * subpath = d->subpaths[subpathIndex];
        int pointCount = subpath->count();
        bool subpathClosed = isClosedSubpath(subpathIndex);
        for (int pointIndex = 0; pointIndex < pointCount; ++pointIndex) {
            if (pointIndex == (pointCount - 1) && ! subpathClosed)
                break;
            KoPathSegment s(subpath->at(pointIndex), subpath->at((pointIndex + 1) % pointCount));
            QRectF controlRect = s.controlPointRect();
            if (! r.intersects(controlRect) && ! controlRect.contains(r))
                continue;
            QRectF bound = s.boundingRect();
            if (! r.intersects(bound) && ! bound.contains(r))
                continue;

            segments.append(s);
        }
    }
    return segments;
}

KoPathPointIndex KoPathShape::pathPointIndex(const KoPathPoint *point) const
{
    for (int subpathIndex = 0; subpathIndex < d->subpaths.size(); ++subpathIndex) {
        KoSubpath * subpath = d->subpaths.at(subpathIndex);
        for (int pointPos = 0; pointPos < subpath->size(); ++pointPos) {
            if (subpath->at(pointPos) == point) {
                return KoPathPointIndex(subpathIndex, pointPos);
            }
        }
    }
    return KoPathPointIndex(-1, -1);
}

KoPathPoint * KoPathShape::pointByIndex(const KoPathPointIndex &pointIndex) const
{
    KoSubpath *subpath = d->subPath(pointIndex.first);

    if (subpath == 0 || pointIndex.second < 0 || pointIndex.second >= subpath->size())
        return 0;

    return subpath->at(pointIndex.second);
}

KoPathSegment KoPathShape::segmentByIndex(const KoPathPointIndex &pointIndex) const
{
    KoPathSegment segment(0, 0);

    KoSubpath *subpath = d->subPath(pointIndex.first);

    if (subpath != 0 && pointIndex.second >= 0 && pointIndex.second < subpath->size()) {
        KoPathPoint * point = subpath->at(pointIndex.second);
        int index = pointIndex.second;
        if ((index == subpath->size() - 1) && point->properties() & KoPathPoint::CloseSubpath)
            index = 0;
        else
            ++index;

        if (index < subpath->size()) {
            segment = KoPathSegment(point, subpath->at(index));
        }
    }
    return segment;
}

int KoPathShape::pointCount() const
{
    int i = 0;
    KoSubpathList::const_iterator pathIt(d->subpaths.constBegin());
    for (; pathIt != d->subpaths.constEnd(); ++pathIt) {
        i += (*pathIt)->size();
    }

    return i;
}

int KoPathShape::subpathCount() const
{
    return d->subpaths.count();
}

int KoPathShape::subpathPointCount(int subpathIndex) const
{
    KoSubpath *subpath = d->subPath(subpathIndex);

    if (subpath == 0)
        return -1;

    return subpath->size();
}

bool KoPathShape::isClosedSubpath(int subpathIndex) const
{
    KoSubpath *subpath = d->subPath(subpathIndex);

    if (subpath == 0)
        return false;

    const bool firstClosed = subpath->first()->properties() & KoPathPoint::CloseSubpath;
    const bool lastClosed = subpath->last()->properties() & KoPathPoint::CloseSubpath;

    return firstClosed && lastClosed;
}

bool KoPathShape::insertPoint(KoPathPoint* point, const KoPathPointIndex &pointIndex)
{
    KoSubpath *subpath = d->subPath(pointIndex.first);

    if (subpath == 0 || pointIndex.second < 0 || pointIndex.second > subpath->size())
        return false;

    KoPathPoint::PointProperties properties = point->properties();
    properties &= ~KoPathPoint::StartSubpath;
    properties &= ~KoPathPoint::StopSubpath;
    properties &= ~KoPathPoint::CloseSubpath;
    if (pointIndex.second == 0) {
        properties |= KoPathPoint::StartSubpath;
        if (subpath->last()->properties() & KoPathPoint::CloseSubpath) {
            properties |= KoPathPoint::CloseSubpath;
        }
        subpath->first()->unsetProperty(KoPathPoint::StartSubpath);
    }
    else if (pointIndex.second == subpath->size()) {
        properties |= KoPathPoint::StopSubpath;
        if (subpath->last()->properties() & KoPathPoint::CloseSubpath) {
            properties = properties | KoPathPoint::CloseSubpath;
        }
        subpath->last()->unsetProperty(KoPathPoint::StopSubpath);
    }

    point->setProperties(properties);
    point->setParent(this);
    subpath->insert(pointIndex.second , point);
    notifyPointsChanged();

    return true;
}

KoPathPoint * KoPathShape::removePoint(const KoPathPointIndex &pointIndex)
{
    KoSubpath *subpath = d->subPath(pointIndex.first);

    if (subpath == 0 || pointIndex.second < 0 || pointIndex.second >= subpath->size())
        return 0;

    KoPathPoint * point = subpath->takeAt(pointIndex.second);
    point->setParent(0);

    if (pointCount()==0) {
        return point;
    }
    else if (pointIndex.second == 0) {
        subpath->first()->setProperty(KoPathPoint::StartSubpath);
        if (subpath->last()->properties() & KoPathPoint::CloseSubpath) {
            subpath->first()->setProperty(KoPathPoint::CloseSubpath);
        }
    }
    else if (pointIndex.second == subpath->size()) {
        subpath->last()->setProperty(KoPathPoint::StopSubpath);
        if (point->properties() & KoPathPoint::CloseSubpath) {
            subpath->last()->setProperty(KoPathPoint::CloseSubpath);
        }
    }

    notifyPointsChanged();

    return point;
}

bool KoPathShape::breakAfter(const KoPathPointIndex &pointIndex)
{
    KoSubpath *subpath = d->subPath(pointIndex.first);

    if (!subpath || pointIndex.second < 0 || pointIndex.second > subpath->size() - 2
            || isClosedSubpath(pointIndex.first))
        return false;

    KoSubpath * newSubpath = new KoSubpath;

    int size = subpath->size();
    for (int i = pointIndex.second + 1; i < size; ++i) {
        newSubpath->append(subpath->takeAt(pointIndex.second + 1));
    }
    newSubpath->first()->setProperty(KoPathPoint::StartSubpath);
    subpath->last()->setProperty(KoPathPoint::StopSubpath);

    d->subpaths.insert(pointIndex.first + 1, newSubpath);
    notifyPointsChanged();

    return true;
}

bool KoPathShape::join(int subpathIndex)
{
    KoSubpath *subpath = d->subPath(subpathIndex);
    KoSubpath *nextSubpath = d->subPath(subpathIndex + 1);

    if (!subpath || !nextSubpath || isClosedSubpath(subpathIndex)
            || isClosedSubpath(subpathIndex+1))
        return false;

    subpath->last()->unsetProperty(KoPathPoint::StopSubpath);
    nextSubpath->first()->unsetProperty(KoPathPoint::StartSubpath);

    Q_FOREACH (KoPathPoint * p, *nextSubpath)
        subpath->append(p);

    d->subpaths.removeAt(subpathIndex + 1);

    delete nextSubpath;

    notifyPointsChanged();

    return true;
}

bool KoPathShape::moveSubpath(int oldSubpathIndex, int newSubpathIndex)
{
    KoSubpath *subpath = d->subPath(oldSubpathIndex);

    if (subpath == 0 || newSubpathIndex >= d->subpaths.size())
        return false;

    if (oldSubpathIndex == newSubpathIndex)
        return true;

    d->subpaths.removeAt(oldSubpathIndex);
    d->subpaths.insert(newSubpathIndex, subpath);

    notifyPointsChanged();

    return true;
}

KoPathPointIndex KoPathShape::openSubpath(const KoPathPointIndex &pointIndex)
{
    KoSubpath *subpath = d->subPath(pointIndex.first);

    if (!subpath || pointIndex.second < 0 || pointIndex.second >= subpath->size()
            || !isClosedSubpath(pointIndex.first))
        return KoPathPointIndex(-1, -1);

    KoPathPoint * oldStartPoint = subpath->first();
    oldStartPoint->unsetProperty(KoPathPoint::StartSubpath);
    subpath->last()->unsetProperty(KoPathPoint::StopSubpath);

    for (int i = 0; i < pointIndex.second; ++i) {
        subpath->append(subpath->takeFirst());
    }
    subpath->first()->setProperty(KoPathPoint::StartSubpath);
    subpath->last()->setProperty(KoPathPoint::StopSubpath);

    notifyPointsChanged();

    return pathPointIndex(oldStartPoint);
}

KoPathPointIndex KoPathShape::closeSubpath(const KoPathPointIndex &pointIndex)
{
    KoSubpath *subpath = d->subPath(pointIndex.first);

    if (!subpath || pointIndex.second < 0 || pointIndex.second >= subpath->size()
            || isClosedSubpath(pointIndex.first))
        return KoPathPointIndex(-1, -1);

    KoPathPoint * oldStartPoint = subpath->first();
    oldStartPoint->unsetProperty(KoPathPoint::StartSubpath);
    subpath->last()->unsetProperty(KoPathPoint::StopSubpath);

    for (int i = 0; i < pointIndex.second; ++i) {
        subpath->append(subpath->takeFirst());
    }
    subpath->first()->setProperty(KoPathPoint::StartSubpath);
    subpath->last()->setProperty(KoPathPoint::StopSubpath);

    closeSubpathPriv(subpath);

    notifyPointsChanged();

    return pathPointIndex(oldStartPoint);
}

bool KoPathShape::reverseSubpath(int subpathIndex)
{
    KoSubpath *subpath = d->subPath(subpathIndex);

    if (subpath == 0)
        return false;

    int size = subpath->size();
    for (int i = 0; i < size; ++i) {
        KoPathPoint *p = subpath->takeAt(i);
        p->reverse();
        subpath->prepend(p);
    }

    KoPathPoint *first = subpath->first();
    KoPathPoint *last = subpath->last();

    KoPathPoint::PointProperties firstProps = first->properties();
    KoPathPoint::PointProperties lastProps = last->properties();

    firstProps |= KoPathPoint::StartSubpath;
    firstProps &= ~KoPathPoint::StopSubpath;
    lastProps |= KoPathPoint::StopSubpath;
    lastProps &= ~KoPathPoint::StartSubpath;
    if (firstProps & KoPathPoint::CloseSubpath) {
        firstProps |= KoPathPoint::CloseSubpath;
        lastProps |= KoPathPoint::CloseSubpath;
    }
    first->setProperties(firstProps);
    last->setProperties(lastProps);

    notifyPointsChanged();

    return true;
}

KoSubpath * KoPathShape::removeSubpath(int subpathIndex)
{
    KoSubpath *subpath = d->subPath(subpathIndex);

    if (subpath != 0) {
        Q_FOREACH (KoPathPoint* point, *subpath) {
            point->setParent(this);
        }
        d->subpaths.removeAt(subpathIndex);
    }

    notifyPointsChanged();

    return subpath;
}

bool KoPathShape::addSubpath(KoSubpath * subpath, int subpathIndex)
{
    if (subpathIndex < 0 || subpathIndex > d->subpaths.size())
        return false;

    Q_FOREACH (KoPathPoint* point, *subpath) {
        point->setParent(this);
    }

    d->subpaths.insert(subpathIndex, subpath);
    notifyPointsChanged();

    return true;
}
int KoPathShape::combine(KoPathShape *path)
{
    int insertSegmentPosition = -1;
    if (!path) return insertSegmentPosition;

    QTransform pathMatrix = path->absoluteTransformation();
    QTransform myMatrix = absoluteTransformation().inverted();

    Q_FOREACH (KoSubpath* subpath, path->d->subpaths) {
        KoSubpath *newSubpath = new KoSubpath();

        Q_FOREACH (KoPathPoint* point, *subpath) {
            KoPathPoint *newPoint = new KoPathPoint(*point, this);
            newPoint->map(pathMatrix);
            newPoint->map(myMatrix);
            newSubpath->append(newPoint);
        }
        d->subpaths.append(newSubpath);

        if (insertSegmentPosition < 0) {
            insertSegmentPosition = d->subpaths.size() - 1;
        }
    }
    normalize();

    notifyPointsChanged();
    return insertSegmentPosition;
}

bool KoPathShape::separate(QList<KoPathShape*> & separatedPaths)
{
    if (! d->subpaths.size())
        return false;

    QTransform myMatrix = absoluteTransformation();

    Q_FOREACH (KoSubpath* subpath, d->subpaths) {
        KoPathShape *shape = new KoPathShape();

        shape->setStroke(stroke());
        shape->setBackground(background());
        shape->setShapeId(shapeId());
        shape->setZIndex(zIndex());

        KoSubpath *newSubpath = new KoSubpath();

        Q_FOREACH (KoPathPoint* point, *subpath) {
            KoPathPoint *newPoint = new KoPathPoint(*point, shape);
            newPoint->map(myMatrix);
            newSubpath->append(newPoint);
        }
        shape->d->subpaths.append(newSubpath);
        shape->normalize();

        separatedPaths.append(shape);
    }
    return true;
}

void KoPathShape::closeSubpathPriv(KoSubpath *subpath)
{
    if (! subpath)
        return;

    subpath->last()->setProperty(KoPathPoint::CloseSubpath);
    subpath->first()->setProperty(KoPathPoint::CloseSubpath);

    notifyPointsChanged();
}

void KoPathShape::closeMergeSubpathPriv(KoSubpath *subpath)
{
    if (! subpath || subpath->size() < 2)
        return;

    KoPathPoint * lastPoint = subpath->last();
    KoPathPoint * firstPoint = subpath->first();

    if (lastPoint->point() == firstPoint->point()) {
        firstPoint->setProperty(KoPathPoint::StartSubpath);
        firstPoint->setProperty(KoPathPoint::CloseSubpath);
        if (lastPoint->activeControlPoint1())
            firstPoint->setControlPoint1(lastPoint->controlPoint1());
        delete subpath->takeLast();
        lastPoint = subpath->last();
        lastPoint->setProperty(KoPathPoint::StopSubpath);
        lastPoint->setProperty(KoPathPoint::CloseSubpath);

        notifyPointsChanged();
    } else {
        closeSubpathPriv(subpath);
    }
}

const KoSubpathList &KoPathShape::subpaths() const
{
    return d->subpaths;
}

KoSubpathList &KoPathShape::subpaths()
{
    return d->subpaths;
}

void KoPathShape::map(const QTransform &matrix)
{
    return d->map(matrix);
}

KoSubpath *KoPathShape::Private::subPath(int subpathIndex) const
{
    if (subpathIndex < 0 || subpathIndex >= subpaths.size())
        return 0;

    return subpaths.at(subpathIndex);
}

QString KoPathShape::pathShapeId() const
{
    return KoPathShapeId;
}

QString KoPathShape::toString(const QTransform &matrix) const
{
    QString pathString;

    KoSubpathList::const_iterator pathIt(d->subpaths.constBegin());
    for (; pathIt != d->subpaths.constEnd(); ++pathIt) {
        KoSubpath::const_iterator pointIt((*pathIt)->constBegin());
        KoPathPoint *firstPoint(*pointIt);
        KoPathPoint *lastPoint = firstPoint;
        bool activeControlPoint2 = false;

        for (; pointIt != (*pathIt)->constEnd(); ++pointIt) {
            KoPathPoint *currPoint(*pointIt);
            if (!currPoint) {
                qWarning() << "Found a zero point in the shape's path!";
                continue;
            }
            if (currPoint == firstPoint) {
                if (currPoint->properties() & KoPathPoint::StartSubpath) {
                    const QPointF p = matrix.map(currPoint->point());
                    pathString += QString("M%1 %2").arg(p.x()).arg(p.y());
                }
            }
            else if (activeControlPoint2 || currPoint->activeControlPoint1()) {
                const bool isCubic = activeControlPoint2 && currPoint->activeControlPoint1();
                KoPathSegment cubicSeg = isCubic ? KoPathSegment(lastPoint, currPoint)
                                                 : KoPathSegment(lastPoint, currPoint).toCubic();
                if (cubicSeg.first()  && cubicSeg.second()) {
                    const QPointF cp1 = matrix.map(cubicSeg.first()->controlPoint2());
                    const QPointF cp2 = matrix.map(cubicSeg.second()->controlPoint1());
                    const QPointF p = matrix.map(cubicSeg.second()->point());
                    pathString += QString("C%1 %2 %3 %4 %5 %6")
                            .arg(cp1.x()).arg(cp1.y())
                            .arg(cp2.x()).arg(cp2.y())
                            .arg(p.x()).arg(p.y());
                }
            }
            else {
                const QPointF p = matrix.map(currPoint->point());
                pathString += QString("L%1 %2").arg(p.x()).arg(p.y());
            }
            if (currPoint->properties() & KoPathPoint::StopSubpath
                    && currPoint->properties() & KoPathPoint::CloseSubpath) {
                if (currPoint->activeControlPoint2() || firstPoint->activeControlPoint1()) {
                    const bool isCubic = currPoint->activeControlPoint2() && firstPoint->activeControlPoint1();
                    KoPathSegment cubicSeg = isCubic ? KoPathSegment(currPoint, firstPoint)
                                                     : KoPathSegment(currPoint, firstPoint).toCubic();
                    if (cubicSeg.first()  && cubicSeg.second()) {
                        const QPointF cp1 = matrix.map(cubicSeg.first()->controlPoint2());
                        const QPointF cp2 = matrix.map(cubicSeg.second()->controlPoint1());

                        const QPointF p = matrix.map(cubicSeg.second()->point());
                        pathString += QString("C%1 %2 %3 %4 %5 %6")
                                .arg(cp1.x()).arg(cp1.y())
                                .arg(cp2.x()).arg(cp2.y())
                                .arg(p.x()).arg(p.y());
                    }
                }
                pathString += QString("Z");
            }

            activeControlPoint2 = currPoint->activeControlPoint2();
            lastPoint = currPoint;
        }
    }

    return pathString;
}

char nodeType(const KoPathPoint * point)
{
    if (point->properties() & KoPathPoint::IsSmooth) {
        return 's';
    }
    else if (point->properties() & KoPathPoint::IsSymmetric) {
        return 'z';
    }
    else {
        return 'c';
    }
}

QString KoPathShape::nodeTypes() const
{
    QString types;
    KoSubpathList::const_iterator pathIt(d->subpaths.constBegin());
    for (; pathIt != d->subpaths.constEnd(); ++pathIt) {
        KoSubpath::const_iterator it((*pathIt)->constBegin());
        for (; it != (*pathIt)->constEnd(); ++it) {
            if (it == (*pathIt)->constBegin()) {
                types.append('c');
            }
            else {
                types.append(nodeType(*it));
            }

            if ((*it)->properties() & KoPathPoint::StopSubpath
                    && (*it)->properties() & KoPathPoint::CloseSubpath) {
                KoPathPoint * firstPoint = (*pathIt)->first();
                types.append(nodeType(firstPoint));
            }
        }
    }
    return types;
}

void updateNodeType(KoPathPoint * point, const QChar & nodeType)
{
    if (nodeType == 's') {
        point->setProperty(KoPathPoint::IsSmooth);
    }
    else if (nodeType == 'z') {
        point->setProperty(KoPathPoint::IsSymmetric);
    }
}

void KoPathShape::loadNodeTypes(const QString &nodeTypes)
{
    QString::const_iterator nIt(nodeTypes.constBegin());
    KoSubpathList::const_iterator pathIt(d->subpaths.constBegin());
    for (; pathIt != d->subpaths.constEnd(); ++pathIt) {
        KoSubpath::const_iterator it((*pathIt)->constBegin());
        for (; it != (*pathIt)->constEnd(); ++it, nIt++) {
            if (nIt == nodeTypes.constEnd()) {
                warnFlake << "not enough nodes in sodipodi:nodetypes";
                return;
            }
            if (it != (*pathIt)->constBegin()) {
                updateNodeType(*it, *nIt);
            }

            if ((*it)->properties() & KoPathPoint::StopSubpath
                    && (*it)->properties() & KoPathPoint::CloseSubpath) {
                ++nIt;
                updateNodeType((*pathIt)->first(), *nIt);
            }
        }
    }
}

Qt::FillRule KoPathShape::fillRule() const
{
    return d->fillRule;
}

void KoPathShape::setFillRule(Qt::FillRule fillRule)
{
    d->fillRule = fillRule;
}

KoPathShape * KoPathShape::createShapeFromPainterPath(const QPainterPath &path)
{
    KoPathShape * shape = new KoPathShape();

    int elementCount = path.elementCount();
    QPointF lastTeleportedToPoint = QPointF();
    for (int i = 0; i < elementCount; i++) {
        QPainterPath::Element element = path.elementAt(i);
        bool nextIsMove = (i == elementCount - 1) || (path.elementAt(i + 1).isMoveTo());
        bool merge = nextIsMove && KisAlgebra2D::fuzzyPointCompare(lastTeleportedToPoint, QPointF(element.x, element.y));

        switch (element.type) {
        case QPainterPath::MoveToElement:
            shape->moveTo(QPointF(element.x, element.y));
            lastTeleportedToPoint = QPointF(element.x, element.y);
            break;
        case QPainterPath::LineToElement:
            shape->lineTo(QPointF(element.x, element.y));
            if (merge) {
                shape->closeMerge();
            }
            break;
        case QPainterPath::CurveToElement:
            shape->curveTo(QPointF(element.x, element.y),
                           QPointF(path.elementAt(i + 1).x, path.elementAt(i + 1).y),
                           QPointF(path.elementAt(i + 2).x, path.elementAt(i + 2).y));
            if (merge) {
                shape->closeMerge();
            }
            break;
        default:
            continue;
        }
    }

    shape->setShapeId(KoPathShapeId);

    return shape;
}

bool KoPathShape::hitTest(const QPointF &position) const
{
    if (parent() && parent()->isClipped(this) && ! parent()->hitTest(position))
        return false;

    QPointF point = absoluteTransformation().inverted().map(position);
    const QPainterPath outlinePath = outline();
    if (stroke()) {
        KoInsets insets;
        stroke()->strokeInsets(this, insets);
        QRectF roi(QPointF(-insets.left, -insets.top), QPointF(insets.right, insets.bottom));

        roi.moveCenter(point);
        if (outlinePath.intersects(roi) || outlinePath.contains(roi))
            return true;
    } else {
        if (outlinePath.contains(point))
            return true;
    }

    return false;
}

void KoPathShape::setMarker(KoMarker *marker, KoFlake::MarkerPosition pos)
{
    if (!marker && d->markersNew.contains(pos)) {
        d->markersNew.remove(pos);
    } else {
        d->markersNew[pos] = marker;
    }

    notifyChanged();
    shapeChangedPriv(StrokeChanged);
}

KoMarker *KoPathShape::marker(KoFlake::MarkerPosition pos) const
{
    return d->markersNew[pos].data();
}

bool KoPathShape::hasMarkers() const
{
    return !d->markersNew.isEmpty();
}

bool KoPathShape::autoFillMarkers() const
{
    return d->autoFillMarkers;
}

void KoPathShape::setAutoFillMarkers(bool value)
{
    d->autoFillMarkers = value;
}

KoPathSegment KoPathShape::segmentAtPoint(const QPointF &point, const QRectF &grabRoi) const
{
    const qreal distanceThreshold = 0.5 * KisAlgebra2D::maxDimension(grabRoi);
    KoPathSegment segment;

    const QPointF p = documentToShape(point);
    const QRectF roi = documentToShape(grabRoi);

    qreal minDistance = std::numeric_limits<qreal>::max();

    const QList<KoPathSegment> segments = segmentsAt(roi);

    foreach (KoPathSegment s, segments) {
        const qreal nearestPointParam = s.nearestPoint(p);
        const QPointF nearestPoint = s.pointAt(nearestPointParam);
        const qreal distance = kisDistance(p, nearestPoint);

        if (distance > distanceThreshold)
            continue;
        if (distance < minDistance) {
            segment = s;
        }
    }

    return segment;
}

void KoPathShape::recommendPointSelectionChange(const QList<KoPathPointIndex> &newSelection)
{
    Q_FOREACH (KoShape::ShapeChangeListener *listener, listeners()) {
        PointSelectionChangeListener *pointListener = dynamic_cast<PointSelectionChangeListener*>(listener);
        if (pointListener) {
            pointListener->recommendPointSelectionChange(this, newSelection);
        }
    }
}

void KoPathShape::notifyPointsChanged()
{
    Q_FOREACH (KoShape::ShapeChangeListener *listener, listeners()) {
        PointSelectionChangeListener *pointListener = dynamic_cast<PointSelectionChangeListener*>(listener);
        if (pointListener) {
            pointListener->notifyPathPointsChanged(this);
        }
    }
}

QPainterPath KoPathShape::pathStroke(const QPen &pen) const
{
    if (d->subpaths.isEmpty()) {
        return QPainterPath();
    }
    QPainterPath pathOutline;

    QPainterPathStroker stroker;
    stroker.setWidth(0);
    stroker.setJoinStyle(Qt::MiterJoin);
    stroker.setWidth(pen.widthF());
    stroker.setJoinStyle(pen.joinStyle());
    stroker.setMiterLimit(pen.miterLimit());
    stroker.setCapStyle(pen.capStyle());
    stroker.setDashOffset(pen.dashOffset());
    stroker.setDashPattern(pen.dashPattern());

    QPainterPath path = stroker.createStroke(outline());

    pathOutline.addPath(path);
    pathOutline.setFillRule(Qt::WindingFill);

    return pathOutline;
}

void KoPathShape::PointSelectionChangeListener::notifyShapeChanged(KoShape::ChangeType type, KoShape *shape)
{
    Q_UNUSED(type);
    Q_UNUSED(shape);
}
