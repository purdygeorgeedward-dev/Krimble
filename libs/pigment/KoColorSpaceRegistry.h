/*
 *  SPDX-FileCopyrightText: 2003 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2004, 2010 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KOCOLORSPACEREGISTRY_H
#define KOCOLORSPACEREGISTRY_H

#include <QObject>
#include <QList>
#include <QString>
#include "kritapigment_export.h"

#include <KoGenericRegistry.h>
#include <KoColorSpace.h>
#include <KoColorSpaceFactory.h>
#include <KoConfig.h>
#include <KoColorProfileConstants.h>

class KoColorProfile;
class KoColorConversionSystem;
class KoColorConversionCache;
class KoColorConversionTransformation;
class QColorSpace;
struct KoColorProfileQuery;

class KRITAPIGMENT_EXPORT KoColorSpaceRegistry
{
public:
    KoColorSpaceRegistry();

    enum ColorSpaceListVisibility {
        OnlyUserVisible = 1,
        AllColorSpaces = 4
    };
    enum ColorSpaceListProfilesSelection {
        OnlyDefaultProfile = 1,
        AllProfiles = 4
    };

    static KoColorSpaceRegistry * instance();

    virtual ~KoColorSpaceRegistry();

public:
    void add(KoColorSpaceFactory* item);

    void remove(KoColorSpaceFactory* item);

    void addProfileToMap(KoColorProfile *p);

    void addProfile(KoColorProfile* profile);
    void addProfile(const KoColorProfile* profile);
    void removeProfile(KoColorProfile* profile);

    void addProfileAlias(const QString& name, const QString& to);

    void removeProfileAlias(const QString& name);

    QString profileAlias(const QString& name) const;

    const KoColorProfile *createColorProfile(const QString & colorModelId, const QString & colorDepthId, const QByteArray& rawData);

    using CustomProfileNameAlias = QHash<QString, QString>;
    const KoColorProfile *createColorProfile(const QString & colorModelId, const QString & colorDepthId, const QByteArray& rawData, CustomProfileNameAlias customProfileNameAlias);

    const KoColorProfile * profileByName(const QString & name) const ;

    const KoColorProfile *profileByUniqueId(const QByteArray &id) const;

    bool profileIsCompatible(const KoColorProfile* profile, const QString &colorSpaceId);

    QList<const KoColorProfile *>  profilesFor(const QString& csID) const;
    QString defaultProfileForColorSpace(const QString &colorSpaceId) const;

    KoColorConversionTransformation* createColorConverter(const KoColorSpace * srcColorSpace, const KoColorSpace * dstColorSpace, KoColorConversionTransformation::Intent renderingIntent, KoColorConversionTransformation::ConversionFlags conversionFlags) const;

    void createColorConverters(const KoColorSpace* colorSpace, const QList< QPair<KoID, KoID> >& possibilities, KoColorConversionTransformation*& fromCS, KoColorConversionTransformation*& toCS) const;

    const KoColorSpace * colorSpace(const QString & colorModelId, const QString & colorDepthId, const KoColorProfile *profile);

    const KoColorSpace * colorSpace(const QString & colorModelId, const QString & colorDepthId, const QString &profileName);

    const KoColorSpace * colorSpace(const QString & colorModelId, const QString & colorDepthId);

    QString colorSpaceId(const QString & colorModelId, const QString & colorDepthId) const;
    QString colorSpaceId(const KoID& colorModelId, const KoID& colorDepthId) const;

    KoID colorSpaceColorModelId(const QString & _colorSpaceId) const;

    KoID colorSpaceColorDepthId(const QString & _colorSpaceId) const;

    const KoColorSpace *alpha8();
    const KoColorSpace *alpha16();
#ifdef HAVE_OPENEXR
    const KoColorSpace *alpha16f();
#endif
    const KoColorSpace *alpha32f();

    const KoColorSpace *graya8(const QString &profile = QString());

    const KoColorSpace *graya8(const KoColorProfile *profile);

    const KoColorSpace *graya16(const QString &profile = QString());

    const KoColorSpace *graya16(const KoColorProfile *profile);

    const KoColorSpace * rgb8(const QString &profileName = QString());

    const KoColorSpace * rgb8(const KoColorProfile * profile);

    const KoColorSpace * rgb16(const QString &profileName = QString());

    const KoColorSpace * rgb16(const KoColorProfile * profile);

    const KoColorSpace * lab16(const QString &profileName = QString());

    const KoColorSpace * lab16(const KoColorProfile * profile);

    const KoColorProfile *p2020G10Profile() const;

    const KoColorProfile *p2020PQProfile() const;

    const KoColorProfile *p709G10Profile() const;

    const KoColorProfile *p709SRGBProfile() const;

    const KoColorProfile *profileFor(const KoColorProfileQuery &query, const bool generate = true) const;

    const KoColorProfile *profileForQColorSpace(const QColorSpace &space);

    QColorSpace QColorSpaceForProfile(const KoColorProfile *profile) const;
    QList<KoID> colorModelsList(ColorSpaceListVisibility option) const;

    QList<KoID> colorDepthList(const KoID& colorModelId, ColorSpaceListVisibility option) const;

    QList<KoID> colorDepthList(const QString & colorModelId, ColorSpaceListVisibility option) const;

    KoColorConversionCache* colorConversionCache() const;

    const KoColorSpace* permanentColorspace(const KoColorSpace* _colorSpace);

    QList<KoID> listKeys() const;

private:

    friend class KisCsConversionTest;
    friend class KisIteratorTest;
    friend class KisIteratorNGTest;
    friend class KisPainterTest;
    friend class KisCrashFilterTest;
    friend class KoColorSpacesBenchmark;
    friend class TestKoColorSpaceSanity;
    friend class TestColorConversionSystem;
    friend struct FriendOfColorSpaceRegistry;
    friend class TestProfileGeneration;

    QList<const KoColorSpace*> allColorSpaces(ColorSpaceListVisibility visibility, ColorSpaceListProfilesSelection pSelection);

    const KoColorConversionSystem* colorConversionSystem() const;

    const KoColorProfile *profileForInternal(const KoColorProfileQuery &query, const bool generate) const;

private:
    KoColorSpaceRegistry(const KoColorSpaceRegistry&);
    KoColorSpaceRegistry operator=(const KoColorSpaceRegistry&);
    void init();

private:
    struct Private;
    Private * const d;
};

#endif
