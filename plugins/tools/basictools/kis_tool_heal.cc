/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_heal.h"
#include "KrimbleEmbeddedPreset.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>

#include <brushengine/kis_paintop_preset.h>
#include <brushengine/kis_paintop_settings.h>
#include <brushengine/kis_paint_information.h>

#include <kis_canvas2.h>
#include <KisViewManager.h>
#include <kis_canvas_resource_provider.h>

KisToolHeal::KisToolHeal(KoCanvasBase *canvas)
    : KisToolBrush(canvas)
{
    setObjectName("tool_heal");
}

KisToolHeal::~KisToolHeal()
{
}

void KisToolHeal::activate(const QSet<KoShape*> &shapes)
{
    KisToolBrush::activate(shapes);

    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager()) {
        return;
    }

    // the built-in Clone (duplicate) brush, with Healing on, 40 px to start with
    KisPaintOpPresetSP preset = krimbleLoadEmbeddedPreset(QStringLiteral("duplicate"));
    if (preset && preset->settings()) {
        preset->settings()->setProperty(QStringLiteral("Duplicateop/Healing"), true);
        preset->settings()->setPaintOpSize(40.0);
        krimbleSetBrushPreset(canvas2->viewManager(), preset);
    }
    m_sourceArmed = true;
    updateButton();
    canvas2->viewManager()->showFloatingMessage(i18n("Healing Brush: tap the spot to copy from, then paint over the flaw"), QIcon(), 3000);
}

// Armed: this tap is the source point (the same as Ctrl-click for the Clone brush); nothing is painted. Otherwise: an ordinary stroke.
void KisToolHeal::beginPrimaryAction(KoPointerEvent *event)
{
    if (m_sourceArmed) {
        KisPaintOpPresetSP preset = currentPaintOpPreset();
        if (preset && preset->settings()) {
            const KisPaintInformation info(convertToPixelCoord(event));
            preset->settings()->mousePressEvent(info, Qt::ControlModifier, currentNode());
            m_sourceArmed = false;
            updateButton();
            KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
            if (canvas2 && canvas2->viewManager()) {
                canvas2->viewManager()->showFloatingMessage(i18n("Source set"), QIcon(), 1500);
            }
        }
        event->accept();
        return;
    }
    KisToolBrush::beginPrimaryAction(event);
}

void KisToolHeal::slotArmSource()
{
    m_sourceArmed = true;
    updateButton();
}

void KisToolHeal::updateButton()
{
    if (m_sourceButton) {
        m_sourceButton->setText(m_sourceArmed ? i18n("Tap the picture to set the source...") : i18n("Set source"));
    }
}

QWidget *KisToolHeal::createOptionWidget()
{
    QWidget *base = KisToolBrush::createOptionWidget();
    QWidget *box = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(box);
    layout->setContentsMargins(0, 0, 0, 0);
    m_sourceButton = new QPushButton(box);
    connect(m_sourceButton, SIGNAL(clicked()), this, SLOT(slotArmSource()));
    layout->addWidget(m_sourceButton);
    if (base) layout->addWidget(base);
    updateButton();
    return box;
}
