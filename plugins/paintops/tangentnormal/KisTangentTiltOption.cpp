/*
 * SPDX-FileCopyrightText: 2015 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KisTangentTiltOption.h"

#include <KisPaintOpOptionUtils.h>
namespace kpou = KisPaintOpOptionUtils;

KisTangentTiltOption::KisTangentTiltOption(const KisPropertiesConfiguration *setting)
    : KisTangentTiltOption(kpou::loadOptionData<KisTangentTiltOptionData>(setting))
{
}

KisTangentTiltOption::KisTangentTiltOption(const KisTangentTiltOptionData &data)
    : m_redChannel(data.redChannel)
    , m_greenChannel(data.greenChannel)
    , m_blueChannel(data.blueChannel)
    , m_directionType(data.directionType)
    , m_elevationSensitivity(data.elevationSensitivity)
    , m_mixValue(data.mixValue)
{
}

void KisTangentTiltOption::swizzleAssign(qreal const horizontal, qreal const vertical, qreal const depth, qreal *component, int index, qreal maxvalue)
{
    switch(index) {
    case 0: *component = horizontal; break;
    case 1: *component = maxvalue-horizontal; break;
    case 2: *component = vertical; break;
    case 3: *component = maxvalue-vertical; break;
    case 4: *component = depth; break;
    case 5: *component = maxvalue-depth; break;
    }
}

void KisTangentTiltOption::apply(const KisPaintInformation& info,qreal *r,qreal *g,qreal *b)
{

    qreal halfvalue = 0.5;
    qreal maxvalue = 1.0;

    qreal direction = KisPaintInformation::tiltDirection(info, true)*360.0;
    qreal elevation= (info.tiltElevation(info, 60.0, 60.0, true)*90.0);

    if (m_directionType == TangentTiltDirectionType::Tilt) {
        direction = KisPaintInformation::tiltDirection(info, true)*360.0;
        elevation= (info.tiltElevation(info, 60.0, 60.0, true)*90.0);
    } else if (m_directionType == TangentTiltDirectionType::Direction) {
        direction = (0.75 + info.drawingAngle() / (2.0 * M_PI))*360.0;
        elevation= 0;
    } else if (m_directionType == TangentTiltDirectionType::Rotation) {
        direction = info.rotation();
        elevation= (info.tiltElevation(info, 60.0, 60.0, true)*90.0);
    } else if (m_directionType == TangentTiltDirectionType::Mix) {
        qreal mixamount = m_mixValue/100.0;
        direction = (KisPaintInformation::tiltDirection(info, true)*360.0*(1.0-mixamount))+((0.75 + info.drawingAngle() / (2.0 * M_PI))*360.0*(mixamount));
        elevation= (info.tiltElevation(info, 60.0, 60.0, true)*90.0);
    }

    if (m_directionType != TangentTiltDirectionType::Direction) {
        direction = normalizeAngleDegrees(direction - info.canvasRotation());
    }

    qreal elevationT = elevation*(m_elevationSensitivity/100.0)+(90-(m_elevationSensitivity*90.0)/100.0);
    elevation = static_cast<int>(elevationT);

    direction = kisDegreesToRadians(direction);
    elevation = kisDegreesToRadians(elevation);

    qreal horizontal, vertical, depth;

    horizontal = cos(elevation)*sin(direction);
    if (horizontal>0.0) {
        horizontal= halfvalue+(fabs(horizontal)*halfvalue);
    }
    else {
        horizontal= halfvalue-(fabs(horizontal)*halfvalue);
    }
    vertical = cos(elevation)*cos(direction);
    if (vertical>0.0) {
        vertical = halfvalue+(fabs(vertical)*halfvalue);
    }
    else {
        vertical = halfvalue-(fabs(vertical)*halfvalue);
    }

    if (info.canvasMirroredH()) {horizontal = maxvalue - horizontal;}
    if (info.canvasMirroredV()) {vertical = maxvalue - vertical;}

    depth = sin(elevation)*maxvalue;

    swizzleAssign(horizontal, vertical, depth, r, m_redChannel, maxvalue);
    swizzleAssign(horizontal, vertical, depth, g, m_greenChannel, maxvalue);
    swizzleAssign(horizontal, vertical, depth, b, m_blueChannel, maxvalue);
}
