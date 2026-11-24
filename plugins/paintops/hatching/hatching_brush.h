/*
 *  SPDX-FileCopyrightText: 2008, 2009 Lukáš Tvrdý <lukast.dev@gmail.com>
 *  SPDX-FileCopyrightText: 2010 José Luis Vergara <pentalis@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef HATCHING_BRUSH_H_
#define HATCHING_BRUSH_H_

#include <KoColor.h>

#include "kis_hatching_paintop_settings.h"

#include <kis_painter.h>
#include <kis_paint_device.h>
#include <brushengine/kis_paint_information.h>

class HatchingBrush
{

public:
    HatchingBrush(KisHatchingPaintOpSettingsSP settings);
    ~HatchingBrush();
    HatchingBrush(KoColor inkColor);

    void hatch(KisPaintDeviceSP dev, qreal x, qreal y, double width, double height, double givenAngle, const KoColor &color, qreal additionalScale);

private:
    void init();
    KoColor m_inkColor;
    int m_counter {0};
    int m_radius {0};
    KisHatchingPaintOpSettingsSP m_settings;
    KisPainter m_painter;

    int thickness {0};

    double angle {0.0};

    double separation {0.0};

    double height_ {0.0};

    double width_ {0.0};

    double origin_x {0.0};

    double origin_y {0.0};

    double baseLineIntercept {0};

    double hotIntercept {0};

    double scanIntercept {0};

    double verticalHotX {0.0};

    double verticalScanX {0.0};

    double slope {0};

    double dx {0};

    double dy {0};

    double cursorLineIntercept {0};

    void iterateLines(bool forward, int lineindex, bool oneline);

    void iterateVerticalLines(bool forward, int lineindex, bool oneline);

    double separationAsFunctionOfParameter(double parameter, double separation, int numintervals);
};

#endif
