/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_FILTER_CONFIGURATION_H_
#define _KIS_FILTER_CONFIGURATION_H_

#include <QMetaType>

#include "kis_properties_configuration.h"

#include "kis_types.h"
#include "kritaimage_export.h"

class KoResource;
typedef QSharedPointer<KoResource> KoResourceSP;

class KisResourcesInterface;
typedef QSharedPointer<KisResourcesInterface> KisResourcesInterfaceSP;

class KoResourceLoadResult;

#define SANITY_CHECK_FILTER_CONFIGURATION_OWNER

class KRITAIMAGE_EXPORT KisFilterConfiguration : public KisPropertiesConfiguration
{
public:

    KisFilterConfiguration(const QString & name, qint32 version, KisResourcesInterfaceSP resourcesInterface);

    virtual KisFilterConfigurationSP clone() const;

protected:
    KisFilterConfiguration(const KisFilterConfiguration & rhs);
public:
    ~KisFilterConfiguration() override;

public:

    virtual void fromLegacyXML(const QDomElement&);

    using KisPropertiesConfiguration::fromXML;
    using KisPropertiesConfiguration::toXML;

    void fromXML(const QDomElement&) override;
    void toXML(QDomDocument&, QDomElement&) const override;

    const QString & name() const;

    qint32 version() const;

    virtual bool isCompatible(const KisPaintDeviceSP) const;

    QBitArray channelFlags() const;

    void setChannelFlags(QBitArray channelFlags);

    KisResourcesInterfaceSP resourcesInterface() const;

    virtual void setResourcesInterface(KisResourcesInterfaceSP resourcesInterface);

    void createLocalResourcesSnapshot(KisResourcesInterfaceSP globalResourcesInterface = nullptr);

    bool hasLocalResourcesSnapshot() const;

    KisFilterConfigurationSP cloneWithResourcesSnapshot(KisResourcesInterfaceSP globalResourcesInterface = nullptr) const;

    QList<KoResourceLoadResult> requiredResources(KisResourcesInterfaceSP globalResourcesInterface) const;

    virtual QList<KoResourceLoadResult> linkedResources(KisResourcesInterfaceSP globalResourcesInterface) const;

    virtual QList<KoResourceLoadResult> embeddedResources(KisResourcesInterfaceSP globalResourcesInterface) const;

    virtual bool compareTo(const KisPropertiesConfiguration *rhs) const override;

#ifdef SANITY_CHECK_FILTER_CONFIGURATION_OWNER
private:
    friend class KisNodeFilterInterface;
    int sanityRefUsageCounter();
    int sanityDerefUsageCounter();

#endif

protected:
    void setVersion(qint32 version);
private:
    struct Private;
    Private* const d;
};

Q_DECLARE_METATYPE(KisFilterConfiguration*)

#endif
