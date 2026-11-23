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

    void setTitleTextHidden(bool hidden);

    /// Krimble: a title shown in the title row in place of the dock's own (the Tool Options docker shows the tool's name),
    /// also when the dock's title is hidden because it sits in a tab group; an empty text goes back to the dock's own title
    void setTitleOverride(const QString &text);
    /// Krimble 2026-11-22: a text for the title row while the panel's own title is hidden (the panel is in a tab group and the tab
    /// already names it). Tool Options puts the tool's name here, so it sits in the title row with the grip after it, like "Layers".
    void setTabbedTitle(const QString &text);

protected:
    void paintEvent(QPaintEvent* event) override; ///< reimplemented from QWidget
    void resizeEvent(QResizeEvent* event) override; ///< reimplemented from QWidget
    void contextMenuEvent(QContextMenuEvent *event) override; ///< Krimble 2026-10-10: Dock Left/Right/Top/Bottom, Undock
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    Q_PRIVATE_SLOT(d, void toggleFloating())
    Q_PRIVATE_SLOT(d, void topLevelChanged(bool topLevel))
    Q_PRIVATE_SLOT(d, void featuresChanged(QDockWidget::DockWidgetFeatures))
    Q_PRIVATE_SLOT(d, void dockWidgetTitleChanged(const QString &title))

    class Private;
    Private * const d;
};

#endif // KODOCKWIDGETTITLEBAR_H_
