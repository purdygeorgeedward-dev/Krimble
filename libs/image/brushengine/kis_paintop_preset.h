/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KIS_PAINTOP_PRESET_H
#define KIS_PAINTOP_PRESET_H

#include <QPointer>

#include <KoResource.h>
#include "KoID.h"

#include "kis_types.h"
#include "kis_shared.h"
#include "kritaimage_export.h"
#include <brushengine/kis_uniform_paintop_property.h>
#include <KisPaintOpPresetUpdateProxy.h>

class KoCanvasResourcesInterface;
using KoCanvasResourcesInterfaceSP = QSharedPointer<KoCanvasResourcesInterface>;

class KoResourceCacheInterface;
using KoResourceCacheInterfaceSP = QSharedPointer<KoResourceCacheInterface>;

class KRITAIMAGE_EXPORT KisPaintOpPreset : public KoResource
{
public:

    class KRITAIMAGE_EXPORT UpdatedPostponer{
    public:
        UpdatedPostponer(KisPaintOpPresetSP preset);

        ~UpdatedPostponer();

    private:
        QPointer<KisPaintOpPresetUpdateProxy> m_updateProxy;
    };

public:

    KisPaintOpPreset();

    KisPaintOpPreset(const QString& filename);

    ~KisPaintOpPreset() override;

    KisPaintOpPreset(const KisPaintOpPreset &rhs);
    KisPaintOpPreset &operator=(const KisPaintOpPreset &rhs) = delete;
    KoResourceSP clone() const override;

    void setPaintOp(const KoID & paintOp);

    KoID paintOp() const;

    QString name() const override;

    void setSettings(KisPaintOpSettingsSP settings);

    KisPaintOpSettingsSP settings() const;

    bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) override;

    bool saveToDevice(QIODevice* dev) const override;

    QPair<QString, QString> resourceType() const override
    {
        return QPair<QString, QString>(ResourceType::PaintOpPresets, "");
    }

    void updateLinkedResourcesMetaData();

    void toXML(QDomDocument& doc, QDomElement& elt) const;

    void fromXML(const QDomElement& elt, KisResourcesInterfaceSP resourcesInterface);

    bool removable() const {
        return true;
    }

    QString defaultFileExtension() const override {
        return ".kpp";
    }

    QPointer<KisPaintOpPresetUpdateProxy> updateProxy() const;
    QPointer<KisPaintOpPresetUpdateProxy> updateProxyNoCreate() const;

    QList<KisUniformPaintOpPropertySP> uniformProperties();

    bool hasMaskingPreset() const;

    KisPaintOpPresetSP createMaskingPreset() const;

    KisResourcesInterfaceSP resourcesInterface() const;

    void setResourcesInterface(KisResourcesInterfaceSP resourcesInterface);

    KoCanvasResourcesInterfaceSP canvasResourcesInterface() const;

    void setCanvasResourcesInterface(KoCanvasResourcesInterfaceSP canvasResourcesInterface);

    bool hasLocalResourcesSnapshot() const;

    KisPaintOpPresetSP cloneWithResourcesSnapshot(KisResourcesInterfaceSP globalResourcesInterface, KoCanvasResourcesInterfaceSP canvasResourcesInterface, KoResourceCacheInterfaceSP cacheInterface) const;

    QList<KoResourceLoadResult> linkedResources(KisResourcesInterfaceSP globalResourcesInterface) const override;

    QList<KoResourceLoadResult> embeddedResources(KisResourcesInterfaceSP globalResourcesInterface) const override;

    QList<KoResourceLoadResult> sideLoadedResources(KisResourcesInterfaceSP globalResourcesInterface) const override;
    void clearSideLoadedResources() override;

    QList<int> requiredCanvasResources() const override;

    void setResourceCacheInterface(KoResourceCacheInterfaceSP cacheInterface);

    KoResourceCacheInterfaceSP resourceCacheInterface() const;

    void regenerateResourceCache(KoResourceCacheInterfaceSP cacheInterface);

    bool sanityCheckResourceCacheIsValid(KoResourceCacheInterfaceSP cacheInterface) const;

private:

    struct Private;
    Private * const d;
};

Q_DECLARE_METATYPE(KisPaintOpPresetSP)

#endif
