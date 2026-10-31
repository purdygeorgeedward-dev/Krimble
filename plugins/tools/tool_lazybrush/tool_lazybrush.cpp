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
    // Krimble 2026-10-30: George: "The four tools you listed are supposed to be commented out." (Dodge, Burn, Colorize Mask Editing and Enclose and Fill: the four tools
    // with no new graphic; Dodge and Burn were commented out on 2026-10-20.) Not registered, so the Colorize Mask Editing tool is not in the toolbox; its code stays.
    // The old comment here said: "Krimble 1.0.45-beta: Colorize Mask Editing excluded from toolbox."
    // KoToolRegistry::instance()->add(new KisToolLazyBrushFactory());
}

ToolLazyBrush::~ToolLazyBrush()
{
}

#include "tool_lazybrush.moc"
