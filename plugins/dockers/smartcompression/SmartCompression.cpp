/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  Krimble 2026-11-13 (task 52): George: "When the user edits the selection for SmartCompression they should start with the smart selection." and "This is how XAT handled it." (the Work with Regions mode, the AutoCompress wand, Remove All).
 */

#include "SmartCompression.h"

#include <kpluginfactory.h>
#include <klocalizedstring.h>

#include <KoDockFactoryBase.h>
#include <KoDockRegistry.h>

#include "SmartCompressionDock.h"

K_PLUGIN_FACTORY_WITH_JSON(SmartCompressionPluginFactory, "kritasmartcompressiondocker.json", registerPlugin<SmartCompressionPlugin>();)

class SmartCompressionDockFactory : public KoDockFactoryBase
{
public:
    SmartCompressionDockFactory() {}

    QString id() const override {
        return QString("SmartCompression");
    }

    QDockWidget *createDockWidget() override {
        SmartCompressionDock *dockWidget = new SmartCompressionDock();
        dockWidget->setObjectName(id());
        return dockWidget;
    }

    DockPosition defaultDockPosition() const override {
        // like the other panels of this build: not docked at the start (they crammed the column); Window > Panels shows it
        return DockMinimized;
    }
};

SmartCompressionPlugin::SmartCompressionPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoDockRegistry::instance()->add(new SmartCompressionDockFactory());
}

SmartCompressionPlugin::~SmartCompressionPlugin()
{
}

#include <SmartCompression.moc>
