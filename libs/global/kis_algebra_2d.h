/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_ALGEBRA_2D_H
#define __KIS_ALGEBRA_2D_H

#include <QPoint>
#include <QPointF>
#include <QVector>
#include <QPolygonF>
#include <QPainterPath>
#include <QTransform>
#include <cmath>
#include <kis_global.h>
#include <kritaglobal_export.h>
#include <functional>
#include <boost/optional.hpp>
#include <optional>

class QPainterPath;
class QTransform;

namespace KisAlgebra2D {

template <class T>
struct PointTypeTraits
{
};

template <>
struct PointTypeTraits<QPoint>
{
    typedef int value_type;
    typedef qreal calculation_type;
    typedef QRect rect_type;
};

template <>
struct PointTypeTraits<QPointF>
{
    typedef qreal value_type;
    typedef qreal calculation_type;
    typedef QRectF rect_type;
};

template <class T>
typename PointTypeTraits<T>::value_type dotProduct(const T &a, const T &b)
{
    return a.x() * b.x() + a.y() * b.y();
}

template <class T>
typename PointTypeTraits<T>::value_type crossProduct(const T &a, const T &b)
{
    return a.x() * b.y() - a.y() * b.x();
}

template <class T>
qreal norm(const T &a)
{
    return std::sqrt(pow2(a.x()) + pow2(a.y()));
}

template <class T>
qreal normSquared(const T &a)
{
    return pow2(a.x()) + pow2(a.y());
}

template <class Point>
Point normalize(const Point &a)
{
    const qreal length = norm(a);
    return (1.0 / length) * a;
}

template <typename Point>
Point lerp(const Point &pt1, const Point &pt2, qreal t)
{
    return pt1 + (pt2 - pt1) * t;
}

template <typename T>
T signPZ(T x) {
    return x >= T(0) ? T(1) : T(-1);
}

template <typename T>
T signZZ(T x) {
    return x == T(0) ? T(0) : x > T(0) ? T(1) : T(-1);
}

template <typename T>
    inline T copysign(T x, T y) {
    T strippedX = qAbs(x);
    return y >= T(0) ? strippedX : -strippedX;
}

template<class T>
typename std::enable_if<std::is_integral<T>::value, T>::type
divideFloor(T a, T b)
{
    const bool a_neg = a < T(0);
    const bool b_neg = b < T(0);

    if (a == T(0)) {
        return 0;
    } else if (a_neg == b_neg) {
        return a / b;
    } else {
        const T a_abs = qAbs(a);
        const T b_abs = qAbs(b);

        return - 1 - (a_abs - T(1)) / b_abs;
    }
}

template <class T>
T leftUnitNormal(const T &a)
{
    T result = a.x() != 0 ? T(-a.y() / a.x(), 1) : T(-1, 0);
    qreal length = norm(result);
    result *= (crossProduct(a, result) >= 0 ? 1 : -1) / length;

    return -result;
}

template <class T>
T rightUnitNormal(const T &a)
{
    return -leftUnitNormal(a);
}

template <class T>
T inwardUnitNormal(const T &a, int polygonDirection)
{
    return polygonDirection * leftUnitNormal(a);
}

template<typename R>
R lazyRound(qreal value);

template<>
inline int lazyRound<int>(qreal value)
{
    return qRound(value);
}

template<>
inline qreal lazyRound<qreal>(qreal value)
{
    return value;
}

template <class T>
int polygonDirection(const QVector<T> &polygon) {

    typename PointTypeTraits<T>::value_type doubleSum = 0;

    const int numPoints = polygon.size();
    for (int i = 1; i <= numPoints; i++) {
        int prev = i - 1;
        int next = i == numPoints ? 0 : i;

        doubleSum +=
            (polygon[next].x() - polygon[prev].x()) *
            (polygon[next].y() + polygon[prev].y());
    }

    return doubleSum >= 0 ? 1 : -1;
}

template <typename T>
bool isInRange(T x, T a, T b) {
    T length = qAbs(a - b);
    return qAbs(x - a) <= length && qAbs(x - b) <= length;
}

void KRITAGLOBAL_EXPORT adjustIfOnPolygonBoundary(const QPolygonF &poly, int polygonDirection, QPointF *pt);

QPointF KRITAGLOBAL_EXPORT transformAsBase(const QPointF &pt, const QPointF &base1, const QPointF &base2);

qreal KRITAGLOBAL_EXPORT angleBetweenVectors(const QPointF &v1, const QPointF &v2);

qreal KRITAGLOBAL_EXPORT directionBetweenPoints(const QPointF &p1, const QPointF &p2,
                                                qreal defaultAngle);

namespace Private {
    inline void resetEmptyRectangle(const QPoint &pt, QRect *rc) {
        *rc = QRect(pt, QSize(1, 1));
    }

