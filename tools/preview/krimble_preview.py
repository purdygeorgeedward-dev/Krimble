#!/usr/bin/env python3
# Krimble 2026-11-20 (756th): the preview check (George: "Do the other steps next."). Renders a menu bar, an open menu, tabs, a list and buttons
# with a Krimble style sheet, off screen, to a PNG, so a style change can be looked at before spending a build. Uses PyQt5 (Qt 5, like Krimble).
# Usage: python3 tools/preview/krimble_preview.py OUT.png [sheet-source ...]
#   sheet-source: a .h file holding one R"KRIMBLE( ... )KRIMBLE" style sheet (KrimbleVisualStyle.h, KrimbleVisualStylePainted.h, KrimbleCommonStyle.h).
# Phone scale: 3 real pixels per unit (QT_SCALE_FACTOR=3), like George's phone.
import os, re, sys
os.environ.setdefault('QT_QPA_PLATFORM', 'offscreen')
os.environ.setdefault('QT_SCALE_FACTOR', '2')   # 2 keeps the menu in one column on the off-screen display
from PyQt5.QtWidgets import (QApplication, QWidget, QMenuBar, QMenu, QTabWidget, QListWidget, QPushButton, QVBoxLayout, QHBoxLayout, QLabel)
from PyQt5.QtGui import QPainter, QImage
from PyQt5.QtCore import Qt, QPoint

def sheet_from(path):
    s = open(path, encoding='utf-8').read()
    m = re.findall(r'R"KRIMBLE\((.*?)\)KRIMBLE"', s, re.S)
    return '\n'.join(m)

def main():
    out = sys.argv[1]
    sheet = '\n'.join(sheet_from(p) for p in sys.argv[2:])
    app = QApplication(sys.argv)
    app.setStyle('Fusion')
    app.setStyleSheet(sheet)
    w = QWidget(); w.resize(360, 420)
    lay = QVBoxLayout(w)
    bar = QMenuBar(); lay.addWidget(bar)
    menu = QMenu('&File', bar)
    for t in ('New...', 'Open...', 'Open Recent', None, 'Save', 'Save As...', 'Revert', None, 'File info...', None, 'Close', 'Exit'):
        menu.addSeparator() if t is None else menu.addAction(t)
    bar.addMenu(menu); bar.addMenu(QMenu('&Edit', bar)); bar.addMenu(QMenu('&Image', bar))
    tabs = QTabWidget(); tabs.addTab(QLabel('Layers page'), 'Layers'); tabs.addTab(QLabel('x'), 'Color Lab'); lay.addWidget(tabs)
    lst = QListWidget(); [lst.addItem(s) for s in ('Background', 'Layer 2', 'Layer 3')]; lst.setCurrentRow(0); lay.addWidget(lst)
    row = QHBoxLayout(); [row.addWidget(QPushButton(t)) for t in ('OK', 'Cancel')]; lay.addLayout(row)
    w.show(); menu.popup(w.mapToGlobal(QPoint(0, bar.height()))); menu.setActiveAction(menu.actions()[1])
    for _ in range(5): app.processEvents()
    a = w.grab().toImage(); b = menu.grab().toImage()
    img = QImage(a.width() + b.width() + 20, max(a.height(), b.height()), QImage.Format_RGB32); img.fill(Qt.black)
    p = QPainter(img); p.drawImage(0, 0, a); p.drawImage(a.width() + 20, 0, b); p.end()
    img.save(out)
    print(out, img.width(), img.height())

if __name__ == '__main__':
    main()
