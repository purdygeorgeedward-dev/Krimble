/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "KisBrushColorWells.h"

#include <QWidgetAction>

#include <klocalizedstring.h>
#include <kactioncollection.h>

#include <KoDualColorButton.h>
#include <KisViewManager.h>
#include <kis_canvas_resource_provider.h>
#include <kis_display_color_converter.h>
#include <kis_canvas2.h>

KisBrushColorWells::KisBrushColorWells(KisViewManager *viewManager)
    : QObject(viewManager)
{
    const KoColorDisplayRendererInterface *displayRenderer =
        KisDisplayColorConverter::dumbConverterInstance()->displayRendererInterface();
    KoDualColorButton *button = new KoDualColorButton(viewManager->canvasResourceProvider(), displayRenderer,
                                                      viewManager->mainWindowAsQWidget(), viewManager->mainWindowAsQWidget());
    button->setFixedSize(40, 40);

    connect(button, SIGNAL(foregroundColorChanged(KoColor)), viewManager->canvasResourceProvider(), SLOT(slotSetFGColor(KoColor)));
    connect(button, SIGNAL(backgroundColorChanged(KoColor)), viewManager->canvasResourceProvider(), SLOT(slotSetBGColor(KoColor)));
    connect(viewManager->canvasResourceProvider(), SIGNAL(sigBGColorChanged(KoColor)), button, SLOT(setBackgroundColor(KoColor)));
    connect(viewManager->canvasResourceProvider(), SIGNAL(sigFGColorChanged(KoColor)), button, SLOT(setForegroundColor(KoColor)));
    connect(viewManager, &KisViewManager::viewChanged, this, [button, viewManager]() {
        if (viewManager->canvasBase()) {
            button->setDisplayRenderer(viewManager->canvasBase()->displayColorConverter()->displayRendererInterface());
        }
    });

    QWidgetAction *action = new QWidgetAction(this);
    action->setText(i18n("Brush: Foreground and Background Colors"));
    action->setDefaultWidget(button);
    viewManager->actionCollection()->addAction(QStringLiteral("brush_color_wells"), action);
}