    inline void resetEmptyRectangle(const QPointF &pt, QRectF *rc) {
        static const qreal eps = 1e-10;
        *rc = QRectF(pt, QSizeF(eps, eps));
    }
}

template <class Point, class Rect>
inline void accumulateBounds(const Point &pt, Rect *bounds)
{
    if (bounds->isEmpty()) {
        Private::resetEmptyRectangle(pt, bounds);
    }

    if (pt.x() < bounds->left()) {
        bounds->setLeft(pt.x());
    } else if (pt.x() > bounds->right()) {
        bounds->setRight(pt.x());
    }

    if (pt.y() < bounds->top()) {
        bounds->setTop(pt.y());
    } else if (pt.y() > bounds->bottom()) {
        bounds->setBottom(pt.y());
    }
}

template <class Point, class Rect>
inline void accumulateBoundsNonEmpty(const Point &pt, Rect *bounds)
{
    if (pt.x() < bounds->left()) {
        bounds->setLeft(pt.x());
    } else if (pt.x() > bounds->right()) {
        bounds->setRight(pt.x());
    }

    if (pt.y() < bounds->top()) {
        bounds->setTop(pt.y());
    } else if (pt.y() > bounds->bottom()) {
        bounds->setBottom(pt.y());
    }
}

template <template <class T> class Container, class Point, class Rect>
inline void accumulateBounds(const Container<Point> &points, Rect *bounds)
{
    Q_FOREACH (const Point &pt, points) {
        accumulateBounds(pt, bounds);
    }
}

template <template <class T> class Container, class Point>
inline typename PointTypeTraits<Point>::rect_type
accumulateBounds(const Container<Point> &points)
{
    typename PointTypeTraits<Point>::rect_type result;

    Q_FOREACH (const Point &pt, points) {
        accumulateBounds(pt, &result);
    }

    return result;
}

template <class Point, class Rect>
inline Point clampPoint(Point pt, const Rect &bounds)
{
    if (pt.x() > bounds.right()) {
        pt.rx() = bounds.right();
    }

    if (pt.x() < bounds.left()) {
        pt.rx() = bounds.left();
    }

    if (pt.y() > bounds.bottom()) {
        pt.ry() = bounds.bottom();
    }

    if (pt.y() < bounds.top()) {
        pt.ry() = bounds.top();
    }

    return pt;
}

template <class Point>
inline typename PointTypeTraits<Point>::rect_type
createRectFromCorners(Point corner1, Point corner2)
{
    return typename PointTypeTraits<Point>::rect_type(qMin(corner1.x(), corner2.x()), qMin(corner1.y(), corner2.y()), qAbs(corner1.x() - corner2.x()), qAbs(corner1.y() - corner2.y()));
}

inline QRectF createRectFromCorners(QLineF line)
{
    QPointF a = line.p1();
    QPointF b = line.p2();
    return createRectFromCorners(a, b);
}

template <class Size>
auto maxDimension(Size size) -> decltype(size.width()) {
    return qMax(size.width(), size.height());
}

template <class Size>
auto minDimension(Size size) -> decltype(size.width()) {
    return qMin(size.width(), size.height());
}

QPainterPath KRITAGLOBAL_EXPORT smallArrow();

template <class Rect>
Rect blowRect(const Rect &rect, qreal coeff)
{
    typedef decltype(rect.x()) CoordType;

    CoordType w = rect.width() * coeff;
    CoordType h = rect.height() * coeff;

    return rect.adjusted(-w, -h, w, h);
}

QPoint KRITAGLOBAL_EXPORT ensureInRect(QPoint pt, const QRect &bounds);
QPointF KRITAGLOBAL_EXPORT ensureInRect(QPointF pt, const QRectF &bounds);

template <class Rect>
Rect ensureRectNotSmaller(Rect rc, const decltype(Rect().size()) &size)
{
    typedef decltype(Rect().size()) Size;
    typedef decltype(Rect().top()) ValueType;

    if (rc.width() < size.width() ||
        rc.height() < size.height()) {

        ValueType width = qMax(rc.width(), size.width());
        ValueType  height = qMax(rc.height(), size.height());

        rc = Rect(rc.topLeft(), Size(width, height));
    }

    return rc;
}

template <class Size>
Size ensureSizeNotSmaller(const Size &size, const Size &bounds)
{
    Size result = size;

    const auto widthBound = qAbs(bounds.width());
    auto width = result.width();
    if (qAbs(width) < widthBound) {
        width = copysign(widthBound, width);
        result.setWidth(width);
    }

    const auto heightBound = qAbs(bounds.height());
    auto height = result.height();
    if (qAbs(height) < heightBound) {
        height = copysign(heightBound, height);
        result.setHeight(height);
    }

    return result;
}

bool KRITAGLOBAL_EXPORT intersectLineRect(QLineF &line, const QRect rect, bool extend);

bool KRITAGLOBAL_EXPORT intersectLineRect(QLineF &line, const QRect rect, bool extendFirst, bool extendSecond);

bool KRITAGLOBAL_EXPORT intersectLineConvexPolygon(QLineF &line, const QPolygonF polygon, bool extendFirst, bool extendSecond);

void KRITAGLOBAL_EXPORT cropLineToRect(QLineF &line, const QRect rect, bool extendFirst, bool extendSecond);

void KRITAGLOBAL_EXPORT cropLineToConvexPolygon(QLineF &line, const QPolygonF polygon, bool extendFirst, bool extendSecond);

QPolygonF KRITAGLOBAL_EXPORT calculateConvexHull(const QPolygonF &polygon);

template <class Point>
inline Point abs(const Point &pt) {
    return Point(qAbs(pt.x()), qAbs(pt.y()));
}

template<typename T, typename std::enable_if<std::is_integral<T>::value, T>::type* = nullptr>
inline T wrapValue(T value, T wrapBounds) {
    value %= wrapBounds;
    if (value < 0) {
        value += wrapBounds;
    }
    return value;
}

template<typename T, typename std::enable_if<std::is_floating_point<T>::value, T>::type* = nullptr>
inline T wrapValue(T value, T wrapBounds) {
    value = std::fmod(value, wrapBounds);
    if (value < 0) {
        value += wrapBounds;
    }
    return value;
}

template<typename T, typename std::enable_if<std::is_same<decltype(T().x()), decltype(T().y())>::value, T>::type* = nullptr>
inline T wrapValue(T value, T wrapBounds) {
    value.rx() = wrapValue(value.x(), wrapBounds.x());
    value.ry() = wrapValue(value.y(), wrapBounds.y());
    return value;
}

template<typename T>
inline T wrapValue(T value, T min, T max) {
    return wrapValue(value - min, max - min) + min;
}

class RightHalfPlane {
public:

