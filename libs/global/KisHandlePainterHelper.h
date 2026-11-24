/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISHANDLEPAINTERHELPER_H
#define KISHANDLEPAINTERHELPER_H

#include "kritaglobal_export.h"
#include "kis_algebra_2d.h"

#include <QPainter>
#include <KisHandleStyle.h>
class QPainter;
class KoShape;
class KoViewConverter;

class KRITAGLOBAL_EXPORT KisHandlePainterHelper
{
public:

    KisHandlePainterHelper(QPainter *_painter, qreal handleRadius = 0.0, int decorationThickness = 1);

    KisHandlePainterHelper(QPainter *_painter, const QTransform &originalPainterTransform, qreal handleRadius, int decorationThickness = 1);

    KisHandlePainterHelper(KisHandlePainterHelper &&rhs);
    KisHandlePainterHelper(KisHandlePainterHelper &rhs) = delete;

    ~KisHandlePainterHelper();

    void setHandleStyle(const KisHandleStyle &style);

    void drawHandleRect(const QPointF &center, qreal radius);
    void drawHandleRect(const QPointF &center, qreal radius, QPoint offset);
    void fillHandleRect(const QPointF &center, qreal radius, QColor fillColor, QPoint offset);

    void drawHandleCircle(const QPointF &center, qreal radius);

    void drawHandleRect(const QPointF &center);

    void drawHandleCircle(const QPointF &center);

    void drawHandleSmallCircle(const QPointF &center);

    void drawHandleLine(const QLineF &line, qreal width = 1.0, QVector<qreal> dashPattern = {}, qreal dashOffset = 0.0);

    void drawGradientHandle(const QPointF &center, qreal radius);

    void drawGradientHandle(const QPointF &center);

    void drawGradientCrossHandle(const QPointF &center, qreal radius);

    void drawGradientArrow(const QPointF &start, const QPointF &end, qreal radius);

    void drawRubberLine(const QPolygonF &poly);

    void drawConnectionLine(const QLineF &line);

    void drawConnectionLine(const QPointF &p1, const QPointF &p2);

    void drawPath(const QPainterPath &path);

    void drawPixmap(const QPixmap &pixmap, QPointF position, int size, QRectF sourceRect);

private:

    void drawArrow(const QPointF &pos, const QPointF &from, qreal radius);

    void init();

private:
    QPainter *m_painter;
    QTransform m_originalPainterTransform;
    QTransform m_painterTransform;
    qreal m_handleRadius;
    int m_decorationThickness;
    KisAlgebra2D::DecomposedMatrix m_decomposedMatrix;
    QTransform m_handleTransform;
    QPolygonF m_handlePolygon;
    KisHandleStyle m_handleStyle;
};

#endif
