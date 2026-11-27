/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2003, 2005 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2007, 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGGRAPHICCONTEXT_H
#define SVGGRAPHICCONTEXT_H

#include "kritaflake_export.h"
#include <KoShapeStroke.h>
#include <QTransform>
#include <text/KoSvgTextProperties.h>

class KRITAFLAKE_EXPORT SvgGraphicsContext
{
public:
    enum StyleType {
        None,
        Solid,
        Complex,
        Inherit
    };

    SvgGraphicsContext();
    SvgGraphicsContext(const SvgGraphicsContext &gc);

    void workaroundClearInheritedFillProperties();

    StyleType     fillType  {Solid};
    Qt::FillRule  fillRule  {Qt::WindingFill};
    QColor        fillColor {QColor(Qt::black)};
    QString       fillId;

    StyleType     strokeType {None};
    QString       strokeId;
    KoShapeStrokeSP stroke;

    QString filterId;
    QString clipPathId;
    QString clipMaskId;
    Qt::FillRule clipRule {Qt::WindingFill};
    qreal opacity {1.0};

    QTransform matrix;
    QColor  currentColor {Qt::black};
    QString xmlBaseDir;
    bool preserveWhitespace {false};

    QRectF currentBoundingBox;
    bool   forcePercentage {false};
    QTransform viewboxTransform;

    bool display {true};
    bool visible {true};
    bool isResolutionFrame {false};
    qreal pixelsPerInch {72.0};

    QString markerStartId;
    QString markerMidId;
    QString markerEndId;

    bool autoFillMarkers {false};

    KoSvgTextProperties textProperties;
    QString shapeInsideValue;
    QString shapeSubtractValue;

    QString paintOrder;
private:
    SvgGraphicsContext& operator=(const SvgGraphicsContext &gc) = default;
};

#endif