    RightHalfPlane(const QPointF &a, const QPointF &b)
        : m_a(a), m_p(b - a), m_norm_p_inv(1.0 / norm(m_p))
    {
    }

    RightHalfPlane(const QLineF &line)
        : RightHalfPlane(line.p1(), line.p2())
    {
    }

    qreal valueSq(const QPointF &pt) const {
        const qreal val = value(pt);
        return signZZ(val) * pow2(val);
    }

    qreal value(const QPointF &pt) const {
        return crossProduct(m_p, pt - m_a) * m_norm_p_inv;
    }

    int pos(const QPointF &pt) const {
        return signZZ(value(pt));
    }

    QLineF getLine() const {
        return QLineF(m_a, m_a + m_p);
    }

private:
    const QPointF m_a;
    const QPointF m_p;
    const qreal m_norm_p_inv;
};

class OuterCircle {
public:

    OuterCircle(const QPointF &c, qreal radius)
        : m_c(c),
          m_radius(radius),
          m_radius_sq(pow2(radius)),
          m_fadeCoeff(1.0 / (pow2(radius + 1.0) - m_radius_sq))
    {
    }

    qreal valueSq(const QPointF &pt) const {
        const qreal val = value(pt);

        return signZZ(val) * pow2(val);
    }

    qreal value(const QPointF &pt) const {
        return kisDistance(pt, m_c) - m_radius;
    }

