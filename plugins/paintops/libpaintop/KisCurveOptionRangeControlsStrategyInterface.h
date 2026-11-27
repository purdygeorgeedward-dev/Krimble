/*
 * SPDX-FileCopyrightText: 2022 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISCURVEOPTIONRANGECONTROLSSTRATEGYINTERFACE_H
#define KISCURVEOPTIONRANGECONTROLSSTRATEGYINTERFACE_H

#include <kritapaintop_export.h>
#include <functional>

class KisCurveRangeModelInterface;
class QWidget;

class PAINTOP_EXPORT KisCurveOptionRangeControlsStrategyInterface
{
public:
    virtual ~KisCurveOptionRangeControlsStrategyInterface();

};

using KisCurveOptionRangeControlsStrategyFactory =
    std::function<
        KisCurveOptionRangeControlsStrategyInterface* (
            KisCurveRangeModelInterface* ,
            QWidget* )>;

#endif
