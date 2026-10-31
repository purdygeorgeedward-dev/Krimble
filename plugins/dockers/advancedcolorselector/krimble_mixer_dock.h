// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-10-30: the Mixer panel. George: "I specifically asked for a color mixing scratch pad. That's why there's a mockup of that. It should be a
// panel." and, asked whether it should stay in Color Studio or be its own panel: "I think it is a good idea for a panel." The mixing pad that was the
// Mixer tab of Color Studio (krimble_color_studio.h) is a panel of its own, "Mixer", opened from the Window menu. The pad is independent of the picture:
// its raster is never part of the document or its undo stack. It paints with the foreground color; Sample sets the foreground color from the pad;
// + Palette adds the foreground color to the Color panel's saved swatches.
#pragma once

#include <QDockWidget>
#include <QPointer>
#include <KoCanvasObserverBase.h>
#include <KoCanvasBase.h>
#include <KoCanvasResourceProvider.h>

#include "krimble_color_studio.h"

class KrimbleMixerDock : public QDockWidget, public KoCanvasObserverBase
{
public:
    KrimbleMixerDock() : QDockWidget(QObject::tr("Mixer"))
    {
        auto *panel = new QWidget(this);
        auto *mixLayout = new QVBoxLayout(panel);
        mixLayout->setContentsMargins(6, 6, 6, 6);
        mixLayout->setSpacing(5);

        auto *mixTools = new QHBoxLayout;
        auto *paintButton = new QPushButton(tr("Paint"), panel);
        auto *eraseButton = new QPushButton(tr("Erase"), panel);
        auto *sampleButton = new QPushButton(tr("Sample"), panel);
        for (QPushButton *button : {paintButton, eraseButton, sampleButton}) {
            button->setCheckable(true);
            button->setMinimumHeight(38);
            mixTools->addWidget(button);
        }
        paintButton->setChecked(true);
        mixLayout->addLayout(mixTools);

        m_pad = new KrimbleMixingPad(panel);
        mixLayout->addWidget(new QLabel(tr("Oil Bristle  \u2022  paint and blend colors"), panel));
        mixLayout->addWidget(m_pad, 1);

        auto *brushControls = new QHBoxLayout;
        brushControls->addWidget(new QLabel(tr("Size"), panel));
        auto *brushSize = new QSpinBox(panel);
        brushSize->setRange(1, 100); brushSize->setValue(18); brushSize->setMinimumHeight(38);
        brushControls->addWidget(brushSize);
        brushControls->addWidget(new QLabel(tr("Opacity"), panel));
        auto *brushOpacity = new QSpinBox(panel);
        brushOpacity->setRange(1, 100); brushOpacity->setValue(85); brushOpacity->setMinimumHeight(38);
        brushOpacity->setSuffix(QStringLiteral("%"));
        brushControls->addWidget(brushOpacity);
        mixLayout->addLayout(brushControls);

        auto *padActions = new QHBoxLayout;
        auto *undoButton = new QPushButton(tr("Undo"), panel);
        auto *redoButton = new QPushButton(tr("Redo"), panel);
        auto *clearButton = new QPushButton(tr("Clear"), panel);
        auto *addMixed = new QPushButton(tr("+ Palette"), panel);
        for (QPushButton *button : {undoButton, redoButton, clearButton, addMixed}) {
            button->setMinimumHeight(38);
            padActions->addWidget(button);
        }
        mixLayout->addLayout(padActions);
        setWidget(panel);

        connect(paintButton, &QPushButton::clicked, this, [=]() {
            paintButton->setChecked(true); eraseButton->setChecked(false);
            sampleButton->setChecked(false); m_pad->setEraser(false);
        });
        connect(eraseButton, &QPushButton::clicked, this, [=]() {
            paintButton->setChecked(false); eraseButton->setChecked(true);
            sampleButton->setChecked(false); m_pad->setEraser(true);
        });
        connect(sampleButton, &QPushButton::clicked, this, [=]() {
            paintButton->setChecked(false); eraseButton->setChecked(false);
            sampleButton->setChecked(true); m_pad->setSampling(true);
        });
        connect(brushSize, qOverload<int>(&QSpinBox::valueChanged), m_pad, &KrimbleMixingPad::setBrushSize);
        connect(brushOpacity, qOverload<int>(&QSpinBox::valueChanged), m_pad, &KrimbleMixingPad::setOpacity);
        connect(undoButton, &QPushButton::clicked, m_pad, &KrimbleMixingPad::undo);
        connect(redoButton, &QPushButton::clicked, m_pad, &KrimbleMixingPad::redo);
        connect(clearButton, &QPushButton::clicked, m_pad, &KrimbleMixingPad::clearPad);
        // + Palette: the foreground color joins the Color panel's saved swatches (the same saved list; the Color panel shows it within a moment)
        connect(addMixed, &QPushButton::clicked, this, [this]() {
            if (!m_canvas) return;
            const QColor current = m_canvas->resourceManager()->foregroundColor().toQColor();
            if (!current.isValid()) return;
            QSettings settings;
            QStringList items = settings.value(QStringLiteral("Krimble/ColorStudioSwatches")).toStringList();
            const QString name = current.name(QColor::HexRgb);
            if (!items.contains(name)) items << name;
            while (items.size() > 24) items.removeFirst();
            settings.setValue(QStringLiteral("Krimble/ColorStudioSwatches"), items);
        });
        // Sample: the sampled color becomes the foreground color
        m_pad->onSample = [this](QColor c) {
            if (m_canvas && c.isValid())
                m_canvas->resourceManager()->setForegroundColor(KoColor(c, KoColorSpaceRegistry::instance()->rgb8()));
        };
        // the pad paints with the foreground color; it follows changes made elsewhere (only while the panel is visible)
        m_syncTimer.setInterval(350);
        connect(&m_syncTimer, &QTimer::timeout, this, [this]() {
            if (!m_canvas || !isVisible()) return;
            const QColor foreground = m_canvas->resourceManager()->foregroundColor().toQColor();
            if (foreground.isValid()) m_pad->setColor(foreground);
        });
        m_syncTimer.start();
    }

    QString observerName() override { return QStringLiteral("KrimbleMixerDock"); }
    void setCanvas(KoCanvasBase *canvas) override
    {
        setEnabled(canvas != nullptr);
        m_canvas = dynamic_cast<KisCanvas2*>(canvas);
        if (m_canvas && m_canvas->resourceManager()) {
            const QColor foreground = m_canvas->resourceManager()->foregroundColor().toQColor();
            if (foreground.isValid()) m_pad->setColor(foreground);
        }
    }
    void unsetCanvas() override { m_canvas = nullptr; setEnabled(false); }

private:
    QPointer<KisCanvas2> m_canvas;
    KrimbleMixingPad *m_pad = nullptr;
    QTimer m_syncTimer;
};
