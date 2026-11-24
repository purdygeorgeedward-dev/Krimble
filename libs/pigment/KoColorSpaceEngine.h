/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KO_COLOR_SPACE_ENGINE_H_
#define _KO_COLOR_SPACE_ENGINE_H_

#include <KoColorConversionTransformationAbstractFactory.h>
#include <KoGenericRegistry.h>
#include <KoColorProfileConstants.h>

class KoColorProfile;
struct KoColorProfileQuery;

class KRITAPIGMENT_EXPORT KoColorSpaceEngine : public KoColorConversionTransformationAbstractFactory
{
public:
    KoColorSpaceEngine(const QString& id, const QString& name);
    ~KoColorSpaceEngine() override;
    const QString& id() const;
    const QString& name() const;
    virtual const KoColorProfile* addProfile(const QString &filename) = 0;
    virtual const KoColorProfile* addProfile(const QByteArray &data) = 0;
    virtual const KoColorProfile* getProfile(const KoColorProfileQuery &query) = 0;
    virtual void removeProfile(const QString &filename) = 0;

    virtual bool supportsColorSpace(const QString& colorModelId, const QString& colorDepthId, const KoColorProfile *profile) const;

private:
    struct Private;
    Private* const d;
};

class KRITAPIGMENT_EXPORT KoColorSpaceEngineRegistry : public KoGenericRegistry< KoColorSpaceEngine* >
{
public:
    KoColorSpaceEngineRegistry();
    ~KoColorSpaceEngineRegistry() override;
    static KoColorSpaceEngineRegistry* instance();
};

#endif
