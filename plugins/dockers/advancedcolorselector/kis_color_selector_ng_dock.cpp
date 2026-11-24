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
#include <QResizeEvent>
#include <QTimer>
#include "krimble_color_studio.h"

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
    m_krimbleDockedMin = minimumSize();
    m_krimbleDockedMax = maximumSize();
    connect(this, &QDockWidget::topLevelChanged, this, [this](bool floating) {
        if (!floating) {
            setMinimumSize(m_krimbleDockedMin);
            setMaximumSize(m_krimbleDockedMax);
            return;
        }
        QTimer::singleShot(0, this, [this]() { krimbleFitFloating(true); });
    });

    setWindowTitle(classic ? i18n("HSV Triangle Color Picker") : i18n("Color Lab"));
}

void KisColorSelectorNgDock::krimbleFitFloating(bool snap)
{
    if (!isFloating() || m_krimbleFitting) return;
    KrimbleColorStudio *studio = m_colorSelectorNgWidget ? m_colorSelectorNgWidget->findChild<KrimbleColorStudio *>() : nullptr;
    QWidget *view = studio ? studio->krimbleRoomView() : nullptr;
    if (!studio || !view) return;
    m_krimbleFitting = true;
    const int chromeW = width() - view->width();
    const int chromeH = height() - view->height();
    const int maxW = chromeW + studio->maximumWidth();
    if (snap && width() != maxW) resize(maxW, height());
    const int fewest = studio->krimbleNeededViewHeight(0);
    const int all = studio->krimbleNeededViewHeight(10);
    if (fewest > 0 && all > 0) {
        setMinimumSize(1, chromeH + fewest);
        setMaximumSize(maxW, chromeH + all);
        if (snap) resize(maxW, chromeH + all);
    }
    m_krimbleFitting = false;
    if (snap) QTimer::singleShot(0, this, [this]() { krimbleFitFloating(false); });
}

void KisColorSelectorNgDock::resizeEvent(QResizeEvent *event)
{
    QDockWidget::resizeEvent(event);
    if (isFloating() && event->size().width() != event->oldSize().width()) {
        QTimer::singleShot(0, this, [this]() { krimbleFitFloating(false); });
    }
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

