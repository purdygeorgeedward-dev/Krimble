// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble Android visual refresh. Kept separate from layout and touch handling.
// Krimble 2026-11-11: THIS IS THE PAINTED (GRADIENT) SHEET, the one the app used before the flat theme (KrimbleVisualStyle.h). George: "Can we put in the Krimble gradient theme as an option?"
// It is now applied only while the theme "Krimble Painted" (krita/data/themes/KrimblePainted.colors) is chosen (thememanager.cpp); the function was krimbleVisualStyleSheet(), now
// krimblePaintedStyleSheet(). The rest of this file is the sheet exactly as it was on 2026-11-10.
// Krimble 2026-11-06: George: "Do the gradient effects on the whole app." The ChatGPT sheet (kept as KrimbleVisualStyle_chatgpt_74828b7.h.txt) with every color and every
// gradient replaced by values SAMPLED from George's mockup pictures (the Picker, Mixer and QuickAdjust mockups), and the same effects added to the main window's menu bar,
// menus, tool bars, headers, progress bars and tool tips (the ChatGPT sheet only styled dialogs). The Claude sheet from before the revert is kept as
// KrimbleVisualStyle_before_revert_2026-11-06.h.txt.
// Krimble 2026-11-06 (4th): George: "blue selections should go to a deeper blue." Every selected-state gradient (tabs, checked buttons, list rows, menu items, check boxes)
// was #0585CE, #016CAC, #015C96, #01548C, #0165A5 (sampled from the Mixer mockup's selected tab); it is now #0B87DD, #0867B8, #0652AD, #05429D (the bottom is the
// mockup's OK button blue).
#pragma once

#include <QString>

inline QString krimblePaintedStyleSheet()
{
    return QStringLiteral(R"KRIMBLE(

QDialog, QMessageBox, QInputDialog, QColorDialog, QFileDialog {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #374267, stop:1 #181f34);
    color: #ffffff;
    border: 1px solid #000000;
    border-top: 1px solid #8292c4;
    border-left: 1px solid #8292c4;
}
QDialog QLabel, QDialog QGroupBox, QDialog QCheckBox,
QDialog QRadioButton, QDialog QTabWidget {
    color: #e9edf8;
}
QGroupBox {
    border: 1px solid #506095;
    border-top: 1px solid #8292c4;
    border-left: 1px solid #8292c4;   /* task 33: bevel */
    border-bottom: 1px solid #000000;
    border-right: 1px solid #000000;
    border-radius: 8px;
    margin-top: 12px;
    padding: 12px 6px 6px 6px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #445079, stop:1 #2e3856);
}
QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }
QPushButton, QDialog QToolButton {
    color: #f1f4fc;
    border: 1px solid #6071a9;
    border-top: 1px solid #8292c4;   /* task 33: bevel */
    border-left: 1px solid #8292c4;
    border-bottom: 1px solid #000000;
    border-right: 1px solid #000000;
    border-radius: 3px;
    padding: 5px 9px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #434f75, stop:1 #262e4b);
}
QPushButton:hover, QDialog QToolButton:hover {
    /* border-color: #0b87dd; */ border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96);
    background: #2f3852;
}
QPushButton:pressed, QDialog QToolButton:pressed,
QPushButton:checked, QDialog QToolButton:checked {
    border: 2px solid #0b87dd; border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96);
    /* background: #0b87dd; */ background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #06538a, stop:0.05 #8fc2e6, stop:0.49 #1a7fc8, stop:0.5 #0872bd, stop:0.85 #2a8fd4, stop:1 #45a0dd);
}
QPushButton:default {
    color: white;
    border: 2px solid #0b87dd; border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96);
    /* background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #0a89cd, stop:0.3 #085892, stop:0.6 #07497e, stop:1 #044a9c); */ background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #06538a, stop:0.05 #8fc2e6, stop:0.49 #1a7fc8, stop:0.5 #0872bd, stop:0.85 #2a8fd4, stop:1 #45a0dd);  /* OK, Create: "buttons need the new style" */
}
QPushButton:disabled, QDialog QToolButton:disabled {
    color: #7a8199;
    border-color: #2b334a;
    background: #232a40;
}
QTabBar::tab {
    color: #e3e8f6;
    padding: 4px 13px;    border: 1px solid #495783;
    border-top: 1px solid #7080b6;   /* task 33: bevel */
    border-left: 1px solid #7080b6;
    border-right: 1px solid #000000;
    border-bottom: 1px solid #2a334a;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #27304f, stop:1 #141a2e);
}
QTabBar::tab:selected {
    color: white;
    border: 1px solid #495783;
    border-top: 1px solid #8a9acd;
    border-left: 1px solid #8a9acd;
    border-right: 1px solid #000000;
    border-bottom: 1px solid #2a334a;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    background: #3d4a7a;
}
QLineEdit, QSpinBox, QDoubleSpinBox {
    color: #f0f3ff;
    selection-background-color: #016cac;
    border: 1px solid #506095;
    border-top: 1px solid #000000;   /* task 33: sunken bevel */
    border-left: 1px solid #000000;
    border-bottom: 1px solid #6071a9;
    border-right: 1px solid #6071a9;
    border-radius: 5px;
    padding: 3px 5px;
    background: #0b101e;
}
QComboBox {
    color: #f0f3ff;
    selection-background-color: #016cac;
    border: 1px solid #3d4967;
    border-top: 1px solid #000000;   /* task 33: sunken bevel */
    border-left: 1px solid #000000;
    border-bottom: 1px solid #6071a9;
    border-right: 1px solid #6071a9;
    border-radius: 4px;
    padding: 3px 5px;
    background: #272f46;
}
QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
    border: 1px solid #0b87dd; border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96);
}
QAbstractItemView::item:selected {
    color: white;
    /* background: #0b87dd; */ background: #0b87dd;
    border: none;
    border-radius: 0px;
}
QSlider::groove:horizontal {
    height: 3px;
    border: none;
    border-radius: 1px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #72749d, stop:0.4 #5c5f88, stop:1 #7378a0);
}
QSlider::sub-page:horizontal {
    height: 3px;
    border: none;
    border-radius: 1px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #3fd1f6, stop:0.4 #0da0ff, stop:1 #008cec);
}
QSlider::handle:horizontal {
    width: 13px;
    height: 13px;
    margin: -5px 0;
    border: 1px solid #9da0c9;
    border-radius: 6px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #f9fdff, stop:0.2 #cfd1e6, stop:0.4 #aeb2cd,
                                stop:0.7 #8f93b0, stop:1 #8a8eab);
}
QCheckBox::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid #5a6a8c;
    border-radius: 4px;
    background: #14182b;
}
QCheckBox::indicator:checked {
    image: url(:/krimble-checkmark.svg);
    border: 2px solid #0b87dd; border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96);
    background: #14233f;
}
QMenuBar {
    color: #f1f4fc;
    background: transparent;
}
QMenuBar::item {
    padding: 1px 9px;
    margin: 1px 1px;
    background: transparent;
    border: 1px solid transparent;
    border-radius: 0px;}
