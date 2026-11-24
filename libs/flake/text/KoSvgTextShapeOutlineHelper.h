/*
 *  SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOSVGTEXTSHAPEOUTLINEHELPER_H
#define KOSVGTEXTSHAPEOUTLINEHELPER_H

#include <KoCanvasBase.h>
#include <KoViewConverter.h>

#include <QPainter>
#include <QPointF>
#include <QScopedPointer>

#include <kritaflake_export.h>
#include <KisHandleStyle.h>

class KoSvgTextShape;
class KRITAFLAKE_EXPORT KoSvgTextShapeOutlineHelper
{
public:
    KoSvgTextShapeOutlineHelper(KoCanvasBase *canvas);
    ~KoSvgTextShapeOutlineHelper();

    void paint(QPainter *painter, const KoViewConverter &converter);

    QRectF decorationRect();

    void setDrawBoundingRect(bool enable);
    bool drawBoundingRect() const;

    void setDrawTextWrappingArea(bool enable);
    bool drawTextWrappingArea() const;

    void setDrawShapeOutlines(bool enable);
    bool drawShapeOutlines() const;

    void setHandleRadius(int radius);
    void setDecorationThickness(int thickness);

    KoSvgTextShape *contourModeButtonHovered(const QPointF &point);

    bool updateTextContourMode();

    void toggleTextContourMode(KoSvgTextShape *shape);

    void setTextAreasHovered(bool enabled);

private:
    void paintTextShape(QPainter *painter, const KoViewConverter &converter, KoSvgTextShape *text,
                        bool contourModeActive = false);
    struct Private;
    QScopedPointer<Private> d;
};

#endif
