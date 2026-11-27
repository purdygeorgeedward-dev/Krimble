/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPathSegment.h"
#include "KoPathPoint.h"
#include <FlakeDebug.h>
#include <QPainterPath>
#include <QTransform>
#include <math.h>

#include "kis_global.h"

#include <KisBezierUtils.h>

class Q_DECL_HIDDEN KoPathSegment::Private
{
public:
    Private(KoPathSegment *qq, KoPathPoint *p1, KoPathPoint *p2)
        : first(p1),
        second(p2),
        q(qq)
    {
    }

    qreal distanceFromChord(const QPointF &point) const;

    qreal chordLength() const;

    QList<QPointF> linesIntersection(const KoPathSegment &segment) const;

    QList<qreal> extrema() const;

    QList<qreal> roots() const;

    void deCasteljau(qreal t, QPointF *p1, QPointF *p2, QPointF *p3, QPointF *p4, QPointF *p5) const;

    KoPathPoint *first;
    KoPathPoint *second;
    KoPathSegment *q;
};

void KoPathSegment::Private::deCasteljau(qreal t, QPointF *p1, QPointF *p2, QPointF *p3, QPointF *p4, QPointF *p5) const
{
    if (!q->isValid())
      return;

    int deg = q->degree();
    QPointF q[4];

    q[0] = first->point();
    if (deg == 2) {
        q[1] = first->activeControlPoint2() ? first->controlPoint2() : second->controlPoint1();
    } else if (deg == 3) {
        q[1] = first->controlPoint2();
        q[2] = second->controlPoint1();
    }
    if (deg >= 0) {
        q[deg] = second->point();
    }

    QPointF p[3];

    for (unsigned short j = 1; j <= deg; ++j) {
        for (unsigned short i = 0; i <= deg - j; ++i) {
            q[i] = (1.0 - t) * q[i] + t * q[i + 1];
        }
        p[j - 1] = q[0];
    }

    if (deg == 2) {
        if (p2)
            *p2 = p[0];
        if (p3)
            *p3 = p[1];
        if (p4)
            *p4 = q[1];
    } else if (deg == 3) {
        if (p1)
            *p1 = p[0];
        if (p2)
            *p2 = p[1];
        if (p3)
            *p3 = p[2];
        if (p4)
            *p4 = q[1];
        if (p5)
            *p5 = q[2];
    }
}

QList<qreal> KoPathSegment::Private::roots() const
{
    QList<qreal> rootParams;

    if (!q->isValid())
        return rootParams;

    int xAxisCrossings = KisBezierUtils::controlPolygonZeros(q->controlPoints());

    if (!xAxisCrossings) {
    }
    else if (xAxisCrossings == 1 && q->isFlat(0.01 / chordLength())) {
        QPointF chord = second->point() - first->point();
        QPointF segStart = first->point();
        rootParams.append((chord.x() * segStart.y() - chord.y() * segStart.x()) / - chord.y());
    }
    else {
        QPair<KoPathSegment, KoPathSegment> splitSegments = q->splitAt(0.5);
        rootParams += splitSegments.first.d->roots();
        rootParams += splitSegments.second.d->roots();
    }

    return rootParams;
}

QList<qreal> KoPathSegment::Private::extrema() const
{
    int deg = q->degree();
    if (deg <= 1)
        return QList<qreal>();

    QList<qreal> params;

    if (deg == 2) {

        QPointF cp = first->activeControlPoint2() ?
                     first->controlPoint2() : second->controlPoint1();
        QPointF x0 = cp - first->point();
        QPointF x1 = second->point() - cp;

        QPointF a = x1 - x0;
        QPointF c = x0;

        if (a.x() != 0.0)
            params.append(-c.x() / a.x());
        if (a.y() != 0.0)
            params.append(-c.y() / a.y());
    } else if (deg == 3) {
        QPointF x0 = first->controlPoint2() - first->point();
        QPointF x1 = second->controlPoint1() - first->controlPoint2();
        QPointF x2 = second->point() - second->controlPoint1();

        QPointF a = x2 - 2.0 * x1 + x0;
        QPointF b = 2.0 * x1 - 2.0 * x0;
        QPointF c = x0;

        if (a.x() == 0.0) {
            params.append(- c.x() / b.x());
        } else {
            qreal rx = b.x() * b.x() - 4.0 * a.x() * c.x();
            if (rx < 0.0)
                rx = 0.0;
            params.append((-b.x() + sqrt(rx)) / (2.0*a.x()));
            params.append((-b.x() - sqrt(rx)) / (2.0*a.x()));
        }

        if (a.y() == 0.0) {
            params.append(- c.y() / b.y());
        } else {
            qreal ry = b.y() * b.y() - 4.0 * a.y() * c.y();
            if (ry < 0.0)
                ry = 0.0;
            params.append((-b.y() + sqrt(ry)) / (2.0*a.y()));
            params.append((-b.y() - sqrt(ry)) / (2.0*a.y()));
        }
    }

    return params;
}

