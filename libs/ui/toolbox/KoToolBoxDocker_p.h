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
    QSize sizeHint() const override;
    void moveEvent(QMoveEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
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
    void snapToColumns();
    bool krimbleFingerAtToolbox() const;
    QAction *m_dragDockAction {nullptr};
    void applyColumns();
    void krimbleLogState(const char *where) const;
    bool m_userDragged {false};
    bool m_justDocked {false};
    Q_INVOKABLE void krimbleDockAtEdge();
    Qt::DockWidgetArea krimbleEdgeArea() const;
    int m_edgeDockToken {0};
    void fitHeightToContents(bool setWidthToo = false);
    void krimbleFitFloatingShape();
public:
    void floatingEdgeDrag(const QSize &startSize, const QPoint &delta, bool changeWidth, bool changeHeight);
private Q_SLOTS:
    void setFloatingEdges(bool floating);
private:
    int iconWidth() const;
    int chromeWidth() const;

private:
    KoToolBox *m_toolBox;
    KoToolBoxScrollArea *m_scrollArea;
    QVBoxLayout *m_containerLayout {nullptr};
    KoDualColorButton *m_dualColorButton {nullptr};
    KisViewManager *m_viewManager {nullptr};
    QMenu *m_contextMenu {nullptr};
    Qt::DockWidgetArea m_dockArea {Qt::NoDockWidgetArea};
    Qt::LayoutDirection m_layoutDir {Qt::LayoutDirectionAuto};
    Orientation m_orientation {Auto};
    int m_columns {2};
    QTimer *m_snapTimer {nullptr};
    QTimer *m_restoreTimer {nullptr};
    QTimer *m_screenTimer {nullptr};
    bool m_windowResizing {false};
    bool m_applying {false};
    QAction *m_columnActions[4] {nullptr, nullptr, nullptr, nullptr};
    QAction *m_dockActions[5] {nullptr, nullptr, nullptr, nullptr, nullptr};
    QWidget *m_edgeRight {nullptr};
    QWidget *m_edgeBottom {nullptr};
};

#endif // _KO_TOOLBOX_DOCKER_H_
