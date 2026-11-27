/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISHANDLESTYLE_H
#define KISHANDLESTYLE_H

#include <QVector>
#include <QPen>
#include <QBrush>

#include "kritaglobal_export.h"

struct KRITAGLOBAL_EXPORT KisHandlePalette
{
    KisHandlePalette()
        : primaryColor(QColor(0, 0, 90, 180))
        , secondaryColor(QColor(0, 0, 255, 127))
        , gradientFillColor(QColor(255, 197, 39))
        , highlightColor(QColor(255, 100, 100))
        , highlightOutlineColor(QColor(155, 0, 0))
        , selectionColor(QColor(164, 227, 243))
        , white(Qt::white)
        , black(Qt::black)
    {
    }

    QColor primaryColor;
    QColor secondaryColor;
    QColor gradientFillColor;
    QColor highlightColor;
    QColor highlightOutlineColor;
    QColor selectionColor;
    QColor white;
    QColor black;
};

class KRITAGLOBAL_EXPORT KisHandleStyle
{
public:

    static KisHandleStyle& inheritStyle();

    static KisHandleStyle& primarySelection(KisHandlePalette palette = KisHandlePalette());

    static KisHandleStyle& secondarySelection(KisHandlePalette palette = KisHandlePalette());

    static KisHandleStyle& gradientHandles(KisHandlePalette palette = KisHandlePalette());

    static KisHandleStyle& gradientArrows(KisHandlePalette palette = KisHandlePalette());

    static KisHandleStyle& highlightedPrimaryHandles(KisHandlePalette palette = KisHandlePalette());

    static KisHandleStyle& highlightedPrimaryHandlesWithSolidOutline(KisHandlePalette palette = KisHandlePalette());

    static KisHandleStyle& partiallyHighlightedPrimaryHandles(KisHandlePalette palette = KisHandlePalette());

    static KisHandleStyle& selectedPrimaryHandles(KisHandlePalette palette = KisHandlePalette());

    struct IterationStyle {
        IterationStyle() : isValid(false) {}
        IterationStyle(const QPen &pen, const QBrush &brush)
            : isValid(true),
              stylePair(pen, brush)
        {
        }

        bool isValid;
        QPair<QPen, QBrush> stylePair;
    };

    QVector<IterationStyle> handleIterations;
    QVector<IterationStyle> lineIterations;
};

#endif