qreal KoPathSegment::Private::distanceFromChord(const QPointF &point) const
{
    QPointF chord = second->point() - first->point();
    QPointF relPoint = point - first->point();
    qreal scale = chord.x() * relPoint.x() + chord.y() * relPoint.y();
    scale /= chord.x() * chord.x() + chord.y() * chord.y();

    QPointF diffVec = scale * chord - relPoint;

    qreal distance = sqrt(diffVec.x() * diffVec.x() + diffVec.y() * diffVec.y());

    if (chord.x()*relPoint.y() - chord.y()*relPoint.x() > 0) {
        return distance;
    } else {
        return -distance;
    }
}

qreal KoPathSegment::Private::chordLength() const
{
    QPointF chord = second->point() - first->point();
    return sqrt(chord.x() * chord.x() + chord.y() * chord.y());
}

QList<QPointF> KoPathSegment::Private::linesIntersection(const KoPathSegment &segment) const
{
    QList<QPointF> isects;
    QPointF A = first->point();
    QPointF B = second->point();
    QPointF C = segment.first()->point();
    QPointF D = segment.second()->point();

    qreal denom = (B.x() - A.x()) * (D.y() - C.y()) - (B.y() - A.y()) * (D.x() - C.x());
    qreal num_r = (A.y() - C.y()) * (D.x() - C.x()) - (A.x() - C.x()) * (D.y() - C.y());
    if (denom == 0.0 && num_r == 0.0)
        return isects;

    qreal num_s = (A.y() - C.y()) * (B.x() - A.x()) - (A.x() - C.x()) * (B.y() - A.y());
    qreal r = num_r / denom;
    qreal s = num_s / denom;

    if (r < 0.0 || r > 1.0)
        return isects;
    if (s < 0.0 || s > 1.0)
        return isects;

    isects.append(A + r * (B - A));

    return isects;
}

KoPathSegment::KoPathSegment(KoPathPoint * first, KoPathPoint * second)
    : d(new Private(this, first, second))
{
}

KoPathSegment::KoPathSegment(const KoPathSegment & segment)
    : d(new Private(this, 0, 0))
{
    if (! segment.first() || segment.first()->parent())
        setFirst(segment.first());
    else
        setFirst(new KoPathPoint(*segment.first()));

    if (! segment.second() || segment.second()->parent())
        setSecond(segment.second());
    else
        setSecond(new KoPathPoint(*segment.second()));
}

KoPathSegment::KoPathSegment(const QPointF &p0, const QPointF &p1)
    : d(new Private(this, new KoPathPoint(), new KoPathPoint()))
{
    d->first->setPoint(p0);
    d->second->setPoint(p1);
}

KoPathSegment::KoPathSegment(const QPointF &p0, const QPointF &p1, const QPointF &p2)
    : d(new Private(this, new KoPathPoint(), new KoPathPoint()))
{
    d->first->setPoint(p0);
    d->first->setControlPoint2(p1);
    d->second->setPoint(p2);
}

KoPathSegment::KoPathSegment(const QPointF &p0, const QPointF &p1, const QPointF &p2, const QPointF &p3)
    : d(new Private(this, new KoPathPoint(), new KoPathPoint()))
{
    d->first->setPoint(p0);
    d->first->setControlPoint2(p1);
    d->second->setControlPoint1(p2);
    d->second->setPoint(p3);
}

