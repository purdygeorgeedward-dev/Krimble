/*
 *  SPDX-FileCopyrightText: 2020 Sharaf Zaman <sharafzaz121@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef SVGMESHARRAY_H
#define SVGMESHARRAY_H

#include <QVector>

#include "SvgMeshPatch.h"

struct SvgMeshPosition {
    int row;
    int col;
    SvgMeshPatch::Type segmentType;

    SvgMeshPosition()
        : row(-1)
        , col(-1)
        , segmentType(SvgMeshPatch::Size)
    {
    }

    SvgMeshPosition(int row, int col, SvgMeshPatch::Type type)
        : row(row)
        , col(col)
        , segmentType(type)
    {
    }

    bool isValid() const {
        return row >= 0 && col >= 0;
    }
};

class KRITAFLAKE_EXPORT SvgMeshArray
{
public:
    SvgMeshArray();

    SvgMeshArray(const SvgMeshArray& other);

    ~SvgMeshArray();

    void createDefaultMesh(const int nrows, const int ncols, const QColor color, const QSizeF size);

    void newRow();

    bool addPatch(QList<QPair<QString, QColor>> stops, const QPointF initialPoint);

    SvgMeshStop getStop(const SvgMeshPatch::Type edge, const int row, const int col) const;

    SvgMeshStop getStop(const SvgMeshPosition &pos) const;

    std::array<QPointF, 4> getPath(const SvgMeshPatch::Type edge, const int row, const int col) const;

    SvgMeshPath getPath(const SvgMeshPosition &pos) const;

    SvgMeshPatch* getPatch(const int row, const int col) const;

    int numRows() const;
    int numColumns() const;

    void setTransform(const QTransform& matrix);

    QRectF boundingRect() const;

    QVector<SvgMeshPosition> getConnectedPaths(const SvgMeshPosition &position) const;

    void modifyHandle(const SvgMeshPosition &position, const std::array<QPointF, 4> &newPath);
    void modifyCorner(const SvgMeshPosition &position, const QPointF &newPos);

    void modifyColor(const SvgMeshPosition &position, const QColor &color);

private:
    QVector<SvgMeshPosition> getSharedPaths(const SvgMeshPosition &position) const;

    QColor getColor(SvgMeshPatch::Type edge, int row, int col) const;

private:
    QVector<QVector<SvgMeshPatch*>> m_array;
};

#endif
