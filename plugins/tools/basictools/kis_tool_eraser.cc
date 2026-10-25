/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_eraser.h"

#include <KoCanvasBase.h>

#include <kis_action.h>
#include <kis_action_manager.h>
#include <kis_canvas2.h>
#include <KisViewManager.h>

KisToolEraser::KisToolEraser(KoCanvasBase *canvas)
    : KisToolBrush(canvas)
{
    setObjectName("tool_eraser");
}

KisToolEraser::~KisToolEraser()
{
}

// the eraser switch of the brush toolbar ("erase_action"): turning it on goes the same way as pressing the button (KisPaintopBox::slotToggleEraseMode)
void KisToolEraser::switchEraserOn()
{
    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager() || !canvas2->viewManager()->actionManager()) return;
    KisAction *eraseAction = canvas2->viewManager()->actionManager()->actionByName(QStringLiteral("erase_action"));
    if (!eraseAction) return;
    if (!eraseAction->isChecked()) {
        eraseAction->setChecked(true);
        m_switchedByTool = true;
    }
}

void KisToolEraser::activate(const QSet<KoShape*> &shapes)
{
    KisToolBrush::activate(shapes);

    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (canvas2 && canvas2->viewManager() && canvas2->viewManager()->actionManager()) {
        KisAction *eraseAction = canvas2->viewManager()->actionManager()->actionByName(QStringLiteral("erase_action"));
        m_switchWasOn = eraseAction && eraseAction->isChecked();
    }
    m_switchedByTool = false;
    switchEraserOn();
}

void KisToolEraser::deactivate()
{
    // the switch goes back to what it was before this tool started (if the tool turned it on)
    if (m_switchedByTool && !m_switchWasOn) {
        KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
        if (canvas2 && canvas2->viewManager() && canvas2->viewManager()->actionManager()) {
            KisAction *eraseAction = canvas2->viewManager()->actionManager()->actionByName(QStringLiteral("erase_action"));
            if (eraseAction && eraseAction->isChecked()) eraseAction->setChecked(false);
        }
    }
    m_switchedByTool = false;
    KisToolBrush::deactivate();
}

void KisToolEraser::beginPrimaryAction(KoPointerEvent *event)
{
    // the switch can be turned off again while this tool is active (a new brush picked from the brush panel puts it back to off): a stroke of the Eraser
    // always erases
    switchEraserOn();
    KisToolBrush::beginPrimaryAction(event);
}