    int pos(const QPointF &pt) const {
        return signZZ(valueSq(pt));
    }

    qreal fadeSq(const QPointF &pt) const {
        const qreal valSq = kisSquareDistance(pt, m_c);
        return (valSq - m_radius_sq) * m_fadeCoeff;
    }

private:
    const QPointF m_c;
    const qreal m_radius;
    const qreal m_radius_sq;
    const qreal m_fadeCoeff;
};

class KRITAGLOBAL_EXPORT VectorPath
{
public:
    struct VectorPathPoint
    {
        typedef enum Type {
            MoveTo,
            LineTo,
            BezierTo
        } Type;

        QPointF endPoint {QPointF()};
        QPointF controlPoint1 {QPointF()};
        QPointF controlPoint2 {QPointF()};

        Type type {MoveTo};

        VectorPathPoint() {
        }

        VectorPathPoint(Type _type, QPointF _endPoint, QPointF c1 = QPointF(), QPointF c2 = QPointF()) {
            type = _type;
            endPoint = _endPoint;
            controlPoint1 = c1;
            controlPoint2 = c2;
        }

        static VectorPathPoint moveTo(QPointF _endPoint) {
            return VectorPathPoint(MoveTo, _endPoint);
        }

        static VectorPathPoint lineTo(QPointF _endPoint) {
            return VectorPathPoint(LineTo, _endPoint);
        }

        static VectorPathPoint bezierTo(QPointF _endPoint, QPointF _controlPoint1, QPointF _controlPoint2) {
            return VectorPathPoint(BezierTo, _endPoint, _controlPoint1, _controlPoint2);
        }
    };

    struct Segment
    {
        VectorPathPoint start;
        VectorPathPoint end;

        QPointF startPoint {QPointF()};
        QPointF endPoint {QPointF()};
        QPointF controlPoint1 {QPointF()};
        QPointF controlPoint2 {QPointF()};

        VectorPathPoint::Type type {VectorPathPoint::MoveTo};

        Segment(VectorPathPoint _start, VectorPathPoint _end)
            : start(_start)
            , end(_end)
            , startPoint(_start.endPoint)
            , endPoint(_end.endPoint)
            , controlPoint1(_end.controlPoint1)
            , controlPoint2(_end.controlPoint2)
            , type(_end.type)
        {

        }

    };

public:
    VectorPath(const QPainterPath& path);
    VectorPath(const QList<VectorPathPoint> path);

    int pointsCount() const;
    VectorPathPoint pointAt(int i) const;
    int segmentsCount() const;
    QList<VectorPathPoint> segmentAt(int i) const;
    std::optional<Segment> segmentAtAsSegment(int i) const;

    QLineF segmentAtAsLine(int i) const;

    static QList<QPointF> intersectSegmentWithLineBounded(const QLineF &line, const Segment &segment);
    static QList<QPointF> intersectSegmentWithLineBounded(const QLineF &line, const VectorPathPoint &p1, const VectorPathPoint &p2);

    int pathIndexToSegmentIndex(int index);
    int segmentIndexToPathIndex(int index);

    VectorPath trulySimplified(qreal epsDegrees = 0.5) const;
    VectorPath reversed() const;

    bool fuzzyComparePointsCyclic(const VectorPath& path, qreal eps = 0.0f) const;

