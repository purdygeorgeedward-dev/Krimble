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

    bool m_switchWasOn = false;
    bool m_switchedByTool = false;
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
        setPriority(25);
        setToolBoxPriority(124);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolEraserFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolEraser(canvas);
    }
};

#endif