KoPathSegment &KoPathSegment::operator=(const KoPathSegment &rhs)
{
    if (this == &rhs)
        return (*this);

    if (! rhs.first() || rhs.first()->parent())
        setFirst(rhs.first());
    else
        setFirst(new KoPathPoint(*rhs.first()));

    if (! rhs.second() || rhs.second()->parent())
        setSecond(rhs.second());
    else
        setSecond(new KoPathPoint(*rhs.second()));

    return (*this);
}

KoPathSegment::~KoPathSegment()
{
    if (d->first && ! d->first->parent())
        delete d->first;
    if (d->second && ! d->second->parent())
        delete d->second;
    delete d;
}

KoPathPoint *KoPathSegment::first() const
{
    return d->first;
}

void KoPathSegment::setFirst(KoPathPoint *first)
{
    if (d->first && !d->first->parent())
        delete d->first;
    d->first = first;
}

KoPathPoint *KoPathSegment::second() const
{
    return d->second;
}

void KoPathSegment::setSecond(KoPathPoint *second)
{
    if (d->second && !d->second->parent())
        delete d->second;
    d->second = second;
}

bool KoPathSegment::isValid() const
{
    return (d->first && d->second);
}

bool KoPathSegment::operator==(const KoPathSegment &rhs) const
{
    if (!isValid() && !rhs.isValid())
        return true;
    if (isValid() && !rhs.isValid())
        return false;
    if (!isValid() && rhs.isValid())
        return false;

    return (*first() == *rhs.first() && *second() == *rhs.second());
}

int KoPathSegment::degree() const
{
    if (!d->first || !d->second)
        return -1;

    bool c1 = d->first->activeControlPoint2();
    bool c2 = d->second->activeControlPoint1();
    if (!c1 && !c2)
        return 1;
    if (c1 && c2)
        return 3;
    return 2;
}

QPointF KoPathSegment::pointAt(qreal t) const
{
    if (!isValid())
        return QPointF();

    if (degree() == 1) {
        return d->first->point() + t * (d->second->point() - d->first->point());
    } else {
        QPointF splitP;

        d->deCasteljau(t, 0, 0, &splitP, 0, 0);

        return splitP;
    }
}

QRectF KoPathSegment::controlPointRect() const
{
    if (!isValid())
        return QRectF();

    QList<QPointF> points = controlPoints();
    QRectF bbox(points.first(), points.first());
    Q_FOREACH (const QPointF &p, points) {
        bbox.setLeft(qMin(bbox.left(), p.x()));
        bbox.setRight(qMax(bbox.right(), p.x()));
        bbox.setTop(qMin(bbox.top(), p.y()));
        bbox.setBottom(qMax(bbox.bottom(), p.y()));
    }

    if (degree() == 1) {
        if (bbox.height() == 0.0)
            bbox.setHeight(0.1);
        if (bbox.width() == 0.0)
            bbox.setWidth(0.1);
    }

    return bbox;
}

QRectF KoPathSegment::boundingRect() const
{
    if (!isValid())
        return QRectF();

    QRectF rect = QRectF(d->first->point(), d->second->point()).normalized();

    if (degree() == 1) {
        if (rect.height() == 0.0)
            rect.setHeight(0.1);
        if (rect.width() == 0.0)
            rect.setWidth(0.1);
    } else {
        foreach (qreal t, d->extrema()) {
            if (t >= 0.0 && t <= 1.0) {
                QPointF p = pointAt(t);
                rect.setLeft(qMin(rect.left(), p.x()));
                rect.setRight(qMax(rect.right(), p.x()));
                rect.setTop(qMin(rect.top(), p.y()));
                rect.setBottom(qMax(rect.bottom(), p.y()));
            }
        }
    }

    return rect;
}

