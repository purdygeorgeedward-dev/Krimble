/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008-2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCREATEPATHTOOL_P_H
#define KOCREATEPATHTOOL_P_H

#include <QPainterPath>

#include "KoCreatePathTool.h"
#include "KoPathPoint.h"
#include "KoPathPointData.h"
#include "KoPathPointMergeCommand.h"
#include "KoParameterShape.h"
#include "KoShapeManager.h"
#include "KoSnapStrategy.h"
#include "KoToolBase_p.h"
#include <KoViewConverter.h>
#include "kis_config.h"

#include "math.h"

class KoStrokeConfigWidget;
class KoConverter;

struct PathConnectionPoint {
    PathConnectionPoint()
        : path(0), point(0) {
    }

    void reset() {
        path = 0;
        point = 0;
    }

    PathConnectionPoint& operator =(KoPathPoint * pathPoint) {
        if (!pathPoint || ! pathPoint->parent()) {
            reset();
        } else {
            path = pathPoint->parent();
            point = pathPoint;
        }
        return *this;
    }

    bool operator != (const PathConnectionPoint &rhs) const {
        return rhs.path != path || rhs.point != point;
    }

    bool operator == (const PathConnectionPoint &rhs) const {
        return rhs.path == path && rhs.point == point;
    }

    bool isValid() const {
        return path && point;
    }

    void validate(KoCanvasBase *canvas) {
        if (!isValid()) {
            return;
        }
        if (!canvas) {
            reset();
            return;
        }
        if (!canvas->shapeManager()->shapes().contains(path)) {
            reset();
            return;
        }
        if (path->pathPointIndex(point) == KoPathPointIndex(-1, -1)) {
            reset();
            return;
        }
    }

    KoPathShape * path;
    KoPathPoint * point;
};

inline qreal squareDistance(const QPointF &p1, const QPointF &p2)
{
    qreal dx = p1.x() - p2.x();
    qreal dy = p1.y() - p2.y();
    return dx * dx + dy * dy;
}

class AngleSnapStrategy : public KoSnapStrategy
{
public:
    explicit AngleSnapStrategy(qreal angleStep, bool active)
        : KoSnapStrategy(KoSnapGuide::CustomSnapping), m_angleStep(angleStep), m_active(active) {
    }

    void setStartPoint(const QPointF &startPoint) {
        m_startPoint = startPoint;
    }

    void setAngleStep(qreal angleStep) {
        m_angleStep = qAbs(angleStep);
    }

    bool snap(const QPointF &mousePosition, KoSnapProxy * proxy, qreal maxSnapDistance) override {
        Q_UNUSED(proxy);

        if (!m_active)
            return false;

        QLineF line(m_startPoint, mousePosition);
        qreal currentAngle = line.angle();
        int prevStep = qAbs(currentAngle / m_angleStep);
        int nextStep = prevStep + 1;
        qreal prevAngle = prevStep * m_angleStep;
        qreal nextAngle = nextStep * m_angleStep;

        if (qAbs(currentAngle - prevAngle) <= qAbs(currentAngle - nextAngle)) {
            line.setAngle(prevAngle);
        } else {
            line.setAngle(nextAngle);
        }

        qreal maxSquareSnapDistance = maxSnapDistance * maxSnapDistance;
        qreal snapDistance = squareDistance(mousePosition, line.p2());
        if (snapDistance > maxSquareSnapDistance)
            return false;

        setSnappedPosition(line.p2(), ToLine);
        return true;
    }

    QPainterPath decoration(const KoViewConverter &converter) const override {
        Q_UNUSED(converter);

        QPainterPath decoration;
        decoration.moveTo(m_startPoint);
        decoration.lineTo(snappedPosition());
        return decoration;
    }

    void deactivate() {
        m_active = false;
    }

    void activate() {
        m_active = true;
    }

private:
    QPointF m_startPoint;
    qreal m_angleStep;
    bool m_active;
};

