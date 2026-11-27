/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_PAINTOP_SETTINGS_H_
#define KIS_PAINTOP_SETTINGS_H_

#include "kis_types.h"
#include "kritaimage_export.h"

#include <QScopedPointer>

#include "kis_properties_configuration.h"
#include <brushengine/kis_paint_information.h>
#include <brushengine/kis_uniform_paintop_property.h>

class KisPaintOpConfigWidget;
class KisPaintOpPresetUpdateProxy;

class KisResourcesInterface;
using KisResourcesInterfaceSP = QSharedPointer<KisResourcesInterface>;

class KoCanvasResourcesInterface;
using KoCanvasResourcesInterfaceSP = QSharedPointer<KoCanvasResourcesInterface>;

class KoResourceCacheInterface;
using KoResourceCacheInterfaceSP = QSharedPointer<KoResourceCacheInterface>;

class KisOptimizedBrushOutline;

const QString AIRBRUSH_ENABLED = "PaintOpSettings/isAirbrushing";

const QString AIRBRUSH_RATE = "PaintOpSettings/rate";

const QString AIRBRUSH_IGNORE_SPACING = "PaintOpSettings/ignoreSpacing";

const QString SPACING_USE_UPDATES = "PaintOpSettings/updateSpacingBetweenDabs";

class KRITAIMAGE_EXPORT KisPaintOpSettings : public KisPropertiesConfiguration
{
public:
    struct UpdateListener {
        virtual ~UpdateListener();
        virtual void setDirty(bool value) = 0;
        virtual bool isDirty() const = 0;
        virtual void notifySettingsChanged() = 0;
    };

    using UpdateListenerSP = QSharedPointer<UpdateListener>;
    using UpdateListenerWSP = QWeakPointer<UpdateListener>;

public:

    KisPaintOpSettings(KisResourcesInterfaceSP resourcesInterface);
    ~KisPaintOpSettings() override;
    KisPaintOpSettings(const KisPaintOpSettings &rhs);

    virtual bool mousePressEvent(const KisPaintInformation &paintInformation, Qt::KeyboardModifiers modifiers, KisNodeWSP currentNode);
    virtual bool mouseReleaseEvent();
    virtual KisPaintOpSettingsSP clone() const;

    virtual void resetSettings(const QStringList &preserveProperties = QStringList());

    KisNodeSP node() const;

    virtual void activate();

    virtual bool paintIncremental() {
        return true;
    }

    virtual QString indirectPaintingCompositeOp() const;

    virtual bool isAirbrushing() const;

    virtual qreal airbrushInterval() const;

    virtual bool useSpacingUpdates() const;

    virtual bool needsAsynchronousUpdates() const;

    struct OutlineMode {
        bool isVisible = false;
        bool forceCircle = false;
        bool showTiltDecoration = false;
        bool forceFullSize = false;
    };

    virtual KisOptimizedBrushOutline brushOutline(const KisPaintInformation &info, const OutlineMode &mode, qreal alignForZoom);

    static KisOptimizedBrushOutline ellipseOutline(qreal width, qreal height, qreal scale, qreal rotation);

    static QPainterPath makeTiltIndicator(KisPaintInformation const& info,
        QPointF const& start, qreal lengthScale, qreal angle);

    virtual void setPaintOpOpacity(qreal value);

    void setPaintOpFlow(qreal value);

    void setPaintOpFade(qreal value);

    void setPaintOpScatter(qreal value);

    void setPaintOpCompositeOp(const QString &value);

    virtual qreal paintOpOpacity();

    qreal paintOpFlow();

    qreal paintOpFade();

    qreal paintOpScatter();

    QString paintOpCompositeOp();

    virtual void setPaintOpSize(qreal value) = 0;

    virtual qreal paintOpSize() const = 0;

    virtual void setPaintOpAngle(qreal value) = 0;

    virtual qreal paintOpAngle() const = 0;

    virtual qreal paintOpPatternSize();

    void setEraserMode(bool value);
    bool eraserMode();

    qreal savedEraserSize() const;
    void setSavedEraserSize(qreal value);
    qreal savedBrushSize() const;
    void setSavedBrushSize(qreal value);

    qreal savedEraserOpacity() const;
    void setSavedEraserOpacity(qreal value);
    qreal savedBrushOpacity() const;
    void setSavedBrushOpacity(qreal value);

    QString effectivePaintOpCompositeOp();

    void setUpdateListener(UpdateListenerWSP listener);

    UpdateListenerWSP updateListener() const;

    virtual QString modelName() const;

    void setModelName(const QString & modelName);

    virtual bool isValid() const;

    void setProperty(const QString & name, const QVariant & value) override;

    virtual QList<KisUniformPaintOpPropertySP> uniformProperties(KisPaintOpSettingsSP settings, QPointer<KisPaintOpPresetUpdateProxy> updateProxy);

    static bool isLodUserAllowed(const KisPropertiesConfigurationSP config);
    static void setLodUserAllowed(KisPropertiesConfigurationSP config, bool value);

    virtual bool lodSizeThresholdSupported() const;

    qreal lodSizeThreshold() const;
    void setLodSizeThreshold(qreal value);

    bool hasMaskingSettings() const;

    KisPaintOpSettingsSP createMaskingSettings() const;

    QString maskingBrushCompositeOp() const;

    KisResourcesInterfaceSP resourcesInterface() const;

    void setResourcesInterface(KisResourcesInterfaceSP resourcesInterface);

    virtual bool hasPatternSettings() const;

    virtual QList<int> requiredCanvasResources() const;

    KoCanvasResourcesInterfaceSP canvasResourcesInterface() const;
    void setCanvasResourcesInterface(KoCanvasResourcesInterfaceSP canvasResourcesInterface);

    virtual void setResourceCacheInterface(KoResourceCacheInterfaceSP cacheInterface);
    KoResourceCacheInterfaceSP resourceCacheInterface() const;

    virtual void regenerateResourceCache(KoResourceCacheInterfaceSP cacheInterface);

    quint64 sanityVersionCookie() const;

protected:

    virtual void onPropertyChanged();

private:

    struct Private;
    const QScopedPointer<Private> d;
};

#endif