QList<QPointF> KoPathSegment::intersections(const KoPathSegment &segment) const
{

    QList<QPointF> isects;

    if (!isValid() || !segment.isValid())
        return isects;

    int degree1 = degree();
    int degree2 = segment.degree();

    QRectF myBound = boundingRect();
    QRectF otherBound = segment.boundingRect();
    if (!myBound.intersects(otherBound)) {
        return isects;
    }

    if (degree1 == 1 && degree2 == 1) {
        isects += d->linesIntersection(segment);
        return isects;
    }

    qreal dmin, dmax;
    if (degree1 == 1) {
        dmin = 0.0;
        dmax = 0.0;
    } else if (degree1 == 2) {
        qreal d1;
        if (d->first->activeControlPoint2())
            d1 = d->distanceFromChord(d->first->controlPoint2());
        else
            d1 = d->distanceFromChord(d->second->controlPoint1());
        dmin = qMin(qreal(0.0), qreal(0.5 * d1));
        dmax = qMax(qreal(0.0), qreal(0.5 * d1));
    } else {
        qreal d1 = d->distanceFromChord(d->first->controlPoint2());
        qreal d2 = d->distanceFromChord(d->second->controlPoint1());
        if (d1*d2 > 0.0) {
            dmin = 0.75 * qMin(qreal(0.0), qMin(d1, d2));
            dmax = 0.75 * qMax(qreal(0.0), qMax(d1, d2));
        } else {
            dmin = 4.0 / 9.0 * qMin(qreal(0.0), qMin(d1, d2));
            dmax = 4.0 / 9.0 * qMax(qreal(0.0), qMax(d1, d2));
        }
    }

    KoPathSegment dt;
    if (degree2 == 1) {
        QPointF p0(0.0, d->distanceFromChord(segment.first()->point()));
        QPointF p1(1.0, d->distanceFromChord(segment.second()->point()));
        dt = KoPathSegment(p0, p1);
    } else if (degree2 == 2) {
        QPointF p0(0.0, d->distanceFromChord(segment.first()->point()));
        QPointF p1 = segment.first()->activeControlPoint2()
                     ? QPointF(0.5, d->distanceFromChord(segment.first()->controlPoint2()))
                     : QPointF(0.5, d->distanceFromChord(segment.second()->controlPoint1()));
        QPointF p2(1.0, d->distanceFromChord(segment.second()->point()));
        dt = KoPathSegment(p0, p1, p2);
    } else if (degree2 == 3) {
        QPointF p0(0.0, d->distanceFromChord(segment.first()->point()));
        QPointF p1(1. / 3., d->distanceFromChord(segment.first()->controlPoint2()));
        QPointF p2(2. / 3., d->distanceFromChord(segment.second()->controlPoint1()));
        QPointF p3(1.0, d->distanceFromChord(segment.second()->point()));
        dt = KoPathSegment(p0, p1, p2, p3);
    } else {
        return isects;
    }

    QList<QPointF> hull = dt.convexHull();

    int hullCount = hull.count();
    qreal tmin = 1.0, tmax = 0.0;
    bool intersectionsFoundMax = false;
    bool intersectionsFoundMin = false;

    for (int i = 0; i < hullCount; ++i) {
        QPointF p1 = hull[i];
        QPointF p2 = hull[(i+1) % hullCount];
        if (p1.y() > dmax && p2.y() > dmax)
            continue;
        if (p1.y() < dmin && p2.y() < dmin)
            continue;
        if (p1.x() == p2.x()) {
            bool dmaxIntersection = (dmax < qMax(p1.y(), p2.y()) && dmax > qMin(p1.y(), p2.y()));
            bool dminIntersection = (dmin < qMax(p1.y(), p2.y()) && dmin > qMin(p1.y(), p2.y()));
            if (dmaxIntersection || dminIntersection) {
                tmin = qMin(tmin, p1.x());
                tmax = qMax(tmax, p1.x());
                if (dmaxIntersection) {
                    intersectionsFoundMax = true;
                } else {
                    intersectionsFoundMin = true;
                }
            }
        } else if (p1.y() == p2.y()) {
            if (p1.y() == dmin || p1.y() == dmax) {
                if (p1.y() == dmin) {
                    intersectionsFoundMin = true;
                } else {
                    intersectionsFoundMax = true;
                }
                tmin = qMin(tmin, p1.x());
                tmin = qMin(tmin, p2.x());
                tmax = qMax(tmax, p1.x());
                tmax = qMax(tmax, p2.x());
            }
        } else {
            qreal dx = p2.x() - p1.x();
            qreal dy = p2.y() - p1.y();
            qreal m = dy / dx;
            qreal n = p1.y() - m * p1.x();
            qreal t1 = (dmax - n) / m;
            if (t1 >= 0.0 && t1 <= 1.0) {
                tmin = qMin(tmin, t1);
                tmax = qMax(tmax, t1);
                intersectionsFoundMax = true;
            }
            qreal t2 = (dmin - n) / m;
            if (t2 >= 0.0 && t2 < 1.0) {
                tmin = qMin(tmin, t2);
                tmax = qMax(tmax, t2);
                intersectionsFoundMin = true;
            }
        }
    }

    bool intersectionsFound = intersectionsFoundMin && intersectionsFoundMax;

    if (!intersectionsFound || (1.0 - (tmax - tmin)) <= 0.2) {
        QPair<KoPathSegment, KoPathSegment> parts = splitAt(0.5);
        if (d->chordLength() < 1e-5)
            isects += parts.first.second()->point();
        else {
            isects += segment.intersections(parts.first);
            isects += segment.intersections(parts.second);
        }
    } else if (qAbs(tmin - tmax) < 1e-5) {
        isects.append(segment.pointAt(tmin));
    } else {
        QPair<KoPathSegment, KoPathSegment> clip1 = segment.splitAt(tmin);
        qreal t = (tmax - tmin) / (1.0 - tmin);
        QPair<KoPathSegment, KoPathSegment> clip2 = clip1.second.splitAt(t);
        isects += clip2.first.intersections(*this);
    }

    return isects;
}

