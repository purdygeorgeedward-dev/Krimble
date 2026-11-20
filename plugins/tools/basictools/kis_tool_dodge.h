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

class KisToolDodge : public KisToolBrush
{
    Q_OBJECT

public:
    explicit KisToolDodge(KoCanvasBase *canvas);
    ~KisToolDodge() override;

    void activate(const QSet<KoShape*> &shapes) override;
    void deactivate() override;

private:
    KoColor m_savedForeground;
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
        setShortcut(QKeySequence(Qt::Key_O));
        setPriority(23);
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
