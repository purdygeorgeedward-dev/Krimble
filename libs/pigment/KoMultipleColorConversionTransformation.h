/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KO_MULTIPLE_COLOR_CONVERSION_TRANSFORMATION_H_
#define _KO_MULTIPLE_COLOR_CONVERSION_TRANSFORMATION_H_

#include <KoColorConversionTransformation.h>

#include "kritapigment_export.h"

class KRITAPIGMENT_EXPORT KoMultipleColorConversionTransformation : public KoColorConversionTransformation
{
public:
    KoMultipleColorConversionTransformation(const KoColorSpace* srcCs, const KoColorSpace* dstCs, KoColorConversionTransformation::Intent, KoColorConversionTransformation::ConversionFlags conversionFlags);
    ~KoMultipleColorConversionTransformation() override;
    void appendTransfo(KoColorConversionTransformation* transfo);
    void transform(const quint8 *src, quint8 *dst, qint32 nPixels) const override;
private:
    struct Private;
    Private* const d;
};

#endif
