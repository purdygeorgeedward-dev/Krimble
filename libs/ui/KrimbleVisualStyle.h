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
     top bar / menu bar / tool bars / status bar / headers / buttons #3e4b78 (V47), window / panels #2c3554 (V33), lists / menus / group boxes #262d47 (V24),
     inactive tabs #20273d (V20), input fields #171c2b (V13). The accent #1d7cb2 with a 2 px #3bd5f7 edge marks everything selected or pressed.
   The whole previous sheet (gradients and all) is kept as KrimbleVisualStyle_before_flat_2026-11-11.h.txt. This sheet is now applied only while the "Krimble dark" theme is chosen
   (thememanager.cpp), so the other themes are not painted over. */


/* Krimble 2026-11-11: George: "The blue glow in the selections doesn't look as cool without the dark areas inside, but it is cleaner." and, asked where: "Tabs, anything really." Everything that glows
   when selected (the selected tab, a pressed, checked or default button, the selected menu and menu bar item, a checked box) is dark navy inside (#072349 at the top to #0E3F78 at the bottom) with the
   bright cyan edge, as the selected tool is; the flat #1D7CB2 fill stays for list rows, the filled part of sliders and progress bars. */

/* Krimble 2026-11-12 (task 20): George (a screenshot of the JPEG options, Metadata tab): "These dark areas are too dark. black disappears against it. please lighten in theme." The three dark
   levels are 6 points brighter (hue 227, saturation 48 as before; brightness in the HSV of Color Lab): lists, menus and group boxes 24 to 30 (#20263d to #28304c), inactive tabs 20 to 26
   (#1b2033 to #232a42), input fields 13 to 19 (#111521 to #191f30); two odd darks follow (#141822 to #1c2230, #1c2130 to #252c40). The panel (33) and the bars (47) are as before. */

/* Krimble 2026-11-12 (task 21): George: "Try two points darker." The three dark levels of the note above are 2 points darker than that change (6 points lighter than before it became 4 points
   lighter): lists, menus and group boxes 28 (#262d47), inactive tabs 24 (#20273d), fields 17 (#171c2b), the two odd darks #191f2b and #22283b. (The colors named in the note above are the ones of task 20.) */

/* Krimble 2026-11-12 (task 22): George: "Try 26." The main dark level (lists, menus and group boxes) is brightness 26 (#232a42), the other levels keep their distance from it: inactive tabs 22 (#1d2438),
   fields 15 (#141926), the odd darks #161c26 and #1f2436. (The colors named in the notes above are the ones of the earlier tasks.) */
