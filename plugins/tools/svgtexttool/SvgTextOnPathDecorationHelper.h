/*
 * SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef SVGTEXTONPATHDECORATIONHELPER_H
#define SVGTEXTONPATHDECORATIONHELPER_H

#include <QPointF>
#include <QPainter>
#include <KisHandleStyle.h>

class KoSvgTextShape;
class KoViewConverter;
class SvgTextOnPathDecorationHelper
{
public:
    SvgTextOnPathDecorationHelper();
    ~SvgTextOnPathDecorationHelper();

    void setPos(int pos);

    void setShape(KoSvgTextShape *shape);

    void setHandleRadius(qreal radius);
    void setDecorationThickness(qreal thickness);

    bool hitTest(QPointF mouseInPts, const QTransform viewToDocument);

    void paint(QPainter *painter, const KoViewConverter &converter, KisHandlePalette handlePalette);

    QRectF decorationRect(const QTransform documentToView) const;

    void setStrategyActive(bool isActive);

private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