KoPathSegment KoPathSegment::mapped(const QTransform &matrix) const
{
    if (!isValid())
        return *this;

    KoPathPoint * p1 = new KoPathPoint(*d->first);
    KoPathPoint * p2 = new KoPathPoint(*d->second);
    p1->map(matrix);
    p2->map(matrix);

    return KoPathSegment(p1, p2);
}

KoPathSegment KoPathSegment::toCubic() const
{
    if (! isValid())
        return KoPathSegment();

    KoPathPoint * p1 = new KoPathPoint(*d->first);
    KoPathPoint * p2 = new KoPathPoint(*d->second);

    if (degree() == 1) {
        p1->setControlPoint2(p1->point() + 0.3 * (p2->point() - p1->point()));
        p2->setControlPoint1(p2->point() + 0.3 * (p1->point() - p2->point()));
    } else if (degree() == 2) {
        QPointF a1 = p1->activeControlPoint2() ? p1->controlPoint2() : p2->controlPoint1();
        QPointF b1 = p1->point() + 2.0 / 3.0 * (a1 - p1->point());
        QPointF b2 = a1 + 1.0 / 3.0 * (p2->point() - a1);
        p1->setControlPoint2(b1);
        p2->setControlPoint1(b2);
    }

    return KoPathSegment(p1, p2);
}

qreal KoPathSegment::length(qreal error) const
{

    int deg = degree();

    if (deg == -1)
        return 0.0;

    QList<QPointF> ctrlPoints = controlPoints();

    qreal chordLen = d->chordLength();

    if (deg == 1) {
        return chordLen;
    }

    qreal polyLength = 0.0;

    for (int i = 0; i < deg; ++i) {
        QPointF ctrlSegment = ctrlPoints[i+1] - ctrlPoints[i];
        polyLength += sqrt(ctrlSegment.x() * ctrlSegment.x() + ctrlSegment.y() * ctrlSegment.y());
    }

    if ((polyLength - chordLen) > error) {
        QPair<KoPathSegment, KoPathSegment> parts = splitAt(0.5);
        return parts.first.length(error) + parts.second.length(error);
    } else {
        if (deg == 3)
            return 0.5 * chordLen + 0.5 * polyLength;
        else
            return (2.0 * chordLen + polyLength) / 3.0;
    }
}

