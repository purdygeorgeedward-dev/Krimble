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
/* Krimble 2026-11-11: George: "The painted in theme has to go. It conflicts with all other themes." and "Get them all to the same hue and saturation but different brightness."
   The gradients are gone: every background is flat, all at hue 227, saturation 48 (Color Lab's HSV), and only the brightness (V) differs:
     top bar / menu bar / tool bars / status bar / headers / buttons #3e4b78 (V47), window / panels #2c3554 (V33), lists / menus / group boxes #20263d (V24),
     inactive tabs #1b2033 (V20), input fields #111521 (V13). The accent #1d7cb2 with a 2 px #3bd5f7 edge marks everything selected or pressed.
   The whole previous sheet (gradients and all) is kept as KrimbleVisualStyle_before_flat_2026-11-11.h.txt. This sheet is now applied only while the "Krimble dark" theme is chosen
   (thememanager.cpp), so the other themes are not painted over. */


/* Krimble 2026-11-11: George: "The blue glow in the selections doesn't look as cool without the dark areas inside, but it is cleaner." and, asked where: "Tabs, anything really." Everything that glows
   when selected (the selected tab, a pressed, checked or default button, the selected menu and menu bar item, a checked box) is dark navy inside (#072349 at the top to #0E3F78 at the bottom) with the
   bright cyan edge, as the selected tool is; the flat #1D7CB2 fill stays for list rows, the filled part of sliders and progress bars. */
QDialog, QMessageBox, QInputDialog, QColorDialog, QFileDialog {
    background: #2c3554;
    color: #ffffff;
}
QDialog QLabel, QDialog QGroupBox, QDialog QCheckBox,
QDialog QRadioButton, QDialog QTabWidget {
    color: #e9edf8;
}
QGroupBox {
    border: 1px solid #000000;
    border-radius: 0px;
    margin-top: 12px;
    padding: 12px 6px 6px 6px;
    background: #20263d;
}
QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }
QPushButton, QDialog QToolButton {
    color: #f1f4fc;
    border: 1px solid #000000;
    border-radius: 7px;
    padding: 5px 9px;
    background: #3e4b78;
}
QPushButton:hover, QDialog QToolButton:hover {
    border-color: #3bd5f7;
    background: #3e4b78;
}
QPushButton:pressed, QDialog QToolButton:pressed,
QPushButton:checked, QDialog QToolButton:checked {
    border: 2px solid #3bd5f7;
    /* background: #1d7cb2; */
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #072349, stop:1 #0e3f78);   /* Krimble 2026-11-11: dark inside the glow */
}
QPushButton:default {
    color: white;
    border: 2px solid #3bd5f7;
    /* background: #1d7cb2; */
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #072349, stop:1 #0e3f78);   /* Krimble 2026-11-11: dark inside the glow */
}
QPushButton:disabled, QDialog QToolButton:disabled {
    color: #7a8199;
    border-color: #000000;
    background: #20263d;
}
QTabBar::tab {
    color: #e3e8f6;
    padding: 7px 13px;
    border: 1px solid #000000;
    border-bottom: 1px solid #000000;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    background: #1b2033;
}
QTabBar::tab:selected {
    color: white;
    border: 2px solid #3bd5f7;
    border-bottom: 2px solid #3bd5f7;
    border-top-left-radius: 9px;
    border-top-right-radius: 9px;
    /* background: #1d7cb2; */
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #072349, stop:1 #0e3f78);   /* Krimble 2026-11-11: dark inside the glow */
}
QLineEdit, QSpinBox, QDoubleSpinBox {
    color: #f0f3ff;
    selection-background-color: #1d7cb2;
    border: 1px solid #000000;
    border-radius: 5px;
    padding: 3px 5px;
    background: #111521;
}
QComboBox {
    color: #f0f3ff;
    selection-background-color: #1d7cb2;
    border: 1px solid #000000;
    border-radius: 4px;
    padding: 3px 5px;
    background: #111521;
}
QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
    border: 1px solid #3bd5f7;
}
QAbstractItemView::item:selected {
    color: white;
    background: #1d7cb2;
    border: none;
    border-radius: 0px;
}
QSlider::groove:horizontal {
    height: 5px;
    border: 1px solid #000000;
    border-radius: 2px;
    background: #111521;
}
QSlider::sub-page:horizontal {
    height: 5px;
    border: none;
    border-radius: 2px;
    background: #1d7cb2;
}
QSlider::handle:horizontal {
    /* Krimble 2026-11-11: George: "make the handles round balls ... A metal ball bearing would look better." A steel ball: a bright reflection at the top, a dark horizon band
       across the middle, a lighter bounce at the bottom, a dark rim (a radial gradient, because a ball is not a flat color). */
    width: 18px;
    height: 18px;
    margin: -7px 0;
    border: 1px solid #141822;
    border-radius: 10px;
    background: qradialgradient(cx:0.5, cy:0.5, radius:0.55, fx:0.34, fy:0.26,
                                stop:0 #ffffff, stop:0.18 #dfe4ee, stop:0.5 #7f889c, stop:0.85 #3a4152, stop:1 #1c2130);
}
QCheckBox::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid #000000;
    border-radius: 4px;
    background: #111521;
}
QCheckBox::indicator:checked {
    image: url(:/krimble-checkmark.svg);
    border: 2px solid #3bd5f7;
    /* background: #1d7cb2; */
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #072349, stop:1 #0e3f78);   /* Krimble 2026-11-11: dark inside the glow */
}
QMenuBar {
    color: #f1f4fc;
    background: #3e4b78;
}
QMenuBar::item {
    padding: 0px 9px;
    margin: 0px 1px;
    background: transparent;
    border: 1px solid transparent;
    border-radius: 0px;
}
QMenuBar::item:selected {
    border: 1px solid #3bd5f7;
    border-radius: 0px;
    /* background: #1d7cb2; */
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #072349, stop:1 #0e3f78);   /* Krimble 2026-11-11: dark inside the glow */
}
QMenu {
    color: #eef1fa;
    border: 1px solid #000000;
    background: #20263d;
}
QMenu::item:selected {
    border: 1px solid #3bd5f7;
    /* background: #1d7cb2; */
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #072349, stop:1 #0e3f78);   /* Krimble 2026-11-11: dark inside the glow */
}
QToolBar {
    border: none;
    background: #3e4b78;
}
QMainWindow, QDockWidget {
    background: #2c3554;
}
QAbstractItemView {
    background: #20263d;
    alternate-background-color: #1b2033;
    color: #ffffff;
}
QDockWidget > QWidget, QScrollArea, QScrollArea > QWidget > QWidget {
    background: transparent;
}
QStatusBar {
    color: #ffffff;
    background: #3e4b78;
}
QHeaderView::section {
    color: #e9edf8;
    border: 1px solid #000000;
    padding: 3px 6px;
    background: #3e4b78;
}
QProgressBar {
    color: white;
    border: 1px solid #000000;
    border-radius: 0px;
    background: #111521;
}
QProgressBar::chunk {
    border-radius: 0px;
    background: #1d7cb2;
}
QToolTip {
    color: #eef1fa;
    border: 1px solid #3bd5f7;
    background: #20263d;
}
QWidget#krimbleStudioCard {
    border: 1px solid #000000;
    border-radius: 0px;
    background: #2c3554;
}
)KRIMBLE");
}
