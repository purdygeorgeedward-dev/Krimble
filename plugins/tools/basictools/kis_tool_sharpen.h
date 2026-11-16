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

// Krimble 2026-11-16 (722nd): George (build 64, the order of the toolbox): "Blur / Sharpen". The Sharpen tool: the same filter brush as the
// Blur tool (KisToolSoften), with the Sharpen filter in place of Gaussian Blur. It is a brush: it sharpens only where it is brushed.
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
        setToolBoxPriority(136);   // in the toolbox: after Blur (134)
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolSharpenFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolSharpen(canvas);
    }
};

#endif // KIS_TOOL_SHARPEN_H_
