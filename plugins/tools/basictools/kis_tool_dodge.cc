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

    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager()) {
        return;
    }

    KisPaintOpPresetSP basePreset = krimbleLoadEmbeddedPreset(QStringLiteral("paintbrush"));
    if (!basePreset) {
        return;
    }

    KisPaintOpPresetSP dodgePreset = basePreset->clone().dynamicCast<KisPaintOpPreset>();
    if (!dodgePreset || !dodgePreset->settings()) {
        return;
    }

    dodgePreset->settings()->setProperty("CompositeOp", COMPOSITE_LINEAR_DODGE);
    dodgePreset->settings()->setPaintOpOpacity(1.0);
    dodgePreset->settings()->setPaintOpFlow(0.008);
    dodgePreset->settings()->setPaintOpSize(80.0);
    dodgePreset->settings()->setProperty("PaintOpAction", 1);
    KisCanvasResourceProvider *provider = canvas2->viewManager()->canvasResourceProvider();
    if (!m_foregroundSaved) {
        m_savedForeground = provider->fgColor();
        m_foregroundSaved = true;
    }
    provider->setFGColor(KoColor(Qt::white, m_savedForeground.colorSpace()));
    krimbleSetBrushPreset(canvas2->viewManager(), dodgePreset);
}

void KisToolDodge::deactivate()
{
    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (m_foregroundSaved && canvas2 && canvas2->viewManager()) {
        canvas2->viewManager()->canvasResourceProvider()->setFGColor(m_savedForeground);
    }
    m_foregroundSaved = false;
    KisToolBrush::deactivate();
}
