/*
 *  SPDX-FileCopyrightText: 2014 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_PSD_LAYER_STYLE_H
#define KIS_PSD_LAYER_STYLE_H

class QIODevice;
class QUuid;

#include <psd.h>

#include "kis_types.h"
#include "kritaimage_export.h"

class KisPSDLayerStyle;
typedef QSharedPointer<KisPSDLayerStyle> KisPSDLayerStyleSP;

class KRITAIMAGE_EXPORT KisPSDLayerStyle : public KoResource
{

public:
    KisPSDLayerStyle(const QString &filename = "", KisResourcesInterfaceSP resourcesInterface = KisResourcesInterfaceSP());
    virtual ~KisPSDLayerStyle();
    KisPSDLayerStyle(const KisPSDLayerStyle& rhs);
    KisPSDLayerStyle operator=(const KisPSDLayerStyle& rhs) = delete;

    KoResourceSP clone() const override;

    bool isSerializable() const override;
    bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) override;
    bool saveToDevice(QIODevice *dev) const override;

    void clear();

    QString name() const override;
    void setName(const QString &value);

    QUuid uuid() const;
    void setUuid(const QUuid &value);

    QString psdUuid() const;
    void setPsdUuid(const QString &value);

    QPair<QString, QString> resourceType() const override
    {
        return QPair<QString, QString>(ResourceType::LayerStyles, "");
    }

    bool isEmpty() const;

    bool isEnabled() const;
    void setEnabled(bool value);

    const psd_layer_effects_context* context() const;
    const psd_layer_effects_drop_shadow* dropShadow() const;
    const psd_layer_effects_inner_shadow* innerShadow() const;
    const psd_layer_effects_outer_glow* outerGlow() const;
    const psd_layer_effects_inner_glow* innerGlow() const;
    const psd_layer_effects_satin* satin() const;
    const psd_layer_effects_color_overlay* colorOverlay() const;
    const psd_layer_effects_gradient_overlay* gradientOverlay() const;
    const psd_layer_effects_pattern_overlay* patternOverlay() const;
    const psd_layer_effects_stroke* stroke() const;
    const psd_layer_effects_bevel_emboss* bevelAndEmboss() const;

    psd_layer_effects_context* context();
    psd_layer_effects_drop_shadow* dropShadow();
    psd_layer_effects_inner_shadow* innerShadow();
    psd_layer_effects_outer_glow* outerGlow();
    psd_layer_effects_inner_glow* innerGlow();
    psd_layer_effects_satin* satin();
    psd_layer_effects_color_overlay* colorOverlay();
    psd_layer_effects_gradient_overlay* gradientOverlay();
    psd_layer_effects_pattern_overlay* patternOverlay();
    psd_layer_effects_stroke* stroke();
    psd_layer_effects_bevel_emboss* bevelAndEmboss();

    KisResourcesInterfaceSP resourcesInterface() const;

    void setResourcesInterface(KisResourcesInterfaceSP resourcesInterface);

    bool hasLocalResourcesSnapshot() const;

    KisPSDLayerStyleSP cloneWithResourcesSnapshot(KisResourcesInterfaceSP globalResourcesInterface, KoCanvasResourcesInterfaceSP canvasResourcesInterface) const;

    QList<KoResourceLoadResult> linkedResources(KisResourcesInterfaceSP globalResourcesInterface) const override;

    QList<KoResourceLoadResult> sideLoadedResources(KisResourcesInterfaceSP globalResourcesInterface) const override;
    void clearSideLoadedResources() override;

    QList<int> requiredCanvasResources() const override;

private:
    friend class KisDlgLayerStyle;
    friend class KisAslLayerStyleSerializer;
    void setSideLoadedResources(const QList<KoEmbeddedResource> &value);

private:
    struct Private;
    Private * const d;
};

#endif
