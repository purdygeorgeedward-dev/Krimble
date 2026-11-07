/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoToolBoxButton_p.h"

#include <kis_debug.h>

#include <KoToolManager.h>
#include <QAction>
#include <QPalette>
#include <QApplication>
#include <klocalizedstring.h>
#include <QKeySequence>
#include <KoIcon.h>
#include "KrimbleToolboxIcons.h"   // Krimble 2026-10-30

KoToolBoxButton::KoToolBoxButton(KoToolAction *toolAction, QWidget *parent)
    : QToolButton(parent)
    , m_toolAction(toolAction)
{
    setObjectName(m_toolAction->id());
    // ensure same L&F
    setCheckable(true);
    setAutoRaise(true);
    // setIcon(kisIcon(m_toolAction->iconName()));
    // Krimble 2026-10-30: the Full color or the Clear style (see KrimbleToolboxIcons.h)
    setIcon(KrimbleToolboxIcons::icon(m_toolAction->iconName()));

    // only a fallback
    setToolTip(m_toolAction->toolTip());

    connect(this, SIGNAL(clicked(bool)), m_toolAction, SLOT(trigger()));
}

// Krimble 2026-10-30: the icon again, after the user changed the icon style in the toolbox's menu
void KoToolBoxButton::krimbleRefreshIcon()
{
    setIcon(KrimbleToolboxIcons::icon(m_toolAction->iconName()));
}

void KoToolBoxButton::attachAction(QAction *action)
{
    if (!action) {
        return;
    }

    connect(action, SIGNAL(changed()), SLOT(slotUpdateActionData()));
    setDataFromToolAction(action);
}

void KoToolBoxButton::setHighlightColor()
{
    // Krimble 2026-11-06: George: "Light blue areas are selected areas" (the mockups: the selected tool of the Mixer's tool strip, the selected tab, the selected color well). A
    // selected tool in the toolbox is drawn like the mockup's selected tool: the dark panel color inside and a 2 px light blue (#38C6FF) outline with rounded corners. The old
    // version below filled the button with the theme's highlight color.
    // QPalette p = qApp->palette();
    // if (isChecked()) {
    //     QPalette palette_highlight(p);
    //     QColor c = p.color(QPalette::Highlight);
    //     palette_highlight.setColor(QPalette::Button, c);
    //     setPalette(palette_highlight);
    // }
    // else {
    //     setPalette(p);
    // }
    setPalette(qApp->palette());
    if (isChecked()) {
        // setStyleSheet(QStringLiteral("QToolButton { border: 2px solid #38c6ff; border-radius: 6px; background-color: #151a33; }"));   // first version: dark inside, George: "It's not just the outline. It's the shading."
        // setStyleSheet(QStringLiteral("QToolButton { border: 2px solid #3bbcf5; border-radius: 6px;"
        //                              " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #0585ce, stop:0.25 #016cac, stop:0.5 #015c96, stop:0.75 #01548c, stop:1 #0165a5); }"));   // sampled from the Mixer mockup
        // Krimble 2026-11-06 (2nd): George: "I don't want an outline around the gradient icon in the toolbox." The same blue gradient, no outline.
        // setStyleSheet(QStringLiteral("QToolButton { border: none; border-radius: 6px;"
        //                              " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #0585ce, stop:0.25 #016cac, stop:0.5 #015c96, stop:0.75 #01548c, stop:1 #0165a5); }"));
        // Krimble 2026-11-06 (3rd): George: "blue selections should go to a deeper blue." (the bottom is now #05429D, the mockup's OK button blue)
        setStyleSheet(QStringLiteral("QToolButton { border: none; border-radius: 6px;"
                                     " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad, stop:1 #05429d); }"));
    } else {
        setStyleSheet(QString());
    }
}

void KoToolBoxButton::setDataFromToolAction(QAction *action)
{
    if (!action) {
        warnUI << "No tool action available, using fallback!";
        setToolTip(m_toolAction->toolTip());
        return;
    }
    setToolTip(action->toolTip());
}

void KoToolBoxButton::slotUpdateActionData()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action) {
        setDataFromToolAction(action);
    }
}
