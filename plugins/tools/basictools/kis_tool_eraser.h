/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_ERASER_H_
#define KIS_TOOL_ERASER_H_

#include "kis_tool_brush.h"

#include <KoIcon.h>
#include <kis_icon.h>

class KoCanvasBase;

// Krimble 2026-10-25: a dedicated Eraser tool in the toolbox (George: "Can we make an eraser?", after "Do we have a way to paint with transparency
// like a color?"). It is the Brush tool with the eraser switch (the "erase_action" of Krita's brush toolbar, key E there) turned on while the tool
// is active: the brush's own settings (size, softness, opacity, the brush picked) are used, and what it paints is transparency. It uses the very same
// switch as the toolbar button, so everything that button does (the saved eraser size and opacity, the blending mode shown) happens here too. The
// state of the switch before the tool started is put back when the tool stops. Nothing is replaced in the user's brush (unlike the retouch tools).
class KisToolEraser : public KisToolBrush
{
    Q_OBJECT

public:
    explicit KisToolEraser(KoCanvasBase *canvas);
    ~KisToolEraser() override;

    void activate(const QSet<KoShape*> &shapes) override;
    void deactivate() override;
    void beginPrimaryAction(KoPointerEvent *event) override;

private:
    void switchEraserOn();

    bool m_switchWasOn = false;       // the eraser switch before this tool started
    bool m_switchedByTool = false;    // this tool turned it on (so it turns it off again)
};

class KisToolEraserFactory : public KisToolPaintFactoryBase
{
public:
    KisToolEraserFactory()
        : KisToolPaintFactoryBase("KritaShape/KisToolEraser")
    {
        setToolTip(i18n("Eraser Tool"));
        setSection(ToolBoxSection::PSOrder);
        setIconName(koIconNameCStr("krita_tool_eraser"));
        // no key: E is the eraser switch of the brush toolbar already (a second user of the key would make it ambiguous)
        setPriority(25);          // after Burn (24)
        setToolBoxPriority(123);  // in the toolbox: after Brush (120), Smudge (121), Soften (122)
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolEraserFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolEraser(canvas);
    }
};

#endif // KIS_TOOL_ERASER_H_
