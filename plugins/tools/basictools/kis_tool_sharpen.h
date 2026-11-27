/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_SHARPEN_H_
#define KIS_TOOL_SHARPEN_H_

#include "kis_tool_brush.h"

#include <KoIcon.h>
#include <kis_icon.h>

class KoCanvasBase;

class KisToolSharpen : public KisToolBrush
{
    Q_OBJECT

public:
    explicit KisToolSharpen(KoCanvasBase *canvas);
    ~KisToolSharpen() override;

    void activate(const QSet<KoShape*> &shapes) override;
};

class KisToolSharpenFactory : public KisToolPaintFactoryBase
{
public:
    KisToolSharpenFactory()
        : KisToolPaintFactoryBase("KritaShape/KisToolSharpen")
    {
        setToolTip(i18n("Sharpen"));
        setSection(ToolBoxSection::PSOrder);
        setIconName(koIconNameCStr("krita_tool_sharpen"));
        setPriority(26);
        setToolBoxPriority(136);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolSharpenFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolSharpen(canvas);
    }
};

#endif
