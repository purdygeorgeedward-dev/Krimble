/*
 *  SPDX-FileCopyrightText: 2006, 2007, 2010 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2017, 2020 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KOLABCOLORSPACEMATHS_H_
#define KOLABCOLORSPACEMATHS_H_

#include <cmath>
#include <limits>

#include "kritapigment_export.h"
#include <KoIntegerMaths.h>
#include "KoChannelInfo.h"
#include "KoLut.h"

#include <KoColorSpaceMaths.h>

#undef _T

template<typename _T>
class KoLabColorSpaceMathsTraits
{
public:
};

template<>
class KRITAPIGMENT_EXPORT KoLabColorSpaceMathsTraits<quint8> : public KoColorSpaceMathsTraits<quint8>
{
public:
    static const quint8 zeroValueL = 0;
    static const quint8 unitValueL = 0x00FF;
    static const quint8 halfValueL = 0x00FF / 2;
    static const quint8 zeroValueAB = 0;
    static const quint8 unitValueAB = 0x00FF;
    static const quint8 halfValueAB = 0x0080;
};

template<>
class KRITAPIGMENT_EXPORT KoLabColorSpaceMathsTraits<quint16> : public KoColorSpaceMathsTraits<quint16>
{
public:
    static const quint16 zeroValueL = 0;
    static const quint16 unitValueL = 0xFFFF;
    static const quint16 halfValueL = 0xFFFF / 2;
    static const quint16 zeroValueAB = 0;
    static const quint16 unitValueAB = 0xFFFF;
    static const quint16 halfValueAB = 0x8080;
};

template<>
class KRITAPIGMENT_EXPORT KoLabColorSpaceMathsTraits<qint16> : public KoColorSpaceMathsTraits<qint16>
{
public:
    static const qint16 zeroValueL = 0;
    static const qint16 unitValueL = 32767;
    static const qint16 halfValueL = 32767 / 2;
    static const qint16 zeroValueAB = 0;
    static const qint16 unitValueAB = 32767;
    static const qint16 halfValueAB = 19549;
};

template<>
class KRITAPIGMENT_EXPORT KoLabColorSpaceMathsTraits<quint32> : public KoColorSpaceMathsTraits<quint32>
{
public:
    static const quint32 zeroValueL = 0;
    static const quint32 unitValueL = 0xFFFFFFFF;
    static const quint32 halfValueL = 0xFFFFFFFF / 2;
    static const quint32 zeroValueAB = 0;
    static const quint32 unitValueAB = 0xFFFFFFFF;
    static const quint32 halfValueAB = 0x80808080;
};

#include <KoConfig.h>
#ifdef HAVE_OPENEXR
#include <half.h>

template<>
class KRITAPIGMENT_EXPORT KoLabColorSpaceMathsTraits<half> : public KoColorSpaceMathsTraits<half>
{
public:
    static const half zeroValueL;
    static const half unitValueL;
    static const half halfValueL;
    static const half zeroValueAB;
    static const half unitValueAB;
    static const half halfValueAB;
};
#endif

template<>
class KRITAPIGMENT_EXPORT KoLabColorSpaceMathsTraits<float> : public KoColorSpaceMathsTraits<float>
{
public:
    static const float zeroValueL;
    static const float unitValueL;
    static const float halfValueL;
    static const float zeroValueAB;
    static const float unitValueAB;
    static const float halfValueAB;
};

template<>
class KRITAPIGMENT_EXPORT KoLabColorSpaceMathsTraits<double> : public KoColorSpaceMathsTraits<double>
{
public:
    static const double zeroValueL;
    static const double unitValueL;
    static const double halfValueL;
    static const double zeroValueAB;
    static const double unitValueAB;
    static const double halfValueAB;
};

#endif