    QPainterPath asPainterPath() const;

private:
    QPainterPath m_originalPath;
    QList<VectorPathPoint> m_points;

};

QDebug KRITAGLOBAL_EXPORT operator<<(QDebug debug, const VectorPath &path);
QDebug KRITAGLOBAL_EXPORT operator<<(QDebug debug, const VectorPath::VectorPathPoint &point);

QVector<QPoint> KRITAGLOBAL_EXPORT sampleRectWithPoints(const QRect &rect);
QVector<QPointF> KRITAGLOBAL_EXPORT sampleRectWithPoints(const QRectF &rect);

QRect KRITAGLOBAL_EXPORT approximateRectFromPoints(const QVector<QPoint> &points);
QRectF KRITAGLOBAL_EXPORT approximateRectFromPoints(const QVector<QPointF> &points);

QRect KRITAGLOBAL_EXPORT approximateRectWithPointTransform(const QRect &rect, std::function<QPointF(QPointF)> func);

KRITAGLOBAL_EXPORT
QRectF cutOffRect(const QRectF &rc, const KisAlgebra2D::RightHalfPlane &p);

KRITAGLOBAL_EXPORT
int quadraticEquation(qreal a, qreal b, qreal c, qreal *x1, qreal *x2);

KRITAGLOBAL_EXPORT
QVector<QPointF> intersectTwoCircles(const QPointF &c1, qreal r1,
                                     const QPointF &c2, qreal r2);

KRITAGLOBAL_EXPORT
QTransform mapToRect(const QRectF &rect);

KRITAGLOBAL_EXPORT
QTransform mapToRectInverse(const QRectF &rect);

inline QPointF relativeToAbsolute(const QPointF &pt, const QRectF &rc) {
    return rc.topLeft() + QPointF(pt.x() * rc.width(), pt.y() * rc.height());
}

inline QPointF absoluteToRelative(const QPointF &pt, const QRectF &rc) {
    const QPointF rel = pt - rc.topLeft();
    return QPointF(rc.width() > 0 ? rel.x() / rc.width() : 0,
                   rc.height() > 0 ? rel.y() / rc.height() : 0);

}

inline qreal relativeToAbsolute(qreal value, const QRectF &rc) {
    const qreal coeff = std::sqrt(pow2(rc.width()) + pow2(rc.height())) / std::sqrt(2.0);
    return value * coeff;
}

inline qreal absoluteToRelative(const qreal value, const QRectF &rc) {
    const qreal coeff = std::sqrt(pow2(rc.width()) + pow2(rc.height())) / std::sqrt(2.0);
    return coeff != 0 ? value / coeff : 0;
}

inline QRectF relativeToAbsolute(const QRectF &rel, const QRectF &rc) {
    return QRectF(relativeToAbsolute(rel.topLeft(), rc), relativeToAbsolute(rel.bottomRight(), rc));
}

inline QRectF absoluteToRelative(const QRectF &rel, const QRectF &rc) {
    return QRectF(absoluteToRelative(rel.topLeft(), rc), absoluteToRelative(rel.bottomRight(), rc));
}

bool KRITAGLOBAL_EXPORT fuzzyMatrixCompare(const QTransform &t1, const QTransform &t2, qreal delta);

bool KRITAGLOBAL_EXPORT fuzzyPointCompare(const QPointF &p1, const QPointF &p2);

bool KRITAGLOBAL_EXPORT fuzzyPointCompare(const QPointF &p1, const QPointF &p2, qreal delta);

template <template<typename> class Cont, class Point>
bool fuzzyPointCompare(const Cont<Point> &c1, const Cont<Point> &c2, qreal delta)
{
    if (c1.size() != c2.size()) return false;

    const qreal eps = delta;

    return std::mismatch(c1.cbegin(),
                         c1.cend(),
                         c2.cbegin(),
                         [eps] (const QPointF &pt1, const QPointF &pt2) {
                               return fuzzyPointCompare(pt1, pt2, eps);
                         })
            .first == c1.cend();
}

template<class Rect, typename Difference = decltype(Rect::width())>
bool fuzzyCompareRects(const Rect &r1, const Rect &r2, Difference tolerance) {
    typedef decltype(r1.topLeft()) Point;

    const Point d1 = abs(r1.topLeft() - r2.topLeft());
    const Point d2 = abs(r1.bottomRight() - r2.bottomRight());

    const Difference maxError = std::max({d1.x(), d1.y(), d2.x(), d2.y()});
    return maxError < tolerance;
}

template<class Polygon, typename Difference = decltype(Polygon::first())>
bool isPolygonRect(const Polygon &poly, Difference tolerance) {
    typedef decltype(poly.first()) Point;

    auto sameVert = [tolerance] (Point p1, Point p2) {
        return std::abs(p2.x() - p1.x()) < tolerance;
    };

    auto sameHoriz = [tolerance] (Point p1, Point p2) {
        return std::abs(p2.y() - p1.y()) < tolerance;
    };

    const int rectCorners = 4;
    if (poly.length() != rectCorners) {
        if (poly.length() != rectCorners + 1 || !fuzzyPointCompare(poly[0], poly[rectCorners], tolerance)) {
            return false;
        }
    }

    bool correctRect = sameVert(poly[0], poly[1]) && sameHoriz(poly[1], poly[2]) && sameVert(poly[2], poly[3]) && sameHoriz(poly[3], poly[0]);
    if (correctRect) {
        return true;
    }
    return sameHoriz(poly[0], poly[1]) && sameVert(poly[1], poly[2]) && sameHoriz(poly[2], poly[3]) && sameVert(poly[3], poly[0]);
}

template<class Polygon, typename Difference = decltype(Polygon::first())>
bool isPolygonPixelAlignedRect(const Polygon &poly, Difference tolerance) {
    if (!isPolygonRect(poly, tolerance)) {
        return false;
    }

    for (int i = 0; i < poly.length(); i++) {
        if (!fuzzyPointCompare(poly[i], QPointF(poly[i].toPoint()), tolerance)) {
            return false;
        }
    }
    return true;

}

template <class T>
bool isPolygonTrulyConvex(const QVector<T> &polygon, bool ensureNoLoops = true) {
    int numPoints = polygon.size();
    if (numPoints < 3)
        return true;

    if (fuzzyPointCompare(polygon[0], polygon[numPoints - 1])) {
        numPoints--;
    }

    int sign = 0;
    qreal angleSum = 0;

    for (int i = 0; i < numPoints; i++) {
        int a = i;
        int b = wrapValue(i + 1, numPoints);
        int c = wrapValue(i + 2, numPoints);

        qreal angle = angleBetweenVectors(static_cast<QPointF>(polygon[b] - polygon[a]), static_cast<QPointF>(polygon[c] - polygon[b]));

        if (angle < -M_PI) {
            angle += 2*M_PI;
        }

        if (angle > M_PI) {
            angle -= 2*M_PI;
        }

        angleSum += angle;

        if (sign == 0) {
            sign = signZZ(angle);
        } else {
            int currentSign = signZZ(angle);
            if (currentSign != 0 && currentSign != sign) {
                return false;
            }
        }
    }

    if (ensureNoLoops && !qFuzzyCompare(qAbs(angleSum), 2*M_PI)) {
        return false;
    }

    return true;
}

struct KRITAGLOBAL_EXPORT DecomposedMatrix {
    DecomposedMatrix();

