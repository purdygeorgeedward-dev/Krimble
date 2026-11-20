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
    m_colorSelectorNgWidget = new KisColorSelectorNgDockerWidget(this, classic);

    setWidget(m_colorSelectorNgWidget);
    m_colorSelectorNgWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto krimbleApplyHeightCap = [this](bool floating) {
        if (floating) {
            m_colorSelectorNgWidget->setMaximumHeight(QWIDGETSIZE_MAX);
            return;
        }
        m_colorSelectorNgWidget->setMaximumHeight(QWIDGETSIZE_MAX);
    };
    krimbleApplyHeightCap(isFloating());
    connect(this, &QDockWidget::topLevelChanged, this, krimbleApplyHeightCap);

    setWindowTitle(classic ? i18n("HSV Triangle Color Picker") : i18n("Color Lab"));
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

