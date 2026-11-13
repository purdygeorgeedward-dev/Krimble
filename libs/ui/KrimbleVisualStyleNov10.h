// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-12 (task 31): George, after the side-by-side pictures of the sheet from before the flat rewrite and the flat sheet: "The Nov 10 version is so much better. Revert to
// that and make it the default." This is that sheet, EXACTLY as it was in libs/ui/KrimbleVisualStyle.h at b0f146d^ (2026-11-11, the parent of the flat rewrite), with only two rules
// ADDED at the end, both copied from the painted sheet and both asked for by George: the menu dividers (task 30) and the opaque menu of a dock's long-press menu. It is applied for the
// theme "Krimble Blue-Gray" (the default); the flat sheet (KrimbleVisualStyle.h) stays for the theme "Krimble Blue", the painted sheet for "Krimble Painted".
#pragma once

#include <QString>

inline QString krimbleNov10StyleSheet()
{
    return QStringLiteral(R"KRIMBLE(
/* Krimble 2026-11-08: George: "UI looks a lot better ... Gradient in UI could be more. Try to match hue between the bar and other UI components." Every color of the window, the panels,
   the group boxes, the buttons, the unselected tabs, the fields, the lists, the status bar, the menu bar and the tool bars is now one hue (226 degrees), at stronger steps of lightness
   from the top to the bottom (the window #374267 to #181f34 was #373C62 to #2C3152; the menu bar was a bluer #35426B to #2A334E, which is why the bar did not match the panels).
   The old values are in the comments above the changes of 2026-11-06, 07 (3rd) and 08. */

QDialog, QMessageBox, QInputDialog, QColorDialog, QFileDialog {
    /* Krimble 2026-11-06 (3rd): George: "When I say gradients I'm talking about long smooth subtle gradients, not shaping every item with shading." The window is one long
       smooth gradient from top to bottom, #212638 to #1A2034 (the mockup's panel background, sampled at the top and at the bottom of the picture). Before: flat #1b2135.
    background-color: #1b2135;
    */
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #212638, stop:1 #1a2034);
    color: #eef1fa;
*/
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #374267, stop:1 #181f34);
    color: #ffffff;
    /* Krimble 2026-11-12 (task 33): George: "Rough. No shadow or hilight lines?" and "ALL themes need ... the hilight and shadow lines." One-pixel bevel edges in the Nov 10 sheet (raised: light top and left, black bottom and right; sunken fields the other way round; no shading). */
    border: 1px solid #000000;
    border-top: 1px solid #8292c4;
    border-left: 1px solid #8292c4;
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
    border: 1px solid #6071a9;
    border-top: 1px solid #8292c4;   /* task 33: bevel */
    border-left: 1px solid #8292c4;
    border-bottom: 1px solid #000000;
    border-right: 1px solid #000000;
    border-radius: 7px;
    padding: 5px 9px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #434f75, stop:1 #262e4b);
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
    padding: 4px 13px;   /* Krimble 2026-11-13: tabs shorter (was 7px 13px) */
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
    border: 1px solid #3d4668;
    border-bottom: 1px solid #2a334a;
*/
    border: 1px solid #495783;
    border-top: 1px solid #7080b6;   /* task 33: bevel */
    border-left: 1px solid #7080b6;
    border-right: 1px solid #000000;
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
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #27304f, stop:1 #141a2e);
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
    /* Krimble 2026-11-13 (task 69): George: "why is the selected tab glowing? It should just be lighter than the other tabs." No glow, no gradient: the selected tab is the same tab, lighter. Before:
    color: white;
    border: 2px solid #35d8f8;
    border-bottom: 2px solid #35d8f8;
    border-top-left-radius: 9px;
    border-top-right-radius: 9px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #1196d8, stop:0.10 #0a78bb, stop:0.5 #073d6b, stop:1 #074a85); */
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
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
    border: 1px solid #3d4967;
    border-radius: 4px;
    padding: 3px 5px;
    background: #0d1221;
*/
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
    border-top: 1px solid #000000;   /* task 33: sunken bevel */
    border-left: 1px solid #000000;
    border-bottom: 1px solid #6071a9;
    border-right: 1px solid #6071a9;
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
    /* Krimble 2026-11-09: George: "Weird curved selection of layer has to go. It's horrible. Not everything selected is a button. You're rounding everything and it sucks. Not every UI element
       needs to glow. Some can just darken." A selected row of a list (the Layers panel, the Preferences page list) is a flat, darker fill: no outline, no rounding, no glow, no gradient.
    color: white;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #1196d8, stop:0.10 #0a78bb, stop:0.5 #073d6b, stop:1 #074a85);
    border: 2px solid #35d8f8;
    border-radius: 9px;
    */
    color: white;
    /* Krimble 2026-11-13 (task 45): George: "Preferences keyboard is never big enough, but column is too wide and selection area is darkening the selected icon. Yuck. What happened to bright blue background for selection?" The flat dark navy #1c3963 darkened the selected rows (and their icons): the bright blue of the flat selection of build 56 is back, still flat and square. Before: background: #1c3963; */
    background: #0b87dd;
    border: none;
    border-radius: 0px;
}
/* Krimble 2026-11-08: a left-to-right version of the slider rules was tried (blue to cyan with a glossy ball) and taken out again: George asked about title bars and progress bars, not
   sliders. The slider rules are as they were. */
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
/* Krimble 2026-11-08: George: "Check boxes could be smaller." 16 px (were 20 px, 18 px before that). */
QCheckBox::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid #5a6a8c;
    border-radius: 4px;
    background: #14182b;
}
QCheckBox::indicator:checked {
    image: url(:/krimble-checkmark.svg);
    border: 2px solid #3be0f6;
    background: #14233f;
}
/* Krimble 2026-11-08: George: "The menu bar looks like shit currently. FIX IT." The bar's background is painted in code (KrimbleMenuBarFilter, KisMainWindow.cpp): dark at both ends and
   lighter in the middle, like the mockups' title bars; here it is transparent. The selected name is a pill with the bright cyan outline of every selected item. Before:
QMenuBar { color: #eef1fa; background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #374676, stop:0.5 #2d385c, stop:1 #242d4c); }
QMenuBar::item:selected { border-radius: 4px; background: a flat blue gradient #0b87dd to #05429d; }
*/
QMenuBar {
    color: #f1f4fc;
    /* Krimble 2026-11-13 (task 57): George (build 57 screenshots): "\"Raised\" buttons are DARKER? WHY? The hilight is also not white or very light. Huge mistake." and "At startup I saw this big blah. Nothing to differentiate it from the other UI elements but a line." The menu bar is a bar again: a flat, lighter band (hue 227, saturation 48, brightness 55 percent; the window behind it is darker at the top) with the one-pixel lines of the bevel overlay. No gradient painting. Before: background: transparent; */
    background: #49588c;
}
QMenuBar::item {
    padding: 1px 9px;
    margin: 1px 1px;
    background: transparent;
    border: 1px solid transparent;
    border-radius: 0px;   /* Krimble 2026-11-09: was 7px */
}
/* Krimble 2026-11-09: George: "Menu selection can darken the menu title to match the menu. The glow is weird." The selected title (the open menu's) is the color of the menu below it: flat, no outline, no rounding.
QMenuBar::item:selected {
    border: 1px solid #35d8f8;
    border-radius: 7px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #1196d8, stop:0.12 #0a78bb, stop:0.55 #073d6b, stop:1 #074a85);
}
*/
QMenuBar::item:selected {
    border: 1px solid transparent;
    border-radius: 0px;
    /* background: #192138;   Krimble 2026-11-13 (task 78): George: "Menu bar names... yes, I think so. solid blue." */
    background: #0b87dd;
}
QMenu {
    color: #eef1fa;
    border: 1px solid #3d4967;
    background: #192138;   /* Krimble 2026-11-06 (3rd): flat (was a #2e354e to #212639 gradient) */
}
QMenu::item:selected {
    /* Krimble 2026-11-13 (task 60): George: "The selected item in the menu should not have a gradient. It should just be solid light blue, the brightest out of the range it currently uses." Solid, the lightest stop of the gradient that was here (the other stops were #0867b8, #0652ad and #05429d):
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad,
                                stop:1 #05429d); */
    background: #0b87dd;
}
QToolBar {
    border: none;
    /* Krimble 2026-11-12 (task 32): George: "Can we reapply the fixes?" The fix of 2026-11-11 (bc57924, as in the painted sheet): George (Nov 9): "ANY bar is NOT SUPPOSED TO BE SHADED ON THE BOTTOM ... IT LOOKS PUFFY. IT WAS SUPPOSED TO BE DARKENED TO THE CORNERS." Across only, dark at both ends. The Nov 10 gradient is kept in the comment. */
    /* background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #2f3a60, stop:1 #192138); */
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                                stop:0 #1c2346, stop:0.5 #394585, stop:1 #1c2346);
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
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
QStatusBar {
    color: #eef1fa;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2e354e, stop:1 #212639);
}
*/
QStatusBar {
    max-height: 22px;
    color: #ffffff;
    /* task 32: across only (see the tool bar). Before: background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #364063, stop:1 #212945); */
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #212945, stop:0.5 #364063, stop:1 #212945);
}
/* Krimble 2026-11-13 (task 73, repair): George (a screenshot of the status bar): "no buttons or bevels or whatever. This was good before. but still too tall." The status bar items are plain, no button border, no bevel, no fill, and the bar is at most 22 px high. */
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
    /* task 32: across only (see the tool bar). Before: background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #434b68, stop:1 #383f57); */
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                                stop:0 #383f57, stop:0.5 #434b68, stop:1 #383f57);
}
/* Krimble 2026-11-08: George: "Left to right looks good on progress bars. Please no curved ends. Just straight edges on progress bars." Square corners (the bar had 4 px, the fill 3 px). */
QProgressBar {
    color: white;
    border: 1px solid #3d4967;
    border-radius: 0px;   /* was 4px */
    background: #0d1221;
}
/* Krimble 2026-11-08: left to right, like the slider bars (it was top to bottom: x2:0,y2:1). */
QProgressBar::chunk {
    border-radius: 0px;   /* was 3px */
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                                stop:0 #0a84f2, stop:0.55 #25bff6, stop:1 #58dcfc);
}
QToolTip {
    color: #eef1fa;
    border: 1px solid #3bbcf5;
    background: #192138;
}
/* Krimble 2026-11-08: the Color panel's picker card (Color Studio, krimble_color_studio.h): a rounded frame with a smooth gradient, as the inner panel of the Color mockup. */
QWidget#krimbleStudioCard {
    border: 1px solid #506095;
    border-top: 1px solid #8292c4;
    border-radius: 10px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #445079, stop:1 #2e3856);
}
/* Krimble 2026-11-11: the same fix as in the flat sheet (George: "Transparent menu."): a menu made with a dock as its parent (the toolbox's long press menu) is a direct child of the dock and the
   rule above made it transparent; this rule has the menu's own fill and edge and is as specific and later. */
QDockWidget > QMenu {
    color: #eef1fa;
    border: 1px solid #3d4967;
    background: #192138;
}
/* Krimble 2026-11-12 (task 30): George (a screenshot of the Image menu): "We had these lovely separators before and you wiped them out somehow." The dividers between menu items were a black line with a highlight line below it (KisWideDockSeparatorStyle, 2026-10-10); a menu that a style sheet paints is drawn by the sheet, not by that style, so they were a thin plain line. One pixel each, as before. */
QMenu::separator {
    height: 0px;
    margin: 4px 6px;
    border-top: 1px solid #000000;
    border-bottom: 1px solid rgba(255, 255, 255, 110);
}
)KRIMBLE");
}
