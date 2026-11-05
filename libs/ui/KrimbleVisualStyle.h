// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble Android visual refresh. Kept separate from layout and touch handling.
#pragma once

#include <QString>

inline QString krimbleVisualStyleSheet()
{
    return QStringLiteral(R"KRIMBLE(
/* Krimble 2026-11-04: George: "Make the other elements match the buttons." Dialogs now use the app's own window color #373952
   (theme "Krimble dark"), not the separate dark blue #282d49; fields and lists use the theme's list color #282a43 (flat). */
QMainWindow, QDockWidget, QToolBar, QStatusBar {
    background-color: #373952;
}
QAbstractItemView {
    background-color: #282a43;
}
QDialog, QMessageBox, QInputDialog, QColorDialog, QFileDialog {
    /* background-color: #282d49; */
    background-color: #373952;
    color: #eef1fa;
}
QDialog QLabel, QDialog QGroupBox, QDialog QCheckBox,
QDialog QRadioButton, QDialog QTabWidget {
    color: #e9edf8;
}
QGroupBox {
    border: 1px solid #535e83;
    border-radius: 6px;
    margin-top: 12px;
    padding: 12px 6px 6px 6px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #3d4060, stop:1 #383a58);
}
QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }
QPushButton, QDialog QToolButton {
    color: #f1f4fc;
    border: 1px solid #59668d;
    border-top: 1px solid #8795bc;
    border-radius: 5px;
    padding: 5px 9px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #4a567b, stop:0.48 #3c4569,
                                stop:1 #2b3251);
}
QPushButton:hover, QDialog QToolButton:hover {
    border-color: #59b8f4;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #4775a7, stop:1 #234770);
}
QPushButton:pressed, QDialog QToolButton:pressed,
QPushButton:checked, QDialog QToolButton:checked {
    border: 2px solid #38c6ff;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #167bb7, stop:0.5 #095889,
                                stop:1 #07345e);
}
QPushButton:disabled, QDialog QToolButton:disabled {
    color: #8990a6;
    border-color: #444c67;
    background: #303650;
}
/* Krimble 2026-10-30: George: "I don't think they should be excluded. we will know in the next build." The five button rules above said
   "QDialog QToolButton" (tool buttons inside dialogs only, so the toolbox and the panels' tool buttons were left out on purpose); they now say
   "QToolButton": every tool button in the app, the toolbox and the panels included. The checked button has the blue border and gradient (the selected
   tool). The roomy padding is kept for tool buttons in dialogs; elsewhere it is small, so the toolbox buttons do not grow. */
/* Krimble 2026-11-04: George: no shaded buttons in the toolbox; shaded buttons stay in dialogs. The button rules above are back to
   "QDialog QToolButton" (tool buttons inside dialogs only), as before the 337th entry. The old selectors were "QToolButton" and
   "QToolButton:hover/pressed/checked/disabled". Selections (checked button, selected tab, selected list row, the blue glow) are unchanged:
   George: "I think it got the selections right before." */
/* QToolButton { padding: 3px; }   (switched off 2026-11-04, see the note above) */
QDialog QToolButton { padding: 5px 9px; }
QTabBar::tab {
    color: #e3e8f6;
    padding: 7px 13px;
    border: 1px solid #5c668a;
    border-bottom: 1px solid #3c4262;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #3e476c, stop:1 #2b3252);
}
QTabBar::tab:selected {
    color: white;
    border: 1px solid #40d1ff;
    border-bottom: 2px solid #32ccff;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #228ac5, stop:0.5 #0b5e93,
                                stop:1 #083858);
}
QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
    color: #f0f3ff;
    selection-background-color: #087aaf;
    border: 1px solid #5a6489;
    border-radius: 4px;
    padding: 3px 5px;
    /* Krimble 2026-11-04: George: no shading on text fields or any other fields: one flat color, the theme's list color. */
    /* background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #222740, stop:1 #363d5e); */
    background: #282a43;
}
QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
    border: 1px solid #43c9ff;
}
QAbstractItemView::item:selected {
    color: white;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #2188c0, stop:0.5 #105b8e,
                                stop:1 #073e68);
    border: 1px solid #43c9ff;
}
/* Krimble 2026-11-05: George: "I want normal sized handles that match the UI. I want thin lines, not giant lines." Every slider in the app (QuickAdjust,
   Brushes, the On-Canvas Brush Editor): a 4 px groove (was 7) and a 14 px round handle (was 19). The old values: groove height 7px, handle width 19px,
   margin -7px 0, border-radius 9px. A border radius of exactly half the handle size (9 px on 18 px) is drawn as a square by Qt, so it is 8 px. */
QSlider::groove:horizontal {
    /* height: 7px; */
    height: 4px;
    border: 1px solid #535c80;
    border-radius: 3px;
    background: #20263f;
}
QSlider::handle:horizontal {
    /* width: 19px; */
    /* margin: -7px 0; */
    width: 14px;
    height: 14px;
    margin: -6px 0;
    border: 2px solid #c9dfff;
    /* border-radius: 9px; */
    border-radius: 8px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #f1f5ff, stop:1 #728eb8);
}
QCheckBox::indicator:checked {
    border: 1px solid #39ccff;
    background: #087eae;
}
/* Krimble 2026-10-30: George: "I would like all the dialogs that show a bar or fill area to represent a percentage to use Krimble orange." Krimble
   orange is #FD6001, the orange of the K in the logo (sampled from the new splash). Progress bars (the progress dialogs, the status bar) and the filled part
   of a slider are orange; the groove stays dark. The value sliders with a number (KisSliderSpinBox) are painted by the code, see kis_slider_spin_box_p.h. */
QProgressBar {
    color: #f4f6ff;
    text-align: center;
    border: 1px solid #535c80;
    border-radius: 4px;
    background: #20263f;
}
QProgressBar::chunk {
    border-radius: 3px;
    /* Krimble 2026-11-04: George: the orange is obnoxious; light blue like the selection highlight. Old orange rule: stop:0 #ff8d42,
       stop:0.5 #fd6001, stop:1 #c44a00. */
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #2a93cc, stop:0.5 #1d7cb2,
                                stop:1 #146a9f);
}
QSlider::sub-page:horizontal {
    /* height: 7px; */
    height: 4px;   /* Krimble 2026-11-05: thin, like the groove (see the QSlider note above) */
    border: 1px solid #146a9f;   /* was #c44a00 (orange) until 2026-11-04 */
    border-radius: 3px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #2a93cc, stop:0.5 #1d7cb2,
                                stop:1 #146a9f);
}
)KRIMBLE");
}
