/*
 *  SPDX-FileCopyrightText: 2022 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISOPTIMIZEDBRUSHOUTLINE_H
#define KISOPTIMIZEDBRUSHOUTLINE_H

#include <QPolygonF>
#include <QTransform>
#include <boost/iterator/iterator_facade.hpp>
#include <optional>
#include <kritaimage_export.h>

class QPainterPath;

class KRITAIMAGE_EXPORT KisOptimizedBrushOutline
{
public:
    class KRITAIMAGE_EXPORT const_iterator :
        public boost::iterator_facade <const_iterator,
                                       QPolygonF,
                                       boost::forward_traversal_tag,
                                       QPolygonF>
    {
    public:
        const_iterator()
            : m_outline(0),
              m_index(0) {}

        const_iterator(const KisOptimizedBrushOutline *outline, int index)
            : m_outline(outline),
              m_index(index) {}

    private:
        friend class boost::iterator_core_access;

        void increment() {
            m_index++;
        }

        bool equal(const_iterator const& other) const {
            return m_index == other.m_index &&
                m_outline == other.m_outline;
        }

        QPolygonF dereference() const;

    private:
        const KisOptimizedBrushOutline *m_outline;
        int m_index;
    };

public:
    KisOptimizedBrushOutline();
    KisOptimizedBrushOutline(const QPainterPath &path, const std::optional<QRectF> &bounds = std::nullopt);
    KisOptimizedBrushOutline(const QVector<QPolygonF> &subpaths, const std::optional<QRectF> &bounds = std::nullopt);

    QRectF boundingRect() const;

    bool isEmpty() const;

    void addRect(const QRectF &rc);
    void addEllipse(const QPointF &center, qreal rx, qreal ry);
    void addPath(const QPainterPath &path);
    void addPath(const KisOptimizedBrushOutline &path);

    void translate(qreal tx, qreal ty);
    void translate(const QPointF &offset);

    void map(const QTransform &t);

    KisOptimizedBrushOutline mapped(const QTransform &t) const;

    const_iterator begin() const;

    const_iterator end() const;

private:
    QVector<QPolygonF> m_subpaths;
    QVector<QPolygonF> m_additionalDecorations;
    std::optional<QRectF> m_explicitBounds;
    QTransform m_transform;
    mutable QRectF m_cachedBoundingRect;
};

#endif
