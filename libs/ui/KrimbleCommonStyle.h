// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-20 (756th): George: "Do the other steps next." (the cleanup plan's "one style sheet"). Rules for size and spacing that every Krimble theme shares live here, once,
// and are added after the theme's own sheet (thememanager.cpp), so a spacing fix can no longer reach one theme and miss another (as the 738th's menu room missed Krimble Painted).
// The theme sheets (KrimbleVisualStyle.h, KrimbleVisualStylePainted.h) keep only their colors and looks. Check a change with tools/preview/krimble_preview.py before a build.
#pragma once

#include <QString>

inline QString krimbleCommonStyleSheet()
{
    return QStringLiteral(R"KRIMBLE(
/* Menus: about one blank line of room under the last item and 12 units at the right (738th, 745th: the last item was cut off and long names ran past the right side). */
QMenu {
    padding-bottom: 16px;
    padding-right: 12px;
}
)KRIMBLE");
}
