/*
 *  SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2006-2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOCOLORSPACEFACTORY_H
#define KOCOLORSPACEFACTORY_H

#include "KoColorSpaceConstants.h"
#include "KoColorConversionTransformation.h"
#include <KoID.h>
#include "kritapigment_export.h"

class KoColorProfile;
struct KoColorProfileQuery;
class KoColorConversionTransformationFactory;

class KRITAPIGMENT_EXPORT KoColorSpaceFactory
{
protected:
    KoColorSpaceFactory();
public:
    virtual ~KoColorSpaceFactory();
    virtual QString id() const = 0;

    virtual QString name() const = 0;

    virtual bool userVisible() const = 0;

    virtual KoID colorModelId() const = 0;

    virtual KoID colorDepthId() const = 0;

    virtual bool profileIsCompatible(const KoColorProfile* profile) const = 0;

    virtual QString colorSpaceEngine() const = 0;

    virtual bool isHdr() const = 0;

    virtual int referenceDepth() const = 0;

    virtual QList<KoColorConversionTransformationFactory*> colorConversionLinks() const = 0;

    virtual int crossingCost() const = 0;

    virtual QString defaultProfile() const = 0;

    virtual KoColorProfile* createColorProfile(const QByteArray& rawData) const = 0;

    const KoColorSpace *grabColorSpace(const KoColorProfile *profile);

    virtual QList<KoColorConversionTransformationFactory*> colorConversionLinksFromProfile(const KoColorProfile *profile) const {
        Q_UNUSED(profile)
        return QList<KoColorConversionTransformationFactory*>();
    }

    virtual QList<KoColorProfileQuery> requiredConnectionProfiles(const KoColorProfile *profile) const;

protected:
    virtual KoColorSpace *createColorSpace(const KoColorProfile *) const = 0;
private:
    struct Private;
    Private* const d;
};

#endif
