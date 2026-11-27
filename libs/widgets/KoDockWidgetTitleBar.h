/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KODOCKWIDGETTITLEBAR_H_
#define KODOCKWIDGETTITLEBAR_H_

#include "kritawidgets_export.h"
#include <QDockWidget>

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

    void setTitleOverride(const QString &text);
    void setTabbedTitle(const QString &text);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
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

#endif
