/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _PRESETDOCKER_DOCK_H_
#define _PRESETDOCKER_DOCK_H_

#include <QPointer>
#include <QDockWidget>
#include <KoCanvasObserverBase.h>
#include <kis_canvas2.h>

class KisPaintOpPresetsChooserPopup;
class QSlider;   // Krimble 2026-10-30: the Size, Opacity and Flow controls
class QLineEdit; // Krimble 2026-10-30: the name of the current brush
class QSpinBox;
class QTimer;

class PresetDockerDock : public QDockWidget, public KoCanvasObserverBase {
    Q_OBJECT
public:
    PresetDockerDock( );
    QString observerName() override { return "PresetDockerDock"; }
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override { m_canvas = 0; setEnabled(false);}
public Q_SLOTS:
    void canvasResourceChanged(int key = 0, const QVariant& v = QVariant());
private Q_SLOTS:
    void slotKrimbleSizeChanged(int value);      // Krimble 2026-10-30
    void slotKrimbleOpacityChanged(int value);
    void slotKrimbleFlowChanged(int value);
    void slotKrimbleSync();                      // shows the current brush's size, opacity and flow
private:
    QPointer<KisCanvas2> m_canvas;
    KisPaintOpPresetsChooserPopup* m_presetChooser;
    // Krimble 2026-10-30: the new controls above the presets
    QLineEdit *m_nameField {nullptr};   // Krimble 2026-10-30: shows the current brush's name
    QSlider *m_sizeSlider {nullptr};
    QSpinBox *m_sizeSpin {nullptr};
    QSlider *m_opacitySlider {nullptr};
    QSpinBox *m_opacitySpin {nullptr};
    QSlider *m_flowSlider {nullptr};
    QSpinBox *m_flowSpin {nullptr};
    QTimer *m_syncTimer {nullptr};
};


#endif
