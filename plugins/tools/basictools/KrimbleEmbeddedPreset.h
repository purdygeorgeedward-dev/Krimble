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
#include <KisViewManager.h>
#include <kis_paintop_box.h>
#include <kis_canvas_resource_provider.h>

inline KisPaintOpPresetSP krimbleLoadEmbeddedPreset(const QString &paintOpId)
{
    const QString path = QStringLiteral(":/presets/") + paintOpId + QStringLiteral(".kpp");
    KisPaintOpPresetSP preset(new KisPaintOpPreset(path));
    if (!preset->load(KisGlobalResourcesInterface::instance())) {
        return KisPaintOpPresetSP();
    }
    preset->setName(QStringLiteral("Krimble Retouch ") + paintOpId);
    return preset;
}

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
