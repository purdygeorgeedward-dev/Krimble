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
    // Krimble 2026-10-30: the Enclose and Fill tool is NOT excluded: it is registered here and is in the toolbox (George's final decision of 2026-10-28, "retain all
    // toolbox tools", and again on 2026-10-30: "I don't think they should be excluded"). The comment that stood here, "Krimble 1.0.45-beta: Enclose and Fill excluded
    // from toolbox.", was the plan before that decision.
    KoToolRegistry::instance()->add(new KisToolEncloseAndFillFactory());
}

KisToolEncloseAndFillPlugin::~KisToolEncloseAndFillPlugin()
{
}

#include "KisToolEncloseAndFillPlugin.moc"
