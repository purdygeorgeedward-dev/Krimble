/*
 *  SPDX-FileCopyrightText: 2020 Peter Schatz <voronwe13@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOCOLORSPACEPRESERVELIGHTNESSUTILS_H
#define KOCOLORSPACEPRESERVELIGHTNESSUTILS_H

#include <KoColorSpaceMaths.h>
#include "kis_global.h"

template<typename CSTraits>
inline static void fillGrayBrushWithColorPreserveLightnessRGB(quint8 *pixels, const QRgb *brush, quint8 *brushColor, qreal strength, qint32 nPixels) {
    using RGBPixel = typename CSTraits::Pixel;
        using channels_type = typename CSTraits::channels_type;
        static const quint32 pixelSize = CSTraits::pixelSize;

        const RGBPixel *srcColorRGB = reinterpret_cast<const RGBPixel*>(brushColor);

        const float srcColorR = KoColorSpaceMaths<channels_type, float>::scaleToA(srcColorRGB->red);
        const float srcColorG = KoColorSpaceMaths<channels_type, float>::scaleToA(srcColorRGB->green);
        const float srcColorB = KoColorSpaceMaths<channels_type, float>::scaleToA(srcColorRGB->blue);
        const float srcColorA = KoColorSpaceMaths<channels_type, float>::scaleToA(srcColorRGB->alpha);

        const float srcColorL = getLightness<HSLType, float>(srcColorR, srcColorG, srcColorB);
        const float lightnessB = 4 * srcColorL - 1;
        const float lightnessA = 1 - lightnessB;

        for (; nPixels > 0; --nPixels, pixels += pixelSize, ++brush) {
            float brushMaskL = qRed(*brush) / 255.0f;
            brushMaskL = (brushMaskL - 0.5) * strength + 0.5;
            const float finalAlpha = qMin(qAlpha(*brush) / 255.0f, srcColorA);
            float finalLightness = lightnessA * pow2(brushMaskL) + lightnessB * brushMaskL;
            finalLightness = qBound(0.0f, finalLightness, 1.0f);

            float pixelR = srcColorR;
            float pixelG = srcColorG;
            float pixelB = srcColorB;

            setLightness<HSLType, float>(pixelR, pixelG, pixelB, finalLightness);

            RGBPixel *pixelRGB = reinterpret_cast<RGBPixel*>(pixels);
            pixelRGB->red = KoColorSpaceMaths<float, channels_type>::scaleToA(pixelR);
            pixelRGB->green = KoColorSpaceMaths<float, channels_type>::scaleToA(pixelG);
            pixelRGB->blue = KoColorSpaceMaths<float, channels_type>::scaleToA(pixelB);
            pixelRGB->alpha = KoColorSpaceMaths<quint8, channels_type>::scaleToA(quint8(finalAlpha * 255));
        }
}

template<typename CSTraits>
inline static void modulateLightnessByGrayBrushRGB(quint8 *pixels, const QRgb *brush, qreal strength, qint32 nPixels) {
    using RGBPixel = typename CSTraits::Pixel;
        using channels_type = typename CSTraits::channels_type;
        static const quint32 pixelSize = CSTraits::pixelSize;

        for (; nPixels > 0; --nPixels, pixels += pixelSize, ++brush) {

            RGBPixel *pixelRGB = reinterpret_cast<RGBPixel*>(pixels);

            const float srcColorR = KoColorSpaceMaths<channels_type, float>::scaleToA(pixelRGB->red);
            const float srcColorG = KoColorSpaceMaths<channels_type, float>::scaleToA(pixelRGB->green);
            const float srcColorB = KoColorSpaceMaths<channels_type, float>::scaleToA(pixelRGB->blue);

            const float srcColorL = getLightness<HSLType, float>(srcColorR, srcColorG, srcColorB);
            float brushMaskL = qRed(*brush) / 255.0f;
            brushMaskL = (brushMaskL - 0.5) * strength * qAlpha(*brush) / 255.0 + 0.5;

            const float lightnessB = 4 * srcColorL - 1;
            const float lightnessA = 1 - lightnessB;

            float finalLightness = lightnessA * pow2(brushMaskL) + lightnessB * brushMaskL;
            finalLightness = qBound(0.0f, finalLightness, 1.0f);

            float pixelR = srcColorR;
            float pixelG = srcColorG;
            float pixelB = srcColorB;

            setLightness<HSLType, float>(pixelR, pixelG, pixelB, finalLightness);

            pixelRGB->red = KoColorSpaceMaths<float, channels_type>::scaleToA(pixelR);
            pixelRGB->green = KoColorSpaceMaths<float, channels_type>::scaleToA(pixelG);
            pixelRGB->blue = KoColorSpaceMaths<float, channels_type>::scaleToA(pixelB);
        }
}

#endif
