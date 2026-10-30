/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Plugin.h"
#include "defaulttool/DefaultToolFactory.h"
#include "referenceimagestool/ToolReferenceImages.h"

#include <KoToolRegistry.h>

#include <kpluginfactory.h>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "calligra_tool_defaults.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoToolRegistry::instance()->add(new DefaultToolFactory());
    // Krimble 2026-10-30: the Reference Images tool is NOT excluded: it is registered here and is in the toolbox (George's final decision of 2026-10-28, "retain all
    // toolbox tools", and again on 2026-10-30: "I don't think they should be excluded"). The comment that stood here, "Krimble 1.0.45-beta: Reference Images excluded
    // from toolbox.", was the plan before that decision.
    KoToolRegistry::instance()->add(new ToolReferenceImagesFactory());
}

#include <Plugin.moc>
