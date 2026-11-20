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
        setPriority(21);
        setToolBoxPriority(135);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolSmudgeFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolSmudge(canvas);
    }
};

#endif // KIS_TOOL_SMUDGE_H_
