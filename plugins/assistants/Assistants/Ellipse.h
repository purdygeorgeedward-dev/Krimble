/*
 * SPDX-FileCopyrightText: 2010 Geoffry Song <goffrie@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _ELLIPSE_H_
#define _ELLIPSE_H_

#include <QPointF>
#include <QTransform>

class Ellipse
{
public:
    Ellipse();
    Ellipse(const QPointF& p1, const QPointF& p2, const QPointF& p3);
    ~Ellipse();

    QPointF project(const QPointF&) const;
    QRectF boundingRect() const;

    bool set(const QPointF& m1, const QPointF& m2, const QPointF& p);

    const QPointF& major1() const { return p1; }
    bool setMajor1(const QPointF& p);
    const QPointF& major2() const { return p2; }
    bool setMajor2(const QPointF& p);
    const QPointF& point() const { return p3; }
    bool setPoint(const QPointF& p);
    const QTransform& getTransform() const { return matrix; }
    const QTransform& getInverse() const { return inverse; }
    qreal semiMajor() const { return a; }
    qreal semiMinor() const { return b; }

private:
    bool changeMajor();
    bool changeMinor();

    QTransform matrix;
    QTransform inverse;
    qreal a;
    qreal b;

    QPointF p1;
    QPointF p2;
    QPointF p3;
};

#endif