QMenuBar::item:selected {
    border: 1px solid transparent;
    border-radius: 0px;
    /* background: #0b87dd; */ background: #0b87dd;
}
QMenu {
    color: #eef1fa;
    border: none;    background: #192138;}
QMenu::item:selected {
    /* background: #0b87dd; */ background: #0b87dd;
}
QMenu::separator {
    height: 0px;
    margin: 4px 6px;
    border-top: 1px solid #000000;
    border-bottom: 1px solid rgba(255, 255, 255, 110);
}
QToolBar {
    border: none;
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                                stop:0 #1c2346, stop:0.5 #394585, stop:1 #1c2346);
}
QMainWindow, QDockWidget {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #374267, stop:1 #181f34);
}
QAbstractItemView {
    /* recessed areas (lists, trees) are darker than the panels around them */
    background: #111624;
    alternate-background-color: #161c2f;
    color: #ffffff;
}
QDockWidget > QWidget, QScrollArea, QScrollArea > QWidget > QWidget {
    background: transparent;
}
QDockWidget > QMenu {
    color: #eef1fa;
    border: 1px solid #3d4967;
    background: #192138;
}
QStatusBar {
    /* max-height: 22px; */
    color: #ffffff;
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #212945, stop:0.5 #364063, stop:1 #212945);
}
QStatusBar QPushButton, QStatusBar QToolButton {
    border: none;
    background: transparent;
    padding: 0px 6px;
    margin: 0px;
    min-height: 0px;
}

QHeaderView::section {
    color: #e9edf8;
    border: 1px solid #343c57;
    padding: 3px 6px;
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                                stop:0 #383f57, stop:0.5 #434b68, stop:1 #383f57);
}
QProgressBar {
    color: white;
    border: 1px solid #3d4967;
    border-radius: 0px;   /* was 4px */
    background: #0d1221;
}
QProgressBar::chunk {
    border-radius: 0px;   /* was 3px */
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                                stop:0 #0a84f2, stop:0.55 #25bff6, stop:1 #58dcfc);
}
QToolTip {
    color: #eef1fa;
    border: 1px solid #0b87dd; border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96);
    background: #192138;
}
QWidget#krimbleStudioCard {
    border: 1px solid #506095;
    border-top: 1px solid #8292c4;
    border-radius: 10px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #445079, stop:1 #2e3856);
}
)KRIMBLE");
}
