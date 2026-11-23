/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KoDockWidgetTitleBar_p_h
#define KoDockWidgetTitleBar_p_h

#include "KoDockWidgetTitleBar.h"
#include "KoDockWidgetTitleBarButton.h"

#include <KoIcon.h>

#include <WidgetsDebug.h>

#include <QAbstractButton>
#include <QAction>
#include <QLayout>
#include <QStyle>
#include <QStylePainter>
#include <QStyleOptionFrame>

class KSqueezedTextLabel;

class Q_DECL_HIDDEN KoDockWidgetTitleBar::Private
{
public:
    Private(KoDockWidgetTitleBar* thePublic)
        : thePublic(thePublic)
        , locked(false)
    {
    }

    KoDockWidgetTitleBar* thePublic {nullptr};
    QIcon lockIcon, floatIcon, removeIcon; 
    QAbstractButton* closeButton {nullptr};
    QAbstractButton* floatButton {nullptr};
    QAbstractButton* lockButton {nullptr};
    KSqueezedTextLabel* titleLabel {nullptr};
    bool titleTextHidden {false};
    QString titleOverride;   // Krimble: see setTitleOverride()
    QString tabbedTitle;     // Krimble: see setTabbedTitle()
    // what the title row shows: the override; or, when the panel's own title is hidden (it is in a tab group), the tabbed title; or the panel's title
    QString shownTitle(const QString &title) const { return !titleOverride.isEmpty() ? titleOverride : (titleTextHidden ? tabbedTitle : title); }
    bool locked {false};
    bool pressed {false};
    bool dragging {false};
    QPoint pressGlobal;
    QPoint grabOffset;
    QDockWidget::DockWidgetFeatures features;

    void toggleFloating();
    void topLevelChanged(bool topLevel);
    void featuresChanged(QDockWidget::DockWidgetFeatures features);
    void dockWidgetTitleChanged(const QString &title);
    void updateIcons();
    void updateButtonSizes();
};
#endif
