/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_SOFTEN_H_
#define KIS_TOOL_SOFTEN_H_

#include "kis_tool_brush.h"

#include <KoIcon.h>
#include <kis_icon.h>

class KoCanvasBase;

// Krimble: dedicated toolbox entry for Krita's existing filterop paintop
// engine (plugins/paintops/filterop/) preconfigured with Gaussian Blur --
// the industry standard's "Blur" tool equivalent for localized softening. On
// activation, auto-loads the bundled "DFP" preset (Krita's existing
// default filter-brush preset, already configured with
// Filter/id = "gaussian blur") so the tool softens immediately, matching
// the same pattern KisToolSmudge uses for the colorsmudge engine.
class KisToolSoften : public KisToolBrush
{
    Q_OBJECT

public:
    explicit KisToolSoften(KoCanvasBase *canvas);
    ~KisToolSoften() override;

    void activate(const QSet<KoShape*> &shapes) override;
};

class KisToolSoftenFactory : public KisToolPaintFactoryBase
{
public:
    KisToolSoftenFactory()
        : KisToolPaintFactoryBase("KritaShape/KisToolSoften")
    {
        setToolTip(i18n("Blur"));   // Krimble 2026-11-16 (722nd): George's list of the toolbox calls it Blur
        setSection(ToolBoxSection::PSOrder);
        setIconName(koIconNameCStr("krita_tool_soften"));
        setShortcut(QKeySequence(Qt::Key_U));
        setPriority(22); // Krimble: directly after Smudge (21) -- matches the industry standard grouping Blur/Soften with the brush-family tools.
        setToolBoxPriority(134);   // Krimble 2026-11-16 (722nd): the toolbox order of the build 64 list (was 122): Blur after the Gradient (130), before Sharpen (136)
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolSoftenFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolSoften(canvas);
    }
};

#endif // KIS_TOOL_SOFTEN_H_
