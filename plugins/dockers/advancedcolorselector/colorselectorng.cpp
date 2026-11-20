/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2010 Adam Celarek <kdedev at xibo dot at>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "colorselectorng.h"

#include <kpluginfactory.h>

#include <KoDockFactoryBase.h>
#include <KoDockRegistry.h>

#include "kis_color_selector_ng_dock.h"
#include "krimble_mixer_dock.h"
#include "krimble_dpaint_dock.h"
#include "kis_color_selector_settings.h"
#include "kis_preference_set_registry.h"

K_PLUGIN_FACTORY_WITH_JSON(ColorSelectorNgPluginFactory, "krita_colorselectorng.json", registerPlugin<ColorSelectorNgPlugin>();)


class ColorSelectorNgDockFactory : public KoDockFactoryBase
{
public:
    ColorSelectorNgDockFactory() {
    }

    QString id() const override {
        return QString("ColorSelectorNg");
    }

    virtual Qt::DockWidgetArea defaultDockWidgetArea() const {
        return Qt::RightDockWidgetArea;
    }

    QDockWidget* createDockWidget() override {
        KisColorSelectorNgDock * dockWidget = new KisColorSelectorNgDock();
        dockWidget->setObjectName(id());

        return dockWidget;
    }

    DockPosition defaultDockPosition() const override {
        return DockMinimized;
    }
};


class KrimbleRingDockFactory : public KoDockFactoryBase
{
public:
    QString id() const override { return QString("KrimbleColorRingTriangle"); }
    QDockWidget* createDockWidget() override {
        KisColorSelectorNgDock *dockWidget = new KisColorSelectorNgDock(true);
        dockWidget->setObjectName(id());
        return dockWidget;
    }
    DockPosition defaultDockPosition() const override { return DockMinimized; }
};

class KrimbleMixerDockFactory : public KoDockFactoryBase
{
public:
    QString id() const override { return QString("KrimbleMixer"); }
    QDockWidget* createDockWidget() override {
        KrimbleMixerDock *dockWidget = new KrimbleMixerDock();
        dockWidget->setObjectName(id());
        return dockWidget;
    }
    DockPosition defaultDockPosition() const override { return DockMinimized; }   // opened from the Window menu
};

class KrimbleDPaintDockFactory : public KoDockFactoryBase
{
public:
    QString id() const override { return QString("KrimbleDPaint"); }
    QDockWidget* createDockWidget() override {
        KrimbleDPaintDock *dockWidget = new KrimbleDPaintDock();
        dockWidget->setObjectName(id());
        return dockWidget;
    }
    DockPosition defaultDockPosition() const override { return DockMinimized; }   // opened from the Window menu
};

ColorSelectorNgPlugin::ColorSelectorNgPlugin(QObject *parent, const QVariantList &)
        : QObject(parent)
{
    KoDockRegistry::instance()->add(new ColorSelectorNgDockFactory());
    KoDockRegistry::instance()->add(new KrimbleRingDockFactory());
    KoDockRegistry::instance()->add(new KrimbleMixerDockFactory());

    KisPreferenceSetRegistry *preferenceSetRegistry = KisPreferenceSetRegistry::instance();

    KisColorSelectorSettingsFactory* settingsFactory = new KisColorSelectorSettingsFactory();

    //load and save preferences
    //if something in kritarc is missing, then the default from this load function will be used and saved back to kconfig.
    //this way, cfg.readEntry() in any part won't be able to set its own default
    KisPreferenceSet* settings = settingsFactory->createPreferenceSet();
    Q_ASSERT(settings);
    settings->loadPreferences();
    settings->savePreferences();
    delete settings;

    preferenceSetRegistry->add("KisColorSelectorSettingsFactory", settingsFactory);
}

ColorSelectorNgPlugin::~ColorSelectorNgPlugin()
{

}

#include "colorselectorng.moc"
