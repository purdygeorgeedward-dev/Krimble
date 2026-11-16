/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_SMUDGE_H_
#define KIS_TOOL_SMUDGE_H_

#include "kis_tool_brush.h"

#include <KoIcon.h>
#include <kis_icon.h>

class KoCanvasBase;

// Krimble: dedicated toolbox entry for Krita's existing colorsmudge paintop
// engine (plugins/paintops/colorsmudge/), giving it its own the industry standard-style
// Smudge tool -- own icon, own toolbox slot, own shortcut -- instead of
// requiring users to dig for it in the brush preset picker. On activation,
// it auto-loads the bundled "smudge" preset so the tool smudges immediately,
// with no manual preset selection required. Reuses KisToolBrush's option
// widget (size/opacity/etc.) since those settings apply here too.
class KisToolSmudge : public KisToolBrush
{
    Q_OBJECT

public:
    explicit KisToolSmudge(KoCanvasBase *canvas);
    ~KisToolSmudge() override;

    void activate(const QSet<KoShape*> &shapes) override;
};

class KisToolSmudgeFactory : public KisToolPaintFactoryBase
{
public:
    KisToolSmudgeFactory()
        : KisToolPaintFactoryBase("KritaShape/KisToolSmudge")
    {
        setToolTip(i18n("Smudge Tool"));
        setSection(ToolBoxSection::PSOrder);
        setIconName(koIconNameCStr("krita_tool_smudge"));
        setShortcut(QKeySequence(Qt::Key_R));
        setPriority(21); // Krimble: directly after Brush (20) -- matches the industry standard grouping Smudge with the brush-family tools.
        setToolBoxPriority(135);   // Krimble 2026-11-16 (722nd): the toolbox order of the build 64 list (was 121): in the flyout of Blur
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolSmudgeFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolSmudge(canvas);
    }
};

#endif // KIS_TOOL_SMUDGE_H_
