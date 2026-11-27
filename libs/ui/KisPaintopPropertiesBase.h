/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISBASEOPTION_H
#define KISBASEOPTION_H

#include <kis_types.h>
#include <kritaui_export.h>
#include "kis_pointer_utils.h"

class KoResource;
using KoResourceSP = QSharedPointer<KoResource>;

class KisResourcesInterface;
using KisResourcesInterfaceSP = QSharedPointer<KisResourcesInterface>;

class KoCanvasResourcesInterface;
using KoCanvasResourcesInterfaceSP = QSharedPointer<KoCanvasResourcesInterface>;

class KoResourceLoadResult;

class KRITAUI_EXPORT KisPaintopPropertiesCanvasResourcesBase
{
public:
    virtual ~KisPaintopPropertiesCanvasResourcesBase();

    template <typename KisPropertiesConfigurationPointer>
    void readOptionSetting(KisPropertiesConfigurationPointer settings, KisResourcesInterfaceSP resourcesInterface, KoCanvasResourcesInterfaceSP canvasResourcesInterface)
    {
        readOptionSettingResourceImpl(removeSharedPointer(settings), resourcesInterface, canvasResourcesInterface);
    }

    template <typename KisPropertiesConfigurationPointer>
    void writeOptionSetting(KisPropertiesConfigurationPointer settings) const
    {
        writeOptionSettingImpl(removeSharedPointer(settings));
    }

    template <typename KisPropertiesConfigurationPointer>
    QList<KoResourceLoadResult> prepareLinkedResources(const KisPropertiesConfigurationPointer settings, KisResourcesInterfaceSP resourcesInterface) const
    {
        return prepareLinkedResourcesImpl(removeSharedPointer(settings), resourcesInterface);
    }

    template <typename KisPropertiesConfigurationPointer>
    QList<KoResourceLoadResult> prepareEmbeddedResources(const KisPropertiesConfigurationPointer settings, KisResourcesInterfaceSP resourcesInterface) const
    {
        return prepareEmbeddedResourcesImpl(removeSharedPointer(settings), resourcesInterface);
    }

protected:
    virtual void readOptionSettingResourceImpl(const KisPropertiesConfiguration *settings, KisResourcesInterfaceSP resourcesInterface, KoCanvasResourcesInterfaceSP canvasResourcesInterface) = 0;
    virtual void writeOptionSettingImpl(KisPropertiesConfiguration *settings) const = 0;
    virtual QList<KoResourceLoadResult> prepareLinkedResourcesImpl(const KisPropertiesConfiguration *settings, KisResourcesInterfaceSP resourcesInterface) const = 0;
    virtual QList<KoResourceLoadResult> prepareEmbeddedResourcesImpl(const KisPropertiesConfiguration *settings, KisResourcesInterfaceSP resourcesInterface) const = 0;
};

class KRITAUI_EXPORT KisPaintopPropertiesResourcesBase
{
public:
    virtual ~KisPaintopPropertiesResourcesBase();

    template <typename KisPropertiesConfigurationPointer>
    void readOptionSetting(KisPropertiesConfigurationPointer settings, KisResourcesInterfaceSP resourcesInterface)
    {
        readOptionSettingResourceImpl(removeSharedPointer(settings), resourcesInterface);
    }

    template <typename KisPropertiesConfigurationPointer>
    void writeOptionSetting(KisPropertiesConfigurationPointer settings) const
    {
        writeOptionSettingImpl(removeSharedPointer(settings));
    }

    template <typename KisPropertiesConfigurationPointer>
    QList<KoResourceLoadResult> prepareLinkedResources(const KisPropertiesConfigurationPointer settings, KisResourcesInterfaceSP resourcesInterface) const
    {
        return prepareLinkedResourcesImpl(removeSharedPointer(settings), resourcesInterface);
    }

    template <typename KisPropertiesConfigurationPointer>
    QList<KoResourceLoadResult> prepareEmbeddedResources(const KisPropertiesConfigurationPointer settings, KisResourcesInterfaceSP resourcesInterface) const
    {
        return prepareEmbeddedResourcesImpl(removeSharedPointer(settings), resourcesInterface);
    }

protected:
    virtual void readOptionSettingResourceImpl(const KisPropertiesConfiguration *settings, KisResourcesInterfaceSP resourcesInterface) = 0;
    virtual void writeOptionSettingImpl(KisPropertiesConfiguration *settings) const = 0;
    virtual QList<KoResourceLoadResult> prepareLinkedResourcesImpl(const KisPropertiesConfiguration *settings, KisResourcesInterfaceSP resourcesInterface) const = 0;
    virtual QList<KoResourceLoadResult> prepareEmbeddedResourcesImpl(const KisPropertiesConfiguration *settings, KisResourcesInterfaceSP resourcesInterface) const = 0;
};

class KRITAUI_EXPORT KisPaintopPropertiesBase
{
public:
    virtual ~KisPaintopPropertiesBase();

    template <typename KisPropertiesConfigurationPointer>
    void readOptionSetting(KisPropertiesConfigurationPointer settings)
    {
        readOptionSettingImpl(removeSharedPointer(settings));
    }

    template <typename KisPropertiesConfigurationPointer>
    void writeOptionSetting(KisPropertiesConfigurationPointer settings) const
    {
        writeOptionSettingImpl(removeSharedPointer(settings));

    }

protected:
    virtual void readOptionSettingImpl(const KisPropertiesConfiguration *settings) = 0;
    virtual void writeOptionSettingImpl(KisPropertiesConfiguration *settings) const = 0;
};

#endif
