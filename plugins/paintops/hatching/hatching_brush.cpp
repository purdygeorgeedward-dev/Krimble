/*
 *  SPDX-FileCopyrightText: 2008, 2009, 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *  SPDX-FileCopyrightText: 2010 José Luis Vergara <pentalis@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "hatching_brush.h"

#include <KoColor.h>
#include <KoColorTransformation.h>

#include <QVariant>

#include "kis_random_accessor_ng.h"
#include <cmath>
#include <time.h>

void inline myround(double *x)
{
    *x = ((*x - floor(*x)) >= 0.5) ? ceil(*x) : floor(*x);
}

HatchingBrush::HatchingBrush(KisHatchingPaintOpSettingsSP settings)
  : m_settings(settings)
  , separation(m_settings->separation)
  , origin_x(m_settings->origin_x)
  , origin_y(m_settings->origin_y)
{
}

HatchingBrush::~HatchingBrush()
{
}

void HatchingBrush::init()
{
}

void HatchingBrush::hatch(KisPaintDeviceSP dev, qreal x, qreal y, double width, double height, double givenAngle, const KoColor &color, qreal additionalScale)
{
    m_painter.begin(dev);
    m_painter.setFillStyle(KisPainter::FillStyleForegroundColor);
    m_painter.setPaintColor(color);
    m_painter.setBackgroundColor(color);

    angle = givenAngle;
    double tempthickness = m_settings->thickness * m_settings->thicknesssensorvalue;
    thickness = qMax(1, qRound(additionalScale * tempthickness));
    separation = additionalScale *
        (m_settings->enabledcurveseparation ?
         separationAsFunctionOfParameter(m_settings->separationsensorvalue, m_settings->separation, m_settings->separationintervals) :
         m_settings->separation);

    height_ = height;
    width_ = width;

    m_painter.setMaskImageSize(width_, height_);

    dy = fabs(separation / cos(angle * M_PI / 180));

    if (!m_settings->subpixelprecision)
        modf(dy, &dy);

    if ((angle == 90) || (angle == -90)) {
        verticalHotX = fmod((origin_x - x), separation);

        iterateVerticalLines(true, 1, false);
        iterateVerticalLines(true, 0, true);
        iterateVerticalLines(false, 1, false);
    }
    else {
        slope = tan(angle * M_PI / 180);
        baseLineIntercept = origin_y - slope * origin_x;
        cursorLineIntercept = y - slope * x;
        hotIntercept = fmod((baseLineIntercept - cursorLineIntercept), dy);

        iterateLines(true, 1, false);
        iterateLines(true, 0, true);
        iterateLines(false, 1, false);
    }
}

void HatchingBrush::iterateLines(bool forward, int lineindex, bool oneline)
{

    double xdraw[2] = {0, 0};
    double ydraw[2] = {0, 0};
    QPointF A, B;
    int append_index = 0;
    bool remaininginnerlines = true;

    while (remaininginnerlines) {

        append_index = 0;
        remaininginnerlines = false;
        if (forward)
            scanIntercept = hotIntercept + dy * lineindex;
        else
            scanIntercept = hotIntercept - dy * lineindex;

        lineindex++;

        if ((scanIntercept >= 0) && (scanIntercept <= height_)) {
            xdraw[append_index] = 0;
            ydraw[append_index] = scanIntercept;
            remaininginnerlines = true;
            append_index++;
        }

        if ((slope * width_ + scanIntercept <= height_) && (slope * width_ + scanIntercept >= 0)) {
            xdraw[append_index] = width_;
            ydraw[append_index] = scanIntercept + slope * width_;
            remaininginnerlines = true;
            append_index++;
        }

        if ((-scanIntercept / slope > 0) && (-scanIntercept / slope < width_)) {
            xdraw[append_index] = -scanIntercept / slope;
            ydraw[append_index] = 0;
            remaininginnerlines = true;
            append_index++;
        }

        if (((height_ - scanIntercept) / slope > 0) && ((height_ - scanIntercept) / slope < width_)) {
            xdraw[append_index] = (height_ - scanIntercept) / slope;
            ydraw[append_index] = height_;
            remaininginnerlines = true;
            append_index++;
        }

        if (!remaininginnerlines)
            break;

        if (!m_settings->subpixelprecision) {
            myround(&xdraw[0]);
            myround(&xdraw[1]);
            myround(&ydraw[0]);
            myround(&ydraw[1]);
        }

        A.setX(xdraw[0]);
        A.setY(ydraw[0]);

        if (append_index == 2) {
            B.setX(xdraw[1]);
            B.setY(ydraw[1]);

            if (m_settings->antialias)
                m_painter.drawThickLine(A, B, thickness, thickness);
            else
                m_painter.drawLine(A, B, thickness, false);

            if (oneline)
                break;
        }
        else {
            continue;
        }
    }
}

void HatchingBrush::iterateVerticalLines(bool forward, int lineindex, bool oneline)
{

    double xdraw = 0;
    double ydraw[2] = {0, height_};
    QPointF A, B;
    bool remaininginnerlines = true;

    while (remaininginnerlines) {

        remaininginnerlines = false;
        if (forward)
            verticalScanX = verticalHotX + separation * lineindex;
        else
            verticalScanX = verticalHotX - separation * lineindex;

        lineindex++;

        if ((verticalScanX >= 0) && (verticalScanX <= width_)) {
            xdraw = verticalScanX;
            remaininginnerlines = true;
        }

        if (!remaininginnerlines)
            break;

        if (!m_settings->subpixelprecision) {
            myround(&xdraw);
            myround(&ydraw[1]);
        }

        A.setX(xdraw);
        A.setY(ydraw[0]);
        B.setX(xdraw);
        B.setY(ydraw[1]);

        if (m_settings->antialias)
            m_painter.drawThickLine(A, B, thickness, thickness);
        else
            m_painter.drawLine(A, B, thickness, false);

        if (oneline)
            break;
        else
            continue;
    }
}

double HatchingBrush::separationAsFunctionOfParameter(double parameter, double separation, int numintervals)
{
    if ((numintervals < 2) || (numintervals > 7)) {
        dbgKrita << "Fix your function" << numintervals << "<> 2-7" ;
        return separation;
    }

    double sizeinterval = 1 / double(numintervals);
    double lowerlimit = 0;
    double upperlimit = 0;
    double factor = 0;

    int basefactor = numintervals / 2;
    if ((numintervals % 2) == 0)
        basefactor--;

    for (quint8 currentinterval = 0; currentinterval < numintervals; currentinterval++) {
        lowerlimit = upperlimit;
        upperlimit += sizeinterval;
        if (currentinterval == (numintervals - 1))
            upperlimit = 1;
        if ((parameter >= lowerlimit) && (parameter <= upperlimit)) {
            factor = pow(2.0, (basefactor - currentinterval));
            return (separation * factor);
        }
    }

    dbgKrita << "Fix your function" << parameter << ">" << upperlimit ;
    return separation;
}
