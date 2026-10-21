/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_smudge.h"
#include "KrimbleEmbeddedPreset.h"

#include <KoCanvasBase.h>

#include <KisResourceModel.h>
#include <KisResourceTypes.h>
#include <brushengine/kis_paintop_preset.h>
#include <brushengine/kis_paintop_settings.h>

#include <kis_canvas2.h>
#include <KisViewManager.h>
#include <kis_canvas_resource_provider.h>

KisToolSmudge::KisToolSmudge(KoCanvasBase *canvas)
    : KisToolBrush(canvas)
{
    setObjectName("tool_smudge");
}

KisToolSmudge::~KisToolSmudge()
{
}

void KisToolSmudge::activate(const QSet<KoShape*> &shapes)
{
    KisToolBrush::activate(shapes);

    // Krimble: auto-load the bundled "smudge" preset (Krita's existing
    // colorsmudge paintop engine) so this tool smudges immediately on
    // selection, matching Photoshop's dedicated Smudge tool -- no manual
    // preset picking required. If the preset isn't found for any reason
    // (renamed/removed resource), the tool still falls back to whatever
    // preset was already active, same as any other brush-family tool.
    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager()) {
        return;
    }

    // KRIMBLE 2026-10-02: the lookup below never found anything (the embedded presets are
    // not in the resource database), so the tool painted with the current brush. Load the
    // built-in Color Smudge preset directly instead. Old code kept for reference:
    // KisResourceModel model(ResourceType::PaintOpPresets);
    // const QVector<KoResourceSP> matches = model.resourcesForName("smudge");
    // if (matches.isEmpty()) {
    //     return;
    // }
    // KisPaintOpPresetSP smudgePreset = matches.first().dynamicCast<KisPaintOpPreset>();
    KisPaintOpPresetSP smudgePreset = krimbleLoadEmbeddedPreset(QStringLiteral("colorsmudge"));
    if (smudgePreset) {
        // Krimble 2026-10-15: George (1.0.36): "The smudge tool is just a brush instead of modifying image with smudges. That's not what it's
        // supposed to do." The default Color Smudge preset (plugins/paintops/defaultpresets/colorsmudge.kpp) has ColorRateValue 0.5: every dab
        // adds 50% of the foreground color to what it smears, so it paints like a brush. A smudge adds no color: the rate is 0 (not
        // pressure-controlled). The idea of this change comes from a patch of a Codex session (not applied; read as a clue); the
        // parameter names are the ones inside the preset file.
        if (smudgePreset->settings()) {
            smudgePreset->settings()->setProperty("ColorRateValue", 0.0);
            smudgePreset->settings()->setProperty("PressureColorRate", false);
        }
        // canvas2->viewManager()->canvasResourceProvider()->setPaintOpPreset(smudgePreset);
        krimbleSetBrushPreset(canvas2->viewManager(), smudgePreset);   // Krimble 2026-10-20 (2nd): through the brush panel (see KrimbleEmbeddedPreset.h)
    }
}
