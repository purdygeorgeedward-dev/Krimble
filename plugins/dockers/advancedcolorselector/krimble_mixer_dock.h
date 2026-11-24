// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "krimble_color_palettes.h"
#include <QDockWidget>
#include <QPointer>
#include <QScrollArea>
#include <KoCanvasObserverBase.h>
#include <KoCanvasBase.h>
#include <KoCanvasResourceProvider.h>

#include "krimble_color_studio.h"
#include <kis_scratch_pad.h>
#include <kis_canvas2.h>
#include <KisViewManager.h>
#include <kis_canvas_resource_provider.h>

class KrimbleMixerDock : public QDockWidget, public KoCanvasObserverBase
{
public:
    KrimbleMixerDock() : QDockWidget(QObject::tr("Mixer"))
    {
        auto *panel = new QWidget(this);
        {
            QFont smaller = panel->font();
            if (smaller.pixelSize() > 0) smaller.setPixelSize(qMax(8, int(smaller.pixelSize() * 0.85)));
            else smaller.setPointSizeF(qMax(6.0, smaller.pointSizeF() * 0.85));
            panel->setFont(smaller);
        }
        panel->setStyleSheet(QStringLiteral("QPushButton { padding: 3px 5px; }"));
        auto *mixLayout = new QVBoxLayout(panel);
        mixLayout->setContentsMargins(3, 3, 3, 3);
        mixLayout->setSpacing(2);

        auto *mixTools = new QHBoxLayout;
        auto *paintButton = new QPushButton(tr("Paint"), panel);
        auto *eraseButton = new QPushButton(tr("Erase"), panel);
        auto *sampleButton = new QPushButton(tr("Sample"), panel);
        for (QPushButton *button : {paintButton, eraseButton, sampleButton}) {
            button->setCheckable(true);
            button->setMinimumHeight(30);
            button->setMinimumWidth(0);
            mixTools->addWidget(button);
        }
        paintButton->setChecked(true);
        mixLayout->addLayout(mixTools);

        m_pad = new KrimbleMixingPad(panel);
        {
            auto *title = new QLabel(tr("Mixing & Brush Test Pad"), panel);
            title->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
            mixLayout->addWidget(title);
        }
        mixLayout->addWidget(m_pad, 0);
        m_pad->hide();
        m_realPadHolder = new QWidget(panel);
        m_realPadLayout = new QVBoxLayout(m_realPadHolder);
        m_realPadLayout->setContentsMargins(0, 0, 0, 0);
        m_realPadHolder->setMinimumHeight(qRound(600.0 / qMax<qreal>(1.0, qApp->devicePixelRatio())));
        mixLayout->addWidget(m_realPadHolder, 1);

        QSpinBox *brushSize = nullptr;
        QSpinBox *brushOpacity = nullptr;
        for (int row = 0; row < 2; ++row) {
            auto *line = new QHBoxLayout;
            line->setSpacing(4);
            auto *name = new QLabel(row == 0 ? tr("Size") : tr("Opacity"), panel);
            name->setFixedWidth(panel->fontMetrics().horizontalAdvance(tr("Opacity")) + 12);
            line->addWidget(name);
            auto *slider = new QSlider(Qt::Horizontal, panel);
            slider->setRange(1, 100);
            slider->setMinimumHeight(16);
            line->addWidget(slider, 1);
            auto *number = new QSpinBox(panel);
            number->setRange(1, 100);
            number->setFixedWidth(76);
            number->setFixedHeight(24);
            if (row == 1) number->setSuffix(QStringLiteral("%"));
            line->addWidget(number);
            number->setValue(row == 0 ? 18 : 85);
            slider->setValue(row == 0 ? 18 : 85);
            connect(slider, &QSlider::valueChanged, number, &QSpinBox::setValue);
            connect(number, qOverload<int>(&QSpinBox::valueChanged), slider, &QSlider::setValue);
            (row == 0 ? brushSize : brushOpacity) = number;
            mixLayout->addLayout(line);
            name->hide(); slider->hide(); number->hide();
        }

        auto *padActions = new QHBoxLayout;
        auto *undoButton = new QPushButton(tr("Undo"), panel);
        auto *redoButton = new QPushButton(tr("Redo"), panel);
        auto *clearButton = new QPushButton(tr("Clear"), panel);
        auto *addMixed = new QPushButton(tr("+ Color"), panel);
        for (QPushButton *button : {undoButton, redoButton, clearButton, addMixed}) {
            button->setMinimumHeight(30);
            button->setMinimumWidth(0);
            padActions->addWidget(button);
        }
        mixLayout->addLayout(padActions);
        {
            auto *scroll = new QScrollArea(this);
            scroll->setWidgetResizable(true);
            scroll->setFrameShape(QFrame::NoFrame);
            scroll->setWidget(panel);
            setWidget(scroll);
        }

        eraseButton->setText(tr("Move"));
        connect(paintButton, &QPushButton::clicked, this, [=]() {
            paintButton->setChecked(true); eraseButton->setChecked(false);
            sampleButton->setChecked(false); m_pad->setEraser(false);
            if (m_realPad) m_realPad->setModeType(QStringLiteral("painting"));
        });
        connect(eraseButton, &QPushButton::clicked, this, [=]() {
            paintButton->setChecked(false); eraseButton->setChecked(true);
            sampleButton->setChecked(false); m_pad->setEraser(true);
            if (m_realPad) m_realPad->setModeType(QStringLiteral("panning"));
        });
        connect(sampleButton, &QPushButton::clicked, this, [=]() {
            paintButton->setChecked(false); eraseButton->setChecked(false);
            sampleButton->setChecked(true); m_pad->setSampling(true);
            if (m_realPad) m_realPad->setModeType(QStringLiteral("colorsampling"));
        });
        connect(brushSize, qOverload<int>(&QSpinBox::valueChanged), m_pad, &KrimbleMixingPad::setBrushSize);
        connect(brushOpacity, qOverload<int>(&QSpinBox::valueChanged), m_pad, &KrimbleMixingPad::setOpacity);
        connect(undoButton, &QPushButton::clicked, m_pad, &KrimbleMixingPad::undo);
        connect(redoButton, &QPushButton::clicked, m_pad, &KrimbleMixingPad::redo);
        connect(clearButton, &QPushButton::clicked, m_pad, &KrimbleMixingPad::clearPad);
        undoButton->hide(); redoButton->hide();
        connect(clearButton, &QPushButton::clicked, this, [this]() { if (m_realPad) m_realPad->fillDefault(); });
        connect(addMixed, &QPushButton::clicked, this, [this]() {
            if (!m_canvas) return;
            const QColor current = m_canvas->resourceManager()->foregroundColor().toQColor();
            if (!current.isValid()) return;
            KrimblePaletteStore::addColor(current);
        });
        m_pad->onSample = [this](QColor c) {
            if (m_canvas && c.isValid())
                m_canvas->resourceManager()->setForegroundColor(KoColor(c, KoColorSpaceRegistry::instance()->rgb8()));
        };
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
        if (m_canvas && !m_realPad && m_canvas->viewManager() && m_canvas->viewManager()->canvasResourceProvider()) {
            m_realPad = new KisScratchPad(m_realPadHolder);
            m_realPad->setupScratchPad(m_canvas->viewManager()->canvasResourceProvider(), Qt::white);
            m_realPad->setModeManually(true);
            m_realPad->setModeType(QStringLiteral("painting"));
            m_realPad->setMinimumSize(50, 50);
            m_realPadLayout->addWidget(m_realPad);
        }
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
    QWidget *m_realPadHolder = nullptr;
    QVBoxLayout *m_realPadLayout = nullptr;
    QPointer<KisScratchPad> m_realPad;
};