qreal KoPathSegment::lengthAt(qreal t, qreal error) const
{
    if (t == 0.0)
        return 0.0;
    if (t == 1.0)
        return length(error);

    QPair<KoPathSegment, KoPathSegment> parts = splitAt(t);
    return parts.first.length(error);
}

qreal KoPathSegment::paramAtLength(qreal length, qreal tolerance) const
{
    const int deg = degree();
    if (deg < 1 || length <= 0.0) {
        return 0.0;
    }

    if (deg == 1) {
        return qMin(qreal(1.0), length / d->chordLength());
    }

    if (length >= d->chordLength() && length >= this->length(tolerance)) {
        return 1.0;
    }

    qreal startT = 0.0;
    qreal midT = 0.5;
    qreal endT = 1.0;

    qreal midLength = lengthAt(0.5);
    while (qAbs(midLength - length) / length > tolerance) {
        if (midLength < length)
            startT = midT;
        else
            endT = midT;

        midT = 0.5 * (startT + endT);
        midLength = lengthAt(midT);
    }

    return midT;
}

bool KoPathSegment::isFlat(qreal tolerance) const
{
    if (degree() <= 1)
        return true;

    QPointF chord = d->second->point() - d->first->point();
    qreal chordAngle = atan2(chord.y(), chord.x());
    QTransform m;
    m.translate(d->first->point().x(), d->first->point().y());
    m.rotate(chordAngle * M_PI / 180.0);
    m.translate(-d->first->point().x(), -d->first->point().y());

    KoPathSegment s = mapped(m);

    qreal minDist = 0.0;
    qreal maxDist = 0.0;

    foreach (qreal t, s.d->extrema()) {
        if (t >= 0.0 && t <= 1.0) {
            QPointF p = pointAt(t);
            qreal dist = s.d->distanceFromChord(p);
            minDist = qMin(dist, minDist);
            maxDist = qMax(dist, maxDist);
        }
    }

    return (maxDist - minDist <= tolerance);
}

QList<QPointF> KoPathSegment::convexHull() const
{
    QList<QPointF> hull;
    int deg = degree();
    if (deg == 1) {
        hull.append(d->first->point());
        hull.append(d->second->point());
    } else if (deg == 2) {
        QPointF chord = d->second->point() - d->first->point();
        QPointF cp = d->first->activeControlPoint2() ? d->first->controlPoint2() : d->second->controlPoint1();
        QPointF relP = cp - d->first->point();
        bool pIsRight = (chord.x() * relP.y() - chord.y() * relP.x() > 0);
        hull.append(d->first->point());
        if (pIsRight)
            hull.append(cp);
        hull.append(d->second->point());
        if (! pIsRight)
            hull.append(cp);
    } else if (deg == 3) {
        QPointF chord = d->second->point() - d->first->point();
        QPointF relP1 = d->first->controlPoint2() - d->first->point();
        bool p1IsRight = (chord.x() * relP1.y() - chord.y() * relP1.x() > 0);
        hull.append(d->first->point());
        if (p1IsRight)
            hull.append(d->first->controlPoint2());
        hull.append(d->second->point());
        if (! p1IsRight)
            hull.append(d->first->controlPoint2());

        bool rightOfEdge[3];
        QPointF lastPoint = d->second->controlPoint1();
        for (int i = 0; i < 3; ++i) {
            QPointF relP = lastPoint - hull[i];
            QPointF edge = hull[(i+1)%3] - hull[i];
            rightOfEdge[i] = (edge.x() * relP.y() - edge.y() * relP.x() > 0);
        }
        for (int i = 0; i < 3; ++i) {
            int prev = (3 + i - 1) % 3;
            int next = (i + 1) % 3;
            if (! rightOfEdge[prev] && rightOfEdge[i] && ! rightOfEdge[next]) {
                hull.insert(i + 1, lastPoint);
                break;
            }
            if (rightOfEdge[i] && rightOfEdge[next]) {
                hull[i+1] = lastPoint;
                break;
            }
            if (rightOfEdge[i] && rightOfEdge[prev]) {
                hull[i] = lastPoint;
                break;
            }
        }
    }

    return hull;
}

