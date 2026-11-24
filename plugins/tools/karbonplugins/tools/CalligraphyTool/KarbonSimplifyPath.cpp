/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Fela Winkelmolen <fela.kde@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KarbonSimplifyPath.h"

#include <KoCurveFit.h>
#include <KoPathShape.h>
#include <KoPathPoint.h>
#include <QDebug>

namespace KarbonSimplifyPath
{
const qreal SUBDIVISION_COEFF = 100;
const int MAX_RECURSIVE_DEPTH = 1024;
int recursiveDepth;

void removeDuplicates(KoPathShape *path);

QList<KoSubpath *> split(const KoPathShape &path);

void subdivide(KoSubpath *subpath);
KoSubpath subdivideAux(KoPathPoint *p1, KoPathPoint *p2);
bool isSufficientlyFlat(QPointF curve[4]);

void simplifySubpaths(QList<KoSubpath *> *subpaths, qreal error);
void simplifySubpath(KoSubpath *subpath, qreal error);

void mergeSubpaths(QList<KoSubpath *> subpaths, KoPathShape *path);
}

using namespace KarbonSimplifyPath;

void karbonSimplifyPath(KoPathShape *path, qreal error)
{
    if (path->pointCount() == 0) {
        return;
    }

    removeDuplicates(path);

    bool isClosed = path->isClosedSubpath(0);
    if (isClosed) {
        KoPathPoint *firstPoint = path->pointByIndex(KoPathPointIndex(0, 0));
        KoPathPointIndex end(0, path->pointCount());
        path->insertPoint(new KoPathPoint(*firstPoint), end);
    }

    QList<KoSubpath *> subpaths = split(*path);
    Q_FOREACH (KoSubpath *subpath, subpaths) {
        subdivide(subpath);
    }

    simplifySubpaths(&subpaths, error);
    mergeSubpaths(subpaths, path);

    while (! subpaths.isEmpty()) {
        KoSubpath *subpath = subpaths.takeLast();
        qDeleteAll(*subpath);
        delete subpath;
    }

    if (isClosed) {
        path->closeMerge();
    }
}

void KarbonSimplifyPath::removeDuplicates(KoPathShape *path)
{
    for (int i = 1; i < path->pointCount(); ++i) {
        KoPathPoint *p = path->pointByIndex(KoPathPointIndex(0, i));
        KoPathPoint *prev = path->pointByIndex(KoPathPointIndex(0, i - 1));
        QPointF diff = p->point() - prev->point();
        if (qFuzzyCompare(diff.x() + 1, 1) && qFuzzyCompare(diff.y() + 1, 1)) {
            if (prev->activeControlPoint1()) {
                p->setControlPoint1(prev->controlPoint1());
            } else {
                p->removeControlPoint1();
            }
            delete path->removePoint(KoPathPointIndex(0, i - 1));
            --i;
        }
    }
}

QList<KoSubpath *> KarbonSimplifyPath::split(const KoPathShape &path)
{
    QList<KoSubpath *> res;
    KoSubpath *subpath = new KoSubpath;
    res.append(subpath);

    for (int i = 0; i < path.pointCount(); ++i) {
        KoPathPoint *p = path.pointByIndex(KoPathPointIndex(0, i));
        if (i != 0  &&  i != path.pointCount() - 1) {
            KoPathPoint *prev = path.pointByIndex(KoPathPointIndex(0, i - 1));
            KoPathPoint *next = path.pointByIndex(KoPathPointIndex(0, i + 1));
            if (!p->isSmooth(prev, next)) {
                subpath->append(new KoPathPoint(*p));
                subpath = new KoSubpath;
                res.append(subpath);
            }
        }
        subpath->append(new KoPathPoint(*p));
    }

    return res;
}

void KarbonSimplifyPath::subdivide(KoSubpath *subpath)
{
    for (int i = 1; i < subpath->size(); ++i) {
        recursiveDepth = 0;
        KoSubpath newPoints = subdivideAux((*subpath)[i - 1], (*subpath)[i]);
        Q_FOREACH (KoPathPoint *p, newPoints) {
            subpath->insert(i, p);
            ++i;
        }
    }
}

