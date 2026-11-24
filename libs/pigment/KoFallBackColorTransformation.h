/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_FALL_BACK_COLOR_TRANSFORMATION_H_
#define _KO_FALL_BACK_COLOR_TRANSFORMATION_H_

#include "KoColorTransformation.h"

#include "kritapigment_export.h"

class KoColorSpace;
class KoColorConversionTransformation;

class KRITAPIGMENT_EXPORT KoFallBackColorTransformation : public KoColorTransformation
{
public:
    KoFallBackColorTransformation(const KoColorSpace* _cs, const KoColorSpace* _fallBackCS, KoColorTransformation* _transfo);
    KoFallBackColorTransformation(KoColorConversionTransformation* _csToFallBack, KoColorConversionTransformation* _fallBackToCs, KoColorTransformation* _transfo);
    ~KoFallBackColorTransformation() override;
    void transform(const quint8 *src, quint8 *dst, qint32 nPixels) const override;
    QList<QString> parameters() const override;
    int parameterId(const QString& name) const override;
    void setParameter(int id, const QVariant& parameter) override;
private:
    struct Private;
    Private* const d;
};

#endif
