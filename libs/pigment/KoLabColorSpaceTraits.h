/*
 *  SPDX-FileCopyrightText: 2006-2007 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2016, 2017, 2020 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KO_LAB_COLORSPACE_TRAITS_H_
#define _KO_LAB_COLORSPACE_TRAITS_H_

#include <KoLabColorSpaceMaths.h>

template<typename _channels_type_>
struct KoLabTraits : public KoColorSpaceTrait<_channels_type_, 4, 3> {
    typedef _channels_type_ channels_type;
    typedef KoColorSpaceTrait<_channels_type_, 4, 3> parent;
    typedef KoLabColorSpaceMathsTraits<channels_type> math_trait;
    static const qint32 L_pos = 0;
    static const qint32 a_pos = 1;
    static const qint32 b_pos = 2;

    struct Pixel {
        channels_type L;
        channels_type a;
        channels_type b;
        channels_type alpha;
    };

    inline static channels_type L(quint8* data) {
        channels_type* d = parent::nativeArray(data);
        return d[L_pos];
    }
    inline static void setL(quint8* data, channels_type nv) {
        channels_type* d = parent::nativeArray(data);
        d[L_pos] = nv;
    }
    inline static channels_type a(quint8* data) {
        channels_type* d = parent::nativeArray(data);
        return d[a_pos];
    }
    inline static void setA(quint8* data, channels_type nv) {
        channels_type* d = parent::nativeArray(data);
        d[a_pos] = nv;
    }
    inline static channels_type b(quint8* data) {
        channels_type* d = parent::nativeArray(data);
        return d[b_pos];
    }
    inline static void setB(quint8* data, channels_type nv) {
        channels_type* d = parent::nativeArray(data);
        d[b_pos] = nv;
    }

    inline static QString normalisedChannelValueText(const quint8 *pixel, quint32 channelIndex)
    {
        if (channelIndex > parent::channels_nb)
            return QString("Error");
        channels_type c = parent::nativeArray(pixel)[channelIndex];
        switch (channelIndex) {
        case L_pos:
            return QString().setNum(100.0 * qBound((qreal)0, ((qreal)c) / math_trait::unitValueL, (qreal)math_trait::unitValueL));
        case a_pos:
        case b_pos:
            if (c <= math_trait::halfValueAB) {
                return QString().setNum(100.0 * (qreal)((c - math_trait::zeroValueAB) / (2.0 * (math_trait::halfValueAB - math_trait::zeroValueAB))));
            } else {
                return QString().setNum(100.0 * (qreal)(0.5 + (c - math_trait::halfValueAB) / (2.0 * (math_trait::unitValueAB - math_trait::halfValueAB))));
            }
        case 3:
            return QString().setNum(100.0 * qBound((qreal)0, ((qreal)c) / math_trait::unitValue, (qreal)math_trait::unitValue));
        default:
            return QString("Error");
        }
    }
    inline static void normalisedChannelsValue(const quint8 *pixel, QVector<float> &v)
    {
        Q_ASSERT((int)v.count() >= (int)parent::channels_nb);
        channels_type c;
        float *channels = v.data();
        for (uint i = 0; i < parent::channels_nb; i++) {
            c = parent::nativeArray(pixel)[i];
            switch (i) {
            case L_pos:
                channels[i] = (qreal)c / math_trait::unitValueL;
                break;
            case a_pos:
            case b_pos:
                if (c <= math_trait::halfValueAB) {
                    channels[i] = ((qreal)c - math_trait::zeroValueAB) / (2.0 * (math_trait::halfValueAB - math_trait::zeroValueAB));
                } else {
                    channels[i] = 0.5 + ((qreal)c - math_trait::halfValueAB) / (2.0 * (math_trait::unitValueAB - math_trait::halfValueAB));
                }
                break;
            case 3:
            default:
                channels[i] = (qreal)c / math_trait::unitValue;
                break;
            }
        }
    }
    inline static void fromNormalisedChannelsValue(quint8 *pixel, const QVector<float> &values)
    {
        Q_ASSERT((int)values.count() >= (int)parent::channels_nb);
        channels_type c;
        for (uint i = 0; i < parent::channels_nb; i++) {
            float b = 0;
            switch (i) {
            case L_pos:
                b = qBound((float)math_trait::zeroValueL,
                           (float)math_trait::unitValueL * values[i],
                           (float)math_trait::unitValueL);
                break;
            case a_pos:
            case b_pos:
                if (values[i] <= 0.5) {
                    b = qBound((float)math_trait::zeroValueAB,
                               (float)(math_trait::zeroValueAB + 2.0 * values[i] * (math_trait::halfValueAB - math_trait::zeroValueAB)),
                               (float)math_trait::halfValueAB);
                }
                else {
                    b = qBound((float)math_trait::halfValueAB,
                               (float)(math_trait::halfValueAB + 2.0 * (values[i] - 0.5) * (math_trait::unitValueAB - math_trait::halfValueAB)),
                               (float)math_trait::unitValueAB);
                }
                break;
            case 3:
                b = qBound((float)math_trait::min,
                           (float)math_trait::unitValue * values[i],
                           (float)math_trait::unitValue);
            default:
                break;
            }
            c = (channels_type)b;
            parent::nativeArray(pixel)[i] = c;
        }
    }
};

struct KoLabU8Traits : public KoLabTraits<quint8> {

};

struct KoLabU16Traits : public KoLabTraits<quint16> {

};

#include <KoConfig.h>
#ifdef HAVE_OPENEXR
#include <half.h>

struct KoLabF16Traits : public KoLabTraits<half> {

};

#endif

struct KoLabF32Traits : public KoLabTraits<float> {

};

struct KoLabF64Traits : public KoLabTraits<double> {

};

#endif
