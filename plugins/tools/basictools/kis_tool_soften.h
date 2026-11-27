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
        setToolTip(i18n("Blur"));
        setSection(ToolBoxSection::PSOrder);
        setIconName(koIconNameCStr("krita_tool_soften"));
        setShortcut(QKeySequence(Qt::Key_U));
        setPriority(22);
        setToolBoxPriority(134);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolSoftenFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolSoften(canvas);
    }
};

#endif