class KoCreatePathToolPrivate : public KoToolBasePrivate
{
    KoCreatePathTool * const q;
public:
    KoCreatePathToolPrivate(KoCreatePathTool * const qq, KoCanvasBase* canvas)
        : KoToolBasePrivate(qq, canvas),
          q(qq),
          shape(0),
          activePoint(0),
          firstPoint(0),
          handleRadius(3),
          decorationThickness(1),
          mouseOverFirstPoint(false),
          pointIsDragged(false),
          finishAfterThisPoint(false),
          hoveredPoint(0),
          angleSnapStrategy(0),
          angleSnappingDelta(15),
          angleSnapStatus(false),
          enableClosePathShortcut(true)
    {
    }

    KoPathShape *shape;
    KoPathPoint *activePoint;
    KoPathPoint *firstPoint;
    int handleRadius;
    int decorationThickness;
    bool mouseOverFirstPoint;
    bool pointIsDragged;
    bool finishAfterThisPoint;
    PathConnectionPoint existingStartPoint;
    PathConnectionPoint existingEndPoint;
    KoPathPoint *hoveredPoint;
    bool prevPointWasDragged = false;
    bool autoSmoothCurves = false;

    QPointF dragStartPoint;

    AngleSnapStrategy *angleSnapStrategy;
    int angleSnappingDelta;
    bool angleSnapStatus;
    bool enableClosePathShortcut;

    void repaintActivePoint() const {
        const bool isFirstPoint = (activePoint == firstPoint);

        if (!isFirstPoint && !pointIsDragged)
            return;

        QRectF rect = activePoint->boundingRect(false);

        const QPointF &point = activePoint->point();
        const QPointF &controlPoint = activePoint->controlPoint2();
        rect = rect.united(QRectF(point, controlPoint).normalized());

        if (isFirstPoint) {
            const QPointF &controlPoint = activePoint->controlPoint1();
            rect = rect.united(QRectF(point, controlPoint).normalized());
        }

        QPointF border = q->canvas()->viewConverter()
                         ->viewToDocument(QPointF(handleRadius, handleRadius));

        rect.adjust(-border.x(), -border.y(), border.x(), border.y());
        q->canvas()->updateCanvas(rect);
    }

    KoPathPoint* endPointAtPosition(const QPointF &position) const {
        QRectF roi = q->handleGrabRect(position);
        QList<KoShape *> shapes = q->canvas()->shapeManager()->shapesAt(roi);

        KoPathPoint * nearestPoint = 0;
        qreal minDistance = HUGE_VAL;
        uint grabSensitivity = q->grabSensitivity();
        qreal maxDistance = q->canvas()->viewConverter()->viewToDocumentX(grabSensitivity);

        Q_FOREACH(KoShape * s, shapes) {
            KoPathShape * path = dynamic_cast<KoPathShape*>(s);
            if (!path)
                continue;
            KoParameterShape *paramShape = dynamic_cast<KoParameterShape*>(s);
            if (paramShape && paramShape->isParametricShape())
                continue;

            KoPathPoint * p = 0;
            uint subpathCount = path->subpathCount();
            for (uint i = 0; i < subpathCount; ++i) {
                if (path->isClosedSubpath(i))
                    continue;
                p = path->pointByIndex(KoPathPointIndex(i, 0));
                qreal d = squareDistance(position, path->shapeToDocument(p->point()));
                if (d < minDistance && d < maxDistance) {
                    nearestPoint = p;
                    minDistance = d;
                }
                p = path->pointByIndex(KoPathPointIndex(i, path->subpathPointCount(i) - 1));
                d = squareDistance(position, path->shapeToDocument(p->point()));
                if (d < minDistance && d < maxDistance) {
                    nearestPoint = p;
                    minDistance = d;
                }
            }
        }

        return nearestPoint;
    }

