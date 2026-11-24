/*
 *  SPDX-FileCopyrightText: 2010 Adam Celarek <kdedev at xibo dot at>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KIS_COLOR_SELECTOR_H
#define KIS_COLOR_SELECTOR_H

#include "kis_color_selector_base.h"
#include <KisColorSelectorConfiguration.h>

#include <resources/KoGamutMask.h>

class KisColorSelectorRing;
class KisColorSelectorComponent;
class KisColorSelectorSimple;
class KisColorSelectorWheel;
class QToolButton;
class KisSignalCompressor;

class KisColorSelector : public KisColorSelectorBase
{
    Q_OBJECT
public:

    KisColorSelector(KisColorSelectorConfiguration conf, QWidget* parent = 0);
    KisColorSelector(QWidget* parent=0);
    KisColorSelectorBase* createPopup() const override;

    void setConfiguration(KisColorSelectorConfiguration conf);
    KisColorSelectorConfiguration configuration() const;
    void setColor(const KoColor &color) override;

    void updateIcons();

    void hasAtLeastOneDocument(bool value);

public Q_SLOTS:
    void reset() override;
    void updateSettings() override;
    void slotGamutMaskSet(KoGamutMaskSP gamutMask);
    void slotGamutMaskUnset();
    void slotGamutMaskPreviewUpdate();
    void slotGamutMaskToggle(bool state);
    void slotGamutMaskDeactivate();

Q_SIGNALS:
    void settingsButtonClicked();

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    bool displaySettingsButton();

private:
    void mouseEvent(QMouseEvent* e);
    void init();

    KisColorSelectorRing* m_ring;
    KisColorSelectorComponent* m_triangle;
    KisColorSelectorSimple* m_slider;
    KisColorSelectorSimple* m_square;
    KisColorSelectorWheel* m_wheel;
    QToolButton* m_button;
    KisColorSelectorComponent* m_mainComponent;
    KisColorSelectorComponent* m_subComponent;
    KisColorSelectorComponent* m_grabbingComponent;

    KisSignalCompressor *m_signalCompressor;

    KisColorSelectorConfiguration m_configuration;

    KoColor m_lastRealColor;
    KoColor m_currentRealColor;
    bool m_blipDisplay;
    Acs::ColorRole m_lastColorRole;

    bool m_hasAtLeastOneDocumentOpen = true;

public:
    void setDisplayBlip(bool disp) {m_blipDisplay = disp;}
    bool displayBlip() const {return m_blipDisplay;}
};

#endif
