// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble Android visual refresh. Kept separate from layout and touch handling.
#pragma once

#include <QString>

inline QString krimbleVisualStyleSheet()
{
    return QStringLiteral(R"KRIMBLE(
QDialog, QMessageBox, QInputDialog, QColorDialog, QFileDialog {
    background-color: #282d49;
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
                                stop:0 #353d61, stop:1 #292e49);
}
QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }
QPushButton, QToolButton {
    color: #f1f4fc;
    border: 1px solid #59668d;
    border-top: 1px solid #8795bc;
    border-radius: 5px;
    padding: 5px 9px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #515d84, stop:0.48 #3c456a,
                                stop:1 #242a48);
}
QPushButton:hover, QToolButton:hover {
    border-color: #59b8f4;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #4775a7, stop:1 #234770);
}
QPushButton:pressed, QToolButton:pressed,
QPushButton:checked, QToolButton:checked {
    border: 2px solid #38c6ff;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #167bb7, stop:0.5 #095889,
                                stop:1 #07345e);
}
QPushButton:disabled, QToolButton:disabled {
    color: #8990a6;
    border-color: #444c67;
    background: #303650;
}
QTabBar::tab {
    color: #e3e8f6;
    padding: 7px 13px;
    border: 1px solid #5c668a;
    border-bottom: 1px solid #3c4262;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #424c72, stop:1 #272d4c);
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
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #222740, stop:1 #363d5e);
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
QSlider::groove:horizontal {
    height: 7px;
    border: 1px solid #535c80;
    border-radius: 3px;
    background: #20263f;
}
QSlider::handle:horizontal {
    width: 19px;
    margin: -7px 0;
    border: 2px solid #c9dfff;
    border-radius: 9px;
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                                stop:0 #f1f5ff, stop:1 #728eb8);
}
QCheckBox::indicator:checked {
    border: 1px solid #39ccff;
    background: #087eae;
}
)KRIMBLE");
}
