// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble Android visual refresh. Kept separate from layout and touch handling.
// Krimble 2026-11-06: George: "Do the gradient effects on the whole app." The ChatGPT sheet (kept as KrimbleVisualStyle_chatgpt_74828b7.h.txt) with every color and every
// gradient replaced by values SAMPLED from George's mockup pictures (the Picker, Mixer and QuickAdjust mockups), and the same effects added to the main window's menu bar,
// menus, tool bars, headers, progress bars and tool tips (the ChatGPT sheet only styled dialogs). The Claude sheet from before the revert is kept as
// KrimbleVisualStyle_before_revert_2026-11-06.h.txt.
// Krimble 2026-11-06 (4th): George: "blue selections should go to a deeper blue." Every selected-state gradient (tabs, checked buttons, list rows, menu items, check boxes)
// was #0585CE, #016CAC, #015C96, #01548C, #0165A5 (sampled from the Mixer mockup's selected tab); it is now #0B87DD, #0867B8, #0652AD, #05429D (the bottom is the
// mockup's OK button blue).
#pragma once

#include <QString>

inline QString krimbleVisualStyleSheet()
{
    return QStringLiteral(R"KRIMBLE(
QDialog, QMessageBox, QInputDialog, QColorDialog, QFileDialog {
    /* Krimble 2026-11-06 (3rd): George: "When I say gradients I'm talking about long smooth subtle gradients, not shaping every item with shading." The window is one long
       smooth gradient from top to bottom, #212638 to #1A2034 (the mockup's panel background, sampled at the top and at the bottom of the picture). Before: flat #1b2135.
    background-color: #1b2135;
    */
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #212638, stop:1 #1a2034);
    color: #eef1fa;
}
QDialog QLabel, QDialog QGroupBox, QDialog QCheckBox,
QDialog QRadioButton, QDialog QTabWidget {
    color: #e9edf8;
}
QGroupBox {
    border: 1px solid #343c57;
    border-top: 1px solid #7382ab;
    border-radius: 6px;
    margin-top: 12px;
    padding: 12px 6px 6px 6px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #232a40, stop:1 #1a2034);
}
QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }
QPushButton, QDialog QToolButton {
    color: #f1f4fc;
    /* Krimble 2026-11-06 (3rd): not sculpted: a flat button with a hair of shading (before: border-top #5c678f and stops #313a56, #272f46, #21293e):
    border: 1px solid #3d4967;
    border-top: 1px solid #5c678f;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #313a56, stop:0.5 #272f46, stop:1 #21293e);
    */
    border: 1px solid #3d4967;
    border-radius: 5px;
    padding: 5px 9px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2d354e, stop:1 #283048);
}
QPushButton:hover, QDialog QToolButton:hover {
    border-color: #3bbcf5;
    background: #2f3852;
}
QPushButton:pressed, QDialog QToolButton:pressed,
QPushButton:checked, QDialog QToolButton:checked {
    border: 2px solid #3bbcf5;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad,
                                stop:1 #05429d);
}
QPushButton:disabled, QDialog QToolButton:disabled {
    color: #7a8199;
    border-color: #2b334a;
    background: #232a40;
}
QTabBar::tab {
    color: #e3e8f6;
    padding: 7px 13px;
    border: 1px solid #3d4668;
    border-bottom: 1px solid #2a334a;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    /* Krimble 2026-11-06 (3rd): before: stops #2e354e, #212639
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #2e354e, stop:1 #212639);
    */
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2b324a, stop:1 #252b42);
}
QTabBar::tab:selected {
    color: white;
    border: 1px solid #3bbcf5;
    border-bottom: 2px solid #3bbcf5;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad,
                                stop:1 #05429d);
}
QLineEdit, QSpinBox, QDoubleSpinBox {
    color: #f0f3ff;
    selection-background-color: #016cac;
    border: 1px solid #3d4967;
    border-radius: 4px;
    padding: 3px 5px;
    background: #0d1221;
}
QComboBox {
    /* Krimble 2026-11-06 (2nd): George: "Do not add sculpted shading to fields." The drop-down box is flat like the text and number boxes (it had the button's gradient and top
       highlight):
    border: 1px solid #3d4967;
    border-top: 1px solid #5c678f;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #313a56, stop:0.5 #272f46, stop:1 #21293e);
    */
    color: #f0f3ff;
    selection-background-color: #016cac;
    border: 1px solid #3d4967;
    border-radius: 4px;
    padding: 3px 5px;
    background: #272f46;
}
QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
    border: 1px solid #3bbcf5;
}
QAbstractItemView::item:selected {
    color: white;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad,
                                stop:1 #05429d);
    border: 1px solid #3bbcf5;
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
QCheckBox::indicator:checked {
    border: 1px solid #3bbcf5;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad,
                                stop:1 #05429d);
}
QMenuBar {
    color: #eef1fa;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #35426b, stop:0.5 #2f3a57, stop:1 #2a334e);
}
QMenuBar::item:selected {
    border-radius: 4px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad,
                                stop:1 #05429d);
}
QMenu {
    color: #eef1fa;
    border: 1px solid #3d4967;
    background: #212639;   /* Krimble 2026-11-06 (3rd): flat (was a #2e354e to #212639 gradient) */
}
QMenu::item:selected {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad,
                                stop:1 #05429d);
}
QToolBar {
    border: none;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #2e354e, stop:1 #212639);
}
QHeaderView::section {
    color: #e9edf8;
    border: 1px solid #343c57;
    padding: 3px 6px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #434b68, stop:1 #383f57);
}
QProgressBar {
    color: white;
    border: 1px solid #3d4967;
    border-radius: 4px;
    background: #0d1221;
}
QProgressBar::chunk {
    border-radius: 3px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #3fd1f6, stop:0.4 #0da0ff, stop:1 #008cec);
}
QToolTip {
    color: #eef1fa;
    border: 1px solid #3bbcf5;
    background: #212639;
}
)KRIMBLE");
}
