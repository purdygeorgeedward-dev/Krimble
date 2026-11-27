/*
 *  SPDX-FileCopyrightText: 2006, 2007, 2010 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2017, 2020 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KOCMYKCOLORSPACEMATHS_H_
#define KOCMYKCOLORSPACEMATHS_H_

#include <cmath>
#include <limits>

#include "kritapigment_export.h"
#include <KoIntegerMaths.h>
#include "KoChannelInfo.h"
#include "KoLut.h"

#include <KoColorSpaceMaths.h>

#undef _T

template<typename _T>
class KoCmykColorSpaceMathsTraits
{
public:
};

template<>
class KRITAPIGMENT_EXPORT KoCmykColorSpaceMathsTraits<quint8> : public KoColorSpaceMathsTraits<quint8>
{
};

template<>
class KRITAPIGMENT_EXPORT KoCmykColorSpaceMathsTraits<quint16> : public KoColorSpaceMathsTraits<quint16>
{
};

template<>
class KRITAPIGMENT_EXPORT KoCmykColorSpaceMathsTraits<qint16> : public KoColorSpaceMathsTraits<qint16>
{
};

template<>
class KRITAPIGMENT_EXPORT KoCmykColorSpaceMathsTraits<quint32> : public KoColorSpaceMathsTraits<quint32>
{
};

#include <KoConfig.h>
#ifdef HAVE_OPENEXR
#include <half.h>

template<>
class KRITAPIGMENT_EXPORT KoCmykColorSpaceMathsTraits<half> : public KoColorSpaceMathsTraits<half>
{
public:
    static const half zeroValueCMYK;
    static const half unitValueCMYK;
    static const half halfValueCMYK;
};
#endif

template<>
class KRITAPIGMENT_EXPORT KoCmykColorSpaceMathsTraits<float> : public KoColorSpaceMathsTraits<float>
{
public:
    static const float zeroValueCMYK;
    static const float unitValueCMYK;
    static const float halfValueCMYK;
};

template<>
class KRITAPIGMENT_EXPORT KoCmykColorSpaceMathsTraits<double> : public KoColorSpaceMathsTraits<double>
{
public:
    static const double zeroValueCMYK;
    static const double unitValueCMYK;
    static const double halfValueCMYK;
};

#endif
