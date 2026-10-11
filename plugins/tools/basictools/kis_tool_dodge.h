/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_DODGE_H_
#define KIS_TOOL_DODGE_H_

#include "kis_tool_brush.h"

#include <KoIcon.h>
#include <kis_icon.h>
#include <KoColor.h>

class KoCanvasBase;

// Krimble: dedicated toolbox entry for Photoshop's Dodge tool. Unlike
// Smudge/Soften, there is no dedicated bundled preset for this -- instead,
// on activation this clones the standard default brush preset ("defaultPreset",
// plugins/paintops/defaultpresets/paintbrush.kpp) and overrides its
// CompositeOp to COMPOSITE_DODGE (already a real, fully-supported composite
// op in KoCompositeOpRegistry.h -- Krita's core painter/compositing engine
// already handles it for any brush, it just had no dedicated toolbox tool).
// The clone is essential: modifying the shared cached preset in place would
// corrupt the default brush everywhere else it's used.
class KisToolDodge : public KisToolBrush
{
    Q_OBJECT

public:
    explicit KisToolDodge(KoCanvasBase *canvas);
    ~KisToolDodge() override;

    void activate(const QSet<KoShape*> &shapes) override;
    void deactivate() override;   // Krimble 2026-10-11: gives the foreground color back

private:
    KoColor m_savedForeground;    // Krimble 2026-10-11: the foreground color before this tool made it white
    bool m_foregroundSaved {false};
};

class KisToolDodgeFactory : public KisToolPaintFactoryBase
{
public:
    KisToolDodgeFactory()
        : KisToolPaintFactoryBase("KritaShape/KisToolDodge")
    {
        setToolTip(i18n("Dodge Tool"));
        setSection(ToolBoxSection::PSOrder);
        setIconName(koIconNameCStr("krita_tool_dodge"));
        setShortcut(QKeySequence(Qt::Key_O)); // Krimble: matches Photoshop's actual Dodge shortcut (Shift+O in PS; Krimble tools use unmodified single keys, see other PSOrder factories)
        setPriority(23); // Krimble: directly after Soften (22).
        setToolBoxPriority(123);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolDodgeFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolDodge(canvas);
    }
};

#endif // KIS_TOOL_DODGE_H_
