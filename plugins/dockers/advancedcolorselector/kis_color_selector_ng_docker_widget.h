/*
 *  SPDX-FileCopyrightText: 2010 Adam Celarek <kdedev at xibo dot at>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */


#ifndef KIS_COLOR_SELECTOR_NG_DOCKER_WIDGET_H
#define KIS_COLOR_SELECTOR_NG_DOCKER_WIDGET_H

#include <QWidget>
#include <QPointer>
#include <QToolButton>

#include <kis_canvas2.h>

class QAction;

class KisCommonColors;
class KisColorHistory;
class KisColorSelectorContainer;
class KrimbleColorStudio;

class QVBoxLayout;
class QHBoxLayout;

class KisColorSelectorNgDockerWidget : public QWidget
{
Q_OBJECT
public:
    // explicit KisColorSelectorNgDockerWidget(QWidget *parent = 0);
    // Krimble 2026-11-08: George: "bring back advance color panel as \"color ring + triangle\"." With classic = true the widget shows the classic selector (ring, triangle, shade
    // selectors, color patches) and no Color Studio; it is the "Color Ring + Triangle" panel.
    explicit KisColorSelectorNgDockerWidget(QWidget *parent = 0, bool classic = false);
    void setCanvas(KisCanvas2* canvas);
    void unsetCanvas();
public Q_SLOTS:
    void openSettings();

Q_SIGNALS:
    void settingsChanged();

protected Q_SLOTS:
    void updateLayout();

private:
    KisColorSelectorContainer* m_colorSelectorContainer;
    KrimbleColorStudio* m_krimbleStudio = nullptr;
    bool m_classic = false;   // Krimble 2026-11-08: the "Color Ring + Triangle" panel
    KisColorHistory* m_colorHistoryWidget;
    KisCommonColors* m_commonColorsWidget;

    QAction * m_colorHistoryAction;
    QAction * m_commonColorsAction;

    QHBoxLayout* m_widgetLayout;
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_horizontalPatchesContainer;
    QVBoxLayout* m_sidebarLayout;

    QHBoxLayout* m_verticalColorPatchesLayout; // vertical color patches should be added here
    QVBoxLayout* m_horizontalColorPatchesLayout;//horizontal ----------"----------------------
    QToolButton* m_fallbackSettingsButton;

    QPointer<KisCanvas2> m_canvas;

};

#endif
