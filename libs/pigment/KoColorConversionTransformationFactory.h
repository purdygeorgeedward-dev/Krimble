/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KO_COLOR_CONVERSION_TRANSFORMATION_FACTORY_H_
#define _KO_COLOR_CONVERSION_TRANSFORMATION_FACTORY_H_

class KoColorSpace;
class KoColorConversionTransformation;

#include <QString>
#include <KoColorConversionTransformationAbstractFactory.h>

class KRITAPIGMENT_EXPORT KoColorConversionTransformationFactory : public KoColorConversionTransformationAbstractFactory
{
public:
    KoColorConversionTransformationFactory(const QString &_srcModelId, const QString &_srcDepthId, const QString &_srcProfile, const QString &_dstModelId, const QString &_dstDepthId, const QString &_dstProfile);
    ~KoColorConversionTransformationFactory() override;

public:
    QString srcColorModelId() const;
    QString srcColorDepthId() const;
    QString srcProfile() const;
    QString dstColorModelId() const;
    QString dstColorDepthId() const;
    QString dstProfile() const;
protected:
    bool canBeSource(const KoColorSpace* srcCS) const;
    bool canBeDestination(const KoColorSpace* dstCS) const;
private:
    struct Private;
    Private* const d;
};

#endif
