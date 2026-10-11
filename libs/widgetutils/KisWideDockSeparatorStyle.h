/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISWIDEDOCKSEPARATORSTYLE_H
#define KISWIDEDOCKSEPARATORSTYLE_H

#include "kritawidgetutils_export.h"
#include <QProxyStyle>
#include <QIcon>

/**
 * Krimble: a proxy style that doubles the pixel metric governing the width
 * of dock widget (panel) separators. On a touchscreen, the default width
 * most desktop styles use (a handful of pixels, sized for a mouse cursor) is
 * too small to reliably grab with a finger to resize a docked panel. Qt ties
 * the visible separator width and its resize hit-test/grab zone to the same
 * single pixel metric, so there's no way to widen only the grab zone without
 * also widening what's drawn on screen -- this doubles both together.
 */
class KRITAWIDGETUTILS_EXPORT KisWideDockSeparatorStyle : public QProxyStyle
{
public:
    KisWideDockSeparatorStyle(QStyle *baseStyle);
    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override; // Krimble 2026-10-10: grip dots only, no line or band
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override; // Krimble 2026-10-10: visible menu dividers, no shortcut text
    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget = nullptr) const override;
    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override; // Krimble 2026-10-11: real X, minimize, maximize and restore glyphs // Krimble 2026-10-10: divider height, no shortcut column
};

#endif // KISWIDEDOCKSEPARATORSTYLE_H
