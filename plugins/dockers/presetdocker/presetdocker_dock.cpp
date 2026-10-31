/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "presetdocker_dock.h"

#include <QHBoxLayout>
#include <QVBoxLayout>   // Krimble 2026-10-30: the new controls
#include <QSlider>
#include <QLineEdit>   // Krimble 2026-10-30: the current brush's name
#include <QSpinBox>
#include <QLabel>
#include <QTimer>
#include <QSignalBlocker>
#include <kis_canvas_resource_provider.h>
#include <brushengine/kis_paintop_settings.h>
#include <kis_image_config.h>

#include <klocalizedstring.h>

#include <KoCanvasResourceProvider.h>
#include <KoCanvasBase.h>

#include "kis_canvas2.h"
#include "KisViewManager.h"
#include "kis_paintop_box.h"
#include "kis_paintop_presets_chooser_popup.h"
#include <brushengine/kis_paintop_preset.h>
#include "KisMainWindow.h"


PresetDockerDock::PresetDockerDock( )
    // KRIMBLE 2026-10-02: panel renamed from "Brush Presets" to "Brushes".
    // : QDockWidget(i18nc("@title:window Brush presets chooser panel", "Brush Presets"))
    : QDockWidget(i18nc("@title:window Brush presets chooser panel", "Brushes"))
    , m_canvas(0)
{
    m_presetChooser = new KisPaintOpPresetsChooserPopup(this);
    m_presetChooser->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_presetChooser->setResponsiveness(true);
    // setWidget(m_presetChooser);
    // Krimble 2026-10-30: George: "replace that panel. it's garbage." The panel was the preset picker and nothing else: no size, opacity or flow. It now has
    // three touch-sized rows on top (Size, Opacity, Flow: slider and number box) that set the current brush, and the preset picker below them.
    QWidget *panel = new QWidget(this);
    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(6, 6, 6, 6);
    panelLayout->setSpacing(4);

    // Krimble 2026-10-30: George: "the panel should have a text field that displays the current brushes name." A read-only field above the rows.
    {
        QHBoxLayout *nameRow = new QHBoxLayout();
        QLabel *nameLabel = new QLabel(i18n("Brush"), panel);
        nameLabel->setMinimumWidth(70);
        m_nameField = new QLineEdit(panel);
        m_nameField->setReadOnly(true);
        m_nameField->setMinimumHeight(38);
        nameRow->addWidget(nameLabel);
        nameRow->addWidget(m_nameField, 1);
        panelLayout->addLayout(nameRow);
    }

    auto addRow = [&](const QString &label, QSlider *&slider, QSpinBox *&spin, int maximum, const QString &suffix) {
        QHBoxLayout *row = new QHBoxLayout();
        QLabel *name = new QLabel(label, panel);
        name->setMinimumWidth(70);
        slider = new QSlider(Qt::Horizontal, panel);
        slider->setMinimumHeight(38);
        slider->setRange(suffix == QLatin1String(" px") ? 1 : 0, maximum);
        spin = new QSpinBox(panel);
        spin->setRange(slider->minimum(), suffix == QLatin1String(" px") ? qMax(1, int(KisImageConfig(true).maxBrushSize())) : maximum);
        spin->setSuffix(suffix);
        row->addWidget(name);
        row->addWidget(slider, 1);
        row->addWidget(spin);
        panelLayout->addLayout(row);
    };
    addRow(i18n("Size"), m_sizeSlider, m_sizeSpin, 500, QStringLiteral(" px"));
    addRow(i18n("Opacity"), m_opacitySlider, m_opacitySpin, 100, QStringLiteral(" %"));
    addRow(i18n("Flow"), m_flowSlider, m_flowSpin, 100, QStringLiteral(" %"));
    panelLayout->addWidget(m_presetChooser, 1);
    setWidget(panel);

    connect(m_sizeSlider, &QSlider::valueChanged, this, [this](int v) { QSignalBlocker b(m_sizeSpin); m_sizeSpin->setValue(v); slotKrimbleSizeChanged(v); });
    connect(m_sizeSpin, qOverload<int>(&QSpinBox::valueChanged), this, [this](int v) { QSignalBlocker b(m_sizeSlider); m_sizeSlider->setValue(v); slotKrimbleSizeChanged(v); });
    connect(m_opacitySlider, &QSlider::valueChanged, this, [this](int v) { QSignalBlocker b(m_opacitySpin); m_opacitySpin->setValue(v); slotKrimbleOpacityChanged(v); });
    connect(m_opacitySpin, qOverload<int>(&QSpinBox::valueChanged), this, [this](int v) { QSignalBlocker b(m_opacitySlider); m_opacitySlider->setValue(v); slotKrimbleOpacityChanged(v); });
    connect(m_flowSlider, &QSlider::valueChanged, this, [this](int v) { QSignalBlocker b(m_flowSpin); m_flowSpin->setValue(v); slotKrimbleFlowChanged(v); });
    connect(m_flowSpin, qOverload<int>(&QSpinBox::valueChanged), this, [this](int v) { QSignalBlocker b(m_flowSlider); m_flowSlider->setValue(v); slotKrimbleFlowChanged(v); });

    // the controls follow the brush if its size, opacity or flow is changed somewhere else (a preset is picked, a shortcut)
    m_syncTimer = new QTimer(this);
    m_syncTimer->setInterval(400);
    connect(m_syncTimer, SIGNAL(timeout()), this, SLOT(slotKrimbleSync()));
    m_syncTimer->start();
}

