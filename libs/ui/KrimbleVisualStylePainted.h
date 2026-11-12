// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble Android visual refresh. Kept separate from layout and touch handling.
// Krimble 2026-11-11: THIS IS THE PAINTED (GRADIENT) SHEET, the one the app used before the flat theme (KrimbleVisualStyle.h). George: "Can we put in the Krimble gradient theme as an option?"
// It is now applied only while the theme "Krimble Painted" (krita/data/themes/KritaPainted.colors) is chosen (thememanager.cpp); the function was krimbleVisualStyleSheet(), now
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
    /* Krimble 2026-11-12 (task 33): George: "Definitely need bevels in the themes, all of them." One-pixel bevel edges (raised: light top and left, black bottom and right; sunken fields the other way round; no shading), as in the Nov 10 default sheet. */
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
    padding: 7px 13px;
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
    background: #1c3963;
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
    background: transparent;
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
    background: #192138;
}
QMenu {
    color: #eef1fa;
    border: 1px solid #3d4967;
    background: #192138;   /* Krimble 2026-11-06 (3rd): flat (was a #2e354e to #212639 gradient) */
}
QMenu::item:selected {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad,
                                stop:1 #05429d);
}
/* Krimble 2026-11-12 (task 30): George (a screenshot of the Image menu): "We had these lovely separators before and you wiped them out somehow." The dividers between menu items were a black line with a highlight line below it (KisWideDockSeparatorStyle, 2026-10-10); a menu that a style sheet paints is drawn by the sheet, not by that style, so they were a thin plain line. One pixel each, as before. */
QMenu::separator {
    height: 0px;
    margin: 4px 6px;
    border-top: 1px solid #000000;
    border-bottom: 1px solid rgba(255, 255, 255, 110);
}
QToolBar {
    border: none;
    /* Krimble 2026-11-11: George: "ANY bar is NOT SUPPOSED TO BE SHADED ON THE BOTTOM ... DARKENED TO THE CORNERS." Across only, dark at both ends. Before: a top-to-bottom gradient, stops #2f3a60 and #192138:
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #2f3a60, stop:1 #192138);
    */
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
/* Krimble 2026-11-11: the same fix as in the flat sheet (George: "Transparent menu."): a menu made with a dock as its parent (the toolbox's long press menu) is a direct child of the dock and the
   rule above made it transparent; this rule has the menu's own fill and edge and is as specific and later. */
QDockWidget > QMenu {
    color: #eef1fa;
    border: 1px solid #3d4967;
    background: #192138;
}
/* Krimble 2026-11-07 (3rd): George: "more contrast" (after the Prefs mockup: "The color and contrast in the mockup are ideal."). Before:
QStatusBar {
    color: #eef1fa;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2e354e, stop:1 #212639);
}
*/
QStatusBar {
    color: #ffffff;
    /* Krimble 2026-11-11: across only, dark at both ends. Before: background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #364063, stop:1 #212945); */
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #212945, stop:0.5 #364063, stop:1 #212945);
}
QHeaderView::section {
    color: #e9edf8;
    border: 1px solid #343c57;
    padding: 3px 6px;
    /* Krimble 2026-11-11: across only, dark at both ends. Before: a top-to-bottom gradient, stops #434b68 and #383f57:
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #434b68, stop:1 #383f57);
    */
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
)KRIMBLE");
}
