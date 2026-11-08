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
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #212638, stop:1 #1a2034);
    color: #eef1fa;
*/
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #373c62, stop:1 #2c3152);
    color: #ffffff;
}
QDialog QLabel, QDialog QGroupBox, QDialog QCheckBox,
QDialog QRadioButton, QDialog QTabWidget {
    color: #e9edf8;
}
QGroupBox {
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
    border: 1px solid #343c57;
    border-top: 1px solid #7382ab;
    border-radius: 6px;
    margin-top: 12px;
    padding: 12px 6px 6px 6px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #232a40, stop:1 #1a2034);
*/
    border: 1px solid #4a5178;
    border-top: 1px solid #8d96c2;
    border-radius: 8px;
    margin-top: 12px;
    padding: 12px 6px 6px 6px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #444a70, stop:1 #343959);
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
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
    border: 1px solid #3d4967;
    border-radius: 5px;
    padding: 5px 9px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2d354e, stop:1 #283048);
*/
    border: 1px solid #6a719b;
    border-radius: 7px;
    padding: 5px 9px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #4d5479, stop:1 #2b3050);
}
QPushButton:hover, QDialog QToolButton:hover {
    border-color: #3bbcf5;
    background: #2f3852;
}
QPushButton:pressed, QDialog QToolButton:pressed,
QPushButton:checked, QDialog QToolButton:checked {
    /* Krimble 2026-11-07: the mockup's selected look (see the selected tab). Before: a 2 px #3BBCF5 outline and the flat blue gradient #0B87DD to #05429D.
    border: 2px solid #3bbcf5;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad,
                                stop:1 #05429d);
    */
    border: 2px solid #35d8f8;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #1196d8, stop:0.10 #0a78bb, stop:0.5 #073d6b, stop:1 #074a85);
}
QPushButton:default {
    /* Krimble 2026-11-07: the mockup's OK button (sampled): #0A89CD at the top, #085892, #07497E, #044A9C at the bottom, a bright cyan outline. */
    color: white;
    border: 2px solid #35d8f8;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #0a89cd, stop:0.3 #085892, stop:0.6 #07497e, stop:1 #044a9c);
}
QPushButton:disabled, QDialog QToolButton:disabled {
    color: #7a8199;
    border-color: #2b334a;
    background: #232a40;
}
QTabBar::tab {
    color: #e3e8f6;
    padding: 7px 13px;
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
    border: 1px solid #3d4668;
    border-bottom: 1px solid #2a334a;
*/
    border: 1px solid #555d86;
    border-bottom: 1px solid #2a334a;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    /* Krimble 2026-11-06 (3rd): before: stops #2e354e, #212639
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #2e354e, stop:1 #212639);
    */
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2b324a, stop:1 #252b42);
*/
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2a3050, stop:1 #171b34);
}
QTabBar::tab:selected {
    /* Krimble 2026-11-07: George: "Look at the selected items in the Prefs mockup." The mockup's selected tab, list entry and OK button (colors sampled from the picture): a teal blue that is
       lighter along the top edge, #073D6B in the middle and #074A85 at the bottom, with a 2 px bright cyan outline (#27F8FF to #35D8F8) and rounder corners. Before: the flat blue
       gradient #0B87DD, #0867B8, #0652AD, #05429D with a #3BBCF5 outline. The soft glow around the outline is not possible in a style sheet (see the notes of 2026-11-07).
    color: white;
    border: 1px solid #3bbcf5;
    border-bottom: 2px solid #3bbcf5;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad,
                                stop:1 #05429d);
    */
    color: white;
    border: 2px solid #35d8f8;
    border-bottom: 2px solid #35d8f8;
    border-top-left-radius: 9px;
    border-top-right-radius: 9px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #1196d8, stop:0.10 #0a78bb, stop:0.5 #073d6b, stop:1 #074a85);
}
QLineEdit, QSpinBox, QDoubleSpinBox {
    color: #f0f3ff;
    selection-background-color: #016cac;
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
    border: 1px solid #3d4967;
    border-radius: 4px;
    padding: 3px 5px;
    background: #0d1221;
*/
    border: 1px solid #5a6490;
    border-radius: 5px;
    padding: 3px 5px;
    background: #0c1024;
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
    /* Krimble 2026-11-07: the mockup's selected look (see the selected tab above). Before: the flat blue gradient #0B87DD to #05429D and a 1 px #3BBCF5 outline.
    color: white;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad,
                                stop:1 #05429d);
    border: 1px solid #3bbcf5;
    */
    color: white;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #1196d8, stop:0.10 #0a78bb, stop:0.5 #073d6b, stop:1 #074a85);
    border: 2px solid #35d8f8;
    border-radius: 9px;
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
/* Krimble 2026-11-07: George: "checked should have an actual check in the box if possible." A checked box shows a white check mark (krimble-checkmark.svg, in svg-icons.qrc), and the
   box is 18 px, checked or not (a picture would otherwise make the checked box bigger than the others). Tested in real Qt (PyQt5 5.15): the check scales to the box. */
/* Krimble 2026-11-07 (2nd): George: "Look at the selected items in the Prefs mockup." The mockup's check box (sampled): a dark box (#16192A), a bright cyan outline (#74ECFD, #28BFE5) when
   checked and a bright cyan-white check (#EAFFFF). The box is 20 px. Before (earlier today): an 18 px box, a blue gradient and a #3BBCF5 outline with a white check. */
QCheckBox::indicator {
    width: 20px;
    height: 20px;
    border: 1px solid #5a6a8c;
    border-radius: 4px;
    background: #14182b;
}
QCheckBox::indicator:checked {
    image: url(:/krimble-checkmark.svg);
    border: 2px solid #3be0f6;
    background: #14233f;
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
/* Krimble 2026-11-07: George (screenshots of the build of fd62f88): "The bland gray sucks for large areas. I'd like to see a smooth gradient as discussed." and, asked whether it is the
   flat blue-gray of the toolbox, the panels and the status bar: "Yeah, the gray blue is bland." and "Would like all colors to match." The main window, the panels (docks, docked or
   floating) and the status bar get the same long smooth gradients as the dialogs and the menu bar: #212638 to #1A2034 (the mockup's panel background) and #2E354E to #212639 for the
   status bar (the tool bar's pair). The contents of a panel and its scroll areas are transparent so the gradient shows through; fields, buttons and sliders keep their own rules.
   Tested in real Qt (PyQt5 5.15) with a main window with a toolbox, tabbed panels with a scroll area of ordinary controls, a status bar and a floating panel. Before: the theme's flat #373952. */
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
QMainWindow, QDockWidget {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #212638, stop:1 #1a2034);
}
*/
QMainWindow, QDockWidget {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #373c62, stop:1 #2c3152);
}
QAbstractItemView {
    /* recessed areas (lists, trees) are darker than the panels around them */
    background: #15182d;
    alternate-background-color: #1a1e36;
    color: #ffffff;
}
QDockWidget > QWidget, QScrollArea, QScrollArea > QWidget > QWidget {
    background: transparent;
}
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
QStatusBar {
    color: #eef1fa;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2e354e, stop:1 #212639);
}
*/
QStatusBar {
    color: #ffffff;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #3d4368, stop:1 #30365a);
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
