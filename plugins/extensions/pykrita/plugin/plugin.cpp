/*
 * SPDX-FileCopyrightText: 2014 Boudewijn Rempt (boud@valdyas.org)
 *
 *  SPDX-License-Identifier: LGPL-2.0-only
 */

#include "plugin.h"

#include <klocalizedstring.h>
#include <kis_debug.h>
#include <kpluginfactory.h>

#include <kis_preference_set_registry.h>
#include "pyqtpluginsettings.h"

#include <QCoreApplication>

#include <Krita.h>

K_PLUGIN_FACTORY_WITH_JSON(KritaPyQtPluginFactory, "kritapykrita.json", registerPlugin<KritaPyQtPlugin>();)

KritaPyQtPlugin::KritaPyQtPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
    , m_autoReload(false)
{
    dbgScript << "Loading Python plugin";

    PyKrita::InitResult initResult = PyKrita::initialize();
    switch (initResult) {
        case PyKrita::INIT_OK:
            break;
        case PyKrita::INIT_CANNOT_LOAD_PYTHON_LIBRARY:
            qWarning() << i18n("Cannot load Python library");
            return;
        case PyKrita::INIT_CANNOT_SET_PYTHON_PATHS:
            qWarning() << i18n("Cannot set Python paths");
            return;
        case PyKrita::INIT_CANNOT_LOAD_PYKRITA_MODULE:
            qWarning() << i18n("Cannot load built-in pykrita module");
            return;
        default:
            qWarning() << i18n("Unexpected error initializing python plugin.");
            return;
    }

    pluginManager = PyKrita::pluginManager();

    KisPreferenceSetRegistry *preferenceSetRegistry = KisPreferenceSetRegistry::instance();
    PyQtPluginSettingsFactory* settingsFactory = new PyQtPluginSettingsFactory(pluginManager);

    KisPreferenceSet* settings = settingsFactory->createPreferenceSet();
    KIS_SAFE_ASSERT_RECOVER_RETURN(settings);
    settings->loadPreferences();
    settings->savePreferences();
    delete settings;

    preferenceSetRegistry->add("PyQtPluginSettingsFactory", settingsFactory);

    PyKrita::Python py = PyKrita::Python();
    PyObject* pykritaPackage = py.moduleImport("pykrita");
    pykritaPackage = py.moduleImport("krita");

    if (pykritaPackage) {
        dbgScript << "Loaded pykrita, now load plugins";
        pluginManager->scanPlugins();
        pluginManager->tryLoadEnabledPlugins();
    } else  {
        dbgScript << "Cannot load pykrita module";
    }

    Q_FOREACH (Extension *extension, Krita::instance()->extensions()) {
        extension->setup();
    }

}

KritaPyQtPlugin::~KritaPyQtPlugin()
{
}

#include "plugin.moc"