// Krimble 2026-10-30: the new controls write into the current brush.
void PresetDockerDock::slotKrimbleSizeChanged(int value)
{
    if (!m_canvas || !m_canvas->viewManager()) return;
    KisPaintOpPresetSP preset = m_canvas->viewManager()->canvasResourceProvider()->currentPreset();
    if (!preset || !preset->settings()) return;
    if (qRound(preset->settings()->paintOpSize()) != value) preset->settings()->setPaintOpSize(value);
}

void PresetDockerDock::slotKrimbleOpacityChanged(int value)
{
    if (!m_canvas || !m_canvas->viewManager()) return;
    m_canvas->viewManager()->canvasResourceProvider()->setOpacity(value / 100.0);
}

void PresetDockerDock::slotKrimbleFlowChanged(int value)
{
    if (!m_canvas || !m_canvas->viewManager()) return;
    m_canvas->viewManager()->canvasResourceProvider()->setFlow(value / 100.0);
}

// Krimble 2026-10-30: shows the current brush's size, opacity and flow (not in a number box that is being typed in).
void PresetDockerDock::slotKrimbleSync()
{
    if (!m_canvas || !m_canvas->viewManager() || !isVisible()) return;
    KisCanvasResourceProvider *provider = m_canvas->viewManager()->canvasResourceProvider();
    if (!provider) return;

    KisPaintOpPresetSP preset = provider->currentPreset();
    if (m_nameField) {   // Krimble 2026-10-30: the current brush's name
        const QString brushName = preset ? preset->name() : QString();
        if (m_nameField->text() != brushName) {
            m_nameField->setText(brushName);
            m_nameField->setCursorPosition(0);
        }
    }
    if (preset && preset->settings() && !m_sizeSpin->hasFocus()) {
        const int size = qBound(m_sizeSpin->minimum(), qRound(preset->settings()->paintOpSize()), m_sizeSpin->maximum());
        if (m_sizeSpin->value() != size) {
            QSignalBlocker b1(m_sizeSpin); QSignalBlocker b2(m_sizeSlider);
            m_sizeSpin->setValue(size); m_sizeSlider->setValue(size);
        }
    }
    if (!m_opacitySpin->hasFocus()) {
        const int opacity = qBound(0, qRound(provider->opacity() * 100.0), 100);
        if (m_opacitySpin->value() != opacity) {
            QSignalBlocker b1(m_opacitySpin); QSignalBlocker b2(m_opacitySlider);
            m_opacitySpin->setValue(opacity); m_opacitySlider->setValue(opacity);
        }
    }
    if (!m_flowSpin->hasFocus()) {
        const int flow = qBound(0, qRound(provider->flow() * 100.0), 100);
        if (m_flowSpin->value() != flow) {
            QSignalBlocker b1(m_flowSpin); QSignalBlocker b2(m_flowSlider);
            m_flowSpin->setValue(flow); m_flowSlider->setValue(flow);
        }
    }
}

void PresetDockerDock::setCanvas(KoCanvasBase *canvas)
{
    setEnabled(canvas != 0);

    if (m_canvas) {
        m_canvas->disconnectCanvasObserver(this);
        m_presetChooser->disconnect(m_canvas->viewManager()->paintOpBox());
    }

    m_canvas = dynamic_cast<KisCanvas2*>(canvas);

    if (!m_canvas || !m_canvas->viewManager() || !m_canvas->resourceManager()) return;

    connect(m_presetChooser, SIGNAL(resourceSelected(KoResourceSP )),
            m_canvas->viewManager()->paintOpBox(), SLOT(resourceSelected(KoResourceSP )));
    connect(m_presetChooser, SIGNAL(resourceClicked(KoResourceSP )),
            m_canvas->viewManager()->paintOpBox(), SLOT(resourceSelected(KoResourceSP )));
    connect(canvas->resourceManager(), SIGNAL(canvasResourceChanged(int,QVariant)),
            this, SLOT(canvasResourceChanged(int,QVariant)));


    connect(m_canvas->viewManager()->mainWindow(), SIGNAL(themeChanged()), m_presetChooser, SLOT(slotThemeChanged()), Qt::UniqueConnection);
    m_presetChooser->slotThemeChanged();

    canvasResourceChanged(KoCanvasResource::CurrentPaintOpPreset);
}

void PresetDockerDock::canvasResourceChanged(int key, const QVariant& /*v*/)
{
    if (key == KoCanvasResource::CurrentPaintOpPreset) {
        if (m_canvas && m_canvas->resourceManager()) {
            if (sender()) sender()->blockSignals(true);
            KisPaintOpPresetSP preset = m_canvas->resourceManager()->resource(KoCanvasResource::CurrentPaintOpPreset).value<KisPaintOpPresetSP>();
            if(preset)
                m_presetChooser->canvasResourceChanged(preset);
            if (sender()) sender()->blockSignals(false);
        }
    }
}



