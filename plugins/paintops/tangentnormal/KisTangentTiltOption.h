/*
 * SPDX-FileCopyrightText: 2015 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KISTANGENTTILT_OPTION_H
#define KISTANGENTTILT_OPTION_H

#include <brushengine/kis_paint_information.h>
#include "KisTangentTiltOptionData.h"

struct KisTangentTiltOptionData;

class KisTangentTiltOption
{
public:
    KisTangentTiltOption(const KisPropertiesConfiguration *setting);

    void swizzleAssign(qreal const horizontal, qreal const vertical, qreal const depth, qreal *component, int index, qreal maxvalue);

    void apply(const KisPaintInformation& info, qreal *r, qreal *g, qreal *b);

private:
    KisTangentTiltOption(const KisTangentTiltOptionData &data);
private:
    int m_redChannel;
    int m_greenChannel;
    int m_blueChannel;
    TangentTiltDirectionType m_directionType;
    double m_elevationSensitivity;
    double m_mixValue;
};

#endif