QDialog, QMessageBox, QInputDialog, QColorDialog, QFileDialog {
    background: #2c3554;
    color: #ffffff;
    /* task 28: the window's own bevel (light #516094 top and left, black bottom and right) */
    border: 1px solid #000000;
    border-top: 1px solid #516094;
    border-left: 1px solid #516094;
}
QDialog QLabel, QDialog QGroupBox, QDialog QCheckBox,
QDialog QRadioButton, QDialog QTabWidget {
    color: #e9edf8;
}
QGroupBox {
    border: 1px solid #000000;
    border-radius: 0px;
    /* task 28: raised edge (light #465380 top and left) */
    border-top: 1px solid #465380;
    border-left: 1px solid #465380;
    margin-top: 12px;
    padding: 12px 6px 6px 6px;
    background: #232a42;
}
QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }
QPushButton, QDialog QToolButton {
    color: #f1f4fc;
    border: 1px solid #000000;
    border-radius: 7px;
    /* task 28: raised edge (light #687abd top and left) */
    border-top: 1px solid #687abd;
    border-left: 1px solid #687abd;
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
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #1196d8, stop:0.10 #0a78bb, stop:0.5 #073d6b, stop:1 #074a85);   /* Krimble 2026-11-13 (task 78): George: "do the same glow on them all": the lit glow of the Bevel and Painted themes; before: dark navy #072349 to #0e3f78 */
}
QPushButton:default {
    color: white;
    border: 2px solid #3bd5f7;
    /* background: #1d7cb2; */
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #0a89cd, stop:0.3 #085892, stop:0.6 #07497e, stop:1 #044a9c);   /* Krimble 2026-11-13 (task 78): same glow as the other themes; before: #072349 to #0e3f78 */
}
QPushButton:disabled, QDialog QToolButton:disabled {
    color: #7a8199;
    border-color: #000000;
    background: #232a42;
}
QTabBar::tab {
    color: #e3e8f6;
    padding: 4px 13px;   /* Krimble 2026-11-13: tabs shorter (was 7px 13px) */
    border: 1px solid #000000;
    border-bottom: 1px solid #000000;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    /* task 28: raised edge (light #3b456b top and left) */
    border-top: 1px solid #3b456b;
    border-left: 1px solid #3b456b;
    background: #1d2438;
}
QTabBar::tab:selected {
    /* Krimble 2026-11-13 (task 69): George: "why is the selected tab glowing? It should just be lighter than the other tabs." No glow, no gradient: the selected tab is the same tab, lighter. Before:
    color: white;
    border: 2px solid #3bd5f7;
    border-bottom: 2px solid #3bd5f7;
    border-top-left-radius: 9px;
    border-top-right-radius: 9px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #072349, stop:1 #0e3f78); */
    color: white;
    border: 1px solid #000000;
    border-top: 1px solid #5d6a9c;
    border-left: 1px solid #5d6a9c;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    background: #3b456b;
}
QLineEdit, QSpinBox, QDoubleSpinBox {
    color: #f0f3ff;
    selection-background-color: #0b87dd;
    border: 1px solid #000000;
    border-radius: 5px;
    /* task 28: sunken edge (light #3b456b bottom and right) */
    border-bottom: 1px solid #3b456b;
    border-right: 1px solid #3b456b;
    padding: 3px 5px;
    background: #141926;
}
QComboBox {
    color: #f0f3ff;
    selection-background-color: #0b87dd;
    border: 1px solid #000000;
    border-radius: 4px;
    /* task 28: sunken edge (light #3b456b bottom and right) */
    border-bottom: 1px solid #3b456b;
    border-right: 1px solid #3b456b;
    padding: 3px 5px;
    background: #141926;
}
QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
    border: 1px solid #3bd5f7;
}
QAbstractItemView::item:selected {
    color: white;
    background: #0b87dd;
    border: none;
    border-radius: 0px;
}
QSlider::groove:horizontal {
    height: 5px;
    border: 1px solid #000000;
    border-radius: 2px;
    background: #141926;
}
QSlider::sub-page:horizontal {
    height: 5px;
    border: none;
    border-radius: 2px;
    background: #0b87dd;
}
QSlider::handle:horizontal {
    /* Krimble 2026-11-11: George: "make the handles round balls ... A metal ball bearing would look better." A steel ball: a bright reflection at the top, a dark horizon band
       across the middle, a lighter bounce at the bottom, a dark rim (a radial gradient, because a ball is not a flat color). */
    width: 18px;
    height: 18px;
    margin: -7px 0;
    border: 1px solid #161c26;
    border-radius: 10px;
    background: qradialgradient(cx:0.5, cy:0.5, radius:0.55, fx:0.34, fy:0.26,
                                stop:0 #ffffff, stop:0.18 #dfe4ee, stop:0.5 #7f889c, stop:0.85 #3a4152, stop:1 #1f2436);
}
QCheckBox::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid #000000;
    border-radius: 4px;
    background: #141926;
}
QCheckBox::indicator:checked {
    image: url(:/krimble-checkmark.svg);
    border: 2px solid #3bd5f7;
    /* background: #1d7cb2; */
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #1196d8, stop:0.10 #0a78bb, stop:0.5 #073d6b, stop:1 #074a85);   /* Krimble 2026-11-13 (task 78): same glow as the other themes; before: #072349 to #0e3f78 */
}
QMenuBar {
    color: #f1f4fc;
    background: #3e4b78;
}
QMenuBar::item {
    /* Krimble 2026-11-12: George: "Please restore the old height." The bar item had no space above and below since the flat sheet (b0f146d, 2026-11-11); the
       1 px padding and 1 px margin of the build 55 sheet are back (the left and right spacing, 9 px and 1 px, was never changed). Before this edit:
    padding: 0px 9px;
    margin: 0px 1px;
    */
    padding: 1px 9px;
    margin: 1px 1px;
    background: transparent;
    border: 1px solid transparent;
    border-radius: 0px;
}
QMenuBar::item:selected {
    /* Krimble 2026-11-13 (task 66): George: "no outline on the menu selection." border: 1px solid #3bd5f7; */
    border: none;
    border-radius: 0px;
    /* background: #1d7cb2; */
    background: #0b87dd;   /* Krimble 2026-11-13 (task 78): George: the menu bar name of an open menu is solid blue; before: #072349 to #0e3f78 gradient */
}
QMenu {
    color: #eef1fa;
    border: 1px solid #000000;
    background: #232a42;
}
QMenu::item:selected {
    /* Krimble 2026-11-13 (task 66): George: "no outline on the menu selection." border: 1px solid #3bd5f7; */
    border: none;
    /* background: #1d7cb2; */
    /* background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #072349, stop:1 #0e3f78);   Krimble 2026-11-11: dark inside the glow */
    /* Krimble 2026-11-13 (task 65): George: "Then remove the gradient rows." Solid light blue, no gradient (the same as the other themes' menu rows). */
    background: #0b87dd;
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
    background: #3e4b78;
}
QMainWindow, QDockWidget {
    background: #2c3554;
}
QAbstractItemView {
    background: #232a42;
    alternate-background-color: #1d2438;
    color: #ffffff;
}
QDockWidget > QWidget, QScrollArea, QScrollArea > QWidget > QWidget {
    background: transparent;
}
/* Krimble 2026-11-11: George (a screenshot of the toolbox's long press menu): "Transparent menu." The rule above makes every direct child of a dock transparent, and a menu made with
   the dock as its parent (the toolbox's Compact / Icon Size / Layout / Columns / Dock menu) is a direct child, and the rule's two names beat the single name of the QMenu rule, so the
   menu had its black edge and no fill. This rule is as specific and comes after it. */
QDockWidget > QMenu {
    color: #eef1fa;
    border: 1px solid #000000;
    background: #232a42;
}
QStatusBar {
    color: #ffffff;
    background: #3e4b78;
    max-height: 22px;   /* task 73 */
}
/* Krimble 2026-11-13 (task 73): George (a screenshot of the status bar): "no buttons or bevels or whatever. This was good before. but still too tall." The status bar: its items (the size and memory readout, the selection indicator) are plain: no button border, no bevel, no fill, no extra padding, so the bar is only as tall as its
   contents, and at most 22 px. */
QStatusBar QPushButton, QStatusBar QToolButton {
    border: none;
    background: transparent;
    padding: 0px 6px;
    margin: 0px;
    min-height: 0px;
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
    background: #141926;
}
QProgressBar::chunk {
    border-radius: 0px;
    background: #0b87dd;
}
QToolTip {
    color: #eef1fa;
    border: 1px solid #3bd5f7;
    background: #232a42;
}
QWidget#krimbleStudioCard {
    border: 1px solid #000000;
    border-radius: 0px;
    background: #2c3554;
}
)KRIMBLE");
}
