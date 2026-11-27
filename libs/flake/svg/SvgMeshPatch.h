/*
 *  SPDX-FileCopyrightText: 2020 Sharaf Zaman <sharafzaz121@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef SVGMESHPATCH_H
#define SVGMESHPATCH_H

#include <array>

#include <QColor>
#include <QPointF>
#include <QVector>
#include <QMap>
#include <QPainterPath>

#include <KoPathShape.h>

struct SvgMeshStop {
    QColor color;
    QPointF point;

    SvgMeshStop()
    {}

    SvgMeshStop(QColor color, QPointF point)
        : color(color), point(point)
    {}

    bool isValid() const { return color.isValid(); }
};

using SvgMeshPath = std::array<QPointF, 4>;

class KRITAFLAKE_EXPORT SvgMeshPatch
{
public:
    enum Type {
        Top = 0,
        Right,
        Bottom,
        Left,
        Size,
    };

    SvgMeshPatch(QPointF startingPoint);
    SvgMeshPatch(const SvgMeshPatch& other);

    void moveTo(const QPointF& p);
    void lineTo(const QPointF& p);
    void curveTo(const QPointF& c1, const QPointF& c2, const QPointF& p);

    SvgMeshStop getStop(Type type) const;

    inline QPointF getMidpointParametric(Type type) const {
        return (m_parametricCoords[type] + m_parametricCoords[(type + 1) % Size]) * 0.5;
    }

    QPointF segmentPointAt(Type type, qreal t) const;

    QPair<std::array<QPointF, 4>, std::array<QPointF, 4>> segmentSplitAt(Type type, qreal t) const;

    std::array<QPointF, 4> getSegment(Type type) const;

    QPainterPath getPath() const;

    QSizeF size() const;

    QRectF boundingRect() const;

    std::array<QPointF, 4> getMidCurve(bool isVertical) const;

    void subdivideHorizontally(QVector<SvgMeshPatch*>& subdivided,
                               const QVector<QColor>& colors) const;

    void subdivideVertically(QVector<SvgMeshPatch*>& subdivided,
                             const QVector<QColor>& colors) const;

    void subdivide(QVector<SvgMeshPatch*>& subdivided,
                   const QVector<QColor>& colors) const;

    bool isDivisibleVertically() const;
    bool isDivisibleHorizontally() const;

    int countPoints() const;

    void addStop(const QString& pathStr, QColor color, Type edge, bool pathIncomplete = false, QPointF lastPoint = QPointF());

    void addStop(const std::array<QPointF, 4>& pathPoints, QColor color, Type edge);

    void addStopLinear(const std::array<QPointF, 2>& pathPoints, QColor color, Type edge);

    void modifyPath(SvgMeshPatch::Type type, std::array<QPointF, 4> newPath);
    void modifyCorner(SvgMeshPatch::Type type, const QPointF &delta);

    void setStopColor(SvgMeshPatch::Type type, const QColor &color);

    void setTransform(const QTransform& matrix);

private:
    QPointF parseMeshPath(const QString& path, bool pathIncomplete = false, const QPointF lastPoint = QPointF());
    const char* getCoord(const char* ptr, qreal& number);

private:
    bool m_newPath;
    int counter {0};

    QPointF m_startingPoint;

    std::array<SvgMeshStop, Size> m_nodes;
    std::array<std::array<QPointF, 4>, 4> controlPoints;
    std::array<QPointF, 4> m_parametricCoords;
};

#endif
