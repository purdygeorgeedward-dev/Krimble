/*
 *  SPDX-FileCopyrightText: 2024 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOCOMPOSITEOPGENERICFUNCTORBASE_H
#define KOCOMPOSITEOPGENERICFUNCTORBASE_H

#include <KoColorSpaceMaths.h>

template <typename T>
struct KoCompositeOpGenericFunctorBase
{
    static inline T clampSourceChannelValue(T value) {
        return value;
    }

    static inline T clampDestinationChannelValue(T value) {
        return value;
    }
};

template <typename T>
struct KoClampedSourceCompositeOpGenericFunctorBase
{
    static inline T clampSourceChannelValue(T value) {
        using namespace Arithmetic;
        return clampChannelToSDR(value);
    }

    static inline T clampDestinationChannelValue(T value) {
        return value;
    }
};

template <typename T>
struct KoClampedSourceAndDestinationBottomCompositeOpGenericFunctorBase
{
    static inline T clampSourceChannelValue(T value) {
        using namespace Arithmetic;
        return clampChannelToSDRBottom(value);
    }

    static inline T clampDestinationChannelValue(T value) {
        using namespace Arithmetic;
        return clampChannelToSDRBottom(value);
    }
};

template <typename T>
struct KoClampedSourceAndDestinationCompositeOpGenericFunctorBase
{
    static inline T clampSourceChannelValue(T value) {
        using namespace Arithmetic;
        return clampChannelToSDR(value);
    }

    static inline T clampDestinationChannelValue(T value) {
        using namespace Arithmetic;
        return clampChannelToSDR(value);
    }
};

template <typename T>
struct KoClampedSourceFullAndDestinationBottomCompositeOpGenericFunctorBase
{
    static inline T clampSourceChannelValue(T value) {
        using namespace Arithmetic;
        return clampChannelToSDR(value);
    }

    static inline T clampDestinationChannelValue(T value) {
        using namespace Arithmetic;
        return clampChannelToSDRBottom(value);
    }
};

#endif
