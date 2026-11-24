/*
 * SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef _KO_COLOR_CONVERSION_CACHE_HPP_
#define _KO_COLOR_CONVERSION_CACHE_HPP_

class KoCachedColorConversionTransformation;
class KoColorSpace;

#include "KoColorConversionTransformation.h"

class KoColorConversionCache
{
public:
    struct CachedTransformation;
public:
    KoColorConversionCache();
    ~KoColorConversionCache();

    KoCachedColorConversionTransformation cachedConverter(const KoColorSpace* src,
                                                          const KoColorSpace* dst,
                                                          KoColorConversionTransformation::Intent _renderingIntent,
                                                          KoColorConversionTransformation::ConversionFlags conversionFlags);

    void colorSpaceIsDestroyed(const KoColorSpace* src);
private:
    struct Private;
    Private* const d;
};

class KoCachedColorConversionTransformation
{
    friend class KoColorConversionCache;
private:
    KoCachedColorConversionTransformation(KoColorConversionCache::CachedTransformation* transfo);
public:
    KoCachedColorConversionTransformation(const KoCachedColorConversionTransformation&);
    ~KoCachedColorConversionTransformation();
public:
    const KoColorConversionTransformation* transformation() const;
private:
    KoColorConversionCache::CachedTransformation* m_transfo;
};

#endif
