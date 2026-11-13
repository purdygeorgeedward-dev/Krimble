/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_HEAL_H_
#define KIS_TOOL_HEAL_H_

#include "kis_tool_brush.h"

#include <KoIcon.h>
#include <kis_icon.h>
#include <QPointer>

class KoCanvasBase;
class QPushButton;

// Krimble 2026-11-13 (task 77): George: "We need the healing brush." The Healing Brush: Krita's Clone (duplicate) brush with its Healing option on, loaded when the tool starts,
// the same way Smudge and Soften load their brushes. A phone has no Ctrl-click to pick the source, so the tool has a "Set source" button (and the first tap after the tool starts
// sets the source): the next tap on the picture is the point to copy from, then painting heals from there (the texture of the source, the color and brightness of the target).
class KisToolHeal : public KisToolBrush
{
    Q_OBJECT

public:
    explicit KisToolHeal(KoCanvasBase *canvas);
    ~KisToolHeal() override;

    void activate(const QSet<KoShape*> &shapes) override;
    void beginPrimaryAction(KoPointerEvent *event) override;
    QWidget *createOptionWidget() override;

private Q_SLOTS:
    void slotArmSource();

private:
    bool m_sourceArmed {true};   // the next tap sets the source (true at start)
    QPointer<QPushButton> m_sourceButton;
    void updateButton();
};

class KisToolHealFactory : public KisToolPaintFactoryBase
{
public:
    KisToolHealFactory()
        : KisToolPaintFactoryBase("KritaShape/KisToolHeal")
    {
        setToolTip(i18n("Healing Brush Tool"));
        setSection(ToolBoxSection::PSOrder);
        setIconName(koIconNameCStr("krita_tool_clone_stamp"));
        setPriority(23);
        setToolBoxPriority(124);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolHealFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolHeal(canvas);
    }
};

#endif // KIS_TOOL_HEAL_H_