QPair<KoPathSegment, KoPathSegment> KoPathSegment::splitAt(qreal t) const
{
    QPair<KoPathSegment, KoPathSegment> results;
    if (!isValid())
        return results;

    if (degree() == 1) {
        QPointF p = d->first->point() + t * (d->second->point() - d->first->point());
        results.first = KoPathSegment(d->first->point(), p);
        results.second = KoPathSegment(p, d->second->point());
    } else {
        QPointF newCP2, newCP1, splitP, splitCP1, splitCP2;

        d->deCasteljau(t, &newCP2, &splitCP1, &splitP, &splitCP2, &newCP1);

        if (degree() == 2) {
            if (second()->activeControlPoint1()) {
                KoPathPoint *s1p1 = new KoPathPoint(0, d->first->point());
                KoPathPoint *s1p2 = new KoPathPoint(0, splitP);
                s1p2->setControlPoint1(splitCP1);
                KoPathPoint *s2p1 = new KoPathPoint(0, splitP);
                KoPathPoint *s2p2 = new KoPathPoint(0, d->second->point());
                s2p2->setControlPoint1(splitCP2);
                results.first = KoPathSegment(s1p1, s1p2);
                results.second = KoPathSegment(s2p1, s2p2);
            } else {
                results.first = KoPathSegment(d->first->point(), splitCP1, splitP);
                results.second = KoPathSegment(splitP, splitCP2, d->second->point());
            }
        } else {
            results.first = KoPathSegment(d->first->point(), newCP2, splitCP1, splitP);
            results.second = KoPathSegment(splitP, splitCP2, newCP1, d->second->point());
        }
    }

    return results;
}

QList<QPointF> KoPathSegment::controlPoints() const
{
    QList<QPointF> controlPoints;
    controlPoints.append(d->first->point());
    if (d->first->activeControlPoint2())
        controlPoints.append(d->first->controlPoint2());
    if (d->second->activeControlPoint1())
        controlPoints.append(d->second->controlPoint1());
    controlPoints.append(d->second->point());

    return controlPoints;
}

qreal KoPathSegment::nearestPoint(const QPointF &point) const
{
    if (!isValid())
        return -1.0;

    return KisBezierUtils::nearestPoint(controlPoints(), point);
}

KoPathSegment KoPathSegment::interpolate(const QPointF &p0, const QPointF &p1, const QPointF &p2, qreal t)
{
    if (t <= 0.0 || t >= 1.0)
        return KoPathSegment();

    QPointF c1 = p1 - (1.0-t) * (1.0-t)*p0 - t * t * p2;

    qreal denom = 2.0 * t * (1.0-t);

    c1.rx() /= denom;
    c1.ry() /= denom;

    return KoPathSegment(p0, c1, p2);
}

QPointF KoPathSegment::angleVectorAtParam(qreal t)
{
    const QPointF dA1 = QLineF(d->first->point(), d->first->controlPoint2()).pointAt(t);
    const QPointF dA2 = QLineF(d->first->controlPoint2(), d->second->controlPoint1()).pointAt(t);
    const QPointF dA3 = QLineF(d->second->controlPoint1(), d->second->point()).pointAt(t);

    QLineF finalDerivative = QLineF(QLineF(dA1, dA2).pointAt(t), QLineF(dA2, dA3).pointAt(t)).unitVector();

    return finalDerivative.p2() - finalDerivative.p1();
}

#if 0
void KoPathSegment::printDebug() const
{
    int deg = degree();
    debugFlake << "degree:" << deg;
    if (deg < 1)
        return;

    debugFlake << "P0:" << d->first->point();
    if (deg == 1) {
        debugFlake << "P2:" << d->second->point();
    } else if (deg == 2) {
        if (d->first->activeControlPoint2())
            debugFlake << "P1:" << d->first->controlPoint2();
        else
            debugFlake << "P1:" << d->second->controlPoint1();
        debugFlake << "P2:" << d->second->point();
    } else if (deg == 3) {
        debugFlake << "P1:" << d->first->controlPoint2();
        debugFlake << "P2:" << d->second->controlPoint1();
        debugFlake << "P3:" << d->second->point();
    }
}
#endif
