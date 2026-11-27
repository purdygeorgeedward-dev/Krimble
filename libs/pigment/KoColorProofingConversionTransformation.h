/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KO_COLOR_PROOFING_CONVERSION_TRANSFORMATION_H_
#define _KO_COLOR_PROOFING_CONVERSION_TRANSFORMATION_H_

#include "KoColorConversionTransformation.h"

#include "kritapigment_export.h"

class KoColorSpace;
class KoColorConversionCache;

class KRITAPIGMENT_EXPORT KoColorProofingConversionTransformation : public KoColorConversionTransformation
{

public:
    KoColorProofingConversionTransformation(const KoColorSpace *srcCs,
                                            const KoColorSpace *dstCs,
                                            const KoColorSpace *proofingSpace,
                                            Intent renderingIntent,
                                            ConversionFlags conversionFlags);
    ~KoColorProofingConversionTransformation() override;

public:

    const KoColorSpace *proofingSpace() const;

private:
    const KoColorSpace *m_proofingSpace;
};

#endif
