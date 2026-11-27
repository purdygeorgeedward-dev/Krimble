/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISWIDEDOCKSEPARATORSTYLE_H
#define KISWIDEDOCKSEPARATORSTYLE_H

#include "kritawidgetutils_export.h"
#include <QProxyStyle>
#include <QIcon>

class KRITAWIDGETUTILS_EXPORT KisWideDockSeparatorStyle : public QProxyStyle
{
public:
    KisWideDockSeparatorStyle(QStyle *baseStyle);
    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget = nullptr) const override;
    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget = nullptr) const override;
    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;
};

#endif
