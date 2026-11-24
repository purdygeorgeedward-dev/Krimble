/*
 *  SPDX-FileCopyrightText: 2019 Agata Cacko <cacko.azh@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_SPIN_BOX_SPLINE_UNITS_CONVERTER_H
#define KIS_SPIN_BOX_SPLINE_UNITS_CONVERTER_H

#include <kritaui_export.h>

class KRITAUI_EXPORT KisSpinBoxSplineUnitConverter
{

public:
    double io2sp(int x, int min, int max);
    int sp2io(double x, int min, int max);

};

#endif
