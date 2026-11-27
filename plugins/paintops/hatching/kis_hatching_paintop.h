/*
 *  SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2008, 2009 Lukáš Tvrdý <lukast.dev@gmail.com>
 *  SPDX-FileCopyrightText: 2010 José Luis Vergara Toloza <pentalis@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_HATCHING_PAINTOP_H_
#define KIS_HATCHING_PAINTOP_H_

#include <brushengine/kis_paintop.h>
#include <kis_brush_based_paintop.h>
#include <kis_types.h>

#include "hatching_brush.h"
#include "kis_hatching_paintop_settings.h"

#include "KisHatchingStandardOptions.h"

#include "KisHatchingOptionsData.h"
#include "KisHatchingPreferencesData.h"

#include <KisStandardOptions.h>
#include "KisOpacityOption.h"

class KisPainter;

class KisHatchingPaintOp : public KisBrushBasedPaintOp
{

public:

    KisHatchingPaintOp(const KisPaintOpSettingsSP settings, KisPainter * painter, KisNodeSP node, KisImageSP image);
    ~KisHatchingPaintOp() override;

    double spinAngle(double spin);

protected:
    KisSpacingInformation paintAt(const KisPaintInformation& info) override;

    KisSpacingInformation updateSpacingImpl(const KisPaintInformation &info) const override;

private:
    KisHatchingPaintOpSettingsSP m_settings;
    KisHatchingOptionsData m_hatchingOptions;
    KisHatchingPreferencesData m_hatchingPreferences;
    HatchingBrush *m_hatchingBrush;

    KisPaintDeviceSP m_hatchedDab;

    KisAngleOption m_angleOption;

    KisCrosshatchingOption m_crosshatchingOption;

    KisSeparationOption m_separationOption;

    KisThicknessOption m_thicknessOption;

    KisOpacityOption m_opacityOption;

    KisSizeOption m_sizeOption;
};

#endif
