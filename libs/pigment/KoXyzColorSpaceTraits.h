/*
 *  SPDX-FileCopyrightText: 2006-2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KO_XYZ_COLORSPACE_TRAITS_H_
#define _KO_XYZ_COLORSPACE_TRAITS_H_

template<typename _channels_type_>
struct KoXyzTraits : public KoColorSpaceTrait<_channels_type_, 4, 3> {

    typedef _channels_type_ channels_type;
    typedef KoColorSpaceTrait<_channels_type_, 4, 3> parent;

    static const qint32 x_pos = 0;
    static const qint32 y_pos = 1;
    static const qint32 z_pos = 2;

    struct Pixel {
        channels_type x;
        channels_type y;
        channels_type z;
        channels_type alpha;
    };

    inline static channels_type x(quint8* data) {
        channels_type* d = parent::nativeArray(data);
        return d[x_pos];
    }
    inline static void setX(quint8* data, channels_type nv) {
        channels_type* d = parent::nativeArray(data);
        d[x_pos] = nv;
    }
    inline static channels_type y(quint8* data) {
        channels_type* d = parent::nativeArray(data);
        return d[y_pos];
    }
    inline static void setY(quint8* data, channels_type nv) {
        channels_type* d = parent::nativeArray(data);
        d[y_pos] = nv;
    }
    inline static channels_type z(quint8* data) {
        channels_type* d = parent::nativeArray(data);
        return d[z_pos];
    }
    inline static void setZ(quint8* data, channels_type nv) {
        channels_type* d = parent::nativeArray(data);
        d[z_pos] = nv;
    }
};

struct KoXyzU8Traits : public KoXyzTraits<quint8> {
};

struct KoXyzU16Traits : public KoXyzTraits<quint16> {
};

#include <KoConfig.h>
#ifdef HAVE_OPENEXR
#include <half.h>

struct KoXyzF16Traits : public KoXyzTraits<half> {
};

#endif

struct KoXyzF32Traits : public KoXyzTraits<float> {
};

struct KoXyzF64Traits : public KoXyzTraits<double> {
};

#endif
