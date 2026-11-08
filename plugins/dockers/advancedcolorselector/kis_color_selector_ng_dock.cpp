/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2010 Adam Celarek <kdedev at xibo dot at>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "kis_color_selector_ng_dock.h"

#include <klocalizedstring.h>
#include "kis_canvas2.h"

#include "kis_color_selector_ng_docker_widget.h"

#include <QGuiApplication>
#include <QScreen>


// KisColorSelectorNgDock::KisColorSelectorNgDock()
KisColorSelectorNgDock::KisColorSelectorNgDock(bool classic)
    : QDockWidget()
{
    // m_colorSelectorNgWidget = new KisColorSelectorNgDockerWidget(this);
    m_colorSelectorNgWidget = new KisColorSelectorNgDockerWidget(this, classic);   // Krimble 2026-11-08

    setWidget(m_colorSelectorNgWidget);
    m_colorSelectorNgWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // KRIMBLE: the color selector was far too tall on Android (about 910px of
    // a 2340px portrait screen). Cap its content height at 42% of the
    // screen's shorter side (about 454px on a 1080px-wide portrait screen,
    // roughly half of before). In landscape the docker is already shorter
    // than this cap, so it is unaffected.
    // if (QScreen *scr = QGuiApplication::primaryScreen()) {
    //     const QSize screenSize = scr->availableGeometry().size();
    //     const int shortSide = qMin(screenSize.width(), screenSize.height());
    //     m_colorSelectorNgWidget->setMaximumHeight(int(shortSide * 0.42));
    // }
    // KRIMBLE 2026-11-04: George: the Color panel has a hard size limit and the corner gadget cannot
    // enlarge it (Color Studio needs more than 42% of the screen's short side). The cap above now
    // applies only while the panel is docked; a floating panel has no height limit.
    auto krimbleApplyHeightCap = [this](bool floating) {
        if (floating) {
            m_colorSelectorNgWidget->setMaximumHeight(QWIDGETSIZE_MAX);
            return;
        }
        // if (QScreen *scr = QGuiApplication::primaryScreen()) {
        //     const QSize screenSize = scr->availableGeometry().size();
        //     const int shortSide = qMin(screenSize.width(), screenSize.height());
        //     m_colorSelectorNgWidget->setMaximumHeight(int(shortSide * 0.42));
        // }
        // KRIMBLE 2026-11-06: George (1.0.45-beta, screenshot of the docked Color panel): "It's still trapped in a scrolling box." Docked, the panel showed
        // only the top 454 px of Color Studio (the 42% cap above) inside a scrolling box, with the rest of the panel empty. That cap was made for the old
        // selector (910 px tall); Color Studio fills the panel and scrolls only when the panel is shorter than it. No cap, docked or floating.
        m_colorSelectorNgWidget->setMaximumHeight(QWIDGETSIZE_MAX);
    };
    krimbleApplyHeightCap(isFloating());
    connect(this, &QDockWidget::topLevelChanged, this, krimbleApplyHeightCap);

    // KRIMBLE: renamed from "Advanced Color Selector" - original kept below.
    // setWindowTitle(i18n("Advanced Color Selector"));
    // KRIMBLE 2026-10-02: panel renamed again, "Color Selector" -> "Color".
    // setWindowTitle(i18n("Color Selector"));
    // setWindowTitle(i18n("Color"));
    setWindowTitle(classic ? i18n("Color Ring + Triangle") : i18n("Color"));   // Krimble 2026-11-08: George: "bring back advance color panel as \"color ring + triangle\"."
}

void KisColorSelectorNgDock::setCanvas(KoCanvasBase * canvas)
{
    setEnabled(canvas != nullptr);
    KisCanvas2* kisCanvas = dynamic_cast<KisCanvas2*>(canvas);
    m_colorSelectorNgWidget->setCanvas(kisCanvas);
}

void KisColorSelectorNgDock::unsetCanvas()
{
    setEnabled(false);
    m_colorSelectorNgWidget->unsetCanvas();
}