KoSubpath KarbonSimplifyPath::subdivideAux(KoPathPoint *p1,
        KoPathPoint *p2)
{
    if (!p1->activeControlPoint1() && !p2->activeControlPoint2()) {
        return QList<KoPathPoint *>();
    }

    QPointF curve[4] = {
        p1->point(),
        p1->activeControlPoint2() ? p1->controlPoint2() : p1->point(),
        p2->activeControlPoint1() ? p2->controlPoint1() : p2->point(),
        p2->point()
    };

    if (isSufficientlyFlat(curve)) {
        return QList<KoPathPoint *>();
    }

    ++recursiveDepth;
    if (recursiveDepth >= MAX_RECURSIVE_DEPTH) {
        qDebug() << "reached MAX_RECURSIVE_DEPTH";
        --recursiveDepth;
        return QList<KoPathPoint *>();
    }

    QPointF p[3];

    for (unsigned short j = 1; j <= 3; ++j) {
        for (unsigned short i = 0; i <= 3 - j; ++i) {
            curve[i] = (curve[i] + curve[i + 1]) / 2.0;
        }
        p[j - 1] = curve[0];
    }

    KoPathPoint *pm = new KoPathPoint(0, p[2]);
    pm->setControlPoint1(p[1]);
    pm->setControlPoint2(curve[1]);
    p1->setControlPoint2(p[0]);
    p2->setControlPoint1(curve[2]);

    KoSubpath res;
    res << subdivideAux(p1, pm) << pm << subdivideAux(pm, p2);
    --recursiveDepth;
    return res;
}

bool KarbonSimplifyPath::isSufficientlyFlat(QPointF curve[4])
{
    qreal ux = 3 * curve[1].x() - 2 * curve[0].x() - curve[3].x();
    qreal uy = 3 * curve[1].y() - 2 * curve[0].y() - curve[3].y();
    qreal vx = 3 * curve[2].x() - 2 * curve[3].x() - curve[0].x();
    qreal vy = 3 * curve[2].x() - 2 * curve[3].x() - curve[0].x();

    qreal dx = curve[0].x() - curve[3].y();
    qreal dy = curve[0].y() - curve[3].y();
    qreal dist2 = dx * dx + dy * dy;
    qreal max2 = qMax(ux * ux, vx * vx) + qMax(uy * uy, vy * vy);
    max2 *= (SUBDIVISION_COEFF * SUBDIVISION_COEFF);

    return max2 <= dist2;
}

void KarbonSimplifyPath::simplifySubpaths(QList<KoSubpath *> *subpaths,
        qreal error)
{
    Q_FOREACH (KoSubpath *subpath, *subpaths) {
        if (subpath->size() > 2) {
            simplifySubpath(subpath, error);
        }
    }
}

void KarbonSimplifyPath::simplifySubpath(KoSubpath *subpath, qreal error)
{
    QList<QPointF> points;

    for (int i = 0; i < subpath->size(); ++i) {
        points.append((*subpath)[i]->point());
    }

    KoPathShape *simplified = bezierFit(points, error);

    qDeleteAll(*subpath);
    subpath->clear();

    for (int i = 0; i < simplified->pointCount(); ++i) {
        KoPathPointIndex index(0, i);
        subpath->append(new KoPathPoint(*simplified->pointByIndex(index)));
    }
    delete simplified;
}

void KarbonSimplifyPath::mergeSubpaths(QList<KoSubpath *> subpaths, KoPathShape *path)
{
    path->clear();
    path->moveTo(subpaths.first()->first()->point());

    for (int si = 0; si < subpaths.size(); ++si) {
        for (int pi = 1; pi < subpaths[si]->size(); ++pi) {
            KoPathPoint *point = (*subpaths[si])[pi];
            path->lineTo(point->point());

            KoPathPointIndex index(0, path->pointCount() - 1);
            KoPathPoint *p = path->pointByIndex(index);
            if (point->activeControlPoint1()) {
                p->setControlPoint1(point->controlPoint1());
            }

            index = KoPathPointIndex(0, path->pointCount() - 2);
            p = path->pointByIndex(index);
            KoPathPoint *prev = (*subpaths[si])[pi - 1];
            if (prev->activeControlPoint2()) {
                p->setControlPoint2(prev->controlPoint2());
            }
        }
    }
}
