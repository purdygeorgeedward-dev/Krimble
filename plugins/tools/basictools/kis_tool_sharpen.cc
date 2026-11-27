/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_sharpen.h"
#include "KrimbleEmbeddedPreset.h"

#include <KoCanvasBase.h>

#include <brushengine/kis_paintop_preset.h>
#include <brushengine/kis_paintop_settings.h>

#include <kis_canvas2.h>
#include <KisViewManager.h>
#include <kis_canvas_resource_provider.h>

KisToolSharpen::KisToolSharpen(KoCanvasBase *canvas)
    : KisToolBrush(canvas)
{
    setObjectName("tool_sharpen");
}

KisToolSharpen::~KisToolSharpen()
{
}

void KisToolSharpen::activate(const QSet<KoShape*> &shapes)
{
    KisToolBrush::activate(shapes);

    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager()) {
        return;
    }

    KisPaintOpPresetSP sharpenPreset = krimbleLoadEmbeddedPreset(QStringLiteral("filter"));
    if (sharpenPreset && sharpenPreset->settings()) {
        sharpenPreset->settings()->setProperty(QStringLiteral("Filter/id"), QStringLiteral("sharpen"));
        sharpenPreset->settings()->setProperty(QStringLiteral("Filter/configuration"), QString());
        sharpenPreset->settings()->setPaintOpSize(100.0);
        krimbleSetBrushPreset(canvas2->viewManager(), sharpenPreset);
    }
}
