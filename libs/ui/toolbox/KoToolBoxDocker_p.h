/*
 * SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2005-2008 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 * SPDX-FileCopyrightText: 2022 Alvin Wong <alvin@alvinhc.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _KO_TOOLBOX_DOCKER_H_
#define _KO_TOOLBOX_DOCKER_H_

#include <kis_mainwindow_observer.h>

#include <QDockWidget>
#include <QTimer>

class KoCanvasBase;
class KoToolBox;
class KoToolBoxScrollArea;
class KoDualColorButton;
class KisViewManager;

class QMenu;
class QVBoxLayout;

class KoToolBoxDocker : public QDockWidget, public KisMainwindowObserver
{
    Q_OBJECT
public:
    explicit KoToolBoxDocker(KoToolBox *toolBox);

    /// reimplemented from KoCanvasObserverBase
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;
    QString observerName() override { return "KoToolBoxDocker"; }
    /// reimplemented from KisMainwindowObserver
    void setViewManager(KisViewManager *viewManager) override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    QSize sizeHint() const override;   // Krimble 2026-10-11: the width the chosen column count needs
    void moveEvent(QMoveEvent *event) override;   // Krimble 2026-10-11: a floating toolbox bounces back inside the screen
    bool eventFilter(QObject *watched, QEvent *event) override; // KRIMBLE 2026-10-06: watches the main window for rotation
    void contextMenuEvent(QContextMenuEvent *event) override;

protected Q_SLOTS:
    void updateToolBoxOrientation(Qt::DockWidgetArea);
    void updateFloating(bool);

private Q_SLOTS:
    void slotUpdateDisplayRenderer();

private:
    enum Orientation {
        Horizontal = Qt::Horizontal,
        Vertical = Qt::Vertical,
        Auto = -1,
    };

    void setToolBoxOrientation(Qt::Orientation);
    void updateLayoutDir();
    void changeLayoutDir(Qt::LayoutDirection);
    void changeOrientation(Orientation);
    void changeCompact(bool);
    // KRIMBLE 2026-10-06: the toolbox keeps its number of icon columns when the screen rotates, and fits to a
    // whole number of columns when it is resized (George: "It should maintain its size. Also, why not snap to
    // multiples of tool icon columns on scale?").
    void snapToColumns();
    void applyColumns();
    bool m_userDragged {false};   // Krimble 2026-10-13: a finger or mouse was down on the dock's edge since the last fit to columns
    Q_INVOKABLE void krimbleDockAtEdge();   // Krimble 2026-10-17: docks a floating toolbox that has reached the left, right or top edge of the screen
    void fitHeightToContents();   // Krimble 2026-10-12: a floating toolbox is only as tall as its tools
public:
    // KRIMBLE 2026-10-07: a floating toolbox has no corner gadget; it is resized by two edge strips (right edge: width,
    // fitted to whole icon columns; bottom edge: height). Called by the strips while they are dragged.
    void floatingEdgeDrag(const QSize &startSize, const QPoint &delta, bool changeWidth, bool changeHeight);
private Q_SLOTS:
    void setFloatingEdges(bool floating);
private:
    int iconWidth() const;
    int chromeWidth() const;

private:
    KoToolBox *m_toolBox;
    KoToolBoxScrollArea *m_scrollArea;
    QVBoxLayout *m_containerLayout {nullptr}; // Krimble: holds scroll area + dual color button
    KoDualColorButton *m_dualColorButton {nullptr}; // Krimble: PS-style FG/BG swap widget, bottom of toolbox
    KisViewManager *m_viewManager {nullptr}; // Krimble: stored for slotUpdateDisplayRenderer
    QMenu *m_contextMenu {nullptr};
    Qt::DockWidgetArea m_dockArea {Qt::NoDockWidgetArea};
    Qt::LayoutDirection m_layoutDir {Qt::LayoutDirectionAuto};
    Orientation m_orientation {Auto};
    // Krimble 2026-10-09: default is 1 column (was 2), so a screen rotation never changes the column count.
    // int m_columns {2};                 // Krimble: the number of icon columns the user wants (2 = the default look)
    // Krimble 2026-10-10 (2nd): default is 2 columns (George).
    // int m_columns {1};                 // Krimble: the number of icon columns the user wants (1 = the default look)
    int m_columns {2};                 // Krimble: the number of icon columns the user wants (2 = the default look)
    QTimer *m_snapTimer {nullptr};     // Krimble: runs a moment after a resize of the docked toolbox stops
    QTimer *m_restoreTimer {nullptr};  // Krimble: runs a moment after the main window stops resizing (rotation)
    QTimer *m_screenTimer {nullptr};   // Krimble 2026-10-11: runs a moment after a floating toolbox stops being moved
    bool m_windowResizing {false};     // Krimble: true while the main window is being resized (rotation)
    bool m_applying {false};           // Krimble: true while we set the width ourselves
    QAction *m_columnActions[4] {nullptr, nullptr, nullptr, nullptr}; // Krimble 2026-10-09: the "Columns" menu entries (1 to 4)
    QAction *m_dockActions[5] {nullptr, nullptr, nullptr, nullptr, nullptr}; // Krimble 2026-10-10: the "Dock" menu entries: left, right, top, bottom, undock
    QWidget *m_edgeRight {nullptr};    // Krimble: edge strip, only while floating
    QWidget *m_edgeBottom {nullptr};   // Krimble: edge strip, only while floating
};

#endif // _KO_TOOLBOX_DOCKER_H_