    bool connectPaths(KoPathShape *pathShape, const PathConnectionPoint &pointAtStart, const PathConnectionPoint &pointAtEnd) const {
        KoPathShape * startShape = 0;
        KoPathShape * endShape = 0;
        KoPathPoint * startPoint = 0;
        KoPathPoint * endPoint = 0;

        if (pointAtStart.isValid()) {
            startShape = pointAtStart.path;
            startPoint = pointAtStart.point;
        }
        if (pointAtEnd.isValid()) {
            endShape = pointAtEnd.path;
            endPoint = pointAtEnd.point;
        }

        if (!startPoint && !endPoint)
            return false;
        if (startPoint == endPoint)
            endPoint = 0;

        uint newPointCount = pathShape->subpathPointCount(0);
        KoPathPointIndex newStartPointIndex(0, 0);
        KoPathPointIndex newEndPointIndex(0, newPointCount - 1);
        KoPathPoint * newStartPoint = pathShape->pointByIndex(newStartPointIndex);
        KoPathPoint * newEndPoint = pathShape->pointByIndex(newEndPointIndex);

        KoPathPointIndex startIndex(-1, -1);
        if (startShape && startPoint) {
            startIndex = startShape->pathPointIndex(startPoint);
            pathShape->combine(startShape);
            pathShape->moveSubpath(0, pathShape->subpathCount() - 1);
        }
        KoPathPointIndex endIndex(-1, -1);
        if (endShape && endPoint) {
            endIndex = endShape->pathPointIndex(endPoint);
            if (endShape != startShape) {
                endIndex.first += pathShape->subpathCount();
                pathShape->combine(endShape);
            }
        }
        bool connectToSingleSubpath = (startShape == endShape && startIndex.first == endIndex.first);

        if (startIndex.second == 0 && !connectToSingleSubpath) {
            pathShape->reverseSubpath(startIndex.first);
            startIndex.second = pathShape->subpathPointCount(startIndex.first) - 1;
        }
        if (endIndex.second > 0 && !connectToSingleSubpath) {
            pathShape->reverseSubpath(endIndex.first);
            endIndex.second = 0;
        }

        KoPathPoint * existingStartPoint = pathShape->pointByIndex(startIndex);
        KoPathPoint * existingEndPoint = pathShape->pointByIndex(endIndex);

        if (existingStartPoint) {
            KoPathPointData pd1(pathShape, pathShape->pathPointIndex(existingStartPoint));
            KoPathPointData pd2(pathShape, pathShape->pathPointIndex(newStartPoint));
            KoPathPointMergeCommand cmd1(pd1, pd2);
            cmd1.redo();
        }
        if (existingEndPoint) {
            KoPathPointData pd3(pathShape, pathShape->pathPointIndex(newEndPoint));
            KoPathPointData pd4(pathShape, pathShape->pathPointIndex(existingEndPoint));
            KoPathPointMergeCommand cmd2(pd3, pd4);
            cmd2.redo();
        }

        return true;
    }

    void addPathShape() {
        if (!shape) return;

        if (shape->pointCount() < 2) {
            cleanUp();
            return;
        }

        KoPathShape *pathShape = shape;
        shape = 0;

        q->addPathShape(pathShape);

        cleanUp();

        return;
    }

    void cleanUp() {
        q->canvas()->updateCanvas(q->canvas()->snapGuide()->boundingRect());
        q->canvas()->snapGuide()->reset();
        angleSnapStrategy = 0;

        delete shape;
        shape = 0;
        existingStartPoint = 0;
        existingEndPoint = 0;
        hoveredPoint = 0;
        activePoint = 0;
    }

    void angleDeltaChanged(qreal value) {
        angleSnappingDelta = static_cast<int>(value);
        if (angleSnapStrategy)
            angleSnapStrategy->setAngleStep(angleSnappingDelta);
    }

    void autoSmoothCurvesChanged(bool value) {
        autoSmoothCurves = value;

        KisConfig cfg(false);
        cfg.setAutoSmoothBezierCurves(value);
    }

    void loadAutoSmoothValueFromConfig() {
        KisConfig cfg(true);
        autoSmoothCurves = cfg.autoSmoothBezierCurves();

        Q_EMIT q->sigUpdateAutoSmoothCurvesGUI(autoSmoothCurves);
    }

    void angleSnapChanged(int angleSnap) {
        angleSnapStatus = ! angleSnapStatus;
        if (angleSnapStrategy) {
            if (angleSnap == Qt::Checked)
                angleSnapStrategy->activate();
            else
                angleSnapStrategy->deactivate();
        }
    }
};

#endif
