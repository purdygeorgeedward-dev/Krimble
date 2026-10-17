/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KODOCKWIDGETTITLEBAR_H_
#define KODOCKWIDGETTITLEBAR_H_

#include "kritawidgets_export.h"
#include <QDockWidget>

/**
 * @short A custom title bar for dock widgets.
 *
 * Allow customization such as hidden text.
 * 
 * @see KoDockWidgetTitleBarButton
 */
class KRITAWIDGETS_EXPORT KoDockWidgetTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit KoDockWidgetTitleBar(QDockWidget *dockWidget);
    ~KoDockWidgetTitleBar() override;

    void updateIcons();

public Q_SLOTS:
    void setLocked(bool locked);

    // Krimble 2026-10-18 (15th): hides the title text (the buttons stay); used for a panel that is in a tab group, whose title is already on its tab
    void setTitleTextHidden(bool hidden);

protected:
    void paintEvent(QPaintEvent* event) override; ///< reimplemented from QWidget
    void resizeEvent(QResizeEvent* event) override; ///< reimplemented from QWidget
    void contextMenuEvent(QContextMenuEvent *event) override; ///< Krimble 2026-10-10: Dock Left/Right/Top/Bottom, Undock
private:
    Q_PRIVATE_SLOT(d, void toggleFloating())
    Q_PRIVATE_SLOT(d, void topLevelChanged(bool topLevel))
    Q_PRIVATE_SLOT(d, void featuresChanged(QDockWidget::DockWidgetFeatures))
    Q_PRIVATE_SLOT(d, void dockWidgetTitleChanged(const QString &title))

    class Private;
    Private * const d;
};

#endif // KODOCKWIDGETTITLEBAR_H_
