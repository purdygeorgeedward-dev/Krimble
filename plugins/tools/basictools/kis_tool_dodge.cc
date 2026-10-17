/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_dodge.h"
#include "KrimbleEmbeddedPreset.h"

#include <KoCanvasBase.h>
#include <KoCompositeOpRegistry.h>

#include <KisResourceModel.h>
#include <KisResourceTypes.h>
#include <brushengine/kis_paintop_preset.h>
#include <brushengine/kis_paintop_settings.h>

#include <kis_canvas2.h>
#include <KisViewManager.h>
#include <kis_canvas_resource_provider.h>

KisToolDodge::KisToolDodge(KoCanvasBase *canvas)
    : KisToolBrush(canvas)
{
    setObjectName("tool_dodge");
}

KisToolDodge::~KisToolDodge()
{
}

void KisToolDodge::activate(const QSet<KoShape*> &shapes)
{
    KisToolBrush::activate(shapes);

    // Krimble: clone the standard default brush preset and override its
    // CompositeOp to "dodge" (COMPOSITE_DODGE) -- see header comment for
    // why a clone is required rather than modifying the shared preset.
    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager()) {
        return;
    }

    // KRIMBLE 2026-10-02: the lookup below never found anything (the embedded presets are
    // not in the resource database), so the tool painted with the current brush. Load the
    // built-in default brush ("defaultPreset") directly instead; it is a fresh copy on every
    // call, so the shared default brush is not changed. Old code kept for reference:
    // KisResourceModel model(ResourceType::PaintOpPresets);
    // const QVector<KoResourceSP> matches = model.resourcesForName("defaultPreset");
    // if (matches.isEmpty()) {
    //     return;
    // }
    // KisPaintOpPresetSP basePreset = matches.first().dynamicCast<KisPaintOpPreset>();
    KisPaintOpPresetSP basePreset = krimbleLoadEmbeddedPreset(QStringLiteral("paintbrush"));
    if (!basePreset) {
        return;
    }

    KisPaintOpPresetSP dodgePreset = basePreset->clone().dynamicCast<KisPaintOpPreset>();
    if (!dodgePreset || !dodgePreset->settings()) {
        return;
    }

    // dodgePreset->settings()->setProperty("CompositeOp", COMPOSITE_DODGE);
    // Krimble 2026-10-11 (QA of 1.0.32): the color dodge op with the foreground color made a faint line
    // (dodge) or an instant black one (burn) on a white canvas. Photoshop's tool lightens what is already there, a little at
    // a time. The linear op does that: with a white brush each dab moves the pixel a small step toward
    // white, so the effect builds up slowly the more you paint over a spot. The tool sets its brush color to
    // white while it is active (see below) and a low flow (4% per dab); flow and opacity can be changed.
    dodgePreset->settings()->setProperty("CompositeOp", COMPOSITE_LINEAR_DODGE);
    dodgePreset->settings()->setPaintOpOpacity(1.0);
    // Krimble 2026-10-18 (12th): George (1.0.37, c666301): "17. Dodge and burn appear to default to black or white crayons." and, asked what he sees: "One drag makes solid white." Every dab
    // adds a step (flow 4% is about 10 of 255 levels at the center of the dab) and the dabs of a drag overlap many times (a pixel is under about a dozen of them), so one pass
    // reached solid white. A dab is now 5 times weaker (flow 0.8%, about 2 levels): one pass is a gentle change and passes build up. The flow can still be changed in the Tool Options.
    // dodgePreset->settings()->setPaintOpFlow(0.04);
    dodgePreset->settings()->setPaintOpFlow(0.008);
    // Krimble 2026-10-18 (13th): George: "It should start with a larger brush size too." The brush of this tool starts at 80 px (the default brush is small, which is right for painting
    // but not for lightening or darkening areas). The size can be changed in the Tool Options; it starts at 80 again the next time the tool is chosen.
    dodgePreset->settings()->setPaintOpSize(80.0);
    // Krimble: base preset defaults to WASH mode (PaintOpAction=2), which
    // applies opacity once per continuous stroke, not while dragging back
    // and forth. Real Dodge accumulates continuously as you paint over the
    // same area within one stroke -- BUILDUP mode (PaintOpAction=1) is
    // required for that.
    dodgePreset->settings()->setProperty("PaintOpAction", 1);
    // Krimble 2026-10-11: paint with white while this tool is active; the foreground color is given back in deactivate().
    KisCanvasResourceProvider *provider = canvas2->viewManager()->canvasResourceProvider();
    if (!m_foregroundSaved) {
        m_savedForeground = provider->fgColor();
        m_foregroundSaved = true;
    }
    provider->setFGColor(KoColor(Qt::white, m_savedForeground.colorSpace()));
    provider->setPaintOpPreset(dodgePreset);
}

void KisToolDodge::deactivate()
{
    // Krimble 2026-10-11: give the foreground color back
    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (m_foregroundSaved && canvas2 && canvas2->viewManager()) {
        canvas2->viewManager()->canvasResourceProvider()->setFGColor(m_savedForeground);
    }
    m_foregroundSaved = false;
    KisToolBrush::deactivate();
}
