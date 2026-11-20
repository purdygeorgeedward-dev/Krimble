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

    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager()) {
        return;
    }

    KisPaintOpPresetSP smudgePreset = krimbleLoadEmbeddedPreset(QStringLiteral("colorsmudge"));
    if (smudgePreset) {
        if (smudgePreset->settings()) {
            smudgePreset->settings()->setProperty("ColorRateValue", 0.0);
            smudgePreset->settings()->setProperty("PressureColorRate", false);
        }
        krimbleSetBrushPreset(canvas2->viewManager(), smudgePreset);
    }
}
