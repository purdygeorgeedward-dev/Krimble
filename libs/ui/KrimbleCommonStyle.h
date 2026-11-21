// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-20 (756th): George: "Do the other steps next." (the cleanup plan's "one style sheet"). Rules for size and spacing that every Krimble theme shares live here, once,
// and are added after the theme's own sheet (thememanager.cpp), so a spacing fix can no longer reach one theme and miss another (as the 738th's menu room missed Krimble Painted).
// The theme sheets (KrimbleVisualStyle.h, KrimbleVisualStylePainted.h) keep only their colors and looks. Check a change with tools/preview/krimble_preview.py before a build.
#pragma once

#include <QString>

inline QString krimbleCommonStyleSheet()
{
    return QStringLiteral(R"KRIMBLE(
QTabWidget::pane { background: #374165; border: none; }
QTabBar::tab:selected { background: #374165; border-bottom: 1px solid #374165; }
QMenuBar::item { padding: 1px 9px 4px 9px; }
QTabBar::scroller { width: 64px; }
QAbstractItemView::item:selected { color: #ffffff; border-radius: 0px; /* background: #0b87dd; */ background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #085f9b, stop:0.05 #dae9f3, stop:0.49 #278fd7, stop:0.5 #0a80d2, stop:0.85 #60b1e9, stop:1 #85c3ee); }
/* Menu items: color and background only. A border or border-radius here gives the hovered item its own (smaller) size, and the menu squeezed when Fill opened its submenu. */
QMenu::item:selected { color: #ffffff; /* background: #0b87dd; */ background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #085f9b, stop:0.05 #dae9f3, stop:0.49 #278fd7, stop:0.5 #0a80d2, stop:0.85 #60b1e9, stop:1 #85c3ee); }
QMenuBar::item:selected, QMenuBar::item:pressed { border-bottom: none; }
NodeView::item { background: transparent; color: #f1f4fc; border: none; }
NodeView::item:selected { background: transparent; color: #ffffff; border: none; border-radius: 0px; }

QSlider:horizontal { min-height: 14px; }
QSlider::handle:horizontal { width: 12px; height: 12px; margin: -6px 0; border: none; background: transparent; border-image: url(:/krimble-ball-bearing-small.png) 0 0 0 0 stretch stretch; }

QMainWindow > QTabBar { qproperty-drawBase: 0; }
QMainWindow > QTabBar::tab:selected { background: palette(window); border-bottom: 1px solid palette(window); }

QMenuBar::item:selected, QMenuBar::item:pressed { background: rgba(0, 0, 0, 120); border-top: 1px solid rgba(0, 0, 0, 210); border-left: 1px solid rgba(0, 0, 0, 210); border-right: 1px solid rgba(255, 255, 255, 70); border-radius: 0px; }

/* Menus: about one blank line of room under the last item and 12 units at the right (738th, 745th: the last item was cut off and long names ran past the right side). */
QMenu {
    padding-bottom: 16px;
    padding-right: 12px;
}
)KRIMBLE");
}
