/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_BURN_H_
#define KIS_TOOL_BURN_H_

#include "kis_tool_brush.h"

#include <KoIcon.h>
#include <kis_icon.h>
#include <KoColor.h>

class KoCanvasBase;

class KisToolBurn : public KisToolBrush
{
    Q_OBJECT

public:
    explicit KisToolBurn(KoCanvasBase *canvas);
    ~KisToolBurn() override;

    void activate(const QSet<KoShape*> &shapes) override;
    void deactivate() override;

private:
    KoColor m_savedForeground;
    bool m_foregroundSaved {false};
};

class KisToolBurnFactory : public KisToolPaintFactoryBase
{
public:
    KisToolBurnFactory()
        : KisToolPaintFactoryBase("KritaShape/KisToolBurn")
    {
        setToolTip(i18n("Burn Tool"));
        setSection(ToolBoxSection::PSOrder);
        setIconName(koIconNameCStr("krita_tool_burn"));
        setShortcut(QKeySequence(Qt::Key_K));
        setPriority(24);
        setToolBoxPriority(124);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolBurnFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolBurn(canvas);
    }
};

#endif // KIS_TOOL_BURN_H_
