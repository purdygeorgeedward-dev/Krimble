/*
 *  SPDX-FileCopyrightText: 2006-2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KO_COLORSPACE_TRAITS_H_
#define _KO_COLORSPACE_TRAITS_H_

#include <QVector>
#include <type_traits>

#include "KoColorSpaceConstants.h"
#include "KoColorSpaceMaths.h"
#include "DebugPigment.h"
#include "kis_global.h"

const int MAX_CHANNELS_TYPE_SIZE = sizeof(double);
const int MAX_CHANNELS_NB = 5;
const int MAX_PIXEL_SIZE = MAX_CHANNELS_NB * MAX_CHANNELS_TYPE_SIZE;

template<typename _channels_type_, int _channels_nb_, int _alpha_pos_>
struct KoColorSpaceTrait {

    static_assert(sizeof(_channels_type_) <= MAX_CHANNELS_TYPE_SIZE, "MAX_CHANNELS_TYPE_SIZE too small");
    static_assert(_channels_nb_ <= MAX_CHANNELS_NB, "MAX_CHANNELS_NB too small");

    typedef _channels_type_ channels_type;

    static const quint32 channels_nb = _channels_nb_;

    static const qint32 alpha_pos = _alpha_pos_;

    static const int depth = KoColorSpaceMathsTraits<_channels_type_>::bits;

    typedef KoColorSpaceMathsTraits<_channels_type_> math_trait;

    static const quint32 pixelSize = channels_nb * sizeof(channels_type);

    inline static quint8 opacityU8(const quint8 * U8_pixel) {
        if (alpha_pos < 0) return OPACITY_OPAQUE_U8;
        channels_type c = nativeArray(U8_pixel)[alpha_pos];
        return  KoColorSpaceMaths<channels_type, quint8>::scaleToA(c);
    }

    inline static qreal opacityF(const quint8 * U8_pixel) {
        if (alpha_pos < 0) return OPACITY_OPAQUE_F;
        channels_type c = nativeArray(U8_pixel)[alpha_pos];
        return  KoColorSpaceMaths<channels_type, qreal>::scaleToA(c);
    }

    inline static void setOpacity(quint8 * pixels, quint8 alpha, qint32 nPixels) {
        if (alpha_pos < 0) return;
        qint32 psize = pixelSize;
        channels_type valpha =  KoColorSpaceMaths<quint8, channels_type>::scaleToA(alpha);
        for (; nPixels > 0; --nPixels, pixels += psize) {
            nativeArray(pixels)[alpha_pos] = valpha;
        }
    }

    inline static void setOpacity(quint8 * pixels, qreal alpha, qint32 nPixels) {
        if (alpha_pos < 0) return;
        qint32 psize = pixelSize;
        channels_type valpha =  KoColorSpaceMaths<qreal, channels_type>::scaleToA(alpha);
        for (; nPixels > 0; --nPixels, pixels += psize) {
            nativeArray(pixels)[alpha_pos] = valpha;
        }
    }

    inline static void copyOpacityU8(quint8* src, quint8* dst, qint32 nPixels) {
        if (alpha_pos < 0) return;
        qint32 psize = pixelSize;
        for (; nPixels > 0; --nPixels, src += psize, dst++) {
            channels_type c = nativeArray(src)[alpha_pos];
            *dst = KoColorSpaceMaths<channels_type, quint8>::scaleToA(c);
        }
    }

    inline static const channels_type* nativeArray(const quint8 * a) {
        return reinterpret_cast<const channels_type*>(a);
    }

    inline static channels_type* nativeArray(quint8 * a) {
        return reinterpret_cast< channels_type*>(a);
    }

    inline static quint8* allocate(quint32 nPixels) {
        return new quint8[ nPixels * pixelSize ];
    }

    inline static void singleChannelPixel(quint8 *dstPixel, const quint8 *srcPixel, quint32 channelIndex) {
        const channels_type* src = nativeArray(srcPixel);
        channels_type* dst = nativeArray(dstPixel);
        for (uint i = 0; i < channels_nb; i++) {
            if (i != channelIndex) {
                dst[i] = 0;
            } else {
                dst[i] = src[i];
            }
        }
    }

    inline static QString channelValueText(const quint8 *pixel, quint32 channelIndex) {
        if (channelIndex > channels_nb) return QString("Error");
        channels_type c = nativeArray(pixel)[channelIndex];
        return QString().setNum(c);
    }

    inline static QString normalisedChannelValueText(const quint8 *pixel, quint32 channelIndex) {
        if (channelIndex > channels_nb) return QString("Error");
        channels_type c = nativeArray(pixel)[channelIndex];
        return QString().setNum(100. *((qreal)c) / KoColorSpaceMathsTraits< channels_type>::unitValue);
    }

    inline static void normalisedChannelsValue(const quint8 *pixel, QVector<float> &v)
    {
        return normalisedChannelsValueImpl<channels_type>(pixel, v);
    }

    template<typename I, typename std::enable_if_t<std::numeric_limits<I>::is_integer, int> = 1>
    inline static void normalisedChannelsValueImpl(const quint8 *pixel, QVector<float> &v)
    {
        Q_ASSERT((int)v.count() >= (int)channels_nb);
        channels_type c;
        float *channels = v.data();
        for (uint i = 0; i < channels_nb; i++) {
            c = nativeArray(pixel)[i];
            channels[i] = ((float)c) / KoColorSpaceMathsTraits<channels_type>::unitValue;
        }
    }

    template<typename I, typename std::enable_if_t<!std::numeric_limits<I>::is_integer, int> = 1>
    inline static void normalisedChannelsValueImpl(const quint8 *pixel, QVector<float> &v)
    {
        Q_ASSERT((int)v.count() >= (int)channels_nb);
        float *channels = v.data();
        for (uint i = 0; i < channels_nb; i++) {
            channels[i] = float(nativeArray(pixel)[i]);
        }
    }

    inline static void fromNormalisedChannelsValue(quint8 *pixel, const QVector<float> &values)
    {
        return fromNormalisedChannelsValueImpl<channels_type>(pixel, values);
    }

    template<typename I, typename std::enable_if_t<std::numeric_limits<I>::is_integer, int> = 1>
    inline static void fromNormalisedChannelsValueImpl(quint8 *pixel, const QVector<float> &values)
    {
        Q_ASSERT((int)values.count() >= (int)channels_nb);
        channels_type c;
        const float *v = values.data();
        for (uint i = 0; i < channels_nb; i++) {
            float b = qBound(
                (float)KoColorSpaceMathsTraits<channels_type>::min,
                (float)KoColorSpaceMathsTraits<channels_type>::unitValue * v[i],
                (float)KoColorSpaceMathsTraits<channels_type>::max);
            c = (channels_type)b;
            nativeArray(pixel)[i] = c;
        }
    }

    template<typename I, typename std::enable_if_t<!std::numeric_limits<I>::is_integer, int> = 1>
    inline static void fromNormalisedChannelsValueImpl(quint8 *pixel, const QVector<float> &values)
    {
        Q_ASSERT((int)values.count() >= (int)channels_nb);
        const float *v = values.data();
        for (uint i = 0; i < channels_nb; i++) {
            nativeArray(pixel)[i] = channels_type(float(KoColorSpaceMathsTraits<channels_type>::unitValue) * v[i]);
        }
    }

    inline static void multiplyAlpha(quint8 * pixels, quint8 alpha, qint32 nPixels) {
        if (alpha_pos < 0) return;

        channels_type valpha =  KoColorSpaceMaths<quint8, channels_type>::scaleToA(alpha);

        for (; nPixels > 0; --nPixels, pixels += pixelSize) {
            channels_type* alphapixel = nativeArray(pixels) + alpha_pos;
            *alphapixel = KoColorSpaceMaths<channels_type>::multiply(*alphapixel, valpha);
        }
    }

    inline static void applyAlphaU8Mask(quint8 * pixels, const quint8 * alpha, qint32 nPixels) {
        if (alpha_pos < 0) return;

        for (; nPixels > 0; --nPixels, pixels += pixelSize, ++alpha) {
            channels_type valpha =  KoColorSpaceMaths<quint8, channels_type>::scaleToA(*alpha);
            channels_type* alphapixel = nativeArray(pixels) + alpha_pos;
            *alphapixel = KoColorSpaceMaths<channels_type>::multiply(*alphapixel, valpha);
        }
    }

    inline static void applyInverseAlphaU8Mask(quint8 * pixels, const quint8 * alpha, qint32 nPixels) {
        if (alpha_pos < 0) return;

        for (; nPixels > 0; --nPixels, pixels += pixelSize, ++alpha) {
            channels_type valpha =  KoColorSpaceMaths<quint8, channels_type>::scaleToA(OPACITY_OPAQUE_U8 - *alpha);
            channels_type* alphapixel = nativeArray(pixels) + alpha_pos;
            *alphapixel = KoColorSpaceMaths<channels_type>::multiply(*alphapixel, valpha);
        }
    }

    inline static void applyAlphaNormedFloatMask(quint8 * pixels, const float * alpha, qint32 nPixels) {
        if (alpha_pos < 0) return;

        for (; nPixels > 0; --nPixels, pixels += pixelSize, ++alpha) {
            channels_type valpha =  channels_type(KoColorSpaceMathsTraits<channels_type>::unitValue * (*alpha));
            channels_type* alphapixel = nativeArray(pixels) + alpha_pos;
            *alphapixel = KoColorSpaceMaths<channels_type>::multiply(*alphapixel, valpha);
        }
    }

    inline static void applyInverseAlphaNormedFloatMask(quint8 * pixels, const float * alpha, qint32 nPixels) {
        if (alpha_pos < 0) return;

        for (; nPixels > 0; --nPixels, pixels += pixelSize, ++alpha) {
            channels_type valpha =  channels_type(KoColorSpaceMathsTraits<channels_type>::unitValue * (1.0f - *alpha));
            channels_type* alphapixel = nativeArray(pixels) + alpha_pos;
            *alphapixel = KoColorSpaceMaths<channels_type>::multiply(*alphapixel, valpha);
        }
    }

    inline static void fillInverseAlphaNormedFloatMaskWithColor(quint8 * pixels, const float * alpha, const quint8 *brushColor, qint32 nPixels) {
        if (alpha_pos < 0) return;

        for (; nPixels > 0; --nPixels, pixels += pixelSize, ++alpha) {
            memcpy(pixels, brushColor, pixelSize);
            channels_type valpha =  channels_type(KoColorSpaceMathsTraits<channels_type>::unitValue * (1.0f - *alpha));
            *(nativeArray(pixels) + alpha_pos) = valpha;
        }
    }

    inline static void fillGrayBrushWithColor(quint8 *pixels, const QRgb *brush, quint8 *brushColor, qint32 nPixels) {
        if (alpha_pos >= 0) {
            for (; nPixels > 0; --nPixels, pixels += pixelSize, ++brush) {
                memcpy(pixels, brushColor, pixelSize);
                const quint8 opacity = KoColorSpaceMaths<quint8>::multiply(OPACITY_OPAQUE_U8 - quint8(qRed(*brush)), quint8(qAlpha(*brush)));
                *(nativeArray(pixels) + alpha_pos) = KoColorSpaceMaths<quint8, channels_type>::scaleToA(opacity);
            }
        } else {
            for (; nPixels > 0; --nPixels, pixels += pixelSize) {
                memcpy(pixels, brushColor, pixelSize);
            }
        }
    }
};

#include "KoRgbColorSpaceTraits.h"
#include "KoBgrColorSpaceTraits.h"
#include "KoGrayColorSpaceTraits.h"
#include "KoLabColorSpaceTraits.h"
#include "KoXyzColorSpaceTraits.h"
#include "KoYcbcrColorSpaceTraits.h"
#include "KoCmykColorSpaceTraits.h"

#endif
