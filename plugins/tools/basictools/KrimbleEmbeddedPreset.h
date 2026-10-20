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

#endif
