/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KO_COLOR_CONVERSION_TRANSFORMATION_H_
#define _KO_COLOR_CONVERSION_TRANSFORMATION_H_

#include "kritapigment_export.h"
#include "KoColorTransformation.h"
#include <QMetaType>

class KoColorSpace;
class KoColorConversionCache;

class KRITAPIGMENT_EXPORT KoColorConversionTransformation : public KoColorTransformation
{
    friend class KoColorConversionCache;
    struct Private;
public:

    enum Intent {
        IntentPerceptual = 0,
        IntentRelativeColorimetric = 1,
        IntentSaturation = 2,
        IntentAbsoluteColorimetric = 3
    };

    enum ConversionFlag {
        Empty                   = 0x0,
        NoOptimization          = 0x0100,
        GamutCheck              = 0x1000,
        SoftProofing            = 0x4000,
        BlackpointCompensation  = 0x2000,
        NoWhiteOnWhiteFixup     = 0x0004,
        HighQuality             = 0x0400,
        LowQuality              = 0x0800,
        CopyAlpha               = 0x04000000,
        NoAdaptationAbsoluteIntent = 01000000

    };
    Q_DECLARE_FLAGS(ConversionFlags, ConversionFlag)

    static Intent internalRenderingIntent() { return IntentPerceptual; }
    static ConversionFlags internalConversionFlags() { return BlackpointCompensation; }

    static Intent adjustmentRenderingIntent() { return IntentPerceptual; }
    static ConversionFlags adjustmentConversionFlags() { return ConversionFlags(BlackpointCompensation | NoWhiteOnWhiteFixup); }

public:
    KoColorConversionTransformation(const KoColorSpace* srcCs,
                                    const KoColorSpace* dstCs,
                                    Intent renderingIntent,
                                    ConversionFlags conversionFlags);
    ~KoColorConversionTransformation() override;
public:

    const KoColorSpace* srcColorSpace() const;

    const KoColorSpace* dstColorSpace() const;

    Intent renderingIntent() const;

    ConversionFlags conversionFlags() const;

    void transform(const quint8 *src, quint8 *dst, qint32 nPixels) const override = 0;

    void transformInPlace(const quint8 *src, quint8 *dst, qint32 nPixels) const;

    bool isValid() const override { return true; }

private:

    void setSrcColorSpace(const KoColorSpace*) const;
    void setDstColorSpace(const KoColorSpace*) const;

    Private * const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KoColorConversionTransformation::ConversionFlags);
Q_DECLARE_METATYPE(KoColorConversionTransformation::Intent);

#endif
