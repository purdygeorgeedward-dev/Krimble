/*
 * KDE. Krita Project.
 *
 * SPDX-FileCopyrightText: 2022 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <kpluginfactory.h>
#include <KoToolRegistry.h>

#include "KisToolEncloseAndFillPlugin.h"
#include "KisToolEncloseAndFillFactory.h"

K_PLUGIN_FACTORY_WITH_JSON(KisToolEncloseAndFillPluginFactory, "kritatoolencloseandfill.json", registerPlugin<KisToolEncloseAndFillPlugin>();)

KisToolEncloseAndFillPlugin::KisToolEncloseAndFillPlugin(QObject *parent, const QVariantList &)
        : QObject(parent)
{
    // Krimble 2026-10-30: George: "The four tools you listed are supposed to be commented out." (Dodge, Burn, Colorize Mask Editing and Enclose and Fill: the four tools
    // with no new graphic; Dodge and Burn were commented out on 2026-10-20.) Not registered, so the Enclose and Fill tool is not in the toolbox; its code stays.
    // The old comment here said: "Krimble 1.0.45-beta: Enclose and Fill excluded from toolbox."
    // KoToolRegistry::instance()->add(new KisToolEncloseAndFillFactory());
}

KisToolEncloseAndFillPlugin::~KisToolEncloseAndFillPlugin()
{
}

#include "KisToolEncloseAndFillPlugin.moc"
