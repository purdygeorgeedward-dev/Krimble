/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOFLAKE_H
#define KOFLAKE_H

#include "kritaflake_export.h"

class QGradient;
class QRectF;
class QPointF;
class QSizeF;

class KoShape;
class QTransform;

#include <Qt>

namespace KoFlake
{
    enum FillVariant {
        Fill,
        StrokeFill
    };

    enum FillType {
        None,
        Solid,
        Gradient,
        Pattern,
        MeshGradient
    };

    enum MarkerPosition {
        StartMarker,
        MidMarker,
        EndMarker
    };

    enum SelectionType {
        FullSelection,
        StrippedSelection,
        TopLevelSelection
    };

    enum SelectionHandle {
        TopMiddleHandle,
        TopRightHandle,
        RightMiddleHandle,
        BottomRightHandle,
        BottomMiddleHandle,
        BottomLeftHandle,
        LeftMiddleHandle,
        TopLeftHandle,
        NoHandle
    };

    enum ShapeSelection {
        Selected,
        Unselected,
        NextUnselected,
        ShapeOnTop
    };

    enum StyleType {
        Background,
        Foreground
    };

    enum AnchorPosition {
        TopLeft,
        Top,
        TopRight,
        Left,
        Center,
        Right,
        BottomLeft,
        Bottom,
        BottomRight,
        NoAnchor,
        NumAnchorPositions
    };

    KRITAFLAKE_EXPORT QPointF anchorToPoint(AnchorPosition anchor, const QRectF rect, bool *valid = 0);

    enum CanvasResource {
        HotPosition = 1410100299
    };

    KRITAFLAKE_EXPORT QGradient *cloneGradient(const QGradient *gradient);

    KRITAFLAKE_EXPORT QGradient *mergeGradient(const QGradient *coordsSource, const QGradient *fillSource);

    KRITAFLAKE_EXPORT QPointF toRelative(const QPointF &absolute, const QSizeF &size);

    KRITAFLAKE_EXPORT QPointF toAbsolute(const QPointF &relative, const QSizeF &size);

    KRITAFLAKE_EXPORT Qt::Orientation significantScaleOrientation(qreal scaleX, qreal scaleY);

    KRITAFLAKE_EXPORT void scaleShape(KoShape *shape, qreal scaleX, qreal scaleY,
                                      const QPointF &absoluteStillPoint,
                                      const QTransform &postScalingCoveringTransform);

    KRITAFLAKE_EXPORT void scaleShapeGlobal(KoShape *shape, qreal scaleX, qreal scaleY,
                          const QPointF &absoluteStillPoint);

    KRITAFLAKE_EXPORT void resizeShape(KoShape *shape, qreal scaleX, qreal scaleY,
                              const QPointF &absoluteStillPoint,
                              bool useGlobalMode);

    KRITAFLAKE_EXPORT void resizeShapeCommon(KoShape *shape, qreal scaleX, qreal scaleY,
                                       const QPointF &absoluteStillPoint,
                                       bool useGlobalMode,
                                       bool usePostScaling, const QTransform &postScalingCoveringTransform);
}

#endif