    DecomposedMatrix(const QTransform &t0);

    inline QTransform scaleTransform() const
    {
        return QTransform::fromScale(scaleX, scaleY);
    }

    inline QTransform shearTransform() const
    {
        QTransform t;
        t.shear(shearXY, 0);
        return t;
    }

    inline QTransform rotateTransform() const
    {
        QTransform t;
        t.rotate(angle);
        return t;
    }

    inline QTransform translateTransform() const
    {
        return QTransform::fromTranslate(dx, dy);
    }

    inline QTransform projectTransform() const
    {
        return
            QTransform(
                1,0,proj[0],
                0,1,proj[1],
                0,0,proj[2]);
    }

    inline QTransform transform() const {
        return
            scaleTransform() *
            shearTransform() *
            rotateTransform() *
            translateTransform() *
            projectTransform();
    }

    inline bool isValid() const {
        return valid;
    }

    qreal scaleX = 1.0;
    qreal scaleY = 1.0;
    qreal shearXY = 0.0;
    qreal angle = 0.0;
    qreal dx = 0.0;
    qreal dy = 0.0;
    qreal proj[3] = {0.0, 0.0, 1.0};

private:
    bool valid = true;
};

std::pair<QPointF, QTransform> KRITAGLOBAL_EXPORT transformEllipse(const QPointF &axes, const QTransform &fullLocalToGlobal);

QPointF KRITAGLOBAL_EXPORT alignForZoom(const QPointF &pt, qreal zoom);

template <typename T>
inline T linearReshapeFunc(T x, T x0, T x1, T y0, T y1)
{
    return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
}

KRITAGLOBAL_EXPORT
boost::optional<QPointF> intersectLines(const QLineF &boundedLine, const QLineF &unboundedLine);

KRITAGLOBAL_EXPORT
boost::optional<QPointF> intersectLines(const QPointF &p1, const QPointF &p2,
                                        const QPointF &q1, const QPointF &q2);

QVector<QPointF> KRITAGLOBAL_EXPORT findTrianglePoint(const QPointF &p1, const QPointF &p2, qreal a, qreal b);

boost::optional<QPointF> KRITAGLOBAL_EXPORT findTrianglePointNearest(const QPointF &p1, const QPointF &p2, qreal a, qreal b, const QPointF &nearest);

bool isOnLine(const QLineF &line, const QPointF &point, const qreal eps, bool boundedStart, bool boundedEnd, bool includeEnds);

QPointF KRITAGLOBAL_EXPORT moveElasticPoint(const QPointF &pt,
                                            const QPointF &base, const QPointF &newBase,
                                            const QPointF &wingA, const QPointF &wingB);

QPointF KRITAGLOBAL_EXPORT moveElasticPoint(const QPointF &pt,
                                            const QPointF &base, const QPointF &newBase,
                                            const QVector<QPointF> &anchorPoints);

QPointF KRITAGLOBAL_EXPORT findNearestPointOnLine(const QPointF &point, const QLineF &line, bool unbounded = true);

QPointF KRITAGLOBAL_EXPORT movePointInTheDirection(const QPointF& point, const QPointF& direction, qreal distance);

QPointF KRITAGLOBAL_EXPORT movePointAlongTheLine(const QPointF& point, const QLineF& direction, qreal distance, bool ensureOnLine);

class HaltonSequenceGenerator
{
public:
    HaltonSequenceGenerator(int base)
        : m_base(base)
    {
    }

