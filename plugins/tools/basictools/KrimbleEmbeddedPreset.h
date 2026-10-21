/*
 * KRIMBLE 2026-10-02: loads one of the brush presets that are built into the app
 * (plugins/paintops/defaultpresets, embedded as ":/presets/<engine>.kpp"), the same way
 * KisPaintopBox::defaultPreset() does.
 *
 * Why this exists: the Smudge, Soften, Dodge and Burn tools used to look their preset
 * up by name in the resource database (KisResourceModel::resourcesForName). The
 * embedded presets ("smudgebrush", "DFP", "defaultPreset", "defaultSmudge") are not
 * database resources, so that lookup found nothing and every tool silently fell back to
 * the current brush, which painted plain black.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KRIMBLE_EMBEDDED_PRESET_H
#define KRIMBLE_EMBEDDED_PRESET_H

#include <QString>

#include <KisGlobalResourcesInterface.h>
#include <brushengine/kis_paintop_preset.h>
#include <KisViewManager.h>               // Krimble 2026-10-20 (2nd): krimbleSetBrushPreset
#include <kis_paintop_box.h>
#include <kis_canvas_resource_provider.h>

// Returns the embedded preset for the given brush engine id ("paintbrush", "colorsmudge",
// "filter", ...), or a null pointer if it cannot be loaded. Each call returns a new copy.
inline KisPaintOpPresetSP krimbleLoadEmbeddedPreset(const QString &paintOpId)
{
    const QString path = QStringLiteral(":/presets/") + paintOpId + QStringLiteral(".kpp");
    KisPaintOpPresetSP preset(new KisPaintOpPreset(path));
    if (!preset->load(KisGlobalResourcesInterface::instance())) {
        return KisPaintOpPresetSP();
    }
    // Krimble 2026-10-20: the retouch tools' presets are named "Krimble Retouch <engine>", so that a painting tool chosen afterwards can tell that the
    // current brush is a retouch brush and take the user's own brush back (KisToolPaint::activate; George, 1.0.40: Brush, Line and Circle were
    // still the Smudge tool after Smudge).
    preset->setName(QStringLiteral("Krimble Retouch ") + paintOpId);
    return preset;
}

// Krimble 2026-10-20 (2nd): George (1.0.41): "Smudge still turns every tool into smudge." The 216th entry's give-back did not work because the brush
// was swapped in the wrong place. The brush panel (KisPaintopBox) keeps its own idea of the current brush: its option widget, its per-pen preset
// map and its per-engine preset map. The retouch tools set the brush on the canvas resource provider only, behind the panel's back, so the panel
// went on holding the user's brush while the provider held the Smudge brush, and when the user's brush was put back the same way, the panel's
// maps and widget were never told. A brush is now always changed through the panel (KisPaintopBox::resourceSelected, the same call a tap in the
// brush chooser makes), which updates the provider, the widget and the maps together; the provider is set directly only if the panel is not there.
inline void krimbleSetBrushPreset(KisViewManager *viewManager, KisPaintOpPresetSP preset)
{
    if (!viewManager || !preset) return;
    KisCanvasResourceProvider *provider = viewManager->canvasResourceProvider();
    if (provider && provider->currentPreset() == preset) return;
    if (KisPaintopBox *box = viewManager->paintOpBox()) {
        box->resourceSelected(preset);
    }
    if (provider && provider->currentPreset() != preset) {
        provider->setPaintOpPreset(preset);
    }
}

#endif
