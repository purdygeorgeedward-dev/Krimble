// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <QString>

inline QString krimbleCommonStyleSheet()
{
    return QStringLiteral(R"KRIMBLE(
QTabWidget::pane { background: #374165; border: none; }
QTabBar::tab:selected { background: #374165; border-bottom: 1px solid #374165; }
QMenuBar::item { padding: 1px 9px 4px 9px; }
QTabBar::scroller { width: 64px; }
QAbstractItemView::item:selected { color: #ffffff; border-radius: 0px; /* background: #0b87dd; */ background: #0b87dd; }
/* Menu items: color and background only. A border or border-radius here gives the hovered item its own (smaller) size, and the menu squeezed when Fill opened its submenu. */
QMenu::item:selected { color: #ffffff; /* background: #0b87dd; */ background: #0b87dd; }
QMenuBar::item:selected, QMenuBar::item:pressed { border-bottom: none; }
/* Krimble 2026-11-23 (874th): no strip of menu bar under an open title. The themes give each title a 1px margin, which left a band of bar color between the pressed title and its menu.
   George: "There is still a dividing line between the menu title and the menu. Remove that." Rule: menus touch their title areas with no dividing lines, ever. */
QMenuBar::item { margin-bottom: 0px; }
NodeView::item { background: transparent; color: #f1f4fc; border: none; }
NodeView::item:selected { background: transparent; color: #ffffff; border: none; border-radius: 0px; }

QSlider:horizontal { min-height: 14px; }
QSlider::handle:horizontal { width: 12px; height: 12px; margin: -6px 0; border: none; background: transparent; border-image: url(:/krimble-ball-bearing-small.png) 0 0 0 0 stretch stretch; }

QMainWindow > QTabBar { qproperty-drawBase: 0; }
QMainWindow > QTabBar::tab:selected { background: palette(window); border-bottom: 1px solid palette(window); }

QMenuBar::item:selected, QMenuBar::item:pressed { background: rgba(0, 0, 0, 120); border-top: 1px solid rgba(0, 0, 0, 210); border-left: 1px solid rgba(0, 0, 0, 210); border-right: 1px solid rgba(255, 255, 255, 70); border-radius: 0px; }

/* Menus: about one blank line of room under the last item and 12 units at the right (738th, 745th: the last item was cut off and long names ran past the right side). */
/* Krimble 2026-11-22: padding-bottom alone did nothing: Qt takes a menu's top and bottom room from padding-top (checked with the Preview Tool:
   padding-bottom 16px with padding-top 6px gave 6 at both ends). So both are set. "Menus still have very short bottoms." */
/* QMenu { padding-bottom: 16px; padding-right: 12px; } */
/* QMenu {
    padding-top: 10px;
    padding-bottom: 10px;
    padding-right: 12px;
} */
/* Krimble 2026-11-22: a visible edge on every menu, so a submenu no longer disappears into the menu under it ("Submenus still have no edges and disappear into menus.") */
/* QMenu { border: 1px solid #8a93b8; } */
/* Krimble 2026-11-23 (865th): menus back the way they were in build 74. George: "I never asked for a bunch of space inside the menus or white outlines around the menus. Remove those turds immediately." */
QMenu {
    padding-bottom: 16px;
    padding-right: 12px;
}
)KRIMBLE");
}