    int generate(int maxRange) {
        generationStep();
        return (m_n * maxRange + m_d / 2) / m_d;
    }

    qreal generate() {
        generationStep();
        return qreal(m_n) / m_d;
    }

    void step() {
        generationStep();
    }

    int currentValue(int maxRange) const {
        return (m_n * maxRange + m_d / 2) / m_d;
    }

    qreal currentValue() const{
        return qreal(m_n) / m_d;
    }

private:
    inline void generationStep() {
        int x = m_d - m_n;

        if (x == 1) {
            m_n = 1;
            m_d *= m_base;
        } else {
            int y = m_d / m_base;
            while (x <= y) {
                y /= m_base;
            }
            m_n = (m_base + 1) * y - x;
        }
    }

private:
    int m_n = 0;
    int m_d = 1;
    const int m_base = 0;
};

qreal KRITAGLOBAL_EXPORT findMinimumGoldenSection(std::function<qreal(qreal)> f, qreal xA, qreal xB, qreal eps, int maxIter);

qreal KRITAGLOBAL_EXPORT findMinimumTernarySection(std::function<qreal(qreal)> f, qreal xA, qreal xB, qreal eps, int maxIter);

qreal KRITAGLOBAL_EXPORT pointToLineDistSquared(const QPointF& pt, const QLineF& line);

QList<QLineF> KRITAGLOBAL_EXPORT getParallelLines(const QLineF& line, const qreal distance);

QPainterPath KRITAGLOBAL_EXPORT getOnePathFromRectangleCutThrough(const QList<QPointF> &points, const QLineF &line, bool left);

QList<QPainterPath> KRITAGLOBAL_EXPORT getPathsFromRectangleCutThrough(const QRectF &rect, const QLineF &leftLine, const QLineF &rightLine);

QList<int> KRITAGLOBAL_EXPORT getLineSegmentCrossingLineIndexes(const QLineF &line, const QPainterPath& shape);

QPainterPath KRITAGLOBAL_EXPORT removeGutterSmart(const QPainterPath& shape1, int index1, const QPainterPath& shape2, int index2, bool isSameShape);

VectorPath mergeShapesWithGutter(const VectorPath& shape1, const VectorPath& shape2, const VectorPath& oneEnd, const VectorPath& otherEnd, int index1, int index2, bool reverseSecondPoly, bool isSameShape);
QPainterPath KRITAGLOBAL_EXPORT trySimplifyPath(const QPainterPath &path, qreal lengthThreshold);

bool KRITAGLOBAL_EXPORT isInsideShape(const VectorPath &path, const QPointF &point);
bool KRITAGLOBAL_EXPORT isInsideShape(const QPainterPath &path, const QPointF &point);

}

#endif
