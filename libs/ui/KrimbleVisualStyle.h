// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <QString>

inline QString krimbleVisualStyleSheet()
{
    return QStringLiteral(R"KRIMBLE(

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
    border-radius: 3px;
    /* task 28: raised edge (light #687abd top and left) */
    border-top: 1px solid #687abd;
    border-left: 1px solid #687abd;
    padding: 5px 9px;
    background: #3e4b78;
}
QPushButton:hover, QDialog QToolButton:hover {
    /* border-color: #0b87dd; */ /* border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96); */ border-color: #0b87dd;
    background: #3e4b78;
}
QPushButton:pressed, QDialog QToolButton:pressed,
QPushButton:checked, QDialog QToolButton:checked {
    border: 2px solid #0b87dd; /* border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96); */ border-color: #0b87dd;
    /* background: #1d7cb2; */
    /* background: #0b87dd; */ background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #06538a, stop:0.05 #8fc2e6, stop:0.49 #1a7fc8, stop:0.5 #0872bd, stop:0.85 #2a8fd4, stop:1 #45a0dd);}
QPushButton:default {
    color: white;
    border: 2px solid #0b87dd; /* border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96); */ border-color: #0b87dd;
    /* background: #1d7cb2; */
    /* background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #0a89cd, stop:0.3 #085892, stop:0.6 #07497e, stop:1 #044a9c); */ background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #06538a, stop:0.05 #8fc2e6, stop:0.49 #1a7fc8, stop:0.5 #0872bd, stop:0.85 #2a8fd4, stop:1 #45a0dd);  /* OK, Create: "buttons need the new style" */}
/* every push button in Aqua, so it is the same everywhere ("The copy button in the show Krimble log window doesn't have the new aqua theme. It seems like it was applied randomly.") */
QPushButton, QPushButton:hover {
    color: #ffffff;
    border: 1px solid #075a96; /* border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96); */ border-color: #0b87dd;
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #06538a, stop:0.05 #8fc2e6, stop:0.49 #1a7fc8, stop:0.5 #0872bd, stop:0.85 #2a8fd4, stop:1 #45a0dd);
}
QPushButton:pressed {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #043d66, stop:0.05 #5f9fcc, stop:0.49 #12639e, stop:0.5 #065b96, stop:0.85 #1c70ab, stop:1 #2f80b8);
}
/* Nov 22: no glistening edges. The real Aqua is painted in code; a push button it does not paint (an unselected toggle, a color button) is a plain raised button. These rules come after the old Aqua rules above and win over them. */
QPushButton, QPushButton:hover, QPushButton:default {
    color: #f1f4fc;
    border: 1px solid #000000;
    border-top: 1px solid #687abd;
    border-left: 1px solid #687abd;
    background: #3e4b78;
}
QPushButton:checked {
    border: 1px solid #075a96;
    background: #0b87dd;
}
QPushButton:pressed {
    border: 1px solid #000000;
    background: #2f3a5e;
}
QPushButton:disabled, QDialog QToolButton:disabled {
    color: #7a8199;
    border-color: #000000;
    background: #232a42;
}
QTabBar::tab {
    color: #e3e8f6;
    padding: 4px 13px;    border: 1px solid #000000;
    border-bottom: 1px solid #000000;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    /* task 28: raised edge (light #3b456b top and left) */
    border-top: 1px solid #3b456b;
    border-left: 1px solid #3b456b;
    background: #1d2438;
}
QTabBar::tab:selected {
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
    /* border-radius: 5px; */ border-radius: 0px;   /* 872nd: square corners, the sunken bevel meets cleanly ("That's better.") */
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
    /* border-radius: 4px; */ border-radius: 0px;   /* 872nd: square corners, the sunken bevel meets cleanly ("That's better.") */
    /* task 28: sunken edge (light #3b456b bottom and right) */
    border-bottom: 1px solid #3b456b;
    border-right: 1px solid #3b456b;
    padding: 3px 5px;
    background: #141926;
}
QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
    border: 1px solid #0b87dd; /* border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96); */ border-color: #0b87dd;
}
QAbstractItemView::item:selected {
    color: white;
    /* background: #0b87dd; */ background: #0b87dd;
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
    /* background: #0b87dd; */ background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #06538a, stop:0.05 #8fc2e6, stop:0.49 #1a7fc8, stop:0.5 #0872bd, stop:0.85 #2a8fd4, stop:1 #45a0dd);
}
QSlider::handle:horizontal {
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
    border: 2px solid #0b87dd; /* border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96); */ border-color: #0b87dd;
    /* background: #1d7cb2; */
    /* background: #0b87dd; */ background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #06538a, stop:0.05 #8fc2e6, stop:0.49 #1a7fc8, stop:0.5 #0872bd, stop:0.85 #2a8fd4, stop:1 #45a0dd);}
QMenuBar {
    color: #f1f4fc;
    background: #3e4b78;
}
QMenuBar::item {
    padding: 1px 9px;
    margin: 1px 1px;
    background: transparent;
    border: 1px solid transparent;
    border-radius: 0px;
}
QMenuBar::item:selected, QMenuBar::item:pressed {
    border: 1px solid #232a42;
    border-radius: 0px;
    background: #232a42;}
QMenu {
    color: #eef1fa;
    border: none;    background: #232a42;
}
QMenu::item:selected {
    /* border: none; */
    /* background: #1d7cb2; */
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
    background: #3e4b78;
}
QMainWindow, QDockWidget {
    background: #2c3554;
}
QAbstractItemView {
    background: #151a2b;
    alternate-background-color: #1a2033;
    color: #ffffff;
}
QDockWidget > QWidget, QScrollArea, QScrollArea > QWidget > QWidget {
    background: transparent;
}
QDockWidget > QMenu {
    color: #eef1fa;
    border: 1px solid #000000;
    background: #232a42;
}
QStatusBar {
    color: #ffffff;
    background: #3e4b78;
    /* max-height: 22px; */
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
    border: 1px solid #000000;
    padding: 3px 6px;
    background: #3e4b78;
}
QProgressBar {
    color: white;
    text-align: center;   /* the percentage in the middle (it sat in the top left corner) */
    border: 1px solid #000000;
    border-radius: 0px;
    background: #141926;
}
QProgressBar::chunk {
    border-radius: 0px;
    /* background: #0b87dd; */ background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #06538a, stop:0.05 #8fc2e6, stop:0.49 #1a7fc8, stop:0.5 #0872bd, stop:0.85 #2a8fd4, stop:1 #45a0dd);
}
QToolTip {
    color: #eef1fa;
    border: 1px solid #0b87dd; /* border-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96); */ border-color: #0b87dd;
    background: #232a42;
}
QWidget#krimbleStudioCard {
    border: 1px solid #000000;
    border-radius: 0px;
    background: #2c3554;
}
)KRIMBLE");
}
