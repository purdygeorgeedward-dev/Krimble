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
#include <brushengine/kis_paintop_settings.h>   // Krimble 2026-10-20: setPaintOpSize

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

    // Krimble: auto-load the bundled "DFP" preset (Krita's existing default
    // filter-brush preset, filterop engine, already configured with
    // Filter/id = "gaussian blur") so this tool softens immediately on
    // selection -- no manual preset/filter picking required. Falls back to
    // whatever preset was already active if not found, same as KisToolSmudge.
    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager()) {
        return;
    }

    // KRIMBLE 2026-10-02: the lookup below never found anything (the embedded presets are
    // not in the resource database). Load the built-in filter-brush preset (named "DFP",
    // gaussian blur) directly instead. Old code kept for reference:
    // KisResourceModel model(ResourceType::PaintOpPresets);
    // const QVector<KoResourceSP> matches = model.resourcesForName("DFP");
    // if (matches.isEmpty()) {
    //     return;
    // }
    // KisPaintOpPresetSP softenPreset = matches.first().dynamicCast<KisPaintOpPreset>();
    KisPaintOpPresetSP softenPreset = krimbleLoadEmbeddedPreset(QStringLiteral("filter"));
    if (softenPreset) {
        // Krimble 2026-10-20: George (1.0.40, test list, item 9): "Soften needs to default to a bigger brush size." The built-in filter preset is
        // small for softening on a phone; it starts at 100 px (the size can still be changed as usual while the tool is in use).
        if (softenPreset->settings()) {
            softenPreset->settings()->setPaintOpSize(100.0);
        }
        // canvas2->viewManager()->canvasResourceProvider()->setPaintOpPreset(softenPreset);
        krimbleSetBrushPreset(canvas2->viewManager(), softenPreset);   // Krimble 2026-10-20 (2nd): through the brush panel
    }
}
