/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_soften.h"
#include "KrimbleEmbeddedPreset.h"

#include <KoCanvasBase.h>

#include <KisResourceModel.h>
#include <KisResourceTypes.h>
#include <brushengine/kis_paintop_preset.h>
#include <brushengine/kis_paintop_settings.h>

#include <kis_canvas2.h>
#include <KisViewManager.h>
#include <kis_canvas_resource_provider.h>

KisToolSoften::KisToolSoften(KoCanvasBase *canvas)
    : KisToolBrush(canvas)
{
    setObjectName("tool_soften");
}

KisToolSoften::~KisToolSoften()
{
}

void KisToolSoften::activate(const QSet<KoShape*> &shapes)
{
    KisToolBrush::activate(shapes);

    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager()) {
        return;
    }

    KisPaintOpPresetSP softenPreset = krimbleLoadEmbeddedPreset(QStringLiteral("filter"));
    if (softenPreset) {
        if (softenPreset->settings()) {
            softenPreset->settings()->setPaintOpSize(100.0);
        }
        krimbleSetBrushPreset(canvas2->viewManager(), softenPreset);
    }
}
