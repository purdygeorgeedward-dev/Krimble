/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_PAINTOP_PLUGIN_UTILS_H
#define __KIS_PAINTOP_PLUGIN_UTILS_H

#include "kis_paint_information.h"
#include "kis_paintop_utils.h"
#include "kis_paintop_settings.h"
#include "KisAirbrushOptionData.h"
#include "KisStandardOptions.h"
#include "KisAirbrushOptionData.h"
#include "KisSpacingOption.h"

namespace KisPaintOpPluginUtils {

KisSpacingInformation effectiveSpacing(qreal dabWidth,
                                       qreal dabHeight,
                                       bool isotropicSpacing,
                                       qreal rotation,
                                       bool axesFlipped,
                                       qreal spacingVal,
                                       bool autoSpacingActive,
                                       qreal autoSpacingCoeff,
                                       qreal lodScale,
                                       const KisAirbrushOptionData *airbrushOption,
                                       const KisSpacingOption *spacingOption,
                                       const KisPaintInformation &pi)
{
    bool distanceSpacingEnabled = true;
    if (airbrushOption && airbrushOption->isChecked) {
        distanceSpacingEnabled = !airbrushOption->ignoreSpacing;
    }
    qreal extraScale = 1.0;
    if (spacingOption && spacingOption->isChecked()) {
        extraScale = spacingOption->apply(pi);
    }

    return KisPaintOpUtils::effectiveSpacing(dabWidth, dabHeight, extraScale,
                                             distanceSpacingEnabled, isotropicSpacing, rotation,
                                             axesFlipped, spacingVal, autoSpacingActive,
                                             autoSpacingCoeff, lodScale);
}

KisTimingInformation effectiveTiming(const KisAirbrushOptionData *airbrushOption,
                                     const KisRateOption *rateOption,
                                     const KisPaintInformation &pi)
{
    bool timingEnabled = false;
    qreal timingInterval = LONG_TIME;
    if (airbrushOption) {
        timingEnabled = airbrushOption->isChecked;
        timingInterval = 1000.0 / airbrushOption->airbrushRate;
    }
    qreal rateExtraScale = 1.0;
    if (rateOption && rateOption->isChecked()) {
        rateExtraScale = rateOption->apply(pi);
    }

    return KisPaintOpUtils::effectiveTiming(timingEnabled, timingInterval, rateExtraScale);
}

}

#endif
