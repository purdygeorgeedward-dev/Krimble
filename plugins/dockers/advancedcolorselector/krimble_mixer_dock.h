// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-10-30: the Mixer panel. George: "I specifically asked for a color mixing scratch pad. That's why there's a mockup of that. It should be a
// panel." and, asked whether it should stay in Color Studio or be its own panel: "I think it is a good idea for a panel." The mixing pad that was the
// Mixer tab of Color Studio (krimble_color_studio.h) is a panel of its own, "Mixer", opened from the Window menu. The pad is independent of the picture:
// its raster is never part of the document or its undo stack. It paints with the foreground color; Sample sets the foreground color from the pad;
// + Palette adds the foreground color to the Color panel's saved swatches.
#pragma once

#include "krimble_color_palettes.h"   // Krimble 2026-11-08
#include <QDockWidget>
#include <QPointer>
#include <QScrollArea>   // Krimble 2026-11-05: the Mixer panel scrolls instead of overlapping when it is small
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
        // Krimble 2026-11-06 (2nd): George: "Let's fix all these new panels and make them look sharp. Make them fit on screen." Measured with the real code (real Qt, stand-ins
        // for Krita's classes): the panel needed 269 x 383 logical px, wider than a docked panel (about 242), so it needed a sideways scroll bar. Its text is 15% smaller like the
        // Color panel's, the buttons are 30 px tall (were 38), Size and Opacity are a thin slider with a number box each, as in the mockup (they were two number boxes in one
        // row), and the pad may be shorter. The pad's picture is sharp (one pixel per screen pixel).
        {
            QFont smaller = panel->font();
            if (smaller.pixelSize() > 0) smaller.setPixelSize(qMax(8, int(smaller.pixelSize() * 0.85)));
            else smaller.setPointSizeF(qMax(6.0, smaller.pointSizeF() * 0.85));
            panel->setFont(smaller);
        }
        // the buttons of this panel have less padding, so a row of four fits a docked panel (3 px above and below, 5 px at the sides)
        panel->setStyleSheet(QStringLiteral("QPushButton { padding: 3px 5px; }"));
        auto *mixLayout = new QVBoxLayout(panel);
        // mixLayout->setContentsMargins(5, 5, 5, 5);
        // mixLayout->setSpacing(4);
        mixLayout->setContentsMargins(3, 3, 3, 3);   // Krimble 2026-11-06 (3rd): George: "more and more unnecessary negative space": 3 px margins, 2 px between the parts (were 5 and 4)
        mixLayout->setSpacing(2);

        auto *mixTools = new QHBoxLayout;
        auto *paintButton = new QPushButton(tr("Paint"), panel);
        auto *eraseButton = new QPushButton(tr("Erase"), panel);
        auto *sampleButton = new QPushButton(tr("Sample"), panel);
        for (QPushButton *button : {paintButton, eraseButton, sampleButton}) {
            button->setCheckable(true);
            // button->setMinimumHeight(38);
            button->setMinimumHeight(30);
            button->setMinimumWidth(0);
            mixTools->addWidget(button);
        }
        paintButton->setChecked(true);
        mixLayout->addLayout(mixTools);

        m_pad = new KrimbleMixingPad(panel);
        // mixLayout->addWidget(new QLabel(tr("Oil Bristle  \u2022  paint and blend colors"), panel));
        // mixLayout->addWidget(new QLabel(tr("Soft Round  \u2022  paint and blend colors"), panel));   // Krimble 2026-11-06: the default brush is a soft round brush now
        // mixLayout->addWidget(new QLabel(tr("Mixing & Brush Test Pad  \u2022  soft round brush"), panel));
        {
            auto *title = new QLabel(tr("Mixing & Brush Test Pad"), panel);   // Krimble 2026-11-06 (2nd): the mockup's title; it may shrink (clip) in a narrow panel
            title->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
            mixLayout->addWidget(title);
        }
        mixLayout->addWidget(m_pad, 1);

        // auto *brushControls = new QHBoxLayout;
        // brushControls->addWidget(new QLabel(tr("Size"), panel));
        // auto *brushSize = new QSpinBox(panel);
        // brushSize->setRange(1, 100); brushSize->setValue(18); brushSize->setMinimumHeight(38);
        // brushControls->addWidget(brushSize);
        // brushControls->addWidget(new QLabel(tr("Opacity"), panel));
        // auto *brushOpacity = new QSpinBox(panel);
        // brushOpacity->setRange(1, 100); brushOpacity->setValue(85); brushOpacity->setMinimumHeight(38);
        // brushOpacity->setSuffix(QStringLiteral("%"));
        // brushControls->addWidget(brushOpacity);
        // mixLayout->addLayout(brushControls);
        QSpinBox *brushSize = nullptr;
        QSpinBox *brushOpacity = nullptr;
        for (int row = 0; row < 2; ++row) {
            auto *line = new QHBoxLayout;
            line->setSpacing(4);
            auto *name = new QLabel(row == 0 ? tr("Size") : tr("Opacity"), panel);
            name->setFixedWidth(panel->fontMetrics().horizontalAdvance(tr("Opacity")) + 12);   // 12 px extra: the style adds some padding
            line->addWidget(name);
            auto *slider = new QSlider(Qt::Horizontal, panel);
            slider->setRange(1, 100);
            slider->setMinimumHeight(16);
            line->addWidget(slider, 1);
            auto *number = new QSpinBox(panel);
            number->setRange(1, 100);
            // number->setFixedWidth(62);   // the same width in both rows, so the sliders end at the same place
            // number->setFixedHeight(20);
            number->setFixedWidth(76);   // Krimble 2026-11-08: the fields of the style sheet have padding and a frame now (the Color panel's boxes showed one digit at 46 px): "100%" needs about 76 px; the same in both rows
            number->setFixedHeight(24);
            if (row == 1) number->setSuffix(QStringLiteral("%"));
            line->addWidget(number);
            number->setValue(row == 0 ? 18 : 85);
            slider->setValue(row == 0 ? 18 : 85);
            connect(slider, &QSlider::valueChanged, number, &QSpinBox::setValue);
            connect(number, qOverload<int>(&QSpinBox::valueChanged), slider, &QSlider::setValue);
            (row == 0 ? brushSize : brushOpacity) = number;
            mixLayout->addLayout(line);
        }

        auto *padActions = new QHBoxLayout;
        auto *undoButton = new QPushButton(tr("Undo"), panel);
        auto *redoButton = new QPushButton(tr("Redo"), panel);
        auto *clearButton = new QPushButton(tr("Clear"), panel);
        // auto *addMixed = new QPushButton(tr("+ Palette"), panel);
        auto *addMixed = new QPushButton(tr("+ Color"), panel);   // Krimble 2026-11-06 (2nd): shorter (was "+ Palette"); it adds the foreground color to the palette
        for (QPushButton *button : {undoButton, redoButton, clearButton, addMixed}) {
            // button->setMinimumHeight(38);
            button->setMinimumHeight(30);
            button->setMinimumWidth(0);
            padActions->addWidget(button);
        }
        mixLayout->addLayout(padActions);
        // setWidget(panel);
        // Krimble 2026-11-05: George (Mixer screenshots): in a small panel the Size and Opacity row sat on top of the pad. The panel is inside a scroll
        // area now, so a panel smaller than its contents scrolls (the contents keep their own minimum size).
        {
            auto *scroll = new QScrollArea(this);
            scroll->setWidgetResizable(true);
            scroll->setFrameShape(QFrame::NoFrame);
            scroll->setWidget(panel);
            setWidget(scroll);
        }

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
            // QSettings settings;
            // QStringList items = settings.value(QStringLiteral("Krimble/ColorStudioSwatches")).toStringList();
            // const QString name = current.name(QColor::HexRgb);
            // if (!items.contains(name)) items << name;
            // while (items.size() > 24) items.removeFirst();
            // settings.setValue(QStringLiteral("Krimble/ColorStudioSwatches"), items);
            KrimblePaletteStore::addColor(current);   // Krimble 2026-11-08: the color joins the ACTIVE palette (a GPL file); the Color panel shows it within a moment
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
