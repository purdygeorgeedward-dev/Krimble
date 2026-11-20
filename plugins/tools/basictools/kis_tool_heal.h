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
        setToolBoxPriority(122);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolHealFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolHeal(canvas);
    }
};

#endif // KIS_TOOL_HEAL_H_
