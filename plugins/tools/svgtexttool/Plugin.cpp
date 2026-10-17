/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2017 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Plugin.h"

#include <kpluginfactory.h>

#include <QQmlEngine>

#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

#include <KisStaticInitializer.h>
#include "SvgTextToolFactory.h"
#include "KrimbleTextTool.h"

#include "glyphpalette/GlyphPaletteProxyModel.h"
#include "SvgTextToolOptionsModel.h"
#include "SvgTextToolOptionsManager.h"

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "krita_tool_svgtext.json", registerPlugin<Plugin>();)

KIS_DECLARE_STATIC_INITIALIZER {
    qmlRegisterType<GlyphPaletteProxyModel>("org.krita.tools.text", 1, 0, "GlyphPaletteProxyModel");

    qmlRegisterType<SvgTextToolOptionsModel>("org.krita.tools.text", 1, 0, "SvgTextToolOptionsModel");
    qmlRegisterType<SvgTextToolOptionsManager>("org.krita.tools.text", 1, 0, "SvgTextToolOptionsManager");
}
Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    // Krimble: temporarily disabled -- Type tool was randomly self-activating
    // in the toolbox (bug #9, still under investigation). Commenting out the
    // registration keeps the plugin building/loading, but the tool can't
    // appear in the toolbox or be auto-activated by anything since it's never
    // added to KoToolRegistry. Re-enable by uncommenting once root cause found.
    // KoToolRegistry::instance()->add(new SvgTextToolFactory());

    // KRIMBLE 2026-10-02: RE-ENABLED at George's request, with the root cause of
    // the self-activation (bug #9) still not found. If the Type tool activates by
    // itself again, note what was being done and report it.
    // KoToolRegistry::instance()->add(new SvgTextToolFactory());
    // Krimble 2026-10-18: George: "The existing type tool is garbage. I'm letting you design the new one from scratch. Just get rid of this crap version." The old
    // tool is no longer registered (its code stays in the plugin, unused). The new Type tool, KrimbleTextTool (a text window, no editing on the canvas), takes its
    // place in the toolbox.
    KoToolRegistry::instance()->add(new KrimbleTextToolFactory());
}

#include <Plugin.moc>

