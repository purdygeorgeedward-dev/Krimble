/*
 *  SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "psd.h"

#include <QPoint>
#include <kis_debug.h>

#include <KoColorModelStandardIds.h>
#include <KoCompositeOpRegistry.h>

#include "kis_global.h"

QPair<QString, QString> psd_colormode_to_colormodelid(psd_color_mode colormode, quint16 channelDepth)
{
    QPair<QString, QString> colorSpaceId;
    switch (colormode) {
    case (Bitmap):
    case (Indexed):
    case (MultiChannel):
    case (RGB):
        colorSpaceId.first = RGBAColorModelID.id();
        break;
    case (CMYK):
        colorSpaceId.first = CMYKAColorModelID.id();
        break;
    case (Grayscale):
        colorSpaceId.first = GrayAColorModelID.id();
        break;
    case (DuoTone):
        colorSpaceId.first = GrayAColorModelID.id();
        break;
    case (Lab):
        colorSpaceId.first = LABAColorModelID.id();
        break;
    default:
        return colorSpaceId;
    }

    switch (channelDepth) {
    case (1):
    case (8):
        colorSpaceId.second = Integer8BitsColorDepthID.id();
        break;
    case (16):
        colorSpaceId.second = Integer16BitsColorDepthID.id();
        break;
    case (32):
        colorSpaceId.second = Float32BitsColorDepthID.id();
        break;
    default:
        break;
    }
    return colorSpaceId;
}

QString psd_blendmode_to_composite_op(const QString &blendmode)
{
    if (blendmode == "pass")
        return COMPOSITE_PASS_THROUGH;
    if (blendmode == "norm")
        return COMPOSITE_OVER;
    if (blendmode == "diss")
        return COMPOSITE_DISSOLVE;
    if (blendmode == "dark")
        return COMPOSITE_DARKEN;
    if (blendmode == "mul ")
        return COMPOSITE_MULT;
    if (blendmode == "idiv")
        return COMPOSITE_BURN;
    if (blendmode == "lbrn")
        return COMPOSITE_LINEAR_BURN;
    if (blendmode == "dkCl")
        return COMPOSITE_DARKER_COLOR;
    if (blendmode == "lite")
        return COMPOSITE_LIGHTEN;
    if (blendmode == "scrn")
        return COMPOSITE_SCREEN;
    if (blendmode == "div ")
        return COMPOSITE_DODGE;
    if (blendmode == "lddg")
        return COMPOSITE_LINEAR_DODGE;
    if (blendmode == "lgCl")
        return COMPOSITE_LIGHTER_COLOR;
    if (blendmode == "over")
        return COMPOSITE_OVERLAY;
    if (blendmode == "sLit")
        return COMPOSITE_SOFT_LIGHT_PHOTOSHOP;
    if (blendmode == "hLit")
        return COMPOSITE_HARD_LIGHT;
    if (blendmode == "vLit")
        return COMPOSITE_VIVID_LIGHT;
    if (blendmode == "lLit")
        return COMPOSITE_LINEAR_LIGHT;
    if (blendmode == "pLit")
        return COMPOSITE_PIN_LIGHT;
    if (blendmode == "hMix")
        return COMPOSITE_HARD_MIX_PHOTOSHOP;
    if (blendmode == "diff")
        return COMPOSITE_DIFF;
    if (blendmode == "smud")
        return COMPOSITE_EXCLUSION;
    if (blendmode == "fsub")
        return COMPOSITE_SUBTRACT;
    if (blendmode == "fdiv")
        return COMPOSITE_DIVIDE;
    if (blendmode == "hue ")
        return COMPOSITE_HUE;
    if (blendmode == "sat ")
        return COMPOSITE_SATURATION;
    if (blendmode == "colr")
        return COMPOSITE_COLOR;
    if (blendmode == "lum ")
        return COMPOSITE_LUMINIZE;

    dbgKrita << "Unknown blendmode:" << blendmode << ". Returning Normal";
    return COMPOSITE_OVER;
}

QString composite_op_to_psd_blendmode(const QString &compositeop)
{
    if (compositeop == COMPOSITE_PASS_THROUGH)
        return "pass";
    if (compositeop == COMPOSITE_OVER)
        return "norm";
    if (compositeop == COMPOSITE_DISSOLVE)
        return "diss";
    if (compositeop == COMPOSITE_DARKEN)
        return "dark";
    if (compositeop == COMPOSITE_MULT)
        return "mul ";
    if (compositeop == COMPOSITE_BURN)
        return "idiv";
    if (compositeop == COMPOSITE_LINEAR_BURN)
        return "lbrn";
    if (compositeop == COMPOSITE_DARKER_COLOR)
        return "dkCl";
    if (compositeop == COMPOSITE_LIGHTEN)
        return "lite";
    if (compositeop == COMPOSITE_SCREEN)
        return "scrn";
    if (compositeop == COMPOSITE_DODGE)
        return "div ";
    if (compositeop == COMPOSITE_LINEAR_DODGE)
        return "lddg";
    if (compositeop == COMPOSITE_LIGHTER_COLOR)
        return "lgCl";
    if (compositeop == COMPOSITE_OVERLAY)
        return "over";
    if (compositeop == COMPOSITE_SOFT_LIGHT_PHOTOSHOP)
        return "sLit";
    if (compositeop == COMPOSITE_SOFT_LIGHT_SVG)
        return "sLit";
    if (compositeop == COMPOSITE_HARD_LIGHT)
        return "hLit";
    if (compositeop == COMPOSITE_VIVID_LIGHT)
        return "vLit";
    if (compositeop == COMPOSITE_LINEAR_LIGHT)
        return "lLit";
    if (compositeop == COMPOSITE_PIN_LIGHT)
        return "pLit";
    if (compositeop == COMPOSITE_HARD_MIX_PHOTOSHOP)
        return "hMix";
    if (compositeop == COMPOSITE_DIFF)
        return "diff";
    if (compositeop == COMPOSITE_EXCLUSION)
        return "smud";
    if (compositeop == COMPOSITE_SUBTRACT)
        return "fsub";
    if (compositeop == COMPOSITE_DIVIDE)
        return "fdiv";
    if (compositeop == COMPOSITE_HUE)
        return "hue ";
    if (compositeop == COMPOSITE_SATURATION)
        return "sat ";
    if (compositeop == COMPOSITE_COLOR)
        return "colr";
    if (compositeop == COMPOSITE_LUMINIZE)
        return "lum ";

    dbgKrita << "Krita blending mode" << compositeop << "does not exist in PS, returning Normal";
    return "norm";
}

QPoint psd_layer_effects_shadow_base::calculateOffset(const psd_layer_effects_context *context) const
{
    Q_UNUSED(context);

    qint32 angle = this->m_angle;

    qint32 distanceX = -qRound(this->m_distance * cos(kisDegreesToRadians(qreal(angle))));
    qint32 distanceY = qRound(this->m_distance * sin(kisDegreesToRadians(qreal(angle))));

    return QPoint(distanceX, distanceY);
}

psd_layer_effects_shadow_base::~psd_layer_effects_shadow_base()
{
}

void psd_layer_effects_shadow_base::scaleLinearSizes(qreal scale)
{
    m_distance *= scale;
    m_size *= scale;
}

psd_layer_effects_shadow_common::~psd_layer_effects_shadow_common()
{
}

psd_layer_effects_drop_shadow::~psd_layer_effects_drop_shadow()
{
}

psd_layer_effects_glow_common::~psd_layer_effects_glow_common()
{
}

psd_layer_effects_inner_glow::~psd_layer_effects_inner_glow()
{
}

psd_layer_effects_outer_glow::~psd_layer_effects_outer_glow()
{
}

psd_layer_effects_inner_shadow::~psd_layer_effects_inner_shadow()
{
}
