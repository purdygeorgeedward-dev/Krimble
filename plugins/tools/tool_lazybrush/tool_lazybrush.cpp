/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tool_lazybrush.h"

#include <kis_debug.h>
#include <kpluginfactory.h>

#include <kis_tool.h>
#include <KoToolRegistry.h>

#include "kis_paint_device.h"
#include "kis_tool_lazy_brush.h"


K_PLUGIN_FACTORY_WITH_JSON(DefaultToolsFactory, "kritatoollazybrush.json", registerPlugin<ToolLazyBrush>();)


ToolLazyBrush::ToolLazyBrush(QObject *parent, const QVariantList &)
        : QObject(parent)
{
    // Krimble 2026-10-30: the Colorize Mask Editing tool is NOT excluded: it is registered here and is in the toolbox (George's final decision of 2026-10-28, "retain all
    // toolbox tools", and again on 2026-10-30: "I don't think they should be excluded"). The comment that stood here, "Krimble 1.0.45-beta: Colorize Mask Editing excluded
    // from toolbox.", was the plan before that decision.
    KoToolRegistry::instance()->add(new KisToolLazyBrushFactory());
}

ToolLazyBrush::~ToolLazyBrush()
{
}

#include "tool_lazybrush.moc"
