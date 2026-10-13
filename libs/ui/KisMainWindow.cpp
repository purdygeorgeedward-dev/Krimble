/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2000-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Thomas zander <zander@kde.org>
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KisMainWindow.h"

#include <KoConfig.h>

// qt includes
#include <QApplication>
#include <QByteArray>
#include <QCloseEvent>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QScreen>
#include <QDialog>
#include <QDockWidget>
#include <QIcon>
#include <QInputDialog>
#include <QLayout>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMutex>
#include <QMutexLocker>
#include <QPointer>
#include <KisSignalMapper.h>
#include <QTabBar>
#include <QMoveEvent>
#include <QUrl>
#include <QMessageBox>
#include <QStatusBar>
#include <QStyleFactory>
#include <algorithm>
#include <QHBoxLayout>
#include <KisAngleSelector.h>
#include <QPainterPath>
#include <kis_selection.h>
#include <QUuid>
#include <kis_pointer_utils.h>
#include "kis_processing_applicator.h"
#include "commands_new/kis_set_layer_style_command.h"
#include <kis_psd_layer_style.h>
#include <KSharedConfig>
#include <KConfigGroup>
#include <QFile>
#include <QImage>
#include <QCoreApplication>
#include <QFileInfo>
#include <QProgressBar>
#include <QComboBox>
#include "kis_node_commands_adapter.h"
#include <KisGlobalResourcesInterface.h>
#include <filter/kis_filter_configuration.h>
#include <filter/kis_filter.h>
#include <filter/kis_filter_registry.h>
#include <kis_adjustment_layer.h>
#include <QMap>
#include <QPushButton>
#include <QSlider>
#include <QDateTime>
#include <QDir>
#include <QSharedPointer>
#include <QUrlQuery>
#include <QToolBar>
#include <QSysInfo>
#include <QLabel>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QMenu>
#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#include <QScreen>
#include <QMouseEvent>
// #include <QSizeGrip>  // no longer used, see KisFloatingDockSizeGrip
#include <QPainter>
#include <QPolygonF>
#include <QDomDocument>
#include <QDomElement>
#include <QHash>
#include <QTimer>
#include <functional>
#include <memory>
#endif
#include <QMenuBar>
#include <KisMimeDatabase.h>
#include <QMimeData>
#include <QStackedWidget>
#include <QProxyStyle>
#include <QScreen>
#include <QAction>
#include <QWindow>
#include <QTemporaryDir>
#include <QScrollArea>
#include <QActionGroup>

#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kis_debug.h>
#include <kedittoolbar.h>
#include <khelpmenu.h>
#include <klocalizedstring.h>
#include <kaboutdata.h>
#include <kis_workspace_resource.h>
#include <input/kis_input_manager.h>
#include "dialogs/KisDlgCreateNewDocument.h"
#include "kis_selection_manager.h"
#include "kis_icon_utils.h"
#include <krecentfilesaction.h>
#include "krita_utils.h"
#include <ktoggleaction.h>
#include <ktoolbar.h>
#include <kmainwindow.h>
#include <kxmlguiwindow.h>
#include <kxmlguifactory.h>
#include <kxmlguiclient.h>
#include <kguiitem.h>
#include <kwindowconfig.h>
#include <kacceleratormanager.h>

#include <KoResourcePaths.h>
#include <KoToolFactoryBase.h>
#include <KoToolRegistry.h>
#include <KoDockFactoryBase.h>
#include <KoDockWidgetTitleBar.h>
#include <kis_utility_title_bar.h>
#include <KoDocumentInfoDlg.h>
#include <KoDocumentInfo.h>
#include <KoFileDialog.h>
#include <kis_icon.h>
#include <KoToolManager.h>
#include "KoToolDocker.h"
#include "toolbox/KoToolBoxFactory.h"
#include <KoDockRegistry.h>
#include <KoPluginLoader.h>
#include <KoColorSpaceEngine.h>
#include <KoUpdater.h>
#include <KisResourceModel.h>
#include <KisResourceLoaderRegistry.h>
#include <KisResourceIterator.h>
#include <KisResourceTypes.h>
#include <KisResourceCacheDb.h>
#include <KisStorageModel.h>
#include <KisStorageFilterProxyModel.h>
#include <KisPlaybackEngine.h>

#ifdef Q_OS_ANDROID
#include "KisAndroidDonations.h"
#include "dialogs/KisDonationManagementDialog.h"
#include <QtAndroid>
#include <KisAndroidUtils.h>
#endif

#include <KisUsageLogger.h>
#include <brushengine/kis_paintop_settings.h>
#include "dialogs/kis_about_application.h"
#include "dialogs/kis_delayed_save_dialog.h"
#include "dialogs/kis_dlg_preferences.h"
#include "kis_action_manager.h"
#include "KisApplication.h"
#include "kis_canvas2.h"
#include "kis_canvas_controller.h"
#include "kis_canvas_resource_provider.h"
#include "kis_clipboard.h"
#include "kis_config.h"
#include "kis_config_notifier.h"
#include "kis_custom_image_widget.h"
#include "animation/KisAnimationRender.h"
#include "animation/KisDlgAnimationRenderer.h"
#include <KisDocument.h>
#include "kis_group_layer.h"
#include "kis_image_from_clipboard_widget.h"
#include "kis_image.h"
#include "kis_image_animation_interface.h"
#include <KisImportExportFilter.h>
#include "KisImportExportManager.h"
#include "kis_mainwindow_observer.h"
#include "kis_node.h"
#include "KisOpenPane.h"
#include "kis_paintop_box.h"
#include "KisPart.h"
#include "KisResourceServerProvider.h"
#include "kis_signal_compressor_with_param.h"
#include "kis_statusbar.h"
#include "KisView.h"
#include "KisViewManager.h"
#include "thememanager.h"
#include "kis_animation_importer.h"
#include "dialogs/kis_dlg_import_image_sequence.h"
#include "animation/KisDlgImportVideoAnimation.h"
#include <KisImageConfigNotifier.h>
#include "KisWindowLayoutManager.h"
#include <KisUndoActionsUpdateManager.h>
#include "KisWelcomePageWidget.h"
#include "KisRecentDocumentsModelWrapper.h"
#include <KritaVersionWrapper.h>
#include "KisCanvasWindow.h"
#include "kis_action.h"
#include <katecommandbar.h>
#include "KisNodeActivationActionCreatorVisitor.h"
#include "KisUiFont.h"
#include <KisResourceUserOperations.h>
#include "KisRecentFilesManager.h"
#include "KisWidgetConnectionUtils.h"
#include "KisToolBarStateModel.h"
#include <config-qmdiarea-always-show-subwindow-title.h>
#include <config-qt-patches-present.h>

#include <mutex>

#if defined(Q_OS_ANDROID) && KRITA_QT_HAS_ANDROID_QPLATFORMSCREEN_DENSITY_ADJUSTMENT
#include <KisAndroidScaling.h>
#endif

#ifdef Q_OS_ANDROID
// KRIMBLE 2026-10-02: resize handle for floating panels (see the
// topLevelChanged handler in createDockWidget()). A visible corner handle with
// a large touch target that resizes its floating panel from the lower right:
// the left and top edges stay where they are.
// (It replaces a QSizeGrip subclass. QSizeGrip works out which corner it is in
// from where it sits in the window: on a narrow panel such as the toolbox its
// 36 pixel width put it in the left half, so it moved the lower left corner.)
class KisFloatingDockSizeGrip : public QWidget
{
public:
    explicit KisFloatingDockSizeGrip(QWidget *parent)
        : QWidget(parent)
    {
        // KRIMBLE 2026-10-04: smaller, was 36 x 36.
        // setFixedSize(36, 36);
        // KRIMBLE 2026-10-04 (2nd pass): 20 px, was 26 (and 36 before that).
        // setFixedSize(26, 26);
        setFixedSize(20, 20);
        // Looked up by this name (see the topLevelChanged handler). This class has no
        // Q_OBJECT, so findChild<KisFloatingDockSizeGrip*>() would match any widget.
        setObjectName(QStringLiteral("krimbleResizeHandle"));
    }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            m_pressPos = event->globalPos();
            m_startSize = window()->size();
            event->accept();
        } else {
            event->ignore();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (event->buttons() & Qt::LeftButton) {
            const QPoint delta = event->globalPos() - m_pressPos;
            window()->resize(qMax(1, m_startSize.width() + delta.x()),
                             qMax(1, m_startSize.height() + delta.y()));
            event->accept();
        }
    }

    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QPen pen(palette().color(QPalette::WindowText));
        pen.setWidthF(2.0);
        p.setPen(pen);
        for (int i = 1; i <= 3; ++i) {
            const qreal offset = i * 4.5;     // was i * 6 (26 px handle) and i * 8 (36 px handle)
            p.drawLine(QPointF(width() - 2.5 - offset, height() - 2.5), QPointF(width() - 2.5, height() - 2.5 - offset));
        }
    }

private:
    QPoint m_pressPos;
    QSize m_startSize;
};

// Keeps the resize handle in the lower right corner of its floating panel.
class KisFloatingDockGripPlacer : public QObject
{
public:
    KisFloatingDockGripPlacer(QWidget *dock, QWidget *grip)
        : QObject(dock)
        , m_dock(dock)
        , m_grip(grip)
    {
    }

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_dock && (event->type() == QEvent::Resize || event->type() == QEvent::Show)) {
            m_grip->move(m_dock->width() - m_grip->width(), m_dock->height() - m_grip->height());
            m_grip->raise();
        }
        return false;
    }

private:
    QWidget *m_dock;
    QWidget *m_grip;
};

// Remembers the size a panel had while it was docked, so that it can be given
// back when the panel is detached (used for the toolbox).
class KisDockedSizeTracker : public QObject
{
public:
    explicit KisDockedSizeTracker(QDockWidget *dock)
        : QObject(dock)
        , m_dock(dock)
    {
    }

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_dock && event->type() == QEvent::Resize && !m_dock->isFloating()) {
            m_dock->setProperty("krimbleDockedSize", m_dock->size());
        }
        // KRIMBLE 2026-10-02: remember where a floating panel was put (centre, in screen
        // coordinates), so that its own float button can dock it there too.
        if (watched == m_dock && m_dock->isFloating()
                && (event->type() == QEvent::Move || event->type() == QEvent::Resize)) {
            m_dock->setProperty("krimbleFloatingCenter", m_dock->geometry().center());
        }
        return false;
    }

private:
    QDockWidget *m_dock;
};
#endif

// KRIMBLE 2026-10-02: used by Settings > Attach Panel.
// Docks a floating panel where the user put it: the dock area nearest to the panel's
// current position, and, in that area, between the panels above and below it.
static void krimbleAttachDockWhereItIs(QMainWindow *win, QDockWidget *dock, const QPoint &center)
{
    const QRect windowRect = win->geometry();
    if (windowRect.width() <= 0 || windowRect.height() <= 0) {
        dock->setFloating(false);
        return;
    }
    // Set while this runs, so the "docked again" handler in createDockWidget() does not
    // start a second placement.
    dock->setProperty("krimbleAttaching", true);

    // Left or right: the side of the main window the panel is nearest to. (Panels are
    // not put above or below the canvas.)
    const qreal fx = qreal(center.x() - windowRect.left()) / windowRect.width();
    const Qt::DockWidgetArea area = (fx < 0.5) ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea;

    const bool vertical = (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea);
    const Qt::Orientation stackOrientation = vertical ? Qt::Vertical : Qt::Horizontal;
    const int centerAlong = vertical ? center.y() : center.x();

    // Panels already docked in that area, in their order along it.
    QList<QDockWidget*> docked;
    Q_FOREACH (QDockWidget *other, win->findChildren<QDockWidget*>()) {
        if (other != dock && other->isVisible() && !other->isFloating() && win->dockWidgetArea(other) == area) {
            docked.append(other);
        }
    }
    std::sort(docked.begin(), docked.end(), [vertical](QDockWidget *a, QDockWidget *b) {
        return vertical ? a->geometry().top() < b->geometry().top() : a->geometry().left() < b->geometry().left();
    });

    // The panel after which the moved panel goes: the last one whose middle lies before it.
    QDockWidget *after = nullptr;
    Q_FOREACH (QDockWidget *other, docked) {
        const QPoint otherCenter = other->geometry().center();
        if ((vertical ? otherCenter.y() : otherCenter.x()) < centerAlong) {
            after = other;
        }
    }

    // A floating panel is only allowed in no area; allow all before docking it.
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    win->addDockWidget(area, dock);
    if (after) {
        win->splitDockWidget(after, dock, stackOrientation);
    } else if (!docked.isEmpty()) {
        // Goes before every panel already there: put the others after it, in order.
        QDockWidget *previous = dock;
        Q_FOREACH (QDockWidget *other, docked) {
            win->splitDockWidget(previous, other, stackOrientation);
            previous = other;
        }
    }
    dock->setFloating(false);
    dock->show();
    dock->setProperty("krimbleAttaching", false);
    dock->setProperty("krimbleFloatingCenter", QVariant());
}

static void krimbleAttachDockWhereItIs(QMainWindow *win, QDockWidget *dock)
{
    krimbleAttachDockWhereItIs(win, dock, dock->geometry().center());
}

// BEGIN KRIMBLE RIGHT PANELS
// KRIMBLE 2026-10-03: one big, easy-to-press side button for the right-hand panels.
//  - a quick TAP hides all the panels on the right-hand side (the toolbox on the left is not
//    touched); while they are hidden the same button sits at the right edge of the window and a
//    tap shows them again;
//  - a LONG PRESS (hold about 0.35 s) turns it into a drag handle that resizes the panels' width
//    (a quick sideways swipe of more than 16 px does the same).
// It is a real widget laid over the middle of the thin bar between the canvas and the panels, so
// it does not depend on Qt's own bar. The same thing is in Settings > "Hide Right Panels".
// (The first version, commit eeb51e1, watched taps on Qt's own bar instead; replaced.)

// Krimble 2026-10-11: the default for "show only an outline while a floating image window is dragged" (mdi_rubberband).
// On Android the picture is shown while dragging (George); elsewhere it follows OpenGL as before.
static int krimbleRubberBandDefault(KisConfig &cfg)
{
#ifdef Q_OS_ANDROID
    Q_UNUSED(cfg);
    return 0;
#else
    return cfg.useOpenGL();
#endif
}

// Krimble 2026-10-12: the toolbars in the top area stack, one on each row, instead of lining up in one row with the rest hidden
// behind the ">>" button (George). A toolbar break is put in front of every top toolbar except the first one.
static void krimbleStackToolBars(QMainWindow *win)
{
    QList<QToolBar*> top;
    Q_FOREACH (QToolBar *toolBar, win->findChildren<QToolBar*>()) {
        if (toolBar->parentWidget() == win && win->toolBarArea(toolBar) == Qt::TopToolBarArea && !toolBar->isHidden()) {
            top.append(toolBar);
        }
    }
    std::sort(top.begin(), top.end(), [](QToolBar *a, QToolBar *b) {
        if (a->geometry().y() != b->geometry().y()) return a->geometry().y() < b->geometry().y();
        return a->geometry().x() < b->geometry().x();
    });
    for (int i = 1; i < top.size(); ++i) {
        if (!win->toolBarBreak(top[i])) {
            win->insertToolBarBreak(top[i]);
        }
    }
}

#ifdef Q_OS_ANDROID
// Krimble 2026-10-12: George: "Menus are still way too wide." Qt adds a column for the longest keyboard shortcut to every menu
// after it has measured the items, so the style cannot take it away. The only way is for the actions not to show their shortcut
// (a touch screen has no keyboard). Done for every action in every menu, again every 2 s for menus filled later.
static void krimbleHideMenuShortcuts(QMenu *menu, int depth)
{
    if (!menu || depth > 6) return;
    Q_FOREACH (QAction *action, menu->actions()) {
        if (action->isShortcutVisibleInContextMenu()) {
            action->setShortcutVisibleInContextMenu(false);
        }
        if (action->menu()) krimbleHideMenuShortcuts(action->menu(), depth + 1);
    }
}
#endif

#ifdef Q_OS_ANDROID
// Krimble 2026-10-12: an invisible touch zone, 14 px wide, along the inner edge of every docked panel (the right edge of a panel
// on the left, the left edge of a panel on the right). Dragging it resizes the panel's side of the window. It draws nothing and
// takes no room of its own, so the edge between the toolbox, the canvas and the panels can be as thin as the base style makes it
// (George: "a usable invisible grab zone there, not more UI fat").
class KrimbleDockEdgeGrab : public QWidget
{
public:
    // Krimble 2026-10-12 (2nd): George: not the inner edge, it overlaps the tools. The zone is a child of the main window now and lies
    // on the OUTER side of the panel's edge, over the first 14 px of the canvas. (The first version was a child of the panel.)
    KrimbleDockEdgeGrab(QMainWindow *win, QDockWidget *dock) : QWidget(win), m_win(win), m_dock(dock)
    {
        setCursor(Qt::SizeHorCursor);
        // setAttribute(Qt::WA_NoSystemBackground);          (the invisible versions)
        // setAttribute(Qt::WA_TranslucentBackground);
        dock->installEventFilter(this);
        reposition();
    }
    void reposition()
    {
        if (!m_dock || m_dock->isFloating() || !m_dock->isVisible()) {
            hide();
            return;
        }
        const Qt::DockWidgetArea area = m_win->dockWidgetArea(m_dock);
        if (area != Qt::LeftDockWidgetArea && area != Qt::RightDockWidgetArea) {
            hide();
            return;
        }
        // Krimble 2026-10-12 (3rd): George: "The touch zone doesn't have to be the whole edge of a panel or toolbox. It could be a small
        // slightly protruding handle in the middle." So one small handle (18 x 80 px) in the middle of each side's edge, on the
        // canvas side, drawn as a little rounded tab with three grip lines. Only the topmost panel of a side has it, and it is
        // centered on the whole column of panels on that side.
        // const int w = 18;
        // const int h = 80;
        // const int w = 12;   // Krimble 2026-10-12 (4th): George: "Handle on toolbox is too big." 12 x 48 (was 18 x 80)
        // const int h = 48;
        const int w = 7;    // Krimble 2026-10-12 (5th): George: "Handle on toolbox is still too big. Should just be a slight curve outward." 7 x 40, a gentle bulge
        const int h = 40;
        QRect column;                       // the area covered by all docked panels on this side
        bool topmost = true;
        Q_FOREACH (QDockWidget *other, m_win->findChildren<QDockWidget*>()) {
            if (other->isFloating() || !other->isVisible() || m_win->dockWidgetArea(other) != area) continue;
            column = column.isNull() ? other->geometry() : column.united(other->geometry());
            if (other->geometry().top() < m_dock->geometry().top()) topmost = false;
        }
        if (!topmost || column.isNull()) {
            hide();
            return;
        }
        // const QRect g = m_dock->geometry();
        // setGeometry(area == Qt::LeftDockWidgetArea ? g.right() + 1 : g.left() - w, g.top(), w, g.height());   (the whole edge, the second version)
        const int y = column.top() + (column.height() - h) / 2;
        setGeometry(area == Qt::LeftDockWidgetArea ? column.right() + 1 : column.left() - w, y, w, h);
        show();
        raise();
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_dock && (event->type() == QEvent::Resize || event->type() == QEvent::Show
                                  || event->type() == QEvent::Move || event->type() == QEvent::ParentChange)) {
            reposition();
        }
        return false;
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        QColor fill = palette().color(QPalette::Mid);
        fill.setAlpha(215);
        p.setPen(Qt::NoPen);
        p.setBrush(fill);
        // (the rounded tab with three grip lines, kept:)
        // p.drawRoundedRect(QRectF(rect()), 7, 7);
        // Krimble 2026-10-12 (5th): a slight curve outward from the panel edge, no grip lines.
        const qreal w = width();
        const qreal h = height();
        QPainterPath bulge;
        if (m_win->dockWidgetArea(m_dock) == Qt::LeftDockWidgetArea) {     // the panel is on the left: the curve bulges to the right
            bulge.moveTo(0, 0);
            bulge.cubicTo(w * 1.3, h * 0.18, w * 1.3, h * 0.82, 0, h);
        } else {                                                           // the panels are on the right: the curve bulges to the left
            bulge.moveTo(w, 0);
            bulge.cubicTo(-w * 0.3, h * 0.18, -w * 0.3, h * 0.82, w, h);
        }
        bulge.closeSubpath();
        p.drawPath(bulge);
    }
    void mousePressEvent(QMouseEvent *e) override
    {
        m_startX = e->globalPos().x();
        m_startWidth = m_dock->width();
        e->accept();
    }
    void mouseMoveEvent(QMouseEvent *e) override
    {
        if (!(e->buttons() & Qt::LeftButton)) return;
        const int dx = e->globalPos().x() - m_startX;
        const bool leftArea = m_win->dockWidgetArea(m_dock) == Qt::LeftDockWidgetArea;
        const int width = qMax(40, leftArea ? m_startWidth + dx : m_startWidth - dx);
        m_win->resizeDocks({m_dock}, {width}, Qt::Horizontal);
        e->accept();
    }
    void mouseReleaseEvent(QMouseEvent *e) override
    {
        e->accept();
    }

private:
    QMainWindow *m_win;
    QDockWidget *m_dock;
    int m_startX {0};
    int m_startWidth {0};
};

static void krimbleInstallDockEdgeGrabs(QMainWindow *win)
{
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        const QString grabName = QStringLiteral("krimbleEdgeGrab_") + dock->objectName();
        QWidget *grab = win->findChild<QWidget*>(grabName, Qt::FindDirectChildrenOnly);
        if (!grab) {
            grab = new KrimbleDockEdgeGrab(win, dock);
            grab->setObjectName(grabName);
        }
        if (KrimbleDockEdgeGrab *edge = static_cast<KrimbleDockEdgeGrab*>(grab)) {
            edge->reposition();
        }
    }
}
#endif

// Krimble 2026-10-12: Layout Presets (Claude's idea: "workspace presets: Retouch, Illustration, Layout and Quick Edit, one tap each";
// George: "Better workspaces! Yes. The Krita ones suck."). Each preset shows the panels that job needs and hides the rest. The
// toolbox is always kept. Panels are matched by their title, so a translated Krimble matches on the English title only.
static void krimbleApplyPanelPreset(QMainWindow *win, const QStringList &show)
{
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (dock->objectName() == QLatin1String("ToolBox") || dock->isFloating()) continue;
        const QString title = dock->windowTitle().remove(QLatin1Char('&')).trimmed();
        bool want = false;
        Q_FOREACH (const QString &name, show) {
            if (title.compare(name, Qt::CaseInsensitive) == 0) want = true;
        }
        dock->setVisible(want);
    }
    if (QDockWidget *toolBox = win->findChild<QDockWidget*>(QStringLiteral("ToolBox"))) {
        toolBox->show();
    }
}

// ============================================================================================================================
// Krimble 2026-10-12: Quick Adjust (Claude's idea; George: "Build quick adjust. It's such a useful feature. Be careful of the width.")
// A small panel with six sliders that adjust the picture WITHOUT changing its pixels: each group of sliders is an adjustment layer
// (created the first time a slider in that group moves away from 0, and edited as the slider moves), so the user can delete or
// hide the layer, or change it later in the Layers panel. Brightness/Contrast, Saturation/Hue/Lightness and Vibrance.
// Narrow on purpose: one slider per row, no side-by-side controls.
// ============================================================================================================================
class KrimbleQuickAdjust : public QObject
{
public:
    KrimbleQuickAdjust(KisMainWindow *win, QWidget *content) : QObject(content), m_win(win)
    {
        QVBoxLayout *layout = new QVBoxLayout(content);
        layout->setContentsMargins(6, 6, 6, 6);
        layout->setSpacing(2);
        for (int i = 0; i < SliderCount; ++i) {
            m_labels[i] = new QLabel(content);
            m_sliders[i] = new QSlider(Qt::Horizontal, content);
            m_sliders[i]->setRange(def(i).min, def(i).max);
            m_sliders[i]->setValue(0);
            m_sliders[i]->setMinimumWidth(1);
            layout->addWidget(m_labels[i]);
            layout->addWidget(m_sliders[i]);
            updateLabel(i);
            connect(m_sliders[i], &QSlider::valueChanged, this, [this, i](int value) {
                m_values[i] = value;
                updateLabel(i);
                m_timer.start(90);
            });
        }
        QPushButton *reset = new QPushButton(i18n("Reset"), content);
        layout->addWidget(reset);
        layout->addStretch(1);
        connect(reset, &QPushButton::clicked, this, [this]() {
            for (int i = 0; i < SliderCount; ++i) {
                m_sliders[i]->setValue(0);     // each change updates its layer
            }
        });
        m_timer.setSingleShot(true);
        connect(&m_timer, &QTimer::timeout, this, [this]() { apply(); });
        for (int i = 0; i < SliderCount; ++i) m_values[i] = 0;
    }

private:
    enum { Brightness, Contrast, Saturation, Hue, Lightness, Vibrance, SliderCount };
    struct Def { const char *name; int min; int max; };
    static Def def(int i)
    {
        static const Def defs[SliderCount] = {
            { "Brightness", -100, 100 }, { "Contrast", -100, 100 }, { "Saturation", -100, 100 },
            { "Hue", -180, 180 }, { "Lightness", -100, 100 }, { "Vibrance", -100, 100 }
        };
        return defs[i];
    }
    void updateLabel(int i)
    {
        m_labels[i]->setText(QStringLiteral("%1   %2").arg(i18n(def(i).name)).arg(m_sliders[i]->value()));
    }
    void apply()
    {
        updateLayer(m_bcLayer, QStringLiteral("brightnesscontrast"), i18n("Quick Adjust: Brightness/Contrast"),
                    { { QStringLiteral("brightness"), m_values[Brightness] }, { QStringLiteral("contrast"), m_values[Contrast] } },
                    m_values[Brightness] != 0 || m_values[Contrast] != 0);
        updateLayer(m_hsvLayer, QStringLiteral("hsvadjustment"), i18n("Quick Adjust: Color"),
                    { { QStringLiteral("h"), m_values[Hue] }, { QStringLiteral("s"), m_values[Saturation] }, { QStringLiteral("v"), m_values[Lightness] } },
                    m_values[Hue] != 0 || m_values[Saturation] != 0 || m_values[Lightness] != 0);
        updateLayer(m_vibLayer, QStringLiteral("vibrance"), i18n("Quick Adjust: Vibrance"),
                    { { QStringLiteral("vibrance"), m_values[Vibrance] } }, m_values[Vibrance] != 0);
    }
    void updateLayer(KisNodeWSP &ref, const QString &filterId, const QString &layerName, const QMap<QString, QVariant> &props, bool anyNonZero)
    {
        KisViewManager *view = m_win->viewManager();
        if (!view) return;
        KisImageSP image = view->image();
        if (!image) return;
        if (m_image.data() != image.data()) {      // another picture: forget the layers of the old one
            m_bcLayer = KisNodeWSP();
            m_hsvLayer = KisNodeWSP();
            m_vibLayer = KisNodeWSP();
            m_image = image;
        }
        KisNodeSP existing = ref;
        KisAdjustmentLayer *layer = (existing && existing->parent()) ? dynamic_cast<KisAdjustmentLayer*>(existing.data()) : nullptr;
        if (!layer) {
            if (!anyNonZero) return;
            KisFilterSP filter = KisFilterRegistry::instance()->value(filterId);
            if (!filter) return;
            KisFilterConfigurationSP config = filter->defaultConfiguration(KisGlobalResourcesInterface::instance());
            for (auto it = props.constBegin(); it != props.constEnd(); ++it) {
                config->setProperty(it.key(), it.value());
            }
            KisAdjustmentLayerSP newLayer = new KisAdjustmentLayer(image, layerName, config, KisSelectionSP());
            KisNodeSP active = view->activeNode();
            KisNodeSP parent = active ? active->parent() : KisNodeSP();
            if (!parent) parent = image->root();
            KisNodeCommandsAdapter adapter(view);
            adapter.addNode(newLayer, parent, active);
            ref = KisNodeSP(newLayer);
        } else {
            KisFilterConfigurationSP config = layer->filter()->clone();
            for (auto it = props.constBegin(); it != props.constEnd(); ++it) {
                config->setProperty(it.key(), it.value());
            }
            layer->setFilter(config);
            layer->setDirty();
        }
    }

    KisMainWindow *m_win;
    QLabel *m_labels[SliderCount];
    QSlider *m_sliders[SliderCount];
    int m_values[SliderCount];
    QTimer m_timer;
    KisImageWSP m_image;
    KisNodeWSP m_bcLayer;
    KisNodeWSP m_hsvLayer;
    KisNodeWSP m_vibLayer;
};

// Opens (or shows and hides) the Quick Adjust panel. It starts floating, 62% of the screen wide, so it never has to fit next to the
// toolbox, canvas and panels.
static void krimbleToggleQuickAdjust(KisMainWindow *win)
{
    QDockWidget *dock = win->findChild<QDockWidget*>(QStringLiteral("KrimbleQuickAdjust"));
    if (dock) {
        dock->setVisible(!dock->isVisible());
        if (dock->isVisible()) dock->raise();
        return;
    }
    dock = new QDockWidget(i18n("Quick Adjust"), win);
    dock->setObjectName(QStringLiteral("KrimbleQuickAdjust"));
    QWidget *content = new QWidget(dock);
    new KrimbleQuickAdjust(win, content);
    dock->setWidget(content);
    win->addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setFloating(true);
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QRect avail = scr->availableGeometry();
        dock->resize(int(avail.width() * 0.62), content->sizeHint().height() + 40);
        dock->move(avail.left() + int(avail.width() * 0.19), avail.top() + int(avail.height() * 0.30));
    }
    dock->show();
    dock->raise();
}

// Krimble 2026-10-12: Mirror Layout (Left-Handed) (Claude's idea; George: "Mirror layout - smart idea. Yes"). Every panel docked on
// the left moves to the right and every panel docked on the right moves to the left, keeping the order top to bottom and keeping
// panels that share tabs together. The window layout is saved when the app closes, so the choice stays.
static void krimbleMirrorLayout(QMainWindow *win)
{
    struct Group { QDockWidget *first; QList<QDockWidget*> others; int y; };
    auto collect = [win](Qt::DockWidgetArea area) {
        QList<Group> groups;
        QList<QDockWidget*> seen;
        QList<QDockWidget*> docks = win->findChildren<QDockWidget*>();
        std::sort(docks.begin(), docks.end(), [](QDockWidget *a, QDockWidget *b) { return a->geometry().y() < b->geometry().y(); });
        Q_FOREACH (QDockWidget *dock, docks) {
            if (dock->isFloating() || win->dockWidgetArea(dock) != area || seen.contains(dock)) continue;
            Group g;
            g.first = dock;
            g.y = dock->geometry().y();
            seen.append(dock);
            Q_FOREACH (QDockWidget *tab, win->tabifiedDockWidgets(dock)) {
                g.others.append(tab);
                seen.append(tab);
            }
            groups.append(g);
        }
        return groups;
    };
    const QList<Group> leftGroups = collect(Qt::LeftDockWidgetArea);
    const QList<Group> rightGroups = collect(Qt::RightDockWidgetArea);
    auto place = [win](const QList<Group> &groups, Qt::DockWidgetArea to) {
        Q_FOREACH (const Group &g, groups) {
            win->addDockWidget(to, g.first);
            Q_FOREACH (QDockWidget *other, g.others) {
                win->addDockWidget(to, other);
                win->tabifyDockWidget(g.first, other);
            }
            g.first->show();
            g.first->raise();
        }
    };
    place(leftGroups, Qt::RightDockWidgetArea);
    place(rightGroups, Qt::LeftDockWidgetArea);
}

// ============================================================================================================================
// Krimble 2026-10-12: Help menu extras: "Where is...?", "Send Feedback" and the "QA Overlay".
// "Where is...?" and the QA overlay were Claude's ideas (Claude's idea); the QA overlay is like the developer tools in a web
// browser (George: "Yes."). "Send Feedback" is Claude's idea too.
// ============================================================================================================================

// "Where is...?": every menu entry with its path, searchable; tapping one opens it (Claude's idea).
static void krimbleCollectMenuActions(QMenu *menu, const QString &path, QList<QPair<QString, QPointer<QAction> > > &out, int depth)
{
    if (!menu || depth > 6) return;
    Q_EMIT menu->aboutToShow();    // some menus (Window, recent files) fill themselves when they are about to be shown
    Q_FOREACH (QAction *action, menu->actions()) {
        if (action->isSeparator()) continue;
        QString text = action->text();
        text.remove(QLatin1Char('&'));
        text = text.section(QLatin1Char('\t'), 0, 0).trimmed();
        if (action->menu()) {
            krimbleCollectMenuActions(action->menu(), path + text + QStringLiteral("  >  "), out, depth + 1);
        } else if (!text.isEmpty()) {
            out.append(qMakePair(path + text, QPointer<QAction>(action)));
        }
    }
}

static void krimbleShowWhereIs(QMainWindow *win)
{
    QSharedPointer<QList<QPair<QString, QPointer<QAction> > > > items(new QList<QPair<QString, QPointer<QAction> > >());
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        if (!top->menu()) continue;
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        krimbleCollectMenuActions(top->menu(), title + QStringLiteral("  >  "), *items, 0);
    }

    QDialog *dialog = new QDialog(win);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(i18n("Where is...?"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(6, 6, 6, 6);
    QLineEdit *edit = new QLineEdit(dialog);
    edit->setPlaceholderText(i18n("Type a feature name, for example Crop"));
    QListWidget *list = new QListWidget(dialog);
    layout->addWidget(edit);
    layout->addWidget(list, 1);

    auto refill = [items, edit, list]() {
        list->clear();
        const QString needle = edit->text().trimmed();
        int shown = 0;
        for (int i = 0; i < items->size() && shown < 80; ++i) {
            if (needle.isEmpty() || items->at(i).first.contains(needle, Qt::CaseInsensitive)) {
                QListWidgetItem *item = new QListWidgetItem(items->at(i).first, list);
                item->setData(Qt::UserRole, i);
                ++shown;
            }
        }
    };
    QObject::connect(edit, &QLineEdit::textChanged, dialog, refill);
    QObject::connect(list, &QListWidget::itemClicked, dialog, [items, dialog](QListWidgetItem *item) {
        QPointer<QAction> action = items->at(item->data(Qt::UserRole).toInt()).second;
        dialog->close();
        if (action && action->isEnabled()) {
            QTimer::singleShot(0, action.data(), [action]() { if (action) action->trigger(); });
        }
    });
    refill();
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QRect avail = scr->availableGeometry();
        dialog->resize(int(avail.width() * 0.92), int(avail.height() * 0.80));
    }
    dialog->show();
    edit->setFocus();
}

// "Cheat Sheet": the names people know from other graphics editors, each with where it is in Krimble (Claude's idea).
// The paths are looked up in the real menus when the sheet opens, so they are always right; a name whose entry is not found
// is left out. Tapping a line opens that entry.
static void krimbleShowCheatSheet(QMainWindow *win)
{
    QList<QPair<QString, QPointer<QAction> > > all;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        if (!top->menu()) continue;
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        krimbleCollectMenuActions(top->menu(), title + QStringLiteral("  >  "), all, 0);
    }
    // familiar name, a word that is in the menu entry's name
    static const char *const terms[][2] = {
        {"Crop", "Crop"}, {"Image Size", "Scale Image"}, {"Canvas Size", "Canvas Size"}, {"Rotate Image", "Rotate Image"},
        {"Flatten Image", "Flatten Image"}, {"Merge Visible", "Merge Visible"}, {"Merge Down", "Merge Down"},
        {"New Layer", "New Layer"}, {"Duplicate Layer", "Duplicate Layer"}, {"Layer Mask", "Mask"},
        {"Free Transform", "Free Transform"}, {"Warp", "Warp"}, {"Liquify", "Liquify"}, {"Flip", "Flip"},
        {"Select All", "Select All"}, {"Deselect", "Deselect"}, {"Inverse Selection", "Inverse"}, {"Feather", "Feather"},
        {"Color Range", "Color Range"}, {"Brightness/Contrast", "Brightness"}, {"Levels", "Levels"}, {"Curves", "Curves"},
        {"Hue/Saturation", "Hue"}, {"Color Balance", "Color Balance"}, {"Vibrance", "Vibrance"}, {"Invert", "Invert"},
        {"Gaussian Blur", "Gaussian"}, {"Unsharp Mask", "Unsharp"}, {"Noise", "Noise"}, {"Pixelate", "Pixelate"},
        {"Convert Color Mode", "Mode"}, {"Fill", "Fill"}, {"Stroke", "Stroke"}, {"Undo", "Undo"}, {"Copy Merged", "Copy Merged"},
        {"Paste as New Image", "Paste as New"}, {"Preferences", "Preferences"}
    };
    QSharedPointer<QList<QPair<QString, QPointer<QAction> > > > shown(new QList<QPair<QString, QPointer<QAction> > >());
    QStringList lines;
    for (const auto &term : terms) {
        for (const auto &entry : all) {
            if (entry.first.contains(QLatin1String(term[1]), Qt::CaseInsensitive)) {
                lines << QStringLiteral("%1   ->   %2").arg(QLatin1String(term[0]), entry.first);
                shown->append(entry);
                break;
            }
        }
    }
    QDialog *dialog = new QDialog(win);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(i18n("Cheat Sheet"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(6, 6, 6, 6);
    QLabel *intro = new QLabel(i18n("The names you know from other graphics editors, and where each one is in Krimble. Tap a line to open it."), dialog);
    intro->setWordWrap(true);
    QListWidget *list = new QListWidget(dialog);
    list->addItems(lines);
    layout->addWidget(intro);
    layout->addWidget(list, 1);
    QObject::connect(list, &QListWidget::currentRowChanged, dialog, [shown, dialog](int row) {
        if (row < 0 || row >= shown->size()) return;
        QPointer<QAction> action = shown->at(row).second;
        dialog->close();
        if (action && action->isEnabled()) {
            QTimer::singleShot(0, action.data(), [action]() { if (action) action->trigger(); });
        }
    });
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QRect avail = scr->availableGeometry();
        dialog->resize(int(avail.width() * 0.92), int(avail.height() * 0.80));
    }
    dialog->show();
}

// "Share": the picture as it looks flattened, through Android's share sheet (Claude's idea; George: "Share out is great.").
static void krimbleShareImage(KisMainWindow *win, bool asJpg)
{
#ifdef Q_OS_ANDROID
    // KisImageSP image = win->viewManager() ? win->viewManager()->image() : KisImageSP();   (did not compile: weak and shared pointer in one ?:)
    KisImageSP image;
    if (win->viewManager()) {
        image = win->viewManager()->image();
    }
    if (!image) {
        QMessageBox::information(win, i18n("Share"), i18n("Open an image first."));
        return;
    }
    image->waitForDone();
    QImage flat = image->convertToQImage(image->bounds(), image->colorSpace()->profile());
    if (flat.isNull()) {
        QMessageBox::warning(win, i18n("Share"), i18n("The image could not be prepared for sharing."));
        return;
    }
    const QString dirPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/share");
    QDir().mkpath(dirPath);
    QString base = QStringLiteral("Krimble-%1").arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss")));
    const QString path = dirPath + QLatin1Char('/') + base + (asJpg ? QStringLiteral(".jpg") : QStringLiteral(".png"));
    bool saved;
    if (asJpg) {
        QImage onWhite(flat.size(), QImage::Format_RGB32);
        onWhite.fill(Qt::white);
        QPainter painter(&onWhite);
        painter.drawImage(0, 0, flat);
        painter.end();
        saved = onWhite.save(path, "JPG", 92);
    } else {
        saved = flat.save(path, "PNG");
    }
    if (!saved) {
        QMessageBox::warning(win, i18n("Share"), i18n("The image could not be saved for sharing."));
        return;
    }
    QAndroidJniObject jPath = QAndroidJniObject::fromString(path);
    QAndroidJniObject jMime = QAndroidJniObject::fromString(asJpg ? QStringLiteral("image/jpeg") : QStringLiteral("image/png"));
    QAndroidJniObject jName = QAndroidJniObject::fromString(base + (asJpg ? QStringLiteral(".jpg") : QStringLiteral(".png")));
    const jboolean ok = QAndroidJniObject::callStaticMethod<jboolean>("org/krimble/android/ShareHelper", "shareFile",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", jPath.object<jstring>(), jMime.object<jstring>(), jName.object<jstring>());
    if (!ok) {
        QMessageBox::warning(win, i18n("Share"), i18n("Sharing did not work on this phone."));
    }
#else
    Q_UNUSED(win);
    Q_UNUSED(asJpg);
#endif
}

// "Batch Process...": resize or convert many pictures with one tap (Claude's idea; George: "Add batch presets.").
// Pick pictures, pick a preset, and the results are saved in Pictures/Krimble/Batch (the originals are not touched).
// Works on pictures Qt can read (JPG, PNG, WebP, BMP and the like), not on layered files.
static void krimbleShowBatch(KisMainWindow *win)
{
#ifdef Q_OS_ANDROID
    struct Preset { const char *label; int width; bool jpg; int quality; const char *suffix; };
    static const Preset presets[] = {
        // (the first version, by width, kept:)
        // { "Resize to 1080 px wide (JPG)",           1080, true,  90, "1080" },
        // { "Resize to 1920 px wide (JPG)",           1920, true,  90, "1920" },
        // { "Small for messages: 1280 px (JPG 80%)",  1280, true,  80, "msg" },
        // { "Thumbnail 256 px wide (PNG)",             256, false, 100, "thumb" },
        // Krimble 2026-10-12: George: "Why 1080 wide? It's usually a height from 1080p." The number is the LONGEST side now, so a picture
        // fits whichever way up it is (landscape or portrait).
        { "HD 1080p: longest side 1920 px (JPG)",    1920, true,  90, "hd" },
        { "Social: longest side 1080 px (JPG)",      1080, true,  90, "social" },
        { "Messages: longest side 1280 px (JPG 80%)", 1280, true,  80, "msg" },
        { "Thumbnail: longest side 256 px (PNG)",     256, false, 100, "thumb" },
        { "Convert to JPG (same size)",                0, true,  92, "jpg" },
        { "Convert to PNG (same size)",                0, false, 100, "png" }
    };
    QDialog *dialog = new QDialog(win);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(i18n("Batch Process"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(8, 8, 8, 8);
    QLabel *intro = new QLabel(i18n("Pick a preset, then the pictures. Results go to Pictures/Krimble/Batch; the originals are not changed."), dialog);
    intro->setWordWrap(true);
    QComboBox *combo = new QComboBox(dialog);
    for (const Preset &p : presets) combo->addItem(i18n(p.label));
    QPushButton *start = new QPushButton(i18n("Choose Pictures and Start..."), dialog);
    QProgressBar *bar = new QProgressBar(dialog);
    bar->setRange(0, 100);
    bar->setValue(0);
    QLabel *status = new QLabel(dialog);
    status->setWordWrap(true);
    // Krimble 2026-10-12: George: "There is no cancel option for Batch process." Cancel stops after the picture being processed (it is
    // enabled only while a batch runs); Close closes the window (enabled only when no batch runs).
    QPushButton *cancel = new QPushButton(i18n("Cancel"), dialog);
    cancel->setEnabled(false);
    QPushButton *closeButton = new QPushButton(i18n("Close"), dialog);
    QHBoxLayout *buttonRow = new QHBoxLayout;
    buttonRow->addWidget(cancel);
    buttonRow->addWidget(closeButton);
    layout->addWidget(intro);
    layout->addWidget(combo);
    layout->addWidget(start);
    layout->addWidget(bar);
    layout->addWidget(status);
    layout->addLayout(buttonRow);
    layout->addStretch(1);
    QSharedPointer<bool> cancelRequested(new bool(false));
    QPointer<QDialog> guard(dialog);
    QObject::connect(closeButton, &QPushButton::clicked, dialog, [dialog]() { dialog->close(); });
    QObject::connect(cancel, &QPushButton::clicked, dialog, [cancel, cancelRequested]() {
        *cancelRequested = true;
        cancel->setEnabled(false);
    });
    // QObject::connect(start, &QPushButton::clicked, dialog, [win, dialog, combo, start, bar, status]() {   (the first version, without Cancel)
    QObject::connect(start, &QPushButton::clicked, dialog, [win, dialog, combo, start, bar, status, cancel, closeButton, cancelRequested, guard]() {
        KoFileDialog fileDialog(win, KoFileDialog::OpenFiles, "KrimbleBatch");
        fileDialog.setCaption(i18n("Choose pictures"));
        fileDialog.setMimeTypeFilters(QStringList() << QStringLiteral("image/jpeg") << QStringLiteral("image/png")
                                                    << QStringLiteral("image/webp") << QStringLiteral("image/bmp"));
        const QStringList files = fileDialog.filenames();
        if (files.isEmpty()) return;
        const Preset &preset = presets[qBound(0, combo->currentIndex(), 5)];
        start->setEnabled(false);
        closeButton->setEnabled(false);
        cancel->setEnabled(true);
        *cancelRequested = false;
        const QString dirPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/batch");
        QDir().mkpath(dirPath);
        int done = 0, failed = 0;
        for (int i = 0; i < files.size(); ++i) {
            status->setText(i18n("Picture %1 of %2...", i + 1, files.size()));
            bar->setValue(100 * i / files.size());
            QCoreApplication::processEvents();
            if (!guard) return;                  // the window was closed (Android's back button) while a picture was processed
            if (*cancelRequested) break;
            QImage image(files.at(i));
            if (image.isNull()) { ++failed; continue; }
            // if (preset.width > 0 && image.width() > preset.width) {
            //     image = image.scaledToWidth(preset.width, Qt::SmoothTransformation);
            // }
            if (preset.width > 0 && qMax(image.width(), image.height()) > preset.width) {   // preset.width is the longest side
                image = image.scaled(preset.width, preset.width, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            if (preset.jpg) {
                QImage onWhite(image.size(), QImage::Format_RGB32);
                onWhite.fill(Qt::white);
                QPainter painter(&onWhite);
                painter.drawImage(0, 0, image);
                painter.end();
                image = onWhite;
            }
            QString base = QFileInfo(files.at(i)).completeBaseName();
            if (base.isEmpty()) base = QStringLiteral("picture");
            const QString name = QStringLiteral("%1-%2.%3").arg(base, QLatin1String(preset.suffix), preset.jpg ? QStringLiteral("jpg") : QStringLiteral("png"));
            const QString temp = dirPath + QLatin1Char('/') + name;
            const bool saved = preset.jpg ? image.save(temp, "JPG", preset.quality) : image.save(temp, "PNG");
            bool stored = false;
            if (saved) {
                QAndroidJniObject jPath = QAndroidJniObject::fromString(temp);
                QAndroidJniObject jMime = QAndroidJniObject::fromString(preset.jpg ? QStringLiteral("image/jpeg") : QStringLiteral("image/png"));
                QAndroidJniObject jName = QAndroidJniObject::fromString(name);
                QAndroidJniObject jFolder = QAndroidJniObject::fromString(QStringLiteral("Batch"));
                stored = QAndroidJniObject::callStaticMethod<jboolean>("org/krimble/android/ShareHelper", "saveToPictures",
                    "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z",
                    jPath.object<jstring>(), jMime.object<jstring>(), jName.object<jstring>(), jFolder.object<jstring>());
                QFile::remove(temp);
            }
            if (stored) ++done; else ++failed;
        }
        if (!guard) return;
        const bool wasCancelled = *cancelRequested;
        bar->setValue(wasCancelled ? bar->value() : 100);
        status->setText(wasCancelled ? i18n("Cancelled: %1 saved in Pictures/Krimble/Batch, %2 skipped, the rest not processed.", done, failed)
                                     : i18n("Done: %1 saved in Pictures/Krimble/Batch, %2 skipped.", done, failed));
        cancel->setEnabled(false);
        closeButton->setEnabled(true);
        start->setEnabled(true);
    });
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        dialog->resize(int(scr->availableGeometry().width() * 0.92), dialog->sizeHint().height() + 30);
    }
    dialog->show();
#else
    Q_UNUSED(win);
#endif
}

// Adds "Share as JPG" and "Share as PNG" to the File menu, just above Quit (Claude's idea).
static void krimbleInstallFileExtras(KisMainWindow *win)
{
    QMenu *file = nullptr;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        if (top->menu() && title == QLatin1String("File")) file = top->menu();
    }
    if (!file || file->property("krimbleFileExtras").toBool()) return;
    file->setProperty("krimbleFileExtras", true);
    QObject::connect(file, &QMenu::aboutToShow, win, [win, file]() {
        if (file->property("krimbleFileExtrasAdded").toBool()) return;
        file->setProperty("krimbleFileExtrasAdded", true);
        QAction *before = nullptr;
        Q_FOREACH (QAction *action, file->actions()) {
            QString text = action->text();
            text.remove(QLatin1Char('&'));
            if (text == QLatin1String("Quit") || text == QLatin1String("Exit")) before = action;
        }
        QAction *shareJpg = new QAction(i18n("Share as JPG..."), file);
        QObject::connect(shareJpg, &QAction::triggered, win, [win]() { krimbleShareImage(win, true); });
        QAction *sharePng = new QAction(i18n("Share as PNG..."), file);
        QObject::connect(sharePng, &QAction::triggered, win, [win]() { krimbleShareImage(win, false); });
        // Krimble 2026-10-12: Batch Process... (Claude's idea)
        QAction *batch = new QAction(i18n("Batch Process..."), file);
        QObject::connect(batch, &QAction::triggered, win, [win]() { krimbleShowBatch(win); });
        QAction *sep = new QAction(file);
        sep->setSeparator(true);
        file->insertAction(before, sep);
        file->insertAction(before, shareJpg);
        file->insertAction(before, sharePng);
        file->insertAction(before, batch);
    });
}

// "Send Feedback": opens a prefilled GitHub issue (Claude's idea).
static void krimbleSendFeedback(QMainWindow *win)
{
    QString device = QSysInfo::prettyProductName();
    if (QScreen *scr = win->screen()) {
        device += QStringLiteral(", screen %1x%2, density %3").arg(scr->size().width()).arg(scr->size().height()).arg(scr->devicePixelRatio());
    }
    device += QStringLiteral(", window %1x%2").arg(win->width()).arg(win->height());
    const QString version = KritaVersionWrapper::versionString();
    const QString body = QStringLiteral("**What happened / what would help:**\n\n\n\n---\nKrimble %1\n%2\n").arg(version, device);
    QUrl url(QStringLiteral("https://github.com/purdygeorgeedward-dev/krimble/issues/new"));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("title"), QStringLiteral("Feedback (Krimble %1)").arg(version));
    query.addQueryItem(QStringLiteral("body"), body);
    url.setQuery(query);
    QDesktopServices::openUrl(url);
}

// The QA overlay: live numbers on the screen, for screenshots (Claude's idea).
static QString krimbleQaReport(QMainWindow *win)
{
    QString r;
    const qreal dpr = win->devicePixelRatioF();
    r += QStringLiteral("%1  window %2x%3  density %4  (sizes are logical px; x density = device px)\n")
            .arg(win->width() > win->height() ? QStringLiteral("LANDSCAPE") : QStringLiteral("PORTRAIT"))
            .arg(win->width()).arg(win->height()).arg(dpr);
    if (QScreen *scr = win->screen()) {
        r += QStringLiteral("screen %1x%2  avail %3x%4\n").arg(scr->size().width()).arg(scr->size().height())
                .arg(scr->availableGeometry().width()).arg(scr->availableGeometry().height());
    }
    if (QWidget *central = win->centralWidget()) {
        r += QStringLiteral("central %1x%2  min %3\n").arg(central->width()).arg(central->height()).arg(central->minimumSizeHint().width());
    }
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (!dock->isVisible()) continue;
        QString where = dock->isFloating() ? QStringLiteral("float") : QString::number(int(win->dockWidgetArea(dock)));
        r += QStringLiteral("%1 %2x%3  min %4  max %5  %6\n").arg(dock->objectName()).arg(dock->width()).arg(dock->height())
                .arg(dock->minimumSizeHint().width()).arg(dock->maximumWidth() > 100000 ? QStringLiteral("-") : QString::number(dock->maximumWidth())).arg(where);
    }
    Q_FOREACH (QToolBar *bar, win->findChildren<QToolBar*>()) {
        if (!bar->isVisible()) continue;
        r += QStringLiteral("bar %1 %2x%3\n").arg(bar->objectName()).arg(bar->width()).arg(bar->height());
    }
    if (QWidget *active = QApplication::activeWindow()) {
        if (QDialog *dlg = qobject_cast<QDialog*>(active)) {
            r += QStringLiteral("DIALOG %1 %2x%3  minHint %4x%5\n").arg(QString::fromLatin1(dlg->metaObject()->className())).arg(dlg->width()).arg(dlg->height())
                    .arg(dlg->minimumSizeHint().width()).arg(dlg->minimumSizeHint().height());
            QList<QPair<int, QString> > widest;
            Q_FOREACH (QWidget *child, dlg->findChildren<QWidget*>()) {
                if (!child->isVisible()) continue;
                widest.append(qMakePair(child->minimumSizeHint().width(),
                    QStringLiteral("%1 '%2'").arg(QString::fromLatin1(child->metaObject()->className()), child->objectName())));
            }
            std::sort(widest.begin(), widest.end(), [](const QPair<int, QString> &a, const QPair<int, QString> &b) { return a.first > b.first; });
            for (int i = 0; i < widest.size() && i < 8; ++i) {
                r += QStringLiteral("  widest %1: %2\n").arg(widest[i].first).arg(widest[i].second);
            }
        }
    }
    return r;
}

static void krimbleSetQaOverlay(QMainWindow *win, bool on)
{
    QLabel *label = win->findChild<QLabel*>(QStringLiteral("krimbleQaOverlay"), Qt::FindDirectChildrenOnly);
    if (!on) {
        if (label) label->deleteLater();
        return;
    }
    if (label) return;
    label = new QLabel(win);
    label->setObjectName(QStringLiteral("krimbleQaOverlay"));
    label->setAttribute(Qt::WA_TransparentForMouseEvents);
    label->setStyleSheet(QStringLiteral("background-color: rgba(0, 0, 0, 190); color: #9CFF9C; padding: 6px; font-family: monospace; font-size: 13px;"));
    label->move(6, 60);
    QTimer *timer = new QTimer(label);
    QObject::connect(timer, &QTimer::timeout, label, [win, label]() {
        label->setText(krimbleQaReport(win));
        label->adjustSize();
        label->raise();
    });
    timer->start(500);
    label->setText(krimbleQaReport(win));
    label->adjustSize();
    label->show();
    label->raise();
}

// Adds the three entries to the end of the Help menu each time it opens (once).
static void krimbleInstallHelpExtras(QMainWindow *win)
{
    QMenu *help = nullptr;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        if (top->menu() && title == QLatin1String("Help")) help = top->menu();
    }
    if (!help || help->property("krimbleHelpExtras").toBool()) return;
    help->setProperty("krimbleHelpExtras", true);
    QObject::connect(help, &QMenu::aboutToShow, win, [win, help]() {
        if (help->property("krimbleHelpExtrasAdded").toBool()) return;
        help->setProperty("krimbleHelpExtrasAdded", true);
        help->addSeparator();
        QAction *whereIs = help->addAction(i18n("Where is...?"));
        QObject::connect(whereIs, &QAction::triggered, win, [win]() { krimbleShowWhereIs(win); });
        // Krimble 2026-10-12: removed (George): choosing a line in the Cheat Sheet crashed the app in 1.0.33. The code is kept.
        // QAction *cheatSheet = help->addAction(i18n("Cheat Sheet"));
        // QObject::connect(cheatSheet, &QAction::triggered, win, [win]() { krimbleShowCheatSheet(win); });
        QAction *feedback = help->addAction(i18n("Send Feedback"));
        QObject::connect(feedback, &QAction::triggered, win, [win]() { krimbleSendFeedback(win); });
        QAction *overlay = help->addAction(i18n("QA Overlay"));
        overlay->setCheckable(true);
        overlay->setChecked(win->findChild<QLabel*>(QStringLiteral("krimbleQaOverlay"), Qt::FindDirectChildrenOnly) != nullptr);
        QObject::connect(overlay, &QAction::toggled, win, [win](bool on) { krimbleSetQaOverlay(win, on); });
    });
}

static QList<QDockWidget*> krimbleRightDocks(QMainWindow *win, bool visibleOnly)
{
    QList<QDockWidget*> docks;
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (dock->isFloating() || win->dockWidgetArea(dock) != Qt::RightDockWidgetArea) {
            continue;
        }
        // KRIMBLE 2026-10-04: the toolbox is never one of the "right-hand panels".
        if (dock->objectName() == QLatin1String("ToolBox")) {
            continue;
        }
        if (visibleOnly && !dock->isVisible()) {
            continue;
        }
        docks << dock;
    }
    return docks;
}

// Sets the width of the right-hand panel column (clamped so the panels keep their smallest width
// and the canvas keeps some room).
static void krimbleSetRightPanelsWidth(QMainWindow *win, int width)
{
    const QList<QDockWidget*> docks = krimbleRightDocks(win, true);
    if (docks.isEmpty()) {
        return;
    }
    int minimum = 80;
    Q_FOREACH (QDockWidget *dock, docks) {
        minimum = qMax(minimum, dock->minimumSizeHint().width());
    }
    const int maximum = qMax(minimum, win->width() - 140);
    width = qBound(minimum, width, maximum);
    QList<int> sizes;
    Q_FOREACH (QDockWidget *dock, docks) {
        Q_UNUSED(dock);
        sizes << width;
    }
    win->resizeDocks(docks, sizes, Qt::Horizontal);
}

static int krimbleRightPanelsWidth(QMainWindow *win)
{
    int width = 0;
    Q_FOREACH (QDockWidget *dock, krimbleRightDocks(win, true)) {
        width = qMax(width, dock->width());
    }
    return width;
}

class KrimbleSideBarButton : public QWidget
{
public:
    // onTap: quick tap. onDrag(dx): finger moved dx pixels sideways since the drag began
    // (negative = to the left); onDragStart is called once when a drag begins.
    // KRIMBLE 2026-10-06: onRelease(globalX) is called when a drag ends (the panels collapse when the finger is let
    // go close to the side). The widget is now a full-height grip along the panels' edge, see paintEvent().
    KrimbleSideBarButton(QWidget *parent, std::function<void()> onTap,
                         std::function<void()> onDragStart, std::function<void(int)> onDrag,
                         std::function<void(int)> onRelease = nullptr)
        : QWidget(parent), m_onTap(onTap), m_onDragStart(onDragStart), m_onDrag(onDrag), m_onRelease(onRelease)
    {
        setObjectName(QStringLiteral("krimbleRightPanelHandle"));
        // A thin sliver on the edge of the panels. The touch area is wider than what is drawn
        // (22 x 140), so it is easy to press, but only a 6 px sliver is visible.
        // KRIMBLE 2026-10-06: a full-height strip (36 px wide) along the panels' edge instead of a 22 x 140 button;
        // the height is set by krimbleUpdateRightPanelUi(). Old line:
        // setFixedSize(22, 140);
        setFixedWidth(36);
        m_timer.setSingleShot(true);
        m_timer.setInterval(350);
        connect(&m_timer, &QTimer::timeout, this, [this]() {
            if (m_pressed && !m_dragging) {
                beginDrag();
            }
        });
    }

    void setCollapsed(bool collapsed)
    {
        if (m_collapsed != collapsed) {
            m_collapsed = collapsed;
            update();
        }
    }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() != Qt::LeftButton) {
            event->ignore();
            return;
        }
        m_pressed = true;
        m_dragging = false;
        m_press = event->globalPos();
        m_timer.start();
        update();
        event->accept();
    }
    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (!m_pressed) {
            return;
        }
        const int dx = event->globalPos().x() - m_press.x();
        // KRIMBLE 2026-10-06: a drag starts after 8 px (was 16)
        if (!m_dragging && qAbs(dx) > 8) {
            beginDrag();
        }
        if (m_dragging && m_onDrag) {
            m_onDrag(dx);
        }
        event->accept();
    }
    void mouseReleaseEvent(QMouseEvent *event) override
    {
        m_timer.stop();
        const bool wasDragging = m_dragging;
        const bool wasPressed = m_pressed;
        m_pressed = false;
        m_dragging = false;
        update();
        if (wasPressed && !wasDragging && (event->globalPos() - m_press).manhattanLength() <= 16 && m_onTap) {
            const std::function<void()> action = m_onTap;
            QTimer::singleShot(0, this, [action]() { action(); });
        }
        if (wasDragging && m_onRelease) {
            const std::function<void(int)> release = m_onRelease;
            const int globalX = event->globalPos().x();
            QTimer::singleShot(0, this, [release, globalX]() { release(globalX); });
        }
        event->accept();
    }
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QColor background = palette().color(QPalette::Highlight);
        background.setAlpha(m_dragging ? 255 : (m_pressed ? 230 : 170));
        p.setPen(Qt::NoPen);
        p.setBrush(background);
        QColor grip = palette().color(QPalette::HighlightedText);
        grip.setAlpha(230);
        if (!m_collapsed) {
            // KRIMBLE 2026-10-06: the grip is the panels' edge: a dark line with a light line beside it (so it can be
            // seen), a faint band while it is held, and a few dots in the middle. Old drawing (a 6 px sliver):
            // const QRectF sliver(width() - 10, (height() - 90) / 2.0, 6, 90);
            // p.drawRoundedRect(sliver, 3, 3);
            // p.setBrush(grip);
            // for (int i = -1; i <= 1; ++i) {
            //     p.drawEllipse(QPointF(sliver.center().x(), height() / 2.0 + i * 9), 1.2, 1.2);
            // }
            p.setRenderHint(QPainter::Antialiasing, false);
            if (m_pressed || m_dragging) {
                QColor band = palette().color(QPalette::Highlight);
                band.setAlpha(m_dragging ? 90 : 60);
                p.fillRect(rect(), band);
            }
            p.fillRect(QRect(width() - 3, 0, 1, height()), QColor(0, 0, 0, 170));          // the darker line
            p.fillRect(QRect(width() - 2, 0, 1, height()), QColor(255, 255, 255, 110));    // the highlight beside it
            p.setRenderHint(QPainter::Antialiasing, true);
            QColor dots = palette().color(QPalette::HighlightedText);
            dots.setAlpha(200);
            p.setPen(Qt::NoPen);
            p.setBrush(dots);
            for (int i = -3; i <= 3; ++i) {
                p.drawEllipse(QPointF(width() - 9, height() / 2.0 + i * 12), 1.8, 1.8);
            }
        } else {
            // Panels hidden: a slightly wider tab on the window's right edge with an arrow.
            const QRectF tab(width() - 12, (height() - 90) / 2.0, 14, 90);
            p.drawRoundedRect(tab, 5, 5);
            p.setBrush(grip);
            QPolygonF arrow;
            arrow << QPointF(width() - 4.5, height() / 2.0 - 5) << QPointF(width() - 4.5, height() / 2.0 + 5)
                  << QPointF(width() - 9.5, height() / 2.0);
            p.drawPolygon(arrow);
        }
    }

private:
    void beginDrag()
    {
        m_dragging = true;
        update();
        if (m_onDragStart) {
            m_onDragStart();
        }
    }

    std::function<void()> m_onTap;
    std::function<void()> m_onDragStart;
    std::function<void(int)> m_onDrag;
    std::function<void(int)> m_onRelease;
    QTimer m_timer;
    QPoint m_press;
    bool m_pressed {false};
    bool m_dragging {false};
    bool m_collapsed {false};
};

static KrimbleSideBarButton *krimbleRightPanelHandle(QMainWindow *win)
{
    // KRIMBLE 2026-10-04: the button hangs on the canvas area (a child of it), so look in all children.
    // return win->findChild<KrimbleSideBarButton*>(QStringLiteral("krimbleRightPanelHandle"),
    //                                               Qt::FindDirectChildrenOnly);
    return win->findChild<KrimbleSideBarButton*>(QStringLiteral("krimbleRightPanelHandle"));
}

// Hides (hide == true) or shows the right-hand panels. When hiding, the panels that were visible
// are remembered so exactly those come back.
static void krimbleSetRightPanelsHidden(QMainWindow *win, bool hide)
{
    if (hide) {
        QStringList names;
        Q_FOREACH (QDockWidget *dock, krimbleRightDocks(win, true)) {
            names << dock->objectName();
            dock->hide();
        }
        if (!names.isEmpty()) {
            win->setProperty("krimbleHiddenRightDocks", names);
        }
    } else {
        QStringList names = win->property("krimbleHiddenRightDocks").toStringList();
        if (names.isEmpty()) {
            // Nothing remembered (for example after a restart): show the usual three.
            names << QStringLiteral("sharedtooldocker") << QStringLiteral("KisLayerBox")
                  << QStringLiteral("ColorSelectorNg");
        }
        Q_FOREACH (const QString &name, names) {
            if (QDockWidget *dock = win->findChild<QDockWidget*>(name)) {
                if (!dock->isFloating() && win->dockWidgetArea(dock) == Qt::RightDockWidgetArea) {
                    dock->show();
                }
            }
        }
        win->setProperty("krimbleHiddenRightDocks", QStringList());
    }
}

// Keeps the side button's place and the menu text in step with what is on screen.
static void krimbleUpdateRightPanelUi(QMainWindow *win)
{
    const QList<QDockWidget*> visibleDocks = krimbleRightDocks(win, true);
    const bool anyVisible = !visibleDocks.isEmpty();
    const bool remembered = !win->property("krimbleHiddenRightDocks").toStringList().isEmpty();

    if (QAction *action = win->findChild<QAction*>(QStringLiteral("settings_toggle_right_panels"))) {
        const QString text = anyVisible ? i18nc("@action:inmenu", "Hide Right Panels")
                                        : i18nc("@action:inmenu", "Show Right Panels");
        if (action->text() != text) {
            action->setText(text);
        }
    }
    // KRIMBLE 2026-10-04 (new way): the button is a child of the canvas area and sits at its right edge,
    // which is exactly where the right-hand panels begin (or the edge of the screen while they are
    // hidden). Its place comes from the canvas area's own size, not from the panels' positions, so it
    // cannot end up anywhere else. Works the same with tabs and with floating picture windows.
    // (The older version worked the place out from the panels' geometry; kept below as a comment.)
    KrimbleSideBarButton *button = krimbleRightPanelHandle(win);
    if (!button) {
        return;
    }
    QWidget *area = button->parentWidget();
    if (!area) {
        return;
    }
    button->setCollapsed(!anyVisible);
    // KRIMBLE 2026-10-06: full height along the panels' edge. Old line:
    // button->move(area->width() - button->width(), (area->height() - button->height()) / 2);
    button->setFixedHeight(area->height());
    button->move(area->width() - button->width(), 0);
    button->show();
    button->raise();
}

#ifdef Q_OS_ANDROID
// Krimble 2026-10-10: dragging the strip between the canvas and the right panels all the way to the screen edge shuts the
// panels (same as Hide Right Panels). No gadget: it watches the finger. A press within the strip next to the right
// panels arms it; letting go within 56 px of the right edge of the window hides them.
class KrimbleEdgeCollapseFilter : public QObject
{
public:
    explicit KrimbleEdgeCollapseFilter(QMainWindow *win) : QObject(win), m_win(win) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        Q_UNUSED(watched);
        if (event->type() == QEvent::MouseButtonPress) {
            const int x = static_cast<QMouseEvent*>(event)->globalPos().x();
            m_armed = false;
            const QList<QDockWidget*> docks = krimbleRightDocks(m_win, true);
            if (!docks.isEmpty()) {
                int left = docks.first()->mapToGlobal(QPoint(0, 0)).x();
                Q_FOREACH (QDockWidget *dock, docks) {
                    left = qMin(left, dock->mapToGlobal(QPoint(0, 0)).x());
                }
                // m_armed = (x >= left - 24 && x <= left + 4);
                m_armed = (x >= left - 24 && x <= left + 20);   // Krimble 2026-10-12: the grab zone is inside the panel now
            }
        } else if (event->type() == QEvent::MouseButtonRelease && m_armed) {
            m_armed = false;
            const int x = static_cast<QMouseEvent*>(event)->globalPos().x();
            const int rightEdge = m_win->mapToGlobal(QPoint(m_win->width(), 0)).x();
            if (x >= rightEdge - 56) {
                QMainWindow *win = m_win;
                QTimer::singleShot(0, win, [win]() {
                    krimbleSetRightPanelsHidden(win, true);
                    krimbleUpdateRightPanelUi(win);
                });
            }
        }
        return false;
    }
private:
    QMainWindow *m_win;
    bool m_armed {false};
};
#endif

#ifdef Q_OS_ANDROID
// ============================================================================================================================
// Krimble 2026-10-12: Startup layouts (George's idea, Claude's design). "Save Layout as Startup" keeps the way the panels, toolbars and
// the toolbox are arranged for the screen's current orientation (portrait or landscape); at start, and each time the screen is rotated,
// the saved layout for that orientation is put back. "Reset Layout" forgets both and returns to the layout the app first started with.
// ============================================================================================================================
static bool krimbleIsLandscape(QMainWindow *win)
{
    return win->width() > win->height();
}

static void krimbleSaveLayout(QMainWindow *win)
{
    KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleLayouts");
    g.writeEntry(krimbleIsLandscape(win) ? "landscape" : "portrait", win->saveState().toBase64());
    g.sync();
}

static void krimbleRestoreSavedLayout(QMainWindow *win)
{
    KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleLayouts");
    const QByteArray saved = g.readEntry(krimbleIsLandscape(win) ? "landscape" : "portrait", QByteArray());
    if (!saved.isEmpty()) {
        win->restoreState(QByteArray::fromBase64(saved));
    }
}

static void krimbleResetLayout(QMainWindow *win)
{
    KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleLayouts");
    g.deleteEntry("portrait");
    g.deleteEntry("landscape");
    g.sync();
    const QByteArray def = g.readEntry("default", QByteArray());
    if (!def.isEmpty()) {
        win->restoreState(QByteArray::fromBase64(def));
    }
}

// ============================================================================================================================
// Krimble 2026-10-12: Layer Style Presets (Claude's idea; George: "Layer Styles - sounds like Alien Skin plugins. I love it. Yes."):
// one tap puts a ready-made layer style on the active layer (with undo). They add to the layer's existing style; "Clear Style"
// switches the whole style off. The full Layer Style window (Layer > Layer Style...) can still fine-tune any of them.
// ============================================================================================================================
static void krimbleApplyLayerStyle(KisMainWindow *win, int preset)
{
    KisViewManager *view = win->viewManager();
    if (!view) return;
    KisImageSP image = view->image();
    if (!image) return;
    KisLayerSP layer = qobject_cast<KisLayer*>(view->activeNode().data());
    if (!layer) return;
    if (!view->blockUntilOperationsFinished(image)) return;

    KoCanvasResourcesInterfaceSP canvasResourcesInterface = view->canvasBase()->resourceManager()->canvasResourcesInterface();
    KisPSDLayerStyleSP oldStyle;
    if (layer->layerStyle()) {
        oldStyle = layer->layerStyle()->clone().dynamicCast<KisPSDLayerStyle>();
    } else {
        oldStyle = toQShared(new KisPSDLayerStyle("", KisGlobalResourcesInterface::instance()))
                       ->cloneWithResourcesSnapshot(KisGlobalResourcesInterface::instance(), canvasResourcesInterface);
    }
    KisPSDLayerStyleSP newStyle = oldStyle->clone().dynamicCast<KisPSDLayerStyle>();
    newStyle->setUuid(QUuid::createUuid());

    const KoColor white = KoColor::fromXML(QStringLiteral("<color channeldepth='U8'><sRGB r='1.0' g='1.0' b='1.0'/></color>"));
    const KoColor black = KoColor::fromXML(QStringLiteral("<color channeldepth='U8'><sRGB r='0.0' g='0.0' b='0.0'/></color>"));
    newStyle->setEnabled(true);
    switch (preset) {
    case 0: // Drop Shadow
        newStyle->dropShadow()->setEffectEnabled(true);
        newStyle->dropShadow()->setDistance(12);
        newStyle->dropShadow()->setSize(14);
        newStyle->dropShadow()->setOpacity(70);
        break;
    case 1: // Inner Shadow
        newStyle->innerShadow()->setEffectEnabled(true);
        break;
    case 2: // Outer Glow (white)
        newStyle->outerGlow()->setEffectEnabled(true);
        newStyle->outerGlow()->setColor(white);
        break;
    case 3: // Inner Glow
        newStyle->innerGlow()->setEffectEnabled(true);
        break;
    case 4: // Stroke, white, 4 px, outside
        newStyle->stroke()->setEffectEnabled(true);
        newStyle->stroke()->setSize(4);
        newStyle->stroke()->setColor(white);
        newStyle->stroke()->setPosition(psd_stroke_outside);
        break;
    case 5: // Stroke, black, 4 px, outside
        newStyle->stroke()->setEffectEnabled(true);
        newStyle->stroke()->setSize(4);
        newStyle->stroke()->setColor(black);
        newStyle->stroke()->setPosition(psd_stroke_outside);
        break;
    case 6: // Bevel and Emboss
        newStyle->bevelAndEmboss()->setEffectEnabled(true);
        break;
    default: // Clear Style
        newStyle->setEnabled(false);
        break;
    }
    KisPSDLayerStyleSP finalStyle = newStyle->cloneWithResourcesSnapshot(KisGlobalResourcesInterface::instance(), canvasResourcesInterface);
    KUndo2Command *cmd = new KisSetLayerStyleCommand(layer, layer->layerStyle(), finalStyle);
    KisProcessingApplicator::runSingleCommandStroke(image, cmd);
    image->waitForDone();
}

// Adds "Rotation Gadget" (a check mark) at the end of the View menu (once, when the menu first opens): turns the canvas rotation
// dial in the status bar off and on (Claude's design; George: "I sometimes accidentally trigger the rotation gadget on the bottom.
// Can we turn it off and on at will?").
static void krimbleInstallViewExtras(KisMainWindow *win)
{
    QMenu *viewMenu = nullptr;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        if (top->menu() && title == QLatin1String("View")) viewMenu = top->menu();
    }
    if (!viewMenu || viewMenu->property("krimbleViewExtras").toBool()) return;
    viewMenu->setProperty("krimbleViewExtras", true);
    QObject::connect(viewMenu, &QMenu::aboutToShow, win, [win, viewMenu]() {
        QAction *gadget = viewMenu->findChild<QAction*>(QStringLiteral("krimbleRotationGadget"));
        if (!gadget) {
            gadget = new QAction(i18n("Rotation Gadget"), viewMenu);
            gadget->setObjectName(QStringLiteral("krimbleRotationGadget"));
            gadget->setCheckable(true);
            viewMenu->addSeparator();
            viewMenu->addAction(gadget);
            QObject::connect(gadget, &QAction::toggled, win, [win](bool on) {
                KisConfig cfg(false);
                cfg.writeEntry<int>("krimble_show_rotation_gadget", on ? 1 : 0);
                // (only the status bar's dial, not the angle selectors in the panels)
                if (KisAngleSelector *dial = win->statusBar() ? win->statusBar()->findChild<KisAngleSelector*>() : nullptr) {
                    if (!on) dial->setVisible(false);
                    else if (win->viewManager() && win->viewManager()->image()) dial->setVisible(true);
                }
            });
        }
        QSignalBlocker blocker(gadget);
        // gadget->setChecked(KisConfig(true).readEntry<int>("krimble_show_rotation_gadget", 1) != 0);   (shown by default)
        gadget->setChecked(KisConfig(true).readEntry<int>("krimble_show_rotation_gadget", 0) != 0);   // George: hidden by default
    });
}

// Adds "Style Presets" right after "Layer Style..." in the Layer menu (once, when the menu first opens).
static void krimbleInstallLayerExtras(KisMainWindow *win)
{
    QMenu *layerMenu = nullptr;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        if (top->menu() && title == QLatin1String("Layer")) layerMenu = top->menu();
    }
    if (!layerMenu || layerMenu->property("krimbleLayerExtras").toBool()) return;
    layerMenu->setProperty("krimbleLayerExtras", true);
    QObject::connect(layerMenu, &QMenu::aboutToShow, win, [win, layerMenu]() {
        if (layerMenu->property("krimbleLayerExtrasAdded").toBool()) return;
        layerMenu->setProperty("krimbleLayerExtrasAdded", true);
        QAction *before = nullptr;
        const QList<QAction*> actions = layerMenu->actions();
        for (int i = 0; i < actions.size(); ++i) {
            QString text = actions.at(i)->text();
            text.remove(QLatin1Char('&'));
            if (text.startsWith(QLatin1String("Layer Style")) && i + 1 < actions.size()) before = actions.at(i + 1);
        }
        QMenu *sub = new QMenu(i18n("Style Presets"), layerMenu);
        static const char *const names[] = { "Drop Shadow", "Inner Shadow", "Outer Glow", "Inner Glow", "Stroke (white, 4 px)",
                                             "Stroke (black, 4 px)", "Bevel and Emboss", "Clear Style" };
        for (int i = 0; i < 8; ++i) {
            QAction *a = sub->addAction(i18n(names[i]));
            QObject::connect(a, &QAction::triggered, win, [win, i]() { krimbleApplyLayerStyle(win, i); });
        }
        layerMenu->insertMenu(before, sub);
    });
}

// Krimble 2026-10-11: after a screen rotation an open menu (the Window menu) stayed on screen and could not be closed.
// Any popup that is open when the main window changes size is closed.
class KrimbleClosePopupsOnResizeFilter : public QObject
{
public:
    explicit KrimbleClosePopupsOnResizeFilter(QMainWindow *win) : QObject(win), m_win(win), m_landscape(krimbleIsLandscape(win)) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_win && event->type() == QEvent::Resize) {
            // Krimble 2026-10-12: after the screen turns, put back the layout saved for the new orientation (if there is one)
            if (krimbleIsLandscape(m_win) != m_landscape) {
                m_landscape = krimbleIsLandscape(m_win);
                QMainWindow *win = m_win;
                QTimer::singleShot(500, win, [win]() { krimbleRestoreSavedLayout(win); });
            }
            while (QWidget *popup = QApplication::activePopupWidget()) {
                popup->close();
            }
            // Krimble 2026-10-11 (2nd): also let go of the menu bar entry (the "Window" button stayed pressed after a rotation).
            if (QMenuBar *bar = m_win->findChild<QMenuBar*>()) {
                bar->setActiveAction(nullptr);
            }
        }
        return false;
    }
private:
    QMainWindow *m_win;
    bool m_landscape;
};
#endif

// ---- Older placement code (2026-10-03/04), kept for reference ----
//     // Watch the right-hand panels too, so the button follows them when they move or resize.
//     if (QObject *watcher = win->findChild<QObject*>(QStringLiteral("krimbleRightPanelsFilter"))) {
//         Q_FOREACH (QDockWidget *dock, krimbleRightDocks(win, false)) {
//             if (!dock->property("krimbleWatched").toBool()) {
//                 dock->installEventFilter(watcher);
//                 dock->setProperty("krimbleWatched", true);
//             }
//         }
//     }
//     KrimbleSideBarButton *button = krimbleRightPanelHandle(win);
//     if (!button) {
//         return;
//     }
//     const bool show = anyVisible || remembered;
//     button->setCollapsed(!anyVisible);
//     if (!show) {
//         button->hide();
//         return;
//     }
//     int x = win->width() - button->width();
//     int y = (win->height() - button->height()) / 2;
//     if (anyVisible) {
//         // Centre it on the bar between the canvas and the left edge of the right-hand panels,
//         // half way down the panels.
//         // KRIMBLE 2026-10-04: only panels that really lie to the right of the middle of the canvas
//         // area count. A panel hidden behind a tab, or not yet laid out, has an old position
//         // (left edge 0) and used to pull the sliver over the toolbox. Without any usable panel
//         // the sliver goes to the right edge of the canvas area.
//         QWidget *central = win->centralWidget();
//         const int canvasMiddle = central ? central->geometry().center().x() : win->width() / 2;
//         int dockLeft = win->width();
//         int top = win->height();
//         int bottom = 0;
//         bool found = false;
//         Q_FOREACH (QDockWidget *dock, visibleDocks) {
//             const QRect g = dock->geometry();
//             if (g.width() < 20 || g.left() <= canvasMiddle) {
//                 continue;
//             }
//             found = true;
//             dockLeft = qMin(dockLeft, g.left());
//             top = qMin(top, g.top());
//             bottom = qMax(bottom, g.bottom());
//         }
//         if (!found) {
//             button->hide();
//             return;
//         }
//         x = dockLeft - button->width() + 4;      // 18 px left of the edge, 4 px over it
//         y = (top + bottom) / 2 - button->height() / 2;
//     }
//     button->move(qBound(0, x, win->width() - button->width()), qBound(0, y, win->height() - button->height()));
//     button->show();
//     button->raise();
// }
//

// Watches the main window and keeps the side button and menu text up to date.
class KrimbleRightPanelsFilter : public QObject
{
public:
    explicit KrimbleRightPanelsFilter(QMainWindow *win) : QObject(win), m_win(win)
    {
        setObjectName(QStringLiteral("krimbleRightPanelsFilter"));
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        // KRIMBLE 2026-10-04: the update must run AFTER the window has laid out its panels. A
        // layout request reaches this filter before that, so the panels' positions were still
        // the old (empty) ones and the side button was drawn at the left edge, on top of the
        // toolbox. Now every change is handled one step later, and moves/resizes of the
        // right-hand panels themselves also trigger it.
        bool relevant = false;
        if (watched == m_win) {
            relevant = (event->type() == QEvent::LayoutRequest || event->type() == QEvent::Resize
                        || event->type() == QEvent::Show);
        } else if (watched->objectName() == QLatin1String("krimbleCanvasArea")) {
            // the canvas area changed size or was shown: its right edge moved
            relevant = (event->type() == QEvent::Resize || event->type() == QEvent::Show);
        } else if (qobject_cast<QDockWidget*>(watched)) {
            relevant = (event->type() == QEvent::Move || event->type() == QEvent::Resize
                        || event->type() == QEvent::Show || event->type() == QEvent::Hide);
        }
        if (relevant && !m_pending) {
            m_pending = true;
            QTimer::singleShot(0, this, [this]() {
                m_pending = false;
                krimbleUpdateRightPanelUi(m_win);
            });
        }
        return false;
    }

private:
    QMainWindow *m_win;
    bool m_pending {false};
};
// END KRIMBLE RIGHT PANELS

#ifdef Q_OS_ANDROID
// KRIMBLE 2026-10-04: the toolbox docked too eagerly (George: "the toolbox wanting a bit too desperately to
// dock"). While the toolbox is being dragged as a floating window, it may dock in a side of the window only when
// the finger is within a small distance of that side; anywhere else in the window it just stays a floating
// window. It works by changing the dock areas the toolbox is allowed in while the finger moves (the window
// system asks for them each time). When the finger is lifted, all areas are allowed again.
// KrimbleDockTolerance::Distance is that distance in pixels (logical, so about 2.5 times as many on the screen).
class KrimbleDockTolerance : public QObject
{
public:
    static const int Distance = 36;

    KrimbleDockTolerance(QMainWindow *win, QDockWidget *dock)
        : QObject(dock), m_win(win), m_dock(dock) {}

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched != m_dock || !m_dock->isFloating()) {
            return false;
        }
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
            if (mouse->buttons() & Qt::LeftButton) {
                const QPoint p = mouse->globalPos();
                const QRect r(m_win->mapToGlobal(QPoint(0, 0)), m_win->size());
                Qt::DockWidgetAreas allowed = Qt::NoDockWidgetArea;
                if (p.x() - r.left() < Distance) allowed |= Qt::LeftDockWidgetArea;
                if (r.right() - p.x() < Distance) allowed |= Qt::RightDockWidgetArea;
                if (p.y() - r.top() < Distance) allowed |= Qt::TopDockWidgetArea;
                if (r.bottom() - p.y() < Distance) allowed |= Qt::BottomDockWidgetArea;
                if (m_dock->allowedAreas() != allowed) {
                    m_dock->setAllowedAreas(allowed);
                }
                m_dragging = true;
            }
        } else if (event->type() == QEvent::MouseButtonRelease && m_dragging) {
            m_dragging = false;
            QPointer<QDockWidget> dock(m_dock);
            QTimer::singleShot(0, m_dock, [dock]() {
                if (dock) {
                    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
                }
            });
        }
        return false;
    }

private:
    QMainWindow *m_win;
    QDockWidget *m_dock;
    bool m_dragging {false};
};
#endif

// KRIMBLE 2026-10-04: the <text> that krita5.xmlgui gives to an <Action> was never used: an action's
// label comes from its own definition (the .action files or the code), so edits of the labels in
// krita5.xmlgui (for example Quit -> Exit) did not show. This applies them: every <Action> in the
// loaded menu file that carries a <text> sets the label of the action with that name.
// The "Hide/Show Right Panels" item and the hidden Attach/Detach items keep their own text.
static void krimbleApplyXmlGuiLabels(KisMainWindow *win)
{
    static QHash<QString, QString> labels;
    if (labels.isEmpty()) {
        const QDomDocument doc = win->domDocument();
        const QDomNodeList actions = doc.elementsByTagName(QStringLiteral("Action"));
        for (int i = 0; i < actions.count(); ++i) {
            const QDomElement element = actions.at(i).toElement();
            const QString name = element.attribute(QStringLiteral("name"));
            const QDomElement text = element.firstChildElement(QStringLiteral("text"));
            if (name.isEmpty() || text.isNull() || text.text().trimmed().isEmpty()) {
                continue;
            }
            if (name == QLatin1String("settings_toggle_right_panels")
                    || name == QLatin1String("settings_attach_panel_menu")
                    || name == QLatin1String("settings_detach_panel_menu")) {
                continue;
            }
            labels.insert(name, text.text().trimmed());
        }
    }
    if (labels.isEmpty()) {
        return;
    }
    // KRIMBLE 2026-10-04 (2nd): the first version only looked at actions that are children of the window. Quit stayed
    // "Quit" on the phone (build b12): actions belong to the action collection, which is not necessarily below the
    // window. Now the actions that are actually IN the menus are used as well (menu->actions()): those are the very
    // objects that are drawn.
    QList<QAction*> candidates;
    QList<QObject*> roots;
    roots << win << KoToolManager::instance();
    Q_FOREACH (QObject *root, roots) {
        candidates << root->findChildren<QAction*>();
    }
    Q_FOREACH (QMenu *menu, win->findChildren<QMenu*>()) {
        candidates << menu->actions();
    }
    if (win->menuBar()) {
        Q_FOREACH (QMenu *menu, win->menuBar()->findChildren<QMenu*>()) {
            candidates << menu->actions();
        }
    }
    Q_FOREACH (QAction *action, candidates) {
        const auto it = labels.constFind(action->objectName());
        if (it != labels.constEnd() && action->text() != it.value()) {
            action->setText(it.value());
        }
    }
}



class ToolDockerFactory : public KoDockFactoryBase
{
public:
    ToolDockerFactory() : KoDockFactoryBase() { }

    QString id() const override {
        return "sharedtooldocker";
    }

    QDockWidget* createDockWidget() override {
        KoToolDocker* dockWidget = new KoToolDocker();
        return dockWidget;
    }

    DockPosition defaultDockPosition() const override {
        return DockRight;
    }
};

class Q_DECL_HIDDEN KisMainWindow::Private
{
public:
    Private(KisMainWindow *parent, QUuid id)
        : q(parent)
        , id(id)
        , styleMenu(new KActionMenu(i18nc("@action:inmenu", "Styles"), parent))
        , dockWidgetMenu(new KActionMenu(i18nc("@action:inmenu", "&Panels"), parent))
        , windowMenu(new KActionMenu(i18nc("@action:inmenu", "&Window"), parent))
        , documentMenu(new KActionMenu(i18nc("@action:inmenu", "New &View"), parent))
        , workspaceMenu(new KActionMenu(i18nc("@action:inmenu", "Wor&kspace"), parent))
        , welcomePage(new KisWelcomePageWidget(parent))
        , widgetStack(new QStackedWidget(parent))
        , mdiArea(new QMdiArea(parent))
        , windowMapper(new KisSignalMapper(parent))
        , documentMapper(new KisSignalMapper(parent))
    {
        if (id.isNull()) this->id = QUuid::createUuid();

        welcomeScroller = new QScrollArea();
        welcomeScroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        welcomeScroller->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        welcomeScroller->setWidget(welcomePage);
        welcomeScroller->setWidgetResizable(true);

        widgetStack->addWidget(welcomeScroller);
        widgetStack->addWidget(mdiArea);
        mdiArea->setTabsMovable(true);
        mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
        mdiArea->setDocumentMode(true);
        mdiArea->setOption(QMdiArea::DontMaximizeSubWindowOnActivation);
#ifdef HAVE_QMDIAREA_ALWAYS_SHOW_SUBWINDOW_TITLE
        mdiArea->setOption(QMdiArea::AlwaysShowSubwindowNameInTitleBar);
#endif /* HAVE_QMDIAREA_ALWAYS_SHOW_SUBWINDOW_TITLE */

        commandBar = new KateCommandBar(parent);
    }

    ~Private() {
        qDeleteAll(toolbarList);
    }

    KisMainWindow *q {nullptr};
    QUuid id;

    KisViewManager *viewManager {nullptr};

    QPointer<KisView> activeView;
    KisSignalAutoConnectionsStore activeViewConnections;

    QList<QAction *> toolbarList;

    bool firstTime {true};
    bool windowSizeDirty {false};
#ifdef Q_OS_ANDROID
    bool flashWindowHackInProgress {false};
#endif

    KisAction *showDocumentInfo {nullptr};
    KisAction *saveAction {nullptr};
    KisAction *saveActionAs {nullptr};
    KisAction *importAnimation {nullptr};
    KisAction *importVideoAnimation {nullptr};
    KisAction *renderAnimation {nullptr};
    KisAction *renderAnimationAgain {nullptr};
    KisAction *closeAll {nullptr};
    KisAction *visitFeatureRequests {nullptr};
    KisAction *visitKrimbleWebsite {nullptr};
    KisAction *visitKrimbleForum {nullptr};
    KisAction *importFile {nullptr};
    KisAction *exportFile {nullptr};
    KisAction *exportFileAdvance {nullptr};
    KisAction *undo {nullptr};
    KisAction *redo {nullptr};
    KisAction *close {nullptr};
    KisAction *mdiCascade {nullptr};
    KisAction *mdiTile {nullptr};
    KisAction *mdiNextWindow {nullptr};
    KisAction *mdiPreviousWindow {nullptr};
    KisAction *toggleDockers {nullptr};
    KisAction *resetConfigurations {nullptr};
    KisAction *toggleDockerTitleBars {nullptr};
#ifdef Q_OS_ANDROID
    KisAction *showDonationManagementDialog {nullptr};
    KisAction *manageSubscriptions {nullptr};
#if KRITA_QT_HAS_ANDROID_QPLATFORMSCREEN_DENSITY_ADJUSTMENT
    KisAction *changeInterfaceScale {nullptr};
#endif
#else
    KisAction *toggleDetachCanvas {nullptr};
    KisAction *newWindow {nullptr};
#endif
    KisAction *fullScreenMode {nullptr};
    KisAction *showSessionManager {nullptr};
    KisAction *commandBarAction {nullptr};
    KisAction *expandingSpacers[2];

    KActionMenu *styleMenu {nullptr};
    QActionGroup* styleActions {nullptr};
    QMap<QString, QAction*> actionMap;

    KActionMenu *dockWidgetMenu {nullptr};
    KActionMenu *windowMenu {nullptr};
    KActionMenu *documentMenu {nullptr};
    KActionMenu *workspaceMenu {nullptr};

    KisKHelpMenu *helpMenu  {nullptr};

    KRecentFilesAction *recentFiles {nullptr};
    KisResourceModel *workspacemodel {nullptr};

    QScopedPointer<KisUndoActionsUpdateManager> undoActionsUpdateManager;

    QString lastExportLocation;

    QMap<QString, QDockWidget *> dockWidgetsMap;
    QByteArray dockerStateBeforeHiding;
    KoToolDocker *toolOptionsDocker {nullptr};

    QCloseEvent *deferredClosingEvent {nullptr};
#ifndef Q_OS_HAIKU
    Digikam::ThemeManager *themeManager {nullptr};
#endif
    QScrollArea *welcomeScroller {nullptr};
    KisWelcomePageWidget *welcomePage {nullptr};


    QStackedWidget *widgetStack {nullptr};

    QMdiArea *mdiArea {nullptr};
    QMdiSubWindow *activeSubWindow  {nullptr};
    KisSignalMapper *windowMapper {nullptr};
    KisSignalMapper *documentMapper {nullptr};
    KisCanvasWindow *canvasWindow {nullptr};

    QByteArray lastExportedFormat;
    QScopedPointer<KisSignalCompressorWithParam<int> > tabSwitchCompressor;
    QMutex savingEntryMutex;

    KConfigGroup windowStateConfig;

    QUuid workspaceBorrowedBy;

    KateCommandBar *commandBar {nullptr};

    KisActionManager * actionManager() {
        return viewManager->actionManager();
    }

    QTabBar* findTabBarHACK() {
        QObjectList objects = mdiArea->children();
        Q_FOREACH (QObject *object, objects) {
            QTabBar *bar = qobject_cast<QTabBar*>(object);
            if (bar) {
                return bar;
            }
        }
        return 0;
    }
};

class ScopedWidgetDisabler
{
    QWidget *widget;
public:
    ScopedWidgetDisabler(QWidget *widget_)
        : widget(widget_)
    {
        widget->setEnabled(false);
    }

    ~ScopedWidgetDisabler()
    {
        widget->setEnabled(true);
    }
};

KisMainWindow::KisMainWindow(QUuid uuid)
    : KXmlGuiWindow()
    , d(new Private(this, uuid))
{
    KAcceleratorManager::setNoAccel(this);

    d->workspacemodel = new KisResourceModel(ResourceType::Workspaces, this);
    connect(d->workspacemodel, SIGNAL(modelReset()), this, SLOT(updateWindowMenu()));
    connect(d->workspacemodel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateWindowMenu()));
    connect(d->workspacemodel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(updateWindowMenu()));

    d->viewManager = new KisViewManager(this, actionCollection());
    KConfigGroup group( KSharedConfig::openConfig(), "theme");
#ifndef Q_OS_HAIKU
    d->themeManager = new Digikam::ThemeManager(group.readEntry("Theme", "Krimble dark"), this);
#endif
    d->windowStateConfig = KSharedConfig::openConfig()->group("MainWindow");

    setStandardToolBarMenuEnabled(true);
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
    setDockNestingEnabled(true);
#ifdef Q_OS_ANDROID
    // On Android, the animations for docks and menus are not only slow, but
    // also can cause crashes in some circumstances. Turning them off feels
    // better and avoids those.
    setAnimated(false);
#endif

    qApp->setStartDragDistance(25);     // 25 px is a distance that works well for Tablet and Mouse events

    connect(this, SIGNAL(restoringDone()), this, SLOT(forceDockTabFonts()));
    connect(this, SIGNAL(themeChanged()), d->viewManager, SLOT(updateIcons()));
    connect(this, SIGNAL(themeChanged()), KoToolManager::instance(), SLOT(themeChanged()));
    connect(KisPart::instance(), SIGNAL(documentClosed(QString)), SLOT(updateWindowMenu()));
    connect(KisPart::instance(), SIGNAL(documentOpened(QString)), SLOT(updateWindowMenu()));
    connect(KisConfigNotifier::instance(), SIGNAL(configChanged()), this, SLOT(configChanged()));

    actionCollection()->addAssociatedWidget(this);
    KoPluginLoader::instance()->load("Krita/ViewPlugin", KoPluginLoader::PluginsConfig(), d->viewManager, false);

    // Load the per-application plugins (Right now, only Python) We do this only once, when the first mainwindow is being created.
    KoPluginLoader::instance()->load("Krita/ApplicationPlugin", KoPluginLoader::PluginsConfig(), qApp, true);

    KoToolBoxFactory toolBoxFactory;
    QDockWidget *toolbox = createDockWidget(&toolBoxFactory);

    KisConfig cfg(true);
    if (cfg.toolOptionsInDocker()) {
        ToolDockerFactory toolDockerFactory;
        d->toolOptionsDocker = qobject_cast<KoToolDocker*>(createDockWidget(&toolDockerFactory));
        if (d->toolOptionsDocker) {
            d->toolOptionsDocker->toggleViewAction()->setEnabled(true);
        }
    }

    QMap<QString, QAction*> dockwidgetActions;

    if (toolbox) {
        dockwidgetActions[toolbox->toggleViewAction()->text()] = toolbox->toggleViewAction();
    }
    Q_FOREACH (const QString & docker, KoDockRegistry::instance()->keys()) {
        KoDockFactoryBase *factory = KoDockRegistry::instance()->value(docker);
        QDockWidget *dw = createDockWidget(factory);
        if (dw) {
            dockwidgetActions[dw->toggleViewAction()->text()] = dw->toggleViewAction();
        }
    }

    if (d->toolOptionsDocker) {
        dockwidgetActions[d->toolOptionsDocker->toggleViewAction()->text()] = d->toolOptionsDocker->toggleViewAction();
    }
    connect(KoToolManager::instance(), SIGNAL(toolOptionWidgetsChanged(KoCanvasController*,QList<QPointer<QWidget> >)), this, SLOT(newOptionWidgets(KoCanvasController*,QList<QPointer<QWidget> >)));

    Q_FOREACH (QString title, dockwidgetActions.keys()) {
        d->dockWidgetMenu->addAction(dockwidgetActions[title]);
    }

    // KRIMBLE 2026-10-02: "Detach Panel" submenu. Lists the panels that are
    // currently shown and docked; choosing one makes it a free-floating window.
    // A locked panel (lock icon in its title bar) is unlocked first, because a
    // locked panel cannot float. The list is rebuilt every time the submenu opens.
    // Moved out of the (very long) Settings > Panels list: it is registered as an
    // action ("settings_detach_panel_menu") and placed in krita5.xmlgui, directly
    // below the Panels entry of the Settings menu.
    KActionMenu *detachPanelAction = new KActionMenu(i18nc("@action:inmenu", "Detach Panel"), this);
    QMenu *detachPanelMenu = detachPanelAction->menu();
    connect(detachPanelMenu, &QMenu::aboutToShow, this, [this, detachPanelMenu]() {
        detachPanelMenu->clear();
        Q_FOREACH (QDockWidget *dock, dockWidgets()) {
            if (!dock || !dock->isVisible() || dock->isFloating()) {
                continue;
            }
            QAction *detachAction = detachPanelMenu->addAction(dock->windowTitle());
            connect(detachAction, &QAction::triggered, this, [dock]() {
                if (dock->property("Locked").toBool()) {
                    if (KoDockWidgetTitleBar *bar = dynamic_cast<KoDockWidgetTitleBar*>(dock->titleBarWidget())) {
                        bar->setLocked(false);
                    } else if (KisUtilityTitleBar *utilityBar = dynamic_cast<KisUtilityTitleBar*>(dock->titleBarWidget())) {
                        utilityBar->setLocked(false);
                    }
                }
                dock->setFloating(true);
            });
        }
        if (detachPanelMenu->isEmpty()) {
            QAction *noneAction = detachPanelMenu->addAction(i18nc("@action:inmenu", "No docked panels"));
            noneAction->setEnabled(false);
        }
    });
    // (Earlier version, kept for reference: added the submenu at the bottom of the
    // Panels list.)
    // d->dockWidgetMenu->addSeparator();
    // d->dockWidgetMenu->menu()->addMenu(detachPanelMenu);
    actionCollection()->addAction("settings_detach_panel_menu", detachPanelAction);
    // KRIMBLE 2026-10-04: was hidden when the single on/off item "Attach Panels" replaced it; George wants the
    // Detach Panel and Attach Panel menus back (2026-10-04, build b12), so it is visible again.
    // detachPanelAction->setVisible(false);

    // KRIMBLE 2026-10-04: "Attach Panels" on/off (replaces the "Attach Panel" submenu, which is
    // kept below as a comment). ON = the standard behaviour: a panel that floats snaps to the
    // edge of the window when dragged there. OFF = floating panels stay wherever they are put.
    // The setting is remembered. The menu item keeps the old action name so krita5.xmlgui is
    // unchanged.
    // KRIMBLE 2026-10-04 (3rd): switched off again (#if 0): George wants the "Detach Panel" and "Attach Panel" menus
    // back. The Attach Panel submenu is made right after this block.
#if 0
    {
        KConfigGroup krimbleGroup = KSharedConfig::openConfig()->group("Krimble");
        const bool attachOn = krimbleGroup.readEntry("AttachPanels", true);
        setProperty("krimbleAttachPanels", attachOn);
        QAction *attachPanelsAction = new QAction(i18nc("@action:inmenu", "Attach Panels"), this);
        attachPanelsAction->setCheckable(true);
        attachPanelsAction->setChecked(attachOn);
        actionCollection()->addAction("settings_attach_panel_menu", attachPanelsAction);
        // KRIMBLE 2026-10-04 (2nd): hidden and switched off; see the note in the topLevelChanged handler.
        attachPanelsAction->setVisible(false);
        attachPanelsAction->setEnabled(false);
        connect(attachPanelsAction, &QAction::toggled, this, [this](bool on) {
            setProperty("krimbleAttachPanels", on);
            KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
            group.writeEntry("AttachPanels", on);
            group.sync();
            Q_FOREACH (QDockWidget *dock, dockWidgets()) {
                if (dock && dock->isFloating()) {
                    dock->setAllowedAreas(on ? Qt::AllDockWidgetAreas : Qt::NoDockWidgetArea);
                }
            }
        });
    }
#endif

    // KRIMBLE 2026-10-04 (3rd): "Attach Panel" submenu, the counterpart of "Detach Panel", back again. It lists the
    // panels that are currently floating; choosing one docks it with the STANDARD behaviour (back into its previous
    // dock area). The earlier "dock where it was put" version (commented below) is not used: it sent panels to the
    // wrong side and one vanished. Placed like Detach Panel (krita5.xmlgui, action "settings_attach_panel_menu").
    {
        KActionMenu *attachPanelAction = new KActionMenu(i18nc("@action:inmenu", "Attach Panel"), this);
        QMenu *attachPanelMenu = attachPanelAction->menu();
        connect(attachPanelMenu, &QMenu::aboutToShow, this, [this, attachPanelMenu]() {
            attachPanelMenu->clear();
            Q_FOREACH (QDockWidget *dock, dockWidgets()) {
                if (!dock || !dock->isVisible() || !dock->isFloating()) {
                    continue;
                }
                QAction *attachAction = attachPanelMenu->addAction(dock->windowTitle());
                connect(attachAction, &QAction::triggered, this, [dock]() {
                    dock->setFloating(false);
                    dock->show();
                });
            }
            if (attachPanelMenu->isEmpty()) {
                QAction *noneAction = attachPanelMenu->addAction(i18nc("@action:inmenu", "No floating panels"));
                noneAction->setEnabled(false);
            }
        });
        actionCollection()->addAction("settings_attach_panel_menu", attachPanelAction);
    }

    // ---- Old "Attach Panel" submenu (2026-10-02), kept for reference ----
    //     // KRIMBLE 2026-10-02: "Attach Panel" submenu, the counterpart of "Detach
    //     // Panel". Lists the panels that are currently floating; choosing one docks
    //     // it back into its previous dock area. Placed like Detach Panel.
    //     KActionMenu *attachPanelAction = new KActionMenu(i18nc("@action:inmenu", "Attach Panel"), this);
    //     QMenu *attachPanelMenu = attachPanelAction->menu();
    //     connect(attachPanelMenu, &QMenu::aboutToShow, this, [this, attachPanelMenu]() {
    //         attachPanelMenu->clear();
    //         Q_FOREACH (QDockWidget *dock, dockWidgets()) {
    //             if (!dock || !dock->isVisible() || !dock->isFloating()) {
    //                 continue;
    //             }
    //             QAction *attachAction = attachPanelMenu->addAction(dock->windowTitle());
    //             connect(attachAction, &QAction::triggered, this, [this, dock]() {
    //                 // KRIMBLE 2026-10-02: dock the panel where it was put (nearest side,
    //                 // between the panels above and below) instead of sending it back to
    //                 // its old place. Old behaviour: dock->setFloating(false);
    //                 krimbleAttachDockWhereItIs(this, dock);
    //             });
    //         }
    //         if (attachPanelMenu->isEmpty()) {
    //             QAction *noneAction = attachPanelMenu->addAction(i18nc("@action:inmenu", "No floating panels"));
    //             noneAction->setEnabled(false);
    //         }
    //     });
    //     // d->dockWidgetMenu->menu()->addMenu(attachPanelMenu);
    //     actionCollection()->addAction("settings_attach_panel_menu", attachPanelAction);

    // KRIMBLE 2026-10-03: "Hide Right Panels" / "Show Right Panels" (Settings menu), see
    // krimbleSetRightPanelsHidden(). The toolbox on the left is not touched.
    QAction *toggleRightPanelsAction = new QAction(i18nc("@action:inmenu", "Hide Right Panels"), this);
    actionCollection()->addAction("settings_toggle_right_panels", toggleRightPanelsAction);
    connect(toggleRightPanelsAction, &QAction::triggered, this, [this]() {
        krimbleSetRightPanelsHidden(this, !krimbleRightDocks(this, true).isEmpty());
        krimbleUpdateRightPanelUi(this);
    });
    // The big side button: tap = hide / show the right panels, long press or sideways swipe = resize.
    // It is only made on Android; the menu entry works everywhere.
    // KRIMBLE 2026-10-04: SWITCHED OFF at George's decision ("the sidebar gadget is just causing problems.
    // It's a failure."). The button is no longer created, so nothing is drawn at the panels' edge; the
    // menu item Settings > Hide/Show Right Panels stays. To bring the button back, change "#if 0" to
    // "#ifdef Q_OS_ANDROID".
    // KRIMBLE 2026-10-04 (2nd): was switched on again, hanging on the canvas area (d->mdiArea) instead of the
    // window; SWITCHED OFF AGAIN at George's decision ("Comment out the sliver. I'll worry about it later.").
    // To bring it back change "#if 0" to "#ifdef Q_OS_ANDROID".
    // KRIMBLE 2026-10-05: ON AGAIN (George: "At one point that side gadget was working"): the version that hangs on
    // the canvas area and is placed from that area's own size. Tap = hide / show the right panels; long press or a
    // sideways swipe = resize them. To park it again change the line below back to "#if 0".
    // KRIMBLE 2026-10-07: OFF AGAIN (George: "Still have unwanted extra gadget on edge of side panel", and it covered
    // the X that closes a document tab). To bring it back change "#if 0" to "#ifdef Q_OS_ANDROID".
#if 0
    {
        struct DragState { int startWidth {0}; };
        auto dragState = std::make_shared<DragState>();
        d->mdiArea->setObjectName(QStringLiteral("krimbleCanvasArea"));
        new KrimbleSideBarButton(d->mdiArea,
            [this]() {                                             // tap
                // KRIMBLE 2026-10-06: no collapse button any more (George: collapse by closeness to the side).
                // A tap still opens the panels when they are hidden. Old body:
                // krimbleSetRightPanelsHidden(this, !krimbleRightDocks(this, true).isEmpty());
                if (krimbleRightDocks(this, true).isEmpty()) {
                    krimbleSetRightPanelsHidden(this, false);
                }
                krimbleUpdateRightPanelUi(this);
            },
            [this, dragState]() {                                  // drag begins
                if (krimbleRightDocks(this, true).isEmpty()) {
                    krimbleSetRightPanelsHidden(this, false);      // dragging from the edge: show first
                    QApplication::processEvents();
                }
                dragState->startWidth = krimbleRightPanelsWidth(this);
            },
            [this, dragState](int dx) {                            // drag moves (left = wider)
                krimbleSetRightPanelsWidth(this, dragState->startWidth - dx);
                krimbleUpdateRightPanelUi(this);
            },
            [this](int globalX) {                                  // drag ends
                // KRIMBLE 2026-10-06: collapse by closeness to the side: let go within 56 px of the right edge of
                // the window, or with the panels dragged below 120 px wide.
                const int rightEdge = mapToGlobal(QPoint(width(), 0)).x();
                if (globalX >= rightEdge - 56 || krimbleRightPanelsWidth(this) < 120) {
                    krimbleSetRightPanelsHidden(this, true);
                    krimbleUpdateRightPanelUi(this);
                }
            });
        // krimbleRightPanelHandle(this)->hide();      // KRIMBLE 2026-10-04: it is placed and shown by krimbleUpdateRightPanelUi
    }
#endif
    {
        KrimbleRightPanelsFilter *rightPanelsFilter = new KrimbleRightPanelsFilter(this);
        installEventFilter(rightPanelsFilter);
        d->mdiArea->installEventFilter(rightPanelsFilter);     // the canvas area: its edge is where the sliver sits
    }
#ifdef Q_OS_ANDROID
    // Krimble 2026-10-10: drag the strip to the screen edge to shut the right panels.
    qApp->installEventFilter(new KrimbleEdgeCollapseFilter(this));
    // Krimble 2026-10-12: stack the top toolbars once the saved layout has been restored.
    QTimer::singleShot(2500, this, [this]() { krimbleStackToolBars(this); krimbleInstallHelpExtras(this); krimbleInstallFileExtras(this);
        // Krimble 2026-10-12: the invisible grab zones along the panel edges, kept up to date for panels that are added later
        krimbleInstallDockEdgeGrabs(this);
        QTimer *grabTimer = new QTimer(this);
        connect(grabTimer, &QTimer::timeout, this, [this]() {
            krimbleInstallDockEdgeGrabs(this);
            Q_FOREACH (QAction *top, menuBar()->actions()) krimbleHideMenuShortcuts(top->menu(), 0);   // Krimble 2026-10-12
        });
        grabTimer->start(2000);
        Q_FOREACH (QAction *top, menuBar()->actions()) krimbleHideMenuShortcuts(top->menu(), 0);
        // (c) Krimble 2026-10-12: George: "Maybe start with toolbox floating by default." The first time the app runs (and only
        // then) the toolbox is floating, at the top left, two columns wide; after that the saved layout decides. A floating
        // toolbox does not have to fit next to the canvas and the panels, which has no room in portrait.
        {
            KisConfig floatCfg(false);
            if (!floatCfg.readEntry<int>("krimble_toolbox_floated_once", 0)) {
                floatCfg.writeEntry<int>("krimble_toolbox_floated_once", 1);
                if (QDockWidget *toolBoxDock = findChild<QDockWidget*>(QStringLiteral("ToolBox"))) {
                    if (!toolBoxDock->isFloating()) {
                        toolBoxDock->setFloating(true);
                        if (QScreen *scr = QGuiApplication::primaryScreen()) {
                            const QRect avail = scr->availableGeometry();
                            toolBoxDock->resize(int(avail.width() * 0.30), int(avail.height() * 0.70));
                            toolBoxDock->move(avail.left() + 6, avail.top() + menuBar()->height() + 60);
                        }
                    }
                }
            }
        }
        // Krimble 2026-10-12: George: "Still loads brushes by default." The first time the app runs (once, remembered in the setting
        // krimble_default_panels_done) the Brushes panel is hidden; it can be shown again in Window > Panels. This is done before the
        // "default" layout below is kept, so Reset Layout also comes back without it.
        {
            KisConfig panelsCfg(false);
            if (!panelsCfg.readEntry<int>("krimble_default_panels_done", 0)) {
                panelsCfg.writeEntry<int>("krimble_default_panels_done", 1);
                Q_FOREACH (QDockWidget *dock, findChildren<QDockWidget*>()) {
                    const QString title = dock->windowTitle().remove(QLatin1Char('&')).trimmed();
                    if (title.compare(QStringLiteral("Brushes"), Qt::CaseInsensitive) == 0
                        || title.compare(QStringLiteral("Brush Presets"), Qt::CaseInsensitive) == 0) {
                        dock->hide();
                    }
                }
            }
        }
        // Krimble 2026-10-12: keep the layout the app first started with (for "Reset Layout"), then put back the layout the
        // user saved for this orientation, if any; and add "Style Presets" to the Layer menu.
        {
            KConfigGroup layoutGroup = KSharedConfig::openConfig()->group("KrimbleLayouts");
            if (!layoutGroup.hasKey("default")) {
                layoutGroup.writeEntry("default", saveState().toBase64());
                layoutGroup.sync();
            }
            krimbleRestoreSavedLayout(this);
            krimbleInstallLayerExtras(this);
            krimbleInstallViewExtras(this);
        }
    });
    // Krimble 2026-10-11: close open menus when the window changes size (rotation).
    installEventFilter(new KrimbleClosePopupsOnResizeFilter(this));
#endif

    // KRIMBLE 2026-10-04: apply the labels typed in krita5.xmlgui (see krimbleApplyXmlGuiLabels). Run
    // once everything is built, and again when a tool is chosen (tool actions appear later).
    QTimer::singleShot(0, this, [this]() {
        krimbleApplyXmlGuiLabels(this);
        // and once more just before any menu is shown (KRIMBLE 2026-10-04, 2nd: Quit did not change on the phone)
        Q_FOREACH (QMenu *menu, this->findChildren<QMenu*>()) {
            connect(menu, &QMenu::aboutToShow, this, [this]() { krimbleApplyXmlGuiLabels(this); });
        }
    });
    connect(KoToolManager::instance(), &KoToolManager::changedTool, this,
            [this](KoCanvasController*) { krimbleApplyXmlGuiLabels(this); });


    // Style menu actions
    d->styleActions = new QActionGroup(this);
    QAction * action;

#ifndef Q_OS_ANDROID

    Q_FOREACH (QString styleName, QStyleFactory::keys()) {

        action = new QAction(styleName, d->styleActions);
        action->setCheckable(true);
        d->actionMap.insert(styleName, action);
        d->styleMenu->addAction(d->actionMap.value(styleName));
    }

    // select the config value, or the current style if that does not exist
    QString styleFromConfig = cfg.widgetStyle().toLower();
    QString styleToSelect = styleFromConfig.isEmpty() ? style()->objectName().toLower() : styleFromConfig;

    Q_FOREACH (auto key, d->actionMap.keys()) {
        if(key.toLower() == styleToSelect) { // does the key match selection
            d->actionMap.value(key)->setChecked(true);
        }
    }

    connect(d->styleActions, SIGNAL(triggered(QAction*)),
            this, SLOT(slotUpdateWidgetStyle()));
#endif

    // Load all the actions from the tool plugins
    // ToolBoxDocker needs them when at setViewManager()
    Q_FOREACH(KoToolFactoryBase *toolFactory, KoToolRegistry::instance()->values()) {
        toolFactory->createActions(actionCollection());
    }


    Q_FOREACH (QDockWidget *wdg, dockWidgets()) {
        if ((wdg->features() & QDockWidget::DockWidgetClosable) == 0) {
            wdg->setVisible(true);
        }
    }

    Q_FOREACH (KoCanvasObserverBase* observer, canvasObservers()) {
        observer->setObservedCanvas(0);
        KisMainwindowObserver* mainwindowObserver = dynamic_cast<KisMainwindowObserver*>(observer);
        if (mainwindowObserver) {
            mainwindowObserver->setViewManager(d->viewManager);
        }
    }

    d->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    d->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    d->mdiArea->setTabPosition(QTabWidget::North);
    d->mdiArea->setTabsClosable(true);
    d->mdiArea->setAcceptDrops(true);

    themeChanged(); // updates icon styles

    setCentralWidget(d->widgetStack);
    d->widgetStack->setCurrentIndex(0);

    connect(d->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(subWindowActivated()));
    connect(d->windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));
    connect(d->documentMapper, SIGNAL(mapped(QObject*)), this, SLOT(newView(QObject*)));

    d->canvasWindow = new KisCanvasWindow(this);
    actionCollection()->addAssociatedWidget(d->canvasWindow);

    createActions();

    // the welcome screen needs to grab actions...so make sure this line goes after the createAction() so they exist
    d->welcomePage->setMainWindow(this);

    d->recentFiles->setRecentFilesModel(&KisRecentDocumentsModelWrapper::instance()->model());

    subWindowActivated();
    updateWindowMenu();

    if (isHelpMenuEnabled() && !d->helpMenu) {
        d->helpMenu = new KisKHelpMenu(this, KAboutData::applicationData(), false);

        // workaround-less version:
        // d->helpMenu = new KisKHelpMenu(this, QString()/*unused*/, false);

        // The difference between using KisKActionCollection->addAction() is that
        // these actions do not get tied to the MainWindow.  What does this all do?
        KisKActionCollection *actions = d->viewManager->actionCollection();
        QAction *helpContentsAction = d->helpMenu->action(KisKHelpMenu::menuHelpContents);
        QAction *whatsThisAction = d->helpMenu->action(KisKHelpMenu::menuWhatsThis);
        QAction *reportBugAction = d->helpMenu->action(KisKHelpMenu::menuReportBug);
        QAction *switchLanguageAction = d->helpMenu->action(KisKHelpMenu::menuSwitchLanguage);
        QAction *aboutAppAction = d->helpMenu->action(KisKHelpMenu::menuAboutApp);
        QAction *aboutKdeAction = d->helpMenu->action(KisKHelpMenu::menuAboutKDE);

        if (helpContentsAction) {
            actions->addAction(helpContentsAction->objectName(), helpContentsAction);
        }
        if (whatsThisAction) {
            actions->addAction(whatsThisAction->objectName(), whatsThisAction);
        }
        if (reportBugAction) {
            actions->addAction(reportBugAction->objectName(), reportBugAction);
        }
        if (switchLanguageAction) {
            actions->addAction(switchLanguageAction->objectName(), switchLanguageAction);
        }
        if (aboutAppAction) {
            actions->addAction(aboutAppAction->objectName(), aboutAppAction);
        }
        if (aboutKdeAction) {
            actions->addAction(aboutKdeAction->objectName(), aboutKdeAction);
        }

        connect(d->helpMenu, SIGNAL(showAboutApplication()), SLOT(showAboutApplication()));
    }

    // KDE' libs 4''s help contents action is broken outside kde, for some reason... We can handle it just as easily ourselves
    QAction *helpAction = actionCollection()->action("help_contents");
    helpAction->disconnect();
    connect(helpAction, SIGNAL(triggered()), this, SLOT(showManual()));

#if 0
    //check for colliding shortcuts
    QSet<QKeySequence> existingShortcuts;
    Q_FOREACH (QAction* action, actionCollection()->actions()) {
        if(action->shortcut() == QKeySequence(0)) {
            continue;
        }
        dbgKrita << "shortcut " << action->text() << " " << action->shortcut();
        Q_ASSERT(!existingShortcuts.contains(action->shortcut()));
        existingShortcuts.insert(action->shortcut());
    }
#endif

    configChanged();

    // Make sure the python plugins create their actions in time
    KisPart::instance()->notifyMainWindowIsBeingCreated(this);

    // If we have customized the toolbars, load that first
    setLocalXMLFile(KoResourcePaths::locateLocal("data", "krita5.xmlgui"));
    setXMLFile(":/kxmlgui5/krita5.xmlgui");

    guiFactory()->addClient(this);
    applyActionIconOverridesFromLocalXML();
    connect(guiFactory(), SIGNAL(makingChanges(bool)), SLOT(slotXmlGuiMakingChanges(bool)));

    // Create and plug toolbar list for Settings menu
    QList<QAction *> toolbarList;
    Q_FOREACH (QWidget* it, guiFactory()->containers("ToolBar")) {
        KisToolBar * toolBar = ::qobject_cast<KisToolBar *>(it);
        if (toolBar) {
            KToggleAction* act = new KToggleAction(i18n("Show %1 Toolbar", toolBar->windowTitle()), this);
            actionCollection()->addAction(toolBar->objectName().toUtf8(), act);
            act->setCheckedState(KGuiItem(i18n("Hide %1 Toolbar", toolBar->windowTitle())));
            connect(act, SIGNAL(toggled(bool)), this, SLOT(slotToolbarToggled(bool)));
            act->setChecked(!toolBar->isHidden());
            toolbarList.append(act);
        } else {
            warnUI << "Toolbar list contains a " << it->metaObject()->className() << " which is not a toolbar!";
        }
    }

    plugActionList("toolbarlist", toolbarList);
    d->toolbarList = toolbarList;

    applyToolBarLayout();

    d->viewManager->updateGUI();
    d->viewManager->updateIcons();

    QTimer::singleShot(1000, Qt::CoarseTimer, this, SLOT(checkSanity()));

    {
        using namespace std::placeholders; // For _1 placeholder
        std::function<void (int)> callback(
                    std::bind(&KisMainWindow::switchTab, this, _1));

        d->tabSwitchCompressor.reset(
                    new KisSignalCompressorWithParam<int>(500, callback, KisSignalCompressor::FIRST_INACTIVE));
    }


    if (cfg.readEntry("CanvasOnlyActive", false)) {
        QString currentWorkspace = cfg.readEntry<QString>("CurrentWorkspace", "Default");
        KoResourceServer<KisWorkspaceResource> * rserver = KisResourceServerProvider::instance()->workspaceServer();
        KisWorkspaceResourceSP workspace = rserver->resource("", "", currentWorkspace);
        if (workspace) {
            restoreWorkspace(workspace);
        }
        cfg.writeEntry("CanvasOnlyActive", false);
        menuBar()->setVisible(true);
    }

    this->winId(); // Ensures the native window has been created.

#ifdef Q_OS_ANDROID
    // HACK: This prevents the mainWindow from going beyond the screen with no
    // way to bring it back. Apparently the size doesn't matter here as long as
    // it remains fixed?
    setFixedSize(KisApplication::primaryScreen()->availableGeometry().size());

    QScreen *s = QGuiApplication::primaryScreen();
    s->setOrientationUpdateMask(Qt::LandscapeOrientation|Qt::InvertedLandscapeOrientation|Qt::PortraitOrientation|Qt::InvertedPortraitOrientation);
    connect(s, SIGNAL(orientationChanged(Qt::ScreenOrientation)), this, SLOT(orientationChanged()));

#if KRITA_QT_HAS_ANDROID_QPLATFORMSCREEN_DENSITY_ADJUSTMENT
    // When the screen's UI scale changes, we have to hide and show it to get
    // its contents to update, otherwise it fails to re-render altogether.
    KisAndroidScaling *androidScaling = KisAndroidScaling::instance();
    if (androidScaling) {
        connect(androidScaling,
                &KisAndroidScaling::sigInterfaceScaleChanged,
                this,
                &KisMainWindow::slotFlashWindowHack,
                Qt::QueuedConnection);
    }
#endif

    // When Krimble starts, Java side sends an event to set applicationState() to active. But, before
    // the event could reach KisApplication's platform integration, it is cleared by KisOpenGLModeProber::probeFormat.
    // So, we send it manually when MainWindow shows up.
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/qt5/android/QtNative", "setApplicationState", "(I)V", Qt::ApplicationActive);
#endif

    setAcceptDrops(true);
    QTabBar *tabBar = d->findTabBarHACK();
    if (tabBar) {
        tabBar->setElideMode(Qt::ElideRight);
        // load customized tab style
        customizeTabBar();
        tabBar->setExpanding(true);
        tabBar->setAcceptDrops(true);
        tabBar->setChangeCurrentOnDrag(true);
    }

    applyMainWindowSettings(d->windowStateConfig);
}

KisMainWindow::~KisMainWindow()
{
    //    Q_FOREACH (QAction *ac, actionCollection()->actions()) {
    //        QAction *action = qobject_cast<QAction*>(ac);
    //        if (action) {
    //        qDebug() << "<Action"
    //                 << "\n\tname=" << action->objectName()
    //                 << "\n\ticon=" << action->icon().name()
    //                 << "\n\ttext="  << action->text().replace("&", "&amp;")
    //                 << "\n\twhatsThis="  << action->whatsThis()
    //                 << "\n\ttoolTip="  << action->toolTip().replace("<html>", "").replace("</html>", "")
    //                 << "\n\ticonText="  << action->iconText().replace("&", "&amp;")
    //                 << "\n\tshortcut="  << action->shortcut().toString()
    //                 << "\n\tisCheckable="  << QString((action->isChecked() ? "true" : "false"))
    //                 << "\n\tstatusTip=" << action->statusTip()
    //                 << "\n/>\n"   ;
    //        }
    //        else {
    //            dbgKrita << "Got a non-qaction:" << ac->objectName();
    //        }
    //    }

    /**
     * Destructor of KisView accesses KisViewManager is multiple ways,
     * directly and via KoToolManager. Hence we should destroy all the
     * subwindows **before** we actually destroy the view manager.
     *
     * Please note that we cannot just `close()` the subwindows, becasue
     * they are deleted with deleteLater(), which may postpone their
     * destruction till after KisViewManager's death.
     */
    Q_FOREACH (QMdiSubWindow *subwin, d->mdiArea->subWindowList()) {
        if (subwin) {
            (void) subwin->close();
            delete subwin;
        }
    }

    // The doc and view might still exist (this is the case when closing the window)
    KisPart::instance()->removeMainWindow(this);

    delete d->viewManager;
    delete d;

}

QMenu *KisMainWindow::createPopupMenu()
{
    return 0;
}

QUuid KisMainWindow::id() const {
    return d->id;
}

void KisMainWindow::addView(KisView *view, QMdiSubWindow *subWindow)
{
    if (d->activeView == view && !subWindow) return;

    if (d->activeView) {
        d->activeView->disconnect(this);
    }

    // register the newly created view in the input manager
    viewManager()->inputManager()->addTrackedCanvas(view->canvasBase());

    showView(view, subWindow);
    Q_EMIT restoringDone();

//    QTabBar *tabBar = d->findTabBarHACK();
//    Q_FOREACH(QObject *c, tabBar->children()) {
//        if (QWidget *w = qobject_cast<QWidget*>(c)) {
//            w->setAcceptDrops(true);
//        }
//    }
}

void KisMainWindow::notifyChildViewDestroyed(KisView *view)
{
    /**
     * If we are the last view of the window, Qt will not activate another tab
     * before destroying tab/window. In this case we should clear all the dangling
     * pointers manually by setting the current view to null
     */
    viewManager()->inputManager()->removeTrackedCanvas(view->canvasBase());
    if (view->canvasBase() == viewManager()->canvasBase()) {
        viewManager()->setCurrentView(0);
    }
}


void KisMainWindow::showView(KisView *imageView, QMdiSubWindow *subwin)
{
    if (imageView && activeView() != imageView) {
        // XXX: find a better way to initialize this!
        imageView->setViewManager(d->viewManager);

        imageView->canvasBase()->setFavoriteResourceManager(d->viewManager->paintOpBox()->favoriteResourcesManager());
        imageView->slotLoadingFinished();

        if (!subwin) {
            QMdiSubWindow *currentSubWin = d->mdiArea->currentSubWindow();
            bool shouldMaximize = currentSubWin ? currentSubWin->isMaximized() : true;
            subwin = d->mdiArea->addSubWindow(imageView);
            if (shouldMaximize) {
                subwin->setWindowState(Qt::WindowMaximized);
            }
        } else {
            subwin->setWidget(imageView);
        }
        imageView->setSubWindow(subwin);
        subwin->setAttribute(Qt::WA_DeleteOnClose, true);
        connect(subwin, SIGNAL(destroyed()), SLOT(updateWindowMenu()));

        KisConfig cfg(true);
        // subwin->setOption(QMdiSubWindow::RubberBandMove, cfg.readEntry<int>("mdi_rubberband", cfg.useOpenGL()));
        subwin->setOption(QMdiSubWindow::RubberBandMove, cfg.readEntry<int>("mdi_rubberband", krimbleRubberBandDefault(cfg)));
        // subwin->setOption(QMdiSubWindow::RubberBandResize, cfg.readEntry<int>("mdi_rubberband", cfg.useOpenGL()));
        subwin->setOption(QMdiSubWindow::RubberBandResize, cfg.readEntry<int>("mdi_rubberband", krimbleRubberBandDefault(cfg)));
        subwin->setWindowIcon(qApp->windowIcon());

#ifdef Q_OS_MACOS
        connect(subwin, SIGNAL(destroyed()), SLOT(updateSubwindowFlags()));
        updateSubwindowFlags();
#endif

        if (d->mdiArea->subWindowList().size() == 1) {
            imageView->showMaximized();
        }
        else {
            imageView->show();
        }

        /**
         * Hack alert!
         *
         * Here we explicitly request KoToolManager to Q_EMIT all the tool
         * activation signals, to reinitialize the tool options docker.
         *
         * That is needed due to a design flaw we have in the
         * initialization procedure.  The tool in the KoToolManager is
         * initialized in KisView::setViewManager() calls, which
         * happens early enough. During this call the tool manager
         * requests KoCanvasControllerWidget to Q_EMIT the signal to
         * update the widgets in the tool docker. *But* at that moment
         * of time the view is not yet connected to the main window,
         * because it happens in KisViewManager::setCurrentView a bit
         * later. This fact makes the widgets updating signals be lost
         * and never reach the tool docker.
         *
         * So here we just explicitly call the tool activation stub.
         */

        KoToolManager::instance()->initializeCurrentToolForCanvas();

        // No, no, no: do not try to call this _before_ the show() has
        // been called on the view; only when that has happened is the
        // opengl context active, and very bad things happen if we tell
        // the dockers to update themselves with a view if the opengl
        // context is not active.
        setActiveView(imageView);

        updateWindowMenu();
    } else {
        unsetActiveView();
    }
}

void KisMainWindow::slotPreferences()
{
    std::optional<KisDlgPreferences::PageDesc>page = std::nullopt;

    KisAction* action = d->actionManager()->actionByName("options_configure");
    QVariant data  = action->data();
    if (data.userType() == QMetaType::QVariantList) {
        // Reset the data first
        action->setData(QVariant());
        QList<QVariant> list = data.toList();

        Q_ASSERT(list.length() == 2);
        Q_ASSERT(list[0].userType() == QMetaType::Int);
        Q_ASSERT(list[1].userType() == QMetaType::Int);

        KisDlgPreferences::Page p = (KisDlgPreferences::Page)list[0].toInt();
        int t = list[1].toInt();
        page = KisDlgPreferences::PageDesc{p, t};
    }

    QScopedPointer<KisDlgPreferences> dlgPreferences(new KisDlgPreferences(this));

    if (dlgPreferences->editPreferences(page)) {
        KisConfigNotifier::instance()->notifyConfigChanged();
        KisConfigNotifier::instance()->notifyPixelGridModeChanged();
        KisImageConfigNotifier::instance()->notifyConfigChanged();

        // XXX: should this be changed for the views in other windows as well?
        Q_FOREACH (QPointer<KisView> koview, KisPart::instance()->views()) {
            KisViewManager *view = qobject_cast<KisViewManager*>(koview);
            if (view) {
                // Update the settings for all nodes -- they don't query
                // KisConfig directly because they need the settings during
                // compositing, and they don't connect to the config notifier
                // because nodes are not QObjects (because only one base class
                // can be a QObject).
                KisNode* node = dynamic_cast<KisNode*>(view->image()->rootLayer().data());
                if (node) {
                    node->updateSettings();
                }
            }

        }
        updateWindowMenu();

        d->viewManager->showHideScrollbars();
    }
}

void KisMainWindow::updateTheme()
{
    // reload action icons!
    Q_FOREACH (QAction *action, actionCollection()->actions()) {
        KisIconUtils::updateIcon(action);
    }
    if (d->mdiArea) {
        d->mdiArea->setPalette(qApp->palette());
        for (int i=0; i<d->mdiArea->subWindowList().size(); i++) {
            QMdiSubWindow *window = d->mdiArea->subWindowList().at(i);
            if (window) {
                window->setPalette(qApp->palette());
                KisView *view = qobject_cast<KisView*>(window->widget());
                if (view) {
                    view->slotThemeChanged(qApp->palette());
                }
            }
        }
    }

    customizeTabBar();

    // Update toolbars
    {
        Q_FOREACH (KisToolBar* aToolBar, toolBars()) {
            QObjectList objects;
            objects.append(aToolBar);
            while (!objects.isEmpty()) {
                QWidget* widget = qobject_cast<QWidget*>(objects.takeFirst());
                if (widget) {
                    objects.append(widget->children());
                    widget->setPalette(qApp->palette());
                }
            }
        }
    }
}

void KisMainWindow::slotThemeChanged()
{
    KConfigGroup group(KSharedConfig::openConfig(), "theme");
#ifndef Q_OS_HAIKU
    if (group.readEntry("Theme", "") == d->themeManager->currentThemeName()) return;

    // save theme changes instantly
    group.writeEntry("Theme", d->themeManager->currentThemeName());
#endif
    updateTheme();

    // Make the other top level windows update as well
    Q_FOREACH (QWidget* topLevelWidget, qApp->topLevelWidgets()) {
        if (topLevelWidget == this) {
            // Skip if the current top level widget is this window
            continue;
        }
        if (topLevelWidget->isHidden()) {
            // Skip unwanted top level widgets like menus, dropdowns, tooltips, etc.
            continue;
        }
        KisMainWindow *topLevelMainWindow = qobject_cast<KisMainWindow*>(topLevelWidget);
        if (topLevelMainWindow) {
            topLevelMainWindow->updateTheme();
            Q_EMIT topLevelMainWindow->themeChanged();
        } else {
            QObjectList objects;
            objects.append(topLevelWidget);
            while (!objects.isEmpty()) {
                QWidget* widget = qobject_cast<QWidget*>(objects.takeFirst());
                if (widget) {
                    objects.append(widget->children());
                    KisIconUtils::updateIconCommon(widget);
                }
            }
        }
    }

    applyActionIconOverridesFromLocalXML();

    Q_EMIT themeChanged();
}

void KisMainWindow::customizeTabBar()
{
    // update MDI area theme
    // Tab close button override
    // just switch this icon out for all OSs so it is easier to see
    QString closeButtonImageUrl;
    QString closeButtonHoverColor;
    if (KisIconUtils::useDarkIcons()) {
        closeButtonImageUrl = QStringLiteral(":/dark_close-tab.svg");
        closeButtonHoverColor = QStringLiteral("lightcoral");
    } else {
        closeButtonImageUrl = QStringLiteral(":/light_close-tab.svg");
        closeButtonHoverColor = QStringLiteral("darkred");
    }
    QString tabStyleSheet = QStringLiteral(R"(
            QTabBar::close-button {
                image: url(%1);
                padding-top: 3px;
            }
            QTabBar::close-button:hover {
                background-color: %2;
            }
            QTabBar::close-button:pressed {
                background-color: red;
            }

            QHeaderView::section {
                padding: 7px;
            }

           )")
           .arg(closeButtonImageUrl, closeButtonHoverColor);


    QTabBar* tabBar = d->findTabBarHACK();
    if (tabBar) {
        tabBar->setStyleSheet(tabStyleSheet);
    }

}

bool KisMainWindow::canvasDetached() const
{
    return centralWidget() != d->widgetStack;
}

void KisMainWindow::setCanvasDetached(bool detach)
{
#ifdef Q_OS_ANDROID
    if (detach) {
        QMessageBox::warning(this, i18nc("@title:window", "Krimble"),
                             "Detach Canvas is unsupported on Android");
    }
#else
    if (detach == canvasDetached()) return;

    QWidget *outgoingWidget = centralWidget() ? takeCentralWidget() : nullptr;
    QWidget *incomingWidget = d->canvasWindow->swapMainWidget(outgoingWidget);

    if (incomingWidget) {
        setCentralWidget(incomingWidget);
    }

    if (detach) {
        d->canvasWindow->show();
    } else {
        d->canvasWindow->hide();
    }
    d->toggleDetachCanvas->setChecked(detach);
#endif
}

QWidget * KisMainWindow::canvasWindow() const
{
    return d->canvasWindow;
}

void KisMainWindow::clearRecentFiles()
{
    KisRecentFilesManager::instance()->clear();
}

void KisMainWindow::removeRecentFile(QString url)
{
    KisRecentFilesManager::instance()->remove(QUrl::fromLocalFile(url));
}

void KisMainWindow::slotUpdateSaveActionTitle(const QString &documentPath)
{
    const QString fileName = QFileInfo(documentPath).fileName();

    if (!fileName.isEmpty()) {
        d->saveAction->setToolTip(i18n("Save as %1", fileName));
    }
    else {
        d->saveAction->setToolTip(i18n("Save"));
    }
}

void KisMainWindow::slotUpdateReadWriteMode(bool readWrite)
{
    Q_UNUSED(readWrite);
    d->actionManager()->updateGUI();
}

KisView *KisMainWindow::activeView() const
{
    if (d->activeView) {
        return d->activeView;
    }
    return 0;
}

bool KisMainWindow::openDocument(const QString &path, OpenFlags flags)
{
    ScopedWidgetDisabler disabler(d->welcomePage->dropFrameBorder);
    QApplication::processEvents(); // make UI more responsive

    if (!QFile(path).exists()) {
        if (!(flags & BatchMode)) {
            QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Krimble"), i18n("The file %1 does not exist.", path));
        }
        KisRecentFilesManager::instance()->remove(QUrl::fromLocalFile(path)); //remove the file from the recent-opened-file-list
        return false;
    }
    return openDocumentInternal(path, flags);
}

bool KisMainWindow::openDocumentInternal(const QString &path, OpenFlags flags)
{
    if (!QFile(path).exists()) {
        qWarning() << "KisMainWindow::openDocumentInternal. Could not open:" << path;
        return false;
    }

    KisDocument *newdoc = KisPart::instance()->createDocument();

    if (flags & BatchMode) {
        newdoc->setFileBatchMode(true);
    }

    d->firstTime = true;
    connect(newdoc, SIGNAL(completed()), this, SLOT(slotLoadCompleted()));
    connect(newdoc, SIGNAL(canceled(QString)), this, SLOT(slotLoadCanceled(QString)));

    KisDocument::OpenFlags openFlags = KisDocument::None;
    // XXX: Why this duplication of OpenFlags...
    if (flags & RecoveryFile) {
        openFlags |= KisDocument::RecoveryFile;
    }

    bool openRet = !(flags & Import) ? newdoc->openPath(path, openFlags) : newdoc->importDocument(path);

    if (!openRet) {
        delete newdoc;
        return false;
    }

    KisPart::instance()->addDocument(newdoc);

    // Try to determine whether this was an unnamed autosave
    if (flags & RecoveryFile &&
            (   path.startsWith(QDir::tempPath())
                || path.startsWith(QDir::homePath())
                ) &&
            (      QFileInfo(path).fileName().startsWith(".krita")
                   || QFileInfo(path).fileName().startsWith("krita")
                   )
            )
    {
        QString path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        if (!QFileInfo(path).exists()) {
            path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
        newdoc->setPath(path + "/" + newdoc->objectName() + ".kra");
    }

    return true;
}

void KisMainWindow::showDocument(KisDocument *document) {
    Q_FOREACH(QMdiSubWindow *subwindow, d->mdiArea->subWindowList()) {
        KisView *view = qobject_cast<KisView*>(subwindow->widget());
        KIS_SAFE_ASSERT_RECOVER_NOOP(view);

        if (view) {
            if (view->document() == document) {
                setActiveSubWindow(subwindow);
                return;
            }
        }
    }

    addViewAndNotifyLoadingCompleted(document);
}

KisView* KisMainWindow::addViewAndNotifyLoadingCompleted(KisDocument *document,
                                                         QMdiSubWindow *subWindow)
{
    showWelcomeScreen(false); // see workaround in function header

    KisView *view = KisPart::instance()->createView(document, d->viewManager, this);
    addView(view, subWindow);

    Q_EMIT guiLoadingFinished();

#ifdef Q_OS_ANDROID
    // HACK: When opening documents on Android, the main window sometimes fails
    // to update until the application is shunted to the background and brought
    // back or the menu bar is fiddled with. Flickering the window fixes this.
    // Having a docker that uses QML somehow also fixes this, so this hack is
    // gone again in 5.3 with the introduction of the text properties docker.
    // UNHACK: But not on Xiaomi devices! There flickering the window makes it
    // sorta exit fullscreen, adding black bars at the top and bottom. Just
    // opening a document seems to work fine on these devices though, it delays
    // for a moment then displays fine, so just skip this there I guess.
    if (!KisAndroidUtils::looksLikeXiaomiDevice()) {
        slotFlashWindowHack();
    }
#endif

    return view;
}

QStringList KisMainWindow::showOpenFileDialog(bool isImporting)
{
    KoFileDialog dialog(this, KoFileDialog::ImportFiles, "OpenDocument");
    dialog.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    dialog.setMimeTypeFilters(KisImportExportManager::supportedMimeTypes(KisImportExportManager::Import));
    dialog.setCaption(isImporting ? i18n("Import Images") : i18n("Open Images"));

    QStringList filenames = dialog.filenames();
    filenames.sort();

    return filenames;
}

// Separate from openDocument to handle async loading (remote URLs)
void KisMainWindow::slotLoadCompleted()
{
    KisDocument *newdoc = qobject_cast<KisDocument*>(sender());
    if (newdoc && newdoc->image()) {
        addViewAndNotifyLoadingCompleted(newdoc);

        disconnect(newdoc, SIGNAL(completed()), this, SLOT(slotLoadCompleted()));
        disconnect(newdoc, SIGNAL(canceled(QString)), this, SLOT(slotLoadCanceled(QString)));

        Q_EMIT loadCompleted();
    }
}

void KisMainWindow::slotLoadCanceled(const QString &errMsg)
{
    KisUsageLogger::log(QString("Loading canceled: %1.").arg(errMsg));
    KisDocument* doc = qobject_cast<KisDocument*>(sender());
    Q_ASSERT(doc);
    disconnect(doc, SIGNAL(completed()), this, SLOT(slotLoadCompleted()));
    disconnect(doc, SIGNAL(canceled(QString)), this, SLOT(slotLoadCanceled(QString)));
}

void KisMainWindow::slotSaveCanceled(const QString &errMsg)
{
    if (!errMsg.isEmpty()) {   // empty when cancelled by user
        KisUsageLogger::log(QString("Saving cancelled. Error:").arg(errMsg));
        QMessageBox::critical(this, i18nc("@title:window", "Krimble"), errMsg);
    }
    else {
        KisUsageLogger::log(QString("Saving cancelled by the user."));
    }
    slotSaveCompleted();
}

void KisMainWindow::slotSaveCompleted()
{
    KisUsageLogger::log(QString("Saving Completed"));
    KisDocument* doc = qobject_cast<KisDocument*>(sender());
    if (doc) {
        disconnect(doc, SIGNAL(completed()), this, SLOT(slotSaveCompleted()));
        disconnect(doc, SIGNAL(canceled(QString)), this, SLOT(slotSaveCanceled(QString)));
    }

    if (d->deferredClosingEvent) {
        KXmlGuiWindow::closeEvent(d->deferredClosingEvent);
    }
}

bool KisMainWindow::hackIsSaving() const
{
    std::unique_lock<QMutex> l(d->savingEntryMutex, std::try_to_lock);
    return !l.owns_lock();
}

bool KisMainWindow::installBundle(const QString &fileName) const
{
    QFileInfo from(fileName);
    QFileInfo to(QStringLiteral("%1/%2").arg(KoResourcePaths::getAppDataLocation(), from.fileName()));
    if (to.exists()) {
        QFile::remove(to.canonicalFilePath());
    }
    return QFile::copy(fileName, to.absoluteFilePath());
}

QImage KisMainWindow::layoutThumbnail()
{
    int size = 256;
    qreal scale = qreal(size)/qreal(qMax(geometry().width(), geometry().height()));
    QImage layoutThumbnail = QImage(qRound(geometry().width()*scale), qRound(geometry().height()*scale), QImage::Format_ARGB32);
    QPainter gc(&layoutThumbnail);
    gc.fillRect(0, 0, layoutThumbnail.width(), layoutThumbnail.height(), this->palette().dark());

    Q_FOREACH(const QObject *child, children()) {
        if (child->isWidgetType()) {
            const QWidget *w = static_cast<const QWidget *>(child);

            if (w->isVisible() && !w->property("_kis_excludeFromLayoutThumbnail").toBool()) {
                QRect wRect = QRectF(w->geometry().x()*scale
                                     , w->geometry().y()*scale
                                     , w->geometry().width()*scale
                                     , w->geometry().height()*scale
                                     ).toRect();

                wRect = wRect.intersected(layoutThumbnail.rect().adjusted(-1, -1, -1, -1));

                gc.setBrush(this->palette().window());
                if (w == d->widgetStack) {
                    gc.setBrush(d->mdiArea->background());
                }
                gc.setPen(this->palette().windowText().color());
                gc.drawRect(wRect);
            }
        }
    }
    gc.end();
    return layoutThumbnail;
}

bool KisMainWindow::saveDocument(KisDocument *document, bool saveas, bool isExporting, bool isAdvancedExporting )
{
    if (!document) {
        return true;
    }

    /**
     * Make sure that we cannot enter this method twice!
     *
     * The lower level functions may call processEvents() so
     * double-entry is quite possible to achieve. Here we try to lock
     * the mutex, and if it is failed, just cancel saving.
     */
    std::unique_lock<QMutex> l(d->savingEntryMutex, std::try_to_lock);
    if (!l.owns_lock()) return false;

    // no busy wait for saving because it is dangerous!
    KisDelayedSaveDialog dlg(document->image(), KisDelayedSaveDialog::SaveDialog, 0, this);
    dlg.blockIfImageIsBusy();

    if (dlg.result() == KisDelayedSaveDialog::Rejected) {
        return false;
    }
    else if (dlg.result() == KisDelayedSaveDialog::Ignored) {
        QMessageBox::critical(qApp->activeWindow(),
                              i18nc("@title:window", "Krimble"),
                              i18n("You are saving a file while the image is "
                                   "still rendering. The saved file may be "
                                   "incomplete or corrupted.\n\n"
                                   "Please select a location where the original "
                                   "file will not be overridden!"));


        saveas = true;
    }

    if (document->isRecovered()) {
        saveas = true;
    }

    if (document->path().isEmpty()) {
        saveas = true;
    }

    QFile target(document->path());
    if (!target.exists()) { // Possible when file is moved/unmounted during Krimble session.
        saveas = true;
    }

    connect(document, SIGNAL(completed()), this, SLOT(slotSaveCompleted()));
    connect(document, SIGNAL(canceled(QString)), this, SLOT(slotSaveCanceled(QString)));

    QByteArray nativeFormat = document->nativeFormatMimeType();
    QByteArray oldMimeFormat = document->mimeType();

    QUrl suggestedURL = QUrl::fromLocalFile(document->path());

    QStringList mimeFilter = KisImportExportManager::supportedMimeTypes(KisImportExportManager::Export);

    mimeFilter = KisImportExportManager::supportedMimeTypes(KisImportExportManager::Export);
    if (!mimeFilter.contains(oldMimeFormat)) {
        dbgUI << "KisMainWindow::saveDocument no export filter for" << oldMimeFormat;

        // --- don't setOutputMimeType in case the user cancels the Save As
        // dialog and then tries to just plain Save ---

        // suggest a different filename extension (yes, we fortunately don't all live in a world of magic :))
        QString suggestedFilename = QFileInfo(suggestedURL.toLocalFile()).completeBaseName();

        if (!suggestedFilename.isEmpty()) {  // ".kra" looks strange for a name
            suggestedFilename = suggestedFilename + "." + KisMimeDatabase::suffixesForMimeType(KIS_MIME_TYPE).first();
            suggestedURL = suggestedURL.adjusted(QUrl::RemoveFilename);
            suggestedURL.setPath(suggestedURL.path() + suggestedFilename);
        }

        // force the user to choose outputMimeType
        saveas = true;
    }

    bool ret = false;

    if (document->path().isEmpty() || isExporting || saveas) {
        // if you're just File/Save As'ing to change filter options you
        // don't want to be reminded about overwriting files etc.
        bool justChangingFilterOptions = false;

        KoFileDialog dialog(this, KoFileDialog::SaveFile, "SaveAs");
        dialog.setCaption(isExporting ? i18n("Exporting") : i18n("Saving As"));

        //qDebug() << ">>>>>" << isExporting << d->lastExportLocation << d->lastExportedFormat << QString::fromLatin1(document->mimeType());

        if (isExporting && !d->lastExportLocation.isEmpty() && !d->lastExportLocation.contains(QDir::tempPath())) {

            // Use the location where we last exported to, if it's set, as the opening location for the file dialog
            QString proposedPath = QFileInfo(d->lastExportLocation).absolutePath();
            // If the document doesn't have a filename yet, use the title
            QString proposedFileName = suggestedURL.isEmpty() ? document->documentInfo()->aboutInfo("title") :  QFileInfo(suggestedURL.toLocalFile()).completeBaseName();
            // Use the last mimetype we exported to by default
            QString proposedMimeType =  d->lastExportedFormat.isEmpty() ? "" : d->lastExportedFormat;
            QString proposedExtension = KisMimeDatabase::suffixesForMimeType(proposedMimeType).first().remove("*,");

            // Set the default dir: this overrides the one loaded from the config file, since we're exporting and the lastExportLocation is not empty
            dialog.setDefaultDir(proposedPath + "/" + proposedFileName + "." + proposedExtension, true);
            dialog.setMimeTypeFilters(mimeFilter, proposedMimeType);
        }
        else { 
            //This section only runs when the user haven't exported yet during the session, behavior set in the File Handling, Default MimeType setting.
            KisConfig cfg(true);
            QByteArray default_mime_type = cfg.exportMimeType(false).toUtf8();
            QString proposedMimeType = QString::fromLatin1(default_mime_type);

            // Get the last used location for saving
            KConfigGroup group =  KSharedConfig::openConfig()->group("File Dialogs");
            QString proposedPath = group.readEntry("SaveAs", "");
            // if that is empty, get the last used location for loading
            if (proposedPath.isEmpty()) {
                proposedPath = group.readEntry("OpenDocument", "");
            }
            // If that is empty, too, use the Pictures location.
            if (proposedPath.isEmpty()) {
                proposedPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
            }
            // But only use that if the suggestedUrl, that is, the document's own url is empty, otherwise
            // open the location where the document currently is.

            //For if the user picked All Files Supported as the default, where there would not be an extension
            if(default_mime_type == "all/mime"){
                dialog.setDefaultDir(suggestedURL.isEmpty() ? proposedPath : proposedPath + "/" + QFileInfo(suggestedURL.toLocalFile()).completeBaseName(), true);
            }

            if (default_mime_type != "all/mime" && !default_mime_type.isEmpty()) {
                QString proposedExtension = KisMimeDatabase::suffixesForMimeType(proposedMimeType).first().remove("*,");
                //This line is responsible for setting filename, which also manipulates filters.
                dialog.setDefaultDir(suggestedURL.isEmpty() ? proposedPath :  proposedPath + "/" + QFileInfo(suggestedURL.toLocalFile()).completeBaseName() + "." + proposedExtension, true);
            }

            if (!isExporting) {
                // If Saving, use the document's mimetype, or if that is empty, kra, which is the safest.
                dialog.setDefaultDir(suggestedURL.isEmpty() ? proposedPath : suggestedURL.toLocalFile(), true);
                default_mime_type = document->mimeType().isEmpty() ? nativeFormat : document->mimeType();
            }
            dialog.setMimeTypeFilters(mimeFilter, QString::fromLatin1(default_mime_type));
        }

        QString newFilePath = dialog.filename();

        bool bOk = true;
        if (newFilePath.isEmpty()) {
            bOk = false;
        }

        if (bOk) {
            if (document->documentInfo()->aboutInfo("title") == i18n("Unnamed")) {
                QString fn = newFilePath;
                QFileInfo info(fn);
                document->documentInfo()->setAboutInfo("title", info.completeBaseName());
            }

            QByteArray outputFormat = nativeFormat;

            QString outputFormatString = KisMimeDatabase::mimeTypeForFile(newFilePath, false);
            outputFormat = outputFormatString.toLatin1();


            if (!isExporting) {
                justChangingFilterOptions = (newFilePath == document->path()) && (outputFormat == document->mimeType());
            }
            else {
                QString path = QFileInfo(d->lastExportLocation).absolutePath();
                QString filename = QFileInfo(document->path()).completeBaseName();
                justChangingFilterOptions = (QFileInfo(newFilePath).absolutePath() == path)
                        && (QFileInfo(newFilePath).completeBaseName() == filename)
                        && (outputFormat == d->lastExportedFormat);
            }

            bool wantToSave = true;

            // don't change this line unless you know what you're doing :)
            if (!justChangingFilterOptions) {
                if (!document->isNativeFormat(outputFormat))
                    wantToSave = true;
            }

            if (wantToSave) {
                if (!isExporting) {  // Save As
                    ret = document->saveAs(newFilePath, outputFormat, true);
                    if (ret) {
                        dbgUI << "Successful Save As!";
                        KisPart::instance()->queueAddRecentURLToAllMainWindowsOnFileSaved(QUrl::fromLocalFile(newFilePath));
                    } else {
                        dbgUI << "Failed Save As!";

                    }
                }
                else { // Export
                    ret = document->exportDocument(newFilePath, outputFormat, isAdvancedExporting, true);
                    if (ret) {
                        d->lastExportLocation = newFilePath;
                        d->lastExportedFormat = outputFormat;
                    }
                }

            }   // if (wantToSave)  {
            else
                ret = false;
        }   // if (bOk) {
        else
            ret = false;
    } else { // saving
        // We cannot "export" into the currently
        // opened document. We are not Gimp.
        KIS_ASSERT_RECOVER_NOOP(!isExporting);

        // be sure document has the correct outputMimeType!
        if (document->isModified()) {
            ret = document->save(true, 0);
        }

        if (!ret) {
            dbgUI << "Failed Save!";
        }
    }

    return ret;
}

void KisMainWindow::undo()
{
    if (activeView()) {
        activeView()->document()->undoStack()->undo();
    }
}

void KisMainWindow::redo()
{
    if (activeView()) {
        activeView()->document()->undoStack()->redo();
    }
}

void KisMainWindow::closeEvent(QCloseEvent *e)
{
    if (hackIsSaving()) {
        e->setAccepted(false);
        return;
    }

    if (!KisPart::instance()->closingSession()) {
        QAction *action= d->viewManager->actionCollection()->action("view_show_canvas_only");
        if ((action) && (action->isChecked())) {
            action->setChecked(false);
        }

        // Save session when last window is closed
        if (KisPart::instance()->mainwindowCount() == 1) {
            bool closeAllowed = KisPart::instance()->closeSession();

            if (!closeAllowed) {
                e->setAccepted(false);
                return;
            }
        }
    }

    d->mdiArea->closeAllSubWindows();

    QList<QMdiSubWindow*> childrenList = d->mdiArea->subWindowList();

    if (childrenList.isEmpty()) {
        // TODO: that assignment looks really suspicious, because `e`
        // is destroyed right after the exit of this function
        d->deferredClosingEvent = e;
        d->canvasWindow->close();
    } else {
        e->setAccepted(false);
    }
}

void KisMainWindow::saveWindowSettings()
{
    KSharedConfigPtr config =  KSharedConfig::openConfig();

    if (d->windowSizeDirty ) {
        dbgUI << "KisMainWindow::saveWindowSettings";
        KConfigGroup group = d->windowStateConfig;
        KWindowConfig::saveWindowSize(windowHandle(), group);
        config->sync();
        d->windowSizeDirty = false;
    }

    if (!d->activeView || d->activeView->document()) {

        // Save toolbar position into the config file of the app, under the doc's component name
        KConfigGroup group = d->windowStateConfig;
        saveMainWindowSettings(group);

        // Save state of dock widgets
        for (QMap<QString, QDockWidget*>::const_iterator i = d->dockWidgetsMap.constBegin();
             i != d->dockWidgetsMap.constEnd(); ++i) {
            if (i.value()->widget()) {
                KConfigGroup dockGroup = group.group(QString("DockWidget ") + i.key());
                dockGroup.writeEntry("Locked", i.value()->property("Locked").toBool());
                dockGroup.writeEntry("DockArea", (int) dockWidgetArea(i.value()));
                dockGroup.writeEntry("xPosition", (int) i.value()->widget()->x());
                dockGroup.writeEntry("yPosition", (int) i.value()->widget()->y());

                dockGroup.writeEntry("width", (int) i.value()->widget()->width());
                dockGroup.writeEntry("height", (int) i.value()->widget()->height());
            }
        }

    }

    KSharedConfig::openConfig()->sync();
    resetAutoSaveSettings(); // Don't let KisKMainWindow override the good stuff we wrote down

}

void KisMainWindow::resizeEvent(QResizeEvent * e)
{
    d->windowSizeDirty = true;
    KXmlGuiWindow::resizeEvent(e);
}


void KisMainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    dragMove(event);
    event->accept();
}

void KisMainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    dragLeave();
    event->accept();
}

bool KisMainWindow::windowsLayoutSavingAllowed() const
{
    QAction *action= d->viewManager->actionCollection()->action("view_show_canvas_only");
    return !action || !action->isChecked();
}

void KisMainWindow::showEvent(QShowEvent *event)
{
    // we're here because, we need to make sure everything (dockers, toolbars etc) is loaded and ready before
    // we can hide it.
    if (!event->spontaneous()) {
        setMainWindowLayoutForCurrentMainWidget(d->widgetStack->currentIndex(), false);
    }
#ifdef Q_OS_ANDROID
    // The user can conceivably purchase a product from the splash screen while
    // Krimble is still loading. In that case, the "pending" flag will be set. The
    // dialog in question will clear the flag.
    KisAndroidDonations *androidDonations = KisAndroidDonations::instance();
    if (androidDonations && androidDonations->isShowDonationManagementDialogPending()) {
        QTimer::singleShot(0, this, &KisMainWindow::slotShowDonationManagementDialog);
    }
#endif
    return KXmlGuiWindow::showEvent(event);
}

void KisMainWindow::setMainWindowLayoutForCurrentMainWidget(int widgetIndex, bool widgetIndexChanged)
{

    if (widgetIndex == 0) {
        if (widgetIndexChanged) {
            /// save the state of the window which existed up-to now (this is before we stop auto-saving).
            ///
            /// saving should happen if the call s has come from a real state change, but not from
            /// on-startup initialization

            if (d->mdiArea->subWindowList().isEmpty()) {
                /**
                 * When closing the latest subwindow we should restore the
                 * canvas-only mode to save stuff properly
                 */
                saveWindowState(true);
            } else {
                saveMainWindowSettings(d->windowStateConfig);
            }
        }
        adjustLayoutForWelcomePage();
    }
    else {
        setAutoSaveSettings(d->windowStateConfig, false);
        statusBar()->setVisible(KisConfig(true).showStatusBar());
    }

    QList<QAction *> actions = d->dockWidgetMenu->menu()->actions();
    actions.append(toolBarMenuAction()->menu()->actions());
    for (QAction *action : actions) {
        if (action) {
            action->setEnabled(widgetIndex);
        }
    }
}

void KisMainWindow::adjustLayoutForWelcomePage()
{
    // This makes sure we don't save window state when we're in welcome page mode, because all the dockers
    // etc are hidden while the user is here.
    resetAutoSaveSettings();

    toggleDockersVisibility(false, true);
    if (statusBar()) {
        statusBar()->hide();
    }
    QList<QToolBar *> toolbars = findChildren<QToolBar *>();
    for (QToolBar *toolbar : toolbars) {
        if (toolbar->objectName() != "mainToolBar") {
            toolbar->hide();
        }
    }
}

void KisMainWindow::setActiveView(KisView* view)
{
    d->activeView = view;
    krimbleApplyXmlGuiLabels(this);     // KRIMBLE 2026-10-04: labels from krita5.xmlgui

    if (d->undoActionsUpdateManager) {
        d->undoActionsUpdateManager->setCurrentDocument(view ? view->document() : 0);
    }

    d->viewManager->setCurrentView(view);

    d->activeViewConnections.clear();
    d->activeViewConnections.addConnection(view->document(),
                                           SIGNAL(sigPathChanged(QString)),
                                           this, SLOT(slotUpdateSaveActionTitle(QString)));
    slotUpdateSaveActionTitle(view->document()->path());
    d->activeViewConnections.addConnection(view->document(),
                                           SIGNAL(sigReadWriteChanged(bool)),
                                           this, SLOT(slotUpdateReadWriteMode(bool)));
    slotUpdateReadWriteMode(view->document()->isReadWrite());

    KisWindowLayoutManager::instance()->activeDocumentChanged(view->document());

    Q_EMIT activeViewChanged();
}

void KisMainWindow::unsetActiveView()
{
    d->activeViewConnections.clear();
    slotUpdateSaveActionTitle(QString());
    slotUpdateReadWriteMode(false);
}

void KisMainWindow::dragMove(QDragMoveEvent * event)
{
    QTabBar *tabBar = d->findTabBarHACK();

    if (!tabBar && d->mdiArea->viewMode() == QMdiArea::TabbedView) {
        qWarning() << "WARNING!!! Cannot find QTabBar in the main window! Looks like Qt has changed behavior. Drag & Drop between multiple tabs might not work properly (tabs will not switch automatically)!";
    }

    if (tabBar && tabBar->isVisible()) {
        QPoint pos = tabBar->mapFromGlobal(mapToGlobal(event->pos()));
        if (tabBar->rect().contains(pos)) {
            const int tabIndex = tabBar->tabAt(pos);

            if (tabIndex >= 0 && tabBar->currentIndex() != tabIndex) {
                d->tabSwitchCompressor->start(tabIndex);
            }
        } else if (d->tabSwitchCompressor->isActive()) {
            d->tabSwitchCompressor->stop();
        }
    }
}

void KisMainWindow::dragLeave()
{
    if (d->tabSwitchCompressor->isActive()) {
        d->tabSwitchCompressor->stop();
    }
}


void KisMainWindow::switchTab(int index)
{
    QTabBar *tabBar = d->findTabBarHACK();
    if (!tabBar) return;

    tabBar->setCurrentIndex(index);
}

void KisMainWindow::showWelcomeScreen(bool show)
{
    const int currentIndex = show ? 0 : 1;
    if (d->widgetStack->currentIndex() != currentIndex) {
        setUpdatesEnabled(false);
        // These have to be done in different sequence to avoid graphical
        // layout glitch during the switch.
        if (show) {
            setMainWindowLayoutForCurrentMainWidget(currentIndex, true);
            d->widgetStack->setCurrentIndex(currentIndex);
        } else {
            d->widgetStack->setCurrentIndex(currentIndex);
            setMainWindowLayoutForCurrentMainWidget(currentIndex, true);
        }
        setUpdatesEnabled(true);
    }
}

void KisMainWindow::slotFileNew()
{
    KisDlgCreateNewDocument* dlg = new KisDlgCreateNewDocument(this);
    dlg->exec();
    dlg->deleteLater();
}

void KisMainWindow::slotImportFile()
{
    dbgUI << "slotImportFile()";
    slotFileOpen(true);
}


void KisMainWindow::slotFileOpen(bool isImporting)
{
    QStringList urls = showOpenFileDialog(isImporting);

    if (urls.isEmpty())
        return;

    Q_FOREACH (const QString& url, urls) {

        if (!url.isEmpty()) {
            OpenFlags flags = isImporting ? Import : None;
            bool res = openDocument(url, flags);
            if (!res) {
                warnKrita << "Loading" << url << "failed";
            }
        }
    }
}

void KisMainWindow::slotFileOpenRecent(const QUrl &url)
{
    (void) openDocument(url.toLocalFile(), None);
}

void KisMainWindow::slotFileSave()
{
    if (saveDocument(d->activeView->document(), false, false,false)) {
        Q_EMIT documentSaved();
    }
}

void KisMainWindow::slotChangeInterfaceScale()
{
#if defined(Q_OS_ANDROID) && KRITA_QT_HAS_ANDROID_QPLATFORMSCREEN_DENSITY_ADJUSTMENT
    KisAndroidScaling *androidScaling = KisAndroidScaling::instance();
    if (androidScaling) {
        androidScaling->showDialog();
    }
#endif
}

void KisMainWindow::slotFileSaveAs()
{
    if (saveDocument(d->activeView->document(), true, false,false)) {
        Q_EMIT documentSaved();
    }
}

void KisMainWindow::slotExportFile()
{
    if (saveDocument(d->activeView->document(), true, true,false)) {
        Q_EMIT documentSaved();
    }
}
void KisMainWindow::slotExportAdvance()
{
    if (saveDocument(d->activeView->document(), true, true,true)) {
        Q_EMIT documentSaved();
    }
}

void KisMainWindow::slotShowSessionManager() {
    KisPart::instance()->showSessionManager();
}

#ifdef Q_OS_ANDROID
void KisMainWindow::slotShowDonationManagementDialog()
{
    // Don't show the donation management dialog on top of another dialog
    // that may have triggered a donation flow, such as the bundle manager.
    QWidget *win = qApp->activeWindow();
    if (win && !qobject_cast<KisMainWindow *>(win) && (win->isModal() || win->windowModality() != Qt::NonModal)) {
        return;
    }

    // We don't use `exec` here because the purchase stuff runs in Android's
    // event loop, so it's legitimately possible that we get hit by another
    // request to show the donation management dialog while it's already up
    // and it's more convenient for the dialog to handle the deduplication.
    QString objectName = QStringLiteral("kisdonationmanagementdialog");
    KisDonationManagementDialog *dlg = findChild<KisDonationManagementDialog *>(objectName, Qt::FindDirectChildrenOnly);
    if (dlg) {
        dlg->reshow();
    } else {
        dlg = new KisDonationManagementDialog(this);

        QAction *action = actionCollection()->action("manage_supporter_bundles");
        if (action) {
            connect(dlg, &KisDonationManagementDialog::sigShowSupporterBundles, action, &QAction::trigger);
        }

        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setObjectName(objectName);
        dlg->show();
    }
}

void KisMainWindow::slotFlashWindowHack()
{
    if (!d->flashWindowHackInProgress) {
        d->flashWindowHackInProgress = true;
        QTimer::singleShot(0, this, [this] {
            hide();
            QTimer::singleShot(0, this, [this] {
                show();
                d->flashWindowHackInProgress = false;
            });
        });
    }
}
#endif

KoCanvasResourceProvider *KisMainWindow::resourceManager() const
{
    return d->viewManager->canvasResourceProvider()->resourceManager();
}

int KisMainWindow::viewCount() const
{
    return d->mdiArea->subWindowList().size();
}

const KConfigGroup &KisMainWindow::windowStateConfig() const
{
    return d->windowStateConfig;
}

void KisMainWindow::saveWindowState(bool restoreNormalState)
{
    // We don't need to save welcome page's layout
    if (d->widgetStack->currentIndex() == 0) {
        // TODO(sh_zam): We should still save position/geometry, right?
        return;
    }

    if (restoreNormalState) {
        QAction *showCanvasOnly = d->viewManager->actionCollection()->action("view_show_canvas_only");

        if (showCanvasOnly && showCanvasOnly->isChecked()) {
            showCanvasOnly->setChecked(false);
        }

        d->windowStateConfig.writeEntry("ko_geometry", saveGeometry().toBase64());
        d->windowStateConfig.writeEntry("State", saveState().toBase64());

        // if the dockers are hidden at this time, save their state.
        if (!d->toggleDockers->isChecked()) {
            restoreState(d->dockerStateBeforeHiding);
        }

        statusBar()->setVisible(true);
        menuBar()->setVisible(true);

        saveWindowSettings();

    } else {
        saveMainWindowSettings(d->windowStateConfig);
    }

}

bool KisMainWindow::restoreWorkspaceState(const QByteArray &state)
{
    QByteArray oldState = saveState();
    const bool showTitlebars = KisConfig(false).showDockerTitleBars();

    // needed because otherwise the layout isn't correctly restored in some situations
    Q_FOREACH (QDockWidget *dock, dockWidgets()) {
        if (dock) {
            dock->setProperty("Locked", false); // Unlock invisible dockers
            dock->toggleViewAction()->setEnabled(true);
            dock->hide();
            if (dock->titleBarWidget() && !dock->titleBarWidget()->inherits("KisUtilityTitleBar")) {
                dock->titleBarWidget()->setVisible(showTitlebars);
            }
        }
    }

    bool success = KXmlGuiWindow::restoreState(state);

    if (!success) {
        KXmlGuiWindow::restoreState(oldState);
        Q_FOREACH (QDockWidget *dock, dockWidgets()) {
            if (dock->titleBarWidget() && !dock->titleBarWidget()->inherits("KisUtilityTitleBar")) {
                dock->titleBarWidget()->setVisible(showTitlebars || dock->isFloating());
            }
        }
        return false;
    }
    return success;
}

void KisMainWindow::restoreWorkspace()
{
    QString md5 = sender()->property("md5").toString();
    KoResourceServer<KisWorkspaceResource> *rserver = KisResourceServerProvider::instance()->workspaceServer();
    KoResourceSP resource = rserver->resource(md5, "", "");
    if (resource) {
        restoreWorkspace(resource);
    }
    else {
        qWarning() << "Could not retrieve resource for" << md5;
    }
}

void KisMainWindow::openCommandBar()
{
    QList<KisKActionCollection *> actionCollections;

    auto clients = guiFactory()->clients();
    int actionsCount = 0;
    for (const KisKXMLGUIClient *c : clients) {
        if (!c) {
            continue;
        }
        if (auto collection = c->actionCollection()) {
            actionCollections.append(collection);
            actionsCount += collection->count();
        }
    }

    if (activeKisView()) {
        KisKActionCollection *layerActionCollection = new KisKActionCollection(0, "layeractions (disposable)");
        layerActionCollection->setComponentDisplayName(i18n("Layers/Masks"));
        KisNodeActivationActionCreatorVisitor v(layerActionCollection, viewManager()->nodeManager());
        activeKisView()->image()->rootLayer()->accept(v);
        actionCollections.append(layerActionCollection);
        actionsCount += layerActionCollection->count();
    }

    d->commandBar->updateBar(actionCollections, actionsCount);

    // The following line is needed to work around input method not working
    // on Windows.
    // See https://bugs.kde.org/show_bug.cgi?id=395598
    // and https://bugs.kde.org/show_bug.cgi?id=438122
    d->commandBar->activateWindow();

    // The following line is present in Kate's version and was ported over
    // but I am sceptical of its use. I worry that it may subtly cause other
    // issues, and since the command bar appears to work fine without it, I
    // believe it may be better to leave it out.  -- Alvin
    // centralWidget()->setFocusProxy(d->commandBar);
}

void KisMainWindow::slotStoragesWarning(const QString &/*location*/)
{
    QString warning;
    if (!checkActiveBundlesAvailable()) {
        warning = i18n("You don't have any resource bundles enabled.");
    }

    if (!checkPaintOpAvailable()) {
        warning += i18n("\nThere are no brush presets available. Please enable a bundle that has presets before continuing.\n");
        QMessageBox::critical(this, i18nc("@title:window", "Krimble"), warning);

        QAction *action = actionCollection()->action("manage_bundles");
        if (action) {
            action->trigger();
        }
    }

    if (!checkActiveBundlesAvailable()) {
        QMessageBox::warning(this, i18nc("@title:window", "Krimble"), warning + i18n("\nOnly your local resources are available."));
    }

}

bool KisMainWindow::restoreWorkspace(KoResourceSP res)
{
    KisWorkspaceResourceSP workspace = res.dynamicCast<KisWorkspaceResource>();

    bool success = restoreWorkspaceState(workspace->dockerState());

    // KRIMBLE 2026-10-02: while the welcome page is shown (no document open) the
    // panels are hidden and the layout from before they were hidden is kept in
    // dockerStateBeforeHiding; it is put back when a document opens. A workspace
    // applied at that point (for example the first-launch Default workspace) left
    // that saved layout untouched, so the first new file brought back the panels
    // from before the workspace (Brush Presets, Color Selector) instead of the
    // workspace's panels. Capture the workspace layout and hide the panels again,
    // exactly as the welcome page does.
    if (success && d->mdiArea->subWindowList().isEmpty()) {
        toggleDockersVisibility(false, true);
    }

    const bool showTitlebars = KisConfig(false).showDockerTitleBars();
    Q_FOREACH (QDockWidget *dock, dockWidgets()) {
        if (dock->titleBarWidget() && !dock->titleBarWidget()->inherits("KisUtilityTitleBar")) {
            dock->titleBarWidget()->setVisible(showTitlebars || dock->isFloating());
        }
    }

    if (activeKisView()) {
        activeKisView()->resourceProvider()->notifyLoadingWorkspace(workspace);
    }

    d->viewManager->notifyWorkspaceLoaded();

    return success;
}

QByteArray KisMainWindow::borrowWorkspace(KisMainWindow *other)
{
    QByteArray currentWorkspace = saveState();

    if (!d->workspaceBorrowedBy.isNull()) {
        if (other->id() == d->workspaceBorrowedBy) {
            // We're swapping our original workspace back
            d->workspaceBorrowedBy = QUuid();
            return currentWorkspace;
        } else {
            // Get our original workspace back before swapping with a third window
            KisMainWindow *borrower = KisPart::instance()->windowById(d->workspaceBorrowedBy);
            if (borrower) {
                QByteArray originalLayout = borrower->borrowWorkspace(this);
                borrower->restoreWorkspaceState(currentWorkspace);

                d->workspaceBorrowedBy = other->id();
                return originalLayout;
            }
        }
    }

    d->workspaceBorrowedBy = other->id();
    return currentWorkspace;
}

void KisMainWindow::swapWorkspaces(KisMainWindow *a, KisMainWindow *b)
{
    QByteArray workspaceA = a->borrowWorkspace(b);
    QByteArray workspaceB = b->borrowWorkspace(a);

    a->restoreWorkspaceState(workspaceB);
    b->restoreWorkspaceState(workspaceA);
}

KisViewManager *KisMainWindow::viewManager() const
{
    return d->viewManager;
}

void KisMainWindow::slotDocumentInfo()
{
    if (!d->activeView->document())
        return;

    KoDocumentInfo *docInfo = d->activeView->document()->documentInfo();

    if (!docInfo)
        return;

    KoDocumentInfoDlg *dlg = d->activeView->document()->createDocumentInfoDialog(this, docInfo);

    if (dlg->exec()) {
        if (dlg->isDocumentSaved()) {
            d->activeView->document()->setModified(false);
        } else {
            d->activeView->document()->setModified(true);
        }
    }

    delete dlg;
}

bool KisMainWindow::slotFileCloseAll()
{
    Q_FOREACH (QMdiSubWindow *subwin, d->mdiArea->subWindowList()) {
        if (subwin) {
            if(!subwin->close())
                return false;
        }
    }

    return true;
}

void KisMainWindow::slotFileQuit()
{
    // Do not close while KisMainWindow has the savingEntryMutex locked, bug409395.
    // After the background saving job is initiated, KisDocument blocks closing
    // while it saves itself.
    if (hackIsSaving()) {
        return;
    }
    KisPart::instance()->closeSession();
}

void KisMainWindow::importAnimation()
{
    if (!activeView()) return;

    KisDocument *document = activeView()->document();
    if (!document) return;

    KisDlgImportImageSequence dlg(this, document);

    if (dlg.exec() == QDialog::Accepted) {
        QStringList files = dlg.files();
        int firstFrame = dlg.firstFrame();
        int step = dlg.step();
        bool startFrom1 = dlg.startFrom1();
        bool autoAddHoldframes = dlg.autoAddHoldframes();


        KoUpdaterPtr updater =
                !document->fileBatchMode() ? viewManager()->createUnthreadedUpdater(i18n("Import frames")) : 0;
        KisAnimationImporter importer(document->image(), updater);
        int isAscending = dlg.isAscending();
        KisImportExportErrorCode status = importer.import(files, firstFrame, step, autoAddHoldframes, startFrom1, isAscending);  // modify here, add a flag

        if (!status.isOk() && !status.isInternalError()) {
            QString msg = status.errorMessage();
            if (!msg.isEmpty())
                QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Krimble"), i18n("Could not finish import animation:\n%1", msg));
        }
        activeView()->canvasBase()->refetchDataFromImage();
    }
}

void KisMainWindow::importVideoAnimation()
{
    // Importing video requires ffmpeg, which is not available on Android.
#ifndef Q_OS_ANDROID
    KisDocument *document;
    KisDlgImportVideoAnimation dlg(this, activeView());

    if (dlg.exec() == QDialog::Accepted) {
        const QTemporaryDir outputLocation(QDir::tempPath() + QDir::separator() + "krita" + QDir::separator() + "import_files");
        RenderedFrames renderedFrames = dlg.renderFrames(QDir(outputLocation.path()));
        dbgFile << "Frames rendered to directory: " << outputLocation.path();
        QStringList documentInfoList = dlg.documentInfo();

        if (renderedFrames.isEmpty()) return;
        
        dbgFile << "Animation Import options: " << documentInfoList;

        int firstFrame = 0;
        const int step = documentInfoList[0].toInt();
        const int fps = documentInfoList[1].toInt();
        const int totalFrames = renderedFrames.framesNeedRelocation() ? (renderedFrames.renderedFrameTargetTimes.last() + 1) : renderedFrames.size() * step;
        const QString name = QFileInfo(documentInfoList[3]).fileName();
        const bool useCurrentDocument = documentInfoList[4].toInt();
        bool useDocumentColorSpace = false;

        if ( useCurrentDocument ) {
            document = activeView()->document();

            dbgFile << "Current frames:" << document->image()->animationInterface()->totalLength() << "total frames:" << totalFrames;
            if ( document->image()->animationInterface()->totalLength() < totalFrames ) {
                document->image()->animationInterface()->setDocumentRangeStartFrame(0);
                document->image()->animationInterface()->setDocumentRangeEndFrame(totalFrames);
            }
        } else {
            const int width = documentInfoList[5].toInt();
            const int height = documentInfoList[6].toInt();
            const double resolution = documentInfoList[7].toDouble();

            const QString colorModel = documentInfoList[8];
            const QString colorDepth = documentInfoList[9];
            const QString profile = documentInfoList[10];
            useDocumentColorSpace = profile != "Default";

            document = KisPart::instance()->createDocument();
            document->setObjectName(name);

            KisPart::instance()->addDocument(document, false);
            const KoColorSpace *cs = KoColorSpaceRegistry::instance()->colorSpace(colorModel, colorDepth, profile);
            Q_ASSERT(cs);

            QColor qc(Qt::white);
            qc.setAlpha(0);
            KoColor bgColor(qc, cs);

            if (!document->newImage(name, width, height, cs, bgColor, KisConfig::RASTER_LAYER, 1, "", double(resolution / 72) )) {
                QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Krimble"), i18n("Failed to create new document. Animation import aborted."));
                return;
            }

            document->image()->animationInterface()->setFramerate(fps);
            document->image()->animationInterface()->setDocumentRangeStartFrame(0);
            document->image()->animationInterface()->setDocumentRangeEndFrame(totalFrames);

            this->showDocument(document);
        }

        KoUpdaterPtr updater =
                !document->fileBatchMode() ? viewManager()->createUnthreadedUpdater(i18n("Import frames")) : 0;
        KisAnimationImporter importer(document->image(), updater);
        KisImportExportErrorCode status = importer.import(renderedFrames.renderedFrameFiles, firstFrame, step, false, false, 0, useDocumentColorSpace, renderedFrames.renderedFrameTargetTimes);

        if (!status.isOk() && !status.isInternalError()) {
            QString msg = status.errorMessage();
            if (!msg.isEmpty())
                QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Krimble"), i18n("Could not finish import animation:\n%1", msg));
        }

        activeView()->canvasBase()->refetchDataFromImage();
        document->image()->refreshGraphAsync();
        document->image()->waitForDone();

    }
#endif
}

void KisMainWindow::renderAnimation()
{
    if (!activeView()) return;

    KisImageSP image = viewManager()->image();

    if (!image) return;
    if (!image->animationInterface()->hasAnimation()) return;

    KisDocument *doc = viewManager()->document();

    KisDlgAnimationRenderer dlgAnimationRenderer(doc, viewManager()->mainWindow());
    dlgAnimationRenderer.setCaption(i18n("Render Animation"));
    if (dlgAnimationRenderer.exec() == QDialog::Accepted) {
        KisAnimationRenderingOptions encoderOptions = dlgAnimationRenderer.getEncoderOptions();
        KisAnimationRender::render(doc, viewManager(), encoderOptions);
    }
}

void KisMainWindow::renderAnimationAgain()
{
    if (!activeView()) return;

    KisImageSP image = viewManager()->image();

    if (!image) return;
    if (!image->animationInterface()->hasAnimation()) return;

    KisDocument *doc = viewManager()->document();

    KisConfig cfg(true);

    KisPropertiesConfigurationSP settings = cfg.exportConfiguration("ANIMATION_EXPORT");

    KisAnimationRenderingOptions encoderOptions;
    encoderOptions.fromProperties(settings);

    KisAnimationRender::render(doc, viewManager(), encoderOptions);
}

void KisMainWindow::slotConfigureToolbars()
{
    saveWindowState();
    KisKEditToolBar edit(factory(), this);
    connect(&edit, SIGNAL(newToolBarConfig()), this, SLOT(slotNewToolbarConfig()));
    (void) edit.exec();
    applyToolBarLayout();
}

void KisMainWindow::slotResetConfigurations()
{
    KisApplication *kisApp = static_cast<KisApplication*>(qApp);
    kisApp->askResetConfig();
}

void KisMainWindow::slotNewToolbarConfig()
{
    applyMainWindowSettings(d->windowStateConfig);

    KisKXMLGUIFactory *factory = guiFactory();
    Q_UNUSED(factory);

    // Check if there's an active view
    if (!d->activeView)
        return;

    plugActionList("toolbarlist", d->toolbarList);

    /**
     * First we should apply icon overrides, and only after that we
     * should update the layout, since the function checks for the
     * presence of the icons in the action.
     */
    applyActionIconOverridesFromLocalXML();
    applyToolBarLayout();
}

void KisMainWindow::slotToolbarToggled(bool toggle)
{
    //dbgUI <<"KisMainWindow::slotToolbarToggled" << sender()->name() <<" toggle=" << true;
    // The action (sender) and the toolbar have the same name
    KisToolBar * bar = toolBar(sender()->objectName());
    if (bar) {
        if (toggle) {
            bar->show();
        }
        else {
            bar->hide();
        }

        if (d->activeView && d->activeView->document()) {
            saveWindowState();
        }
    } else
        warnUI << "slotToolbarToggled : Toolbar " << sender()->objectName() << " not found!";
}

void KisMainWindow::viewFullscreen(bool fullScreen)
{
#ifdef Q_OS_ANDROID
    // Full-screening on Android applies to the entire application, not this
    // window in particular. Qt tries to paper over that matter, but it causes
    // some pretty horrid flickering when the main window gets hidden. We just
    // talk to the Android interface directly to get around that.
    KisAndroidUtils::setFullScreen(fullScreen);
    d->fullScreenMode->setChecked(fullScreen);
#else
    KisConfig cfg(false);
    cfg.setFullscreenMode(fullScreen);

    if (fullScreen) {
        setWindowState(windowState() | Qt::WindowFullScreen);   // set
    } else {
        setWindowState(windowState() & ~Qt::WindowFullScreen);   // reset
    }
    d->fullScreenMode->setChecked(isFullScreen());
#endif
}

QDockWidget* KisMainWindow::createDockWidget(KoDockFactoryBase* factory)
{
    QDockWidget* dockWidget = 0;
    const bool showTitlebars = KisConfig(false).showDockerTitleBars();

    if (!d->dockWidgetsMap.contains(factory->id())) {
        dockWidget = factory->createDockWidget();
        KAcceleratorManager::setNoAccel(dockWidget);

        // It is quite possible that a dock factory cannot create the dock; don't
        // do anything in that case.
        if (!dockWidget) {
            warnKrita << "Could not create docker for" << factory->id();
            return 0;
        }

        KoDockWidgetTitleBar *titleBar = dynamic_cast<KoDockWidgetTitleBar*>(dockWidget->titleBarWidget());

        // Check if the dock widget is supposed to be collapsible
        if (!dockWidget->titleBarWidget()) {
            titleBar = new KoDockWidgetTitleBar(dockWidget);
            dockWidget->setTitleBarWidget(titleBar);
        }
        if (titleBar) {
            titleBar->setFont(KisUiFont::dockFont());
        }

        if (dockWidget->titleBarWidget() && !dockWidget->titleBarWidget()->inherits("KisUtilityTitleBar")) {
            dockWidget->titleBarWidget()->setVisible(KisConfig(true).showDockerTitleBars());
        }

        dockWidget->setObjectName(factory->id());
        dockWidget->setParent(this);
        // Krimble (2026-09-04, reverted): this previously called
        // dockWidget->setAllowedAreas(Qt::NoDockWidgetArea) to stop a
        // dragged panel from auto-snapping back into a dock zone. That
        // broke docking entirely instead -- addDockWidget() below
        // immediately places the widget into `side`, an area it had
        // just been told it wasn't allowed in at all, a direct
        // self-contradiction fed to Qt's dock-layout engine. This is a
        // strong candidate for both bug #7 (window snap broken --
        // dragged panels can't be dropped back into ANY area, since
        // zero areas are allowed) and bug #12 (docker panels rendering
        // as unmanageable narrow columns -- plausible symptom of Qt's
        // internal dock-layout width allocation misbehaving for a
        // widget that declares itself allowed nowhere). Removed;
        // QDockWidget defaults to Qt::AllDockWidgetAreas when this is
        // never called, restoring normal dock/undock/snap behavior.
        // // dockWidget->setAllowedAreas(Qt::NoDockWidgetArea);
        if (!showTitlebars) {
            if (dockWidget->titleBarWidget() && !dockWidget->titleBarWidget()->inherits("KisUtilityTitleBar")) {
                dockWidget->titleBarWidget()->setVisible(false);
            }
            dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
        }
        if (dockWidget->widget() && dockWidget->widget()->layout())
            dockWidget->widget()->layout()->setContentsMargins(1, 1, 1, 1);

        Qt::DockWidgetArea side = Qt::RightDockWidgetArea;
        bool visible = true;

        switch (factory->defaultDockPosition()) {
        case KoDockFactoryBase::DockTornOff:
            dockWidget->setFloating(true); // position nicely?
            break;
        case KoDockFactoryBase::DockTop:
            side = Qt::TopDockWidgetArea; break;
        case KoDockFactoryBase::DockLeft:
            side = Qt::LeftDockWidgetArea; break;
        case KoDockFactoryBase::DockBottom:
            side = Qt::BottomDockWidgetArea; break;
        case KoDockFactoryBase::DockRight:
            side = Qt::RightDockWidgetArea; break;
        case KoDockFactoryBase::DockMinimized:
        default:
            side = Qt::RightDockWidgetArea;
            visible = false;
        }

        KConfigGroup group = d->windowStateConfig.group("DockWidget " + factory->id());
        side = static_cast<Qt::DockWidgetArea>(group.readEntry("DockArea", static_cast<int>(side)));
        if (side == Qt::NoDockWidgetArea) side = Qt::RightDockWidgetArea;

        addDockWidget(side, dockWidget);
        if (!visible) {
            dockWidget->hide();
        }

        bool locked = group.readEntry("Locked", false);
        if (titleBar && locked) {
            titleBar->setLocked(true);
        }
        else if ( dockWidget->titleBarWidget()->inherits("KisUtilityTitleBar") && locked){
            KisUtilityTitleBar *utilityTitleBar = dynamic_cast<KisUtilityTitleBar*>(dockWidget->titleBarWidget());
            utilityTitleBar->setLocked(true);
        }

#ifdef Q_OS_ANDROID
        // KRIMBLE 2026-10-02: floating panels on a touch screen.
        // 1) They re-docked too easily: dragging a floating panel across a dock
        //    area snapped it back in. While a panel floats it is now allowed in
        //    no dock area, so it stays where it is dropped. It goes back into the
        //    layout only through its float button or Settings > Panels > Attach
        //    Panel; its allowed areas are restored as soon as it is docked again.
        //    (The allowed areas are changed only here, after the panel has been
        //    added to a dock area. Setting them before addDockWidget() was
        //    tried on 2026-09-04 and broke docking; see the note above.)
        // 2) They were hard to grab. While floating, the title bar gets extra
        //    padding top and bottom. This is done with the title bar's layout
        //    margins so the panel lays out correctly; a larger minimum height on
        //    the title bar made it overlap the panel's content.
        // 3) They were too big and could not be resized. A panel that floats is
        //    capped to a default size (60% x 75% of the screen's shorter side) and
        //    gets a resize handle in its lower right corner.
        dockWidget->installEventFilter(new KisDockedSizeTracker(dockWidget));
        // KRIMBLE 2026-10-04: the toolbox docks only close to a side of the window (see KrimbleDockTolerance).
        if (dockWidget->objectName() == QLatin1String("ToolBox")) {
            dockWidget->installEventFilter(new KrimbleDockTolerance(this, dockWidget));
        }
        connect(dockWidget, &QDockWidget::topLevelChanged, this, [this, dockWidget](bool floating) {
            // KRIMBLE 2026-10-04: Settings > "Attach Panels" decides what a floating panel does.
            // ON (the default) = the standard behaviour: a panel dragged to the edge of the window
            // docks there. OFF = floating panels never snap anywhere. Old line:
            // dockWidget->setAllowedAreas(floating ? Qt::NoDockWidgetArea : Qt::AllDockWidgetAreas);
            // KRIMBLE 2026-10-04 (2nd): the Attach/Detach features are switched off again, so panels
            // behave as they did before them: they may always dock, wherever they float. (The
            // "Attach Panels" menu item is hidden below; the lines below are kept for reference.)
            // const bool attachOn = property("krimbleAttachPanels").toBool();
            // dockWidget->setAllowedAreas((floating && !attachOn) ? Qt::NoDockWidgetArea : Qt::AllDockWidgetAreas);
            dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);

            // KRIMBLE 2026-10-02: a panel docked again with its own float button used to
            // fly back to its old place. Dock it where it was put instead, exactly as
            // Settings > Attach Panel does. (Attach Panel sets krimbleAttaching while it
            // runs, so it is not handled twice.)
            // KRIMBLE 2026-10-04: switched off. Detach and Attach together became unpredictable
            // (a panel could vanish), so the float button is back to the standard behaviour.
            if (false && !floating && dockWidget->isVisible() && !dockWidget->property("krimbleAttaching").toBool()) {
                const QVariant floatingCenter = dockWidget->property("krimbleFloatingCenter");
                if (floatingCenter.isValid()) {
                    krimbleAttachDockWhereItIs(this, dockWidget, floatingCenter.toPoint());
                }
            }

            if (QWidget *bar = dockWidget->titleBarWidget()) {
                if (QLayout *barLayout = bar->layout()) {
                    // KRIMBLE 2026-10-04: the extra title padding (was 14 on top and 14 at the bottom)
                    // made floating panels show big empty bands; removed.
                    // const int extra = 14;
                    const int extra = 0;
                    QMargins margins = barLayout->contentsMargins();
                    const bool padded = bar->property("krimbleTitlePadded").toBool();
                    if (floating && !padded) {
                        margins.setTop(margins.top() + extra);
                        margins.setBottom(margins.bottom() + extra);
                        barLayout->setContentsMargins(margins);
                        bar->setProperty("krimbleTitlePadded", true);
                    } else if (!floating && padded) {
                        margins.setTop(margins.top() - extra);
                        margins.setBottom(margins.bottom() - extra);
                        barLayout->setContentsMargins(margins);
                        bar->setProperty("krimbleTitlePadded", false);
                    }
                }
            }

            // KRIMBLE 2026-10-04: a floating panel may be made much smaller than its docked
            // minimum (Layers could not go below about 312 x 276). While floating, its content
            // gets a small minimum size and its layout no longer forces the bigger one; docked
            // again, both are put back.
            if (QWidget *content = dockWidget->widget()) {
                QLayout *contentLayout = content->layout();
                if (floating) {
                    if (!dockWidget->property("krimbleContentMin").isValid()) {
                        dockWidget->setProperty("krimbleContentMin", content->minimumSize());
                        dockWidget->setProperty("krimbleLayoutConstraint",
                                                contentLayout ? int(contentLayout->sizeConstraint()) : int(QLayout::SetDefaultConstraint));
                    }
                    if (contentLayout) {
                        contentLayout->setSizeConstraint(QLayout::SetNoConstraint);
                    }
                    // KRIMBLE 2026-10-07: the toolbox keeps its own small minimum (George: it must not get a minimum width)
                    if (dockWidget->objectName() != QLatin1String("ToolBox")) {
                        content->setMinimumSize(150, 120);
                    }
                    // content->setMinimumSize(150, 120);   // old line, for every floating panel
                } else if (dockWidget->property("krimbleContentMin").isValid()) {
                    content->setMinimumSize(dockWidget->property("krimbleContentMin").toSize());
                    if (contentLayout) {
                        contentLayout->setSizeConstraint(QLayout::SizeConstraint(dockWidget->property("krimbleLayoutConstraint").toInt()));
                    }
                    dockWidget->setProperty("krimbleContentMin", QVariant());
                }
            }

            // KRIMBLE 2026-10-02: the resize handle covered whatever sits in the
            // lower right corner of a panel (the Delete button of Layers). While a
            // panel floats, a strip as high as the handle is reserved under its
            // content, and the handle sits in that strip. Docked: strip removed.
            // const int gripStrip = 36;     // KRIMBLE 2026-10-04: smaller, matches the 26 px handle
            // const int gripStrip = 26;     // KRIMBLE 2026-10-04 (2nd pass): 20, matches the 20 px handle
            const int gripStrip = 20;
            const bool stripped = dockWidget->property("krimbleGripStrip").toBool();
            // KRIMBLE 2026-10-07: not for the toolbox (it has edge strips of its own, no corner gadget)
            const bool isToolBox = dockWidget->objectName() == QLatin1String("ToolBox");
            if (floating && !stripped && !isToolBox) {
                QMargins m = dockWidget->contentsMargins();
                m.setBottom(m.bottom() + gripStrip);
                dockWidget->setContentsMargins(m);
                dockWidget->setProperty("krimbleGripStrip", true);
            } else if (!floating && stripped) {
                QMargins m = dockWidget->contentsMargins();
                m.setBottom(qMax(0, m.bottom() - gripStrip));
                dockWidget->setContentsMargins(m);
                dockWidget->setProperty("krimbleGripStrip", false);
            }

            // KRIMBLE 2026-10-02: look the handle up by name. With the class no longer
            // derived from QSizeGrip (cfa2343), findChild<KisFloatingDockSizeGrip*>()
            // matched whichever widget was the first child of the panel, so no handle was
            // ever created and the panel's own widget was moved to the corner instead.
            // KisFloatingDockSizeGrip *grip = dockWidget->findChild<KisFloatingDockSizeGrip*>();
            KisFloatingDockSizeGrip *grip = static_cast<KisFloatingDockSizeGrip*>(
                dockWidget->findChild<QWidget*>(QStringLiteral("krimbleResizeHandle"), Qt::FindDirectChildrenOnly));
            if (floating && isToolBox) {
                // KRIMBLE 2026-10-07: the toolbox has no corner gadget: it keeps the size it had while docked and is
                // resized by the two edge strips in KoToolBoxDocker.
                const QSize dockedSize = dockWidget->property("krimbleDockedSize").toSize();
                if (dockedSize.isValid()) {
                    dockWidget->resize(dockedSize);
                }
                if (grip) {
                    grip->hide();
                }
            } else if (floating) {
                if (!grip) {
                    grip = new KisFloatingDockSizeGrip(dockWidget);
                    dockWidget->installEventFilter(new KisFloatingDockGripPlacer(dockWidget, grip));
                }
                // KRIMBLE 2026-10-02: the toolbox keeps the size it had while docked
                // (a narrow strip of tools); it is not capped to the default size.
                const QSize dockedSize = dockWidget->property("krimbleDockedSize").toSize();
                if (dockWidget->objectName() == QLatin1String("ToolBox")) {
                    if (dockedSize.isValid()) {
                        dockWidget->resize(dockedSize);
                    }
                } else if (QScreen *scr = QGuiApplication::primaryScreen()) {
                    const QSize screenSize = scr->availableGeometry().size();
                    const int shortSide = qMin(screenSize.width(), screenSize.height());
                    // KRIMBLE 2026-10-04: smaller default, was 60% x 75% of the screen's shorter side.
                    // dockWidget->resize(qMin(dockWidget->width(), int(shortSide * 0.6)),
                    //                    qMin(dockWidget->height(), int(shortSide * 0.75)));
                    dockWidget->resize(qMin(dockWidget->width(), int(shortSide * 0.5)),
                                       qMin(dockWidget->height(), int(shortSide * 0.5)));
                }
                grip->move(dockWidget->width() - grip->width(), dockWidget->height() - grip->height());
                grip->show();
                grip->raise();
            } else if (grip) {
                grip->hide();
            }
        });
#endif

        d->dockWidgetsMap.insert(factory->id(), dockWidget);
    }
    else {
        dockWidget = d->dockWidgetsMap[factory->id()];
    }

#ifdef Q_OS_MACOS
    dockWidget->setAttribute(Qt::WA_MacSmallSize, true);
#endif
    dockWidget->setFont(KisUiFont::dockFont());

    connect(dockWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(forceDockTabFonts()));

    return dockWidget;
}

void KisMainWindow::forceDockTabFonts()
{
    Q_FOREACH (QObject *child, children()) {
        if (child->inherits("QTabBar")) {
            ((QTabBar *)child)->setFont(KisUiFont::dockFont());
        }
    }
}

void KisMainWindow::slotUpdateWidgetStyle()
{
     KisConfig cfg(true);
     QString themeFromConfig = cfg.widgetStyle();

     Q_FOREACH (auto key, d->actionMap.keys()) { // find checked style to save to config
         if(d->actionMap.value(key)->isChecked()) {
            cfg.setWidgetStyle(key);
            qApp->setProperty(currentUnderlyingStyleNameProperty, key);
            qApp->setStyle(key);

            // When switching to a style that uses system colors, reset the theme
#ifndef Q_OS_HAIKU
            QString currentStyle = qApp->style()->objectName();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
            if (currentStyle == "macintosh" || currentStyle == "windowsvista") {
                d->themeManager->setCurrentTheme("System");
            }
#else
            if (currentStyle == "macos" || currentStyle == "windowsvista") {
                d->themeManager->setCurrentTheme("System");
            }
#endif
#endif
         }
     }
}

QList<QDockWidget*> KisMainWindow::dockWidgets() const
{
    return d->dockWidgetsMap.values();
}

QDockWidget* KisMainWindow::dockWidget(const QString &id)
{
    if (!d->dockWidgetsMap.contains(id)) return 0;
    return d->dockWidgetsMap[id];
}

QList<KoCanvasObserverBase*> KisMainWindow::canvasObservers() const
{
    QList<KoCanvasObserverBase*> observers;

    observers.append(static_cast<KoCanvasObserverBase*>(KisPart::instance()->playbackEngine()));

    Q_FOREACH (QDockWidget *docker, dockWidgets()) {
        KoCanvasObserverBase *observer = dynamic_cast<KoCanvasObserverBase*>(docker);
        if (observer) {
            observers << observer;
        }
        else {
            warnKrita << docker << "is not a canvas observer";
        }
    }

    return observers;
}


void KisMainWindow::toggleDockersVisibility(bool visible, bool onWelcomePage)
{
    if (!visible) {
        d->dockerStateBeforeHiding = saveState();

        Q_FOREACH (QObject* widget, children()) {
            if (widget->inherits("QDockWidget")) {
                QDockWidget* dw = static_cast<QDockWidget*>(widget);
                if (dw->isVisible() && !(onWelcomePage && dw->property("ShowOnWelcomePage").toBool())) {
                    dw->hide();
                }
            }
        }
    }
    else {
        restoreState(d->dockerStateBeforeHiding);
    }
}

void KisMainWindow::subWindowActivated()
{
    bool enabled = (activeKisView() != 0);

    d->mdiCascade->setEnabled(enabled);
    d->mdiNextWindow->setEnabled(enabled);
    d->mdiPreviousWindow->setEnabled(enabled);
    d->mdiTile->setEnabled(enabled);
    d->close->setEnabled(enabled);
    d->closeAll->setEnabled(enabled);

    setActiveSubWindow(d->mdiArea->activeSubWindow());

    /**
     * Qt has a weirdness, it has hardcoded shortcuts added to an action
     * in the window menu. We need to reset the shortcuts for that menu
     * to nothing, otherwise the shortcuts cannot be made configurable.
     *
     * See: https://bugs.kde.org/show_bug.cgi?id=352205
     *      https://bugs.kde.org/show_bug.cgi?id=375524
     *      https://bugs.kde.org/show_bug.cgi?id=398729
     */
    QMdiSubWindow *subWindow = d->mdiArea->currentSubWindow();
    if (subWindow) {
        QMenu *menu = subWindow->systemMenu();
        if (menu && menu->actions().size() == 8) {
            Q_FOREACH (QAction *action, menu->actions()) {
                action->setShortcut(QKeySequence());

            }
            menu->actions().last()->deleteLater();
        }
    }

    d->actionManager()->updateGUI();
}

void KisMainWindow::windowFocused()
{
    /**
     * Notify selection manager so that it could update selection mask overlay
     */
    if (viewManager() && viewManager()->selectionManager()) {
        viewManager()->selectionManager()->selectionChanged();
    }

    KisPart *kisPart = KisPart::instance();
    KisWindowLayoutManager *layoutManager = KisWindowLayoutManager::instance();
    if (!layoutManager->primaryWorkspaceFollowsFocus()) return;

    QUuid primary = layoutManager->primaryWindowId();
    if (primary.isNull()) return;

    if (d->id == primary) {
        if (!d->workspaceBorrowedBy.isNull()) {
            KisMainWindow *borrower = kisPart->windowById(d->workspaceBorrowedBy);
            if (!borrower) return;
            swapWorkspaces(this, borrower);
        }
    } else {
        if (d->workspaceBorrowedBy == primary) return;

        KisMainWindow *primaryWindow = kisPart->windowById(primary);
        if (!primaryWindow) return;
        swapWorkspaces(this, primaryWindow);
    }
}


void KisMainWindow::updateWindowMenu()
{
    QMenu *menu = d->windowMenu->menu();
    menu->clear();

    // Krimble 2026-10-11: two switches at the very top of the Window menu (George): tabs or floating windows, and showing the
    // picture while a floating window is dragged. They write the same settings the Preferences window does and apply at once.
    {
        KisConfig windowCfg(true);
        const bool tabbed = windowCfg.readEntry<int>("mdi_viewmode", (int)QMdiArea::SubWindowView) == (int)QMdiArea::TabbedView;
        // QAction *viewModeAction = menu->addAction(tabbed ? i18n("Switch to Windows") : i18n("Switch to Tabs"));
        QAction *viewModeAction = menu->addAction(tabbed ? i18n("Switch to Floating Windows") : i18n("Switch to Tabs"));   // Krimble 2026-10-11 (George)
        connect(viewModeAction, &QAction::triggered, this, [this, tabbed]() {
            KisConfig cfg(false);
            cfg.writeEntry<int>("mdi_viewmode", tabbed ? (int)QMdiArea::SubWindowView : (int)QMdiArea::TabbedView);
            KisConfigNotifier::instance()->notifyConfigChanged();
            if (tabbed) {
                // the pictures were maximized as tabs: lay them out as floating windows
                QTimer::singleShot(0, this, [this]() { d->mdiArea->cascadeSubWindows(); });
            }
        });
        QAction *showDragAction = menu->addAction(i18n("Show Contents While Dragging"));
        showDragAction->setCheckable(true);
        showDragAction->setChecked(windowCfg.readEntry<int>("mdi_rubberband", krimbleRubberBandDefault(windowCfg)) == 0);
        connect(showDragAction, &QAction::toggled, this, [](bool showContents) {
            KisConfig cfg(false);
            cfg.writeEntry<int>("mdi_rubberband", showContents ? 0 : 1);
            KisConfigNotifier::instance()->notifyConfigChanged();
        });
        menu->addSeparator();
    }

#ifndef Q_OS_ANDROID
    menu->addAction(d->newWindow);
#endif
    menu->addAction(d->documentMenu);

    QMenu *docMenu = d->documentMenu->menu();
    docMenu->clear();

    QFontMetrics fontMetrics = docMenu->fontMetrics();
    QRect geom = this->geometry();
    QPoint p(geom.width() / 2 + geom.left(), geom.height() / 2 + geom.top());
    QScreen *screen = qApp->screenAt(p);
    int fileStringWidth = 300;
    if (screen) {
        fileStringWidth = int(screen->availableGeometry().width() * .40f);
    }
    Q_FOREACH (QPointer<KisDocument> doc, KisPart::instance()->documents()) {
        if (doc) {
            QString title = fontMetrics.elidedText(doc->path(), Qt::ElideMiddle, fileStringWidth);
            if (title.isEmpty() && doc->image()) {
                title = doc->image()->objectName();
            }
            QAction *action = docMenu->addAction(title);
            action->setIcon(qApp->windowIcon());
            connect(action, SIGNAL(triggered()), d->documentMapper, SLOT(map()));
            d->documentMapper->setMapping(action, doc);
        }
    }

    menu->addAction(d->workspaceMenu);
    QMenu *workspaceMenu = d->workspaceMenu->menu();
    workspaceMenu->clear();
    KisResourceModel resourceModel(ResourceType::Workspaces);
    KisResourceIterator resourceIterator(&resourceModel);
    KisMainWindow *m_this = this;
   
    while (resourceIterator.hasNext()) {
        KisResourceItemSP resource = resourceIterator.next();
        QAction *action = workspaceMenu->addAction(resource->name());
        action->setProperty("md5", QVariant::fromValue<QString>(resource->md5sum()));
        connect(action, SIGNAL(triggered()), this, SLOT(restoreWorkspace()));
    }
    workspaceMenu->addSeparator();
    connect(workspaceMenu->addAction(i18nc("@action:inmenu", "&Import Workspace...")),
            &QAction::triggered,
            this,
            [&]()
    {
        QStringList mimeTypes = KisResourceLoaderRegistry::instance()->mimeTypes(ResourceType::Workspaces);

        KoFileDialog dialog(0, KoFileDialog::OpenFile, "OpenDocument");
        dialog.setMimeTypeFilters(mimeTypes);
        dialog.setCaption(i18nc("@title:window", "Choose File to Add"));
        QString filename = dialog.filename();

        KisResourceUserOperations::importResourceFileWithUserInput(this, "", ResourceType::Workspaces, filename);
    });

    connect(workspaceMenu->addAction(i18nc("@action:inmenu", "&New Workspace...")),
            &QAction::triggered,
            [=]() {
        QString name;
        name = QInputDialog::getText(this, i18nc("@title:window", "New Workspace..."),
                                                                i18nc("@label:textbox", "Name:"));
        if (name.isEmpty()) {
            return;
        }

        KisWorkspaceResourceSP workspace(new KisWorkspaceResource(""));
        workspace->setDockerState(m_this->saveState());
        workspace->setImage(layoutThumbnail());
        workspace->setValid(true);

        // this line must happen before we save the workspace to resource folder or other places
        // because it mostly just triggers palettes to be saved into the workspace
        d->viewManager->canvasResourceProvider()->notifySavingWorkspace(workspace);
        workspace->setValid(true);

        workspace->setFilename(name.replace(" ", "_") + workspace->defaultFileExtension());
        workspace->setName(name);

        KisResourceUserOperations::addResourceWithUserInput(this, workspace);
    });

    // TODO: What to do about delete?
    //    workspaceMenu->addAction(i18nc("@action:inmenu", "&Delete Workspace..."));

    menu->addSeparator();
    menu->addAction(d->close);
    menu->addAction(d->closeAll);
    if (d->mdiArea->viewMode() == QMdiArea::SubWindowView) {
        menu->addSeparator();
        menu->addAction(d->mdiTile);
        menu->addAction(d->mdiCascade);
    }
    menu->addSeparator();
    menu->addAction(d->mdiNextWindow);
    menu->addAction(d->mdiPreviousWindow);
    menu->addSeparator();

    QList<QMdiSubWindow *> windows = d->mdiArea->subWindowList();
    for (int i = 0; i < windows.size(); ++i) {
        QPointer<KisView>child = qobject_cast<KisView*>(windows.at(i)->widget());
        if (child && child->document()) {
            QString text;
            if (i < 9) {
                text = i18n("&%1 %2", i + 1, fontMetrics.elidedText(child->document()->path(), Qt::ElideMiddle, fileStringWidth));
            }
            else {
                text = i18n("%1 %2", i + 1, fontMetrics.elidedText(child->document()->path(), Qt::ElideMiddle, fileStringWidth));
            }

            QAction *action  = menu->addAction(text);
            action->setIcon(qApp->windowIcon());
            action->setCheckable(true);
            action->setChecked(child == activeKisView());
            connect(action, SIGNAL(triggered()), d->windowMapper, SLOT(map()));
            d->windowMapper->setMapping(action, windows.at(i));
        }
    }

    // Krimble 2026-10-10 (3rd): Panels now sits with the panel items at the end of the menu (below).
    // menu->addSeparator();
    // menu->addAction(d->dockWidgetMenu);

    // Krimble 2026-10-10 (2nd): these items keep the order and separators they had in the View menu; Panels sits with the panel items.
    {
        auto addActionByName = [this, menu](const char *name) {
            if (QAction *action = actionCollection()->action(QLatin1String(name))) {
                menu->addAction(action);
            }
        };
        // Krimble 2026-10-11: Themes, Styles and Language moved to the end of the menu (George); they were here:
        // menu->addSeparator();
        // addActionByName("theme_menu");
        // addActionByName("style_menu");
        // addActionByName("switch_application_language");
        menu->addSeparator();
        QMenu *toolbarsMenu = menu->addMenu(i18nc("@action:inmenu", "&Toolbars"));
        connect(toolbarsMenu, &QMenu::aboutToShow, this, [this, toolbarsMenu]() {
            toolbarsMenu->clear();
            Q_FOREACH (KisToolBar *toolBar, toolBars()) {
                const QString title = toolBar->windowTitle().isEmpty() ? toolBar->objectName() : toolBar->windowTitle();
                QAction *toggle = toolbarsMenu->addAction(title);
                toggle->setCheckable(true);
                toggle->setChecked(toolBar->isVisible());
                connect(toggle, &QAction::toggled, toolBar, &QWidget::setVisible);
            }
            // Krimble 2026-10-11: Reset Toolbars (George): every toolbar goes back to the top, horizontal and visible, as the
            // menu file (krita5.xmlgui) defines them.
            toolbarsMenu->addSeparator();
            // Krimble 2026-10-12: Close All Toolbars (George: "add a Close All item to the toolbar submenu")
            QAction *closeAllToolbars = toolbarsMenu->addAction(i18n("Close All Toolbars"));
            connect(closeAllToolbars, &QAction::triggered, this, [this]() {
                Q_FOREACH (KisToolBar *toolBar, toolBars()) {
                    toolBar->hide();
                }
            });
            QAction *resetToolbars = toolbarsMenu->addAction(i18n("Reset Toolbars"));
            connect(resetToolbars, &QAction::triggered, this, [this]() {
                Q_FOREACH (KisToolBar *toolBar, toolBars()) {
                    toolBar->setFloatable(true);
                    // if (toolBar->isFloating()) toolBar->setFloating(false);   // not floating (KisToolBar has no setFloating; removeToolBar and addToolBar below dock it again)
                    removeToolBar(toolBar);
                    addToolBar(Qt::TopToolBarArea, toolBar);
                    toolBar->setOrientation(Qt::Horizontal);
                    toolBar->show();
                }
                // Krimble 2026-10-12: and one on each row
                QTimer::singleShot(0, this, [this]() { krimbleStackToolBars(this); });
            });
        });
        addActionByName("lock_toolbars");
        menu->addSeparator();
        menu->addAction(d->dockWidgetMenu);
        addActionByName("view_toggledockers");
        addActionByName("settings_detach_panel_menu");
        addActionByName("settings_attach_panel_menu");
        addActionByName("settings_toggle_right_panels");
        // Krimble 2026-10-11: Hide All Panels (and Show All Panels while they are hidden), next to Hide Right Panels (George).
        QAction *hideAllPanels = menu->addAction(d->toggleDockers->isChecked() ? i18n("Hide All Panels") : i18n("Show All Panels"));
        connect(hideAllPanels, &QAction::triggered, this, [this]() {
            d->toggleDockers->setChecked(!d->toggleDockers->isChecked());
        });
        // Krimble 2026-10-12: Mirror Layout (Left-Handed) (Claude's idea): checked while the toolbox is on the right
        // Krimble 2026-10-12: Mirror Layout removed from the menu (George): it corrupted the display in 1.0.33. The code is kept.
        // QAction *mirrorLayout = menu->addAction(i18n("Mirror Layout (Left-Handed)"));
        // mirrorLayout->setCheckable(true);
        // if (QDockWidget *toolBoxDock = findChild<QDockWidget*>(QStringLiteral("ToolBox"))) {
        //     mirrorLayout->setChecked(!toolBoxDock->isFloating() && dockWidgetArea(toolBoxDock) == Qt::RightDockWidgetArea);
        // }
        // connect(mirrorLayout, &QAction::triggered, this, [this]() { krimbleMirrorLayout(this); });
        // Krimble 2026-10-12: Quick Adjust (Claude's idea)
        QAction *quickAdjust = menu->addAction(i18n("Quick Adjust"));
        connect(quickAdjust, &QAction::triggered, this, [this]() { krimbleToggleQuickAdjust(this); });
        // Krimble 2026-10-12: Layout Presets (Claude's idea)
        QMenu *presets = menu->addMenu(i18n("Layout Presets"));
        struct PresetDef { const char *label; QStringList panels; };
        const QList<PresetDef> presetList = {
            { "Quick Edit",   { QStringLiteral("Layers"), QStringLiteral("Tool Options") } },
            { "Retouch",      { QStringLiteral("Layers"), QStringLiteral("Tool Options"), QStringLiteral("Color"), QStringLiteral("Overview") } },
            { "Illustration", { QStringLiteral("Layers"), QStringLiteral("Color"), QStringLiteral("Brushes"), QStringLiteral("Tool Options") } },
            { "Design",       { QStringLiteral("Layers"), QStringLiteral("Tool Options"), QStringLiteral("Undo History") } },
            { "Focus (toolbox only)", { } }
        };
        Q_FOREACH (const PresetDef &def, presetList) {
            QAction *presetAction = presets->addAction(i18n(def.label));
            const QStringList panels = def.panels;
            connect(presetAction, &QAction::triggered, this, [this, panels]() { krimbleApplyPanelPreset(this, panels); });
        }
        // Krimble 2026-10-12: Startup layouts (George's idea, Claude's design)
        QAction *saveLayout = menu->addAction(i18n("Save Layout as Startup"));
        connect(saveLayout, &QAction::triggered, this, [this]() {
            krimbleSaveLayout(this);
            QMessageBox::information(this, i18n("Startup layout"),
                krimbleIsLandscape(this) ? i18n("The landscape layout is saved. It will be used at start and whenever the screen is turned to landscape.")
                                         : i18n("The portrait layout is saved. It will be used at start and whenever the screen is turned to portrait."));
        });
        QAction *resetLayout = menu->addAction(i18n("Reset Layout"));
        connect(resetLayout, &QAction::triggered, this, [this]() { krimbleResetLayout(this); });
        menu->addSeparator();
        addActionByName("theme_menu");
        addActionByName("style_menu");
        addActionByName("switch_application_language");
    }

    // Krimble 2026-10-10: the first version of the block above (kept, not deleted).
#if 0
    // Krimble 2026-10-10: panel, toolbar and appearance controls live in the Window menu, not the View menu (George).
    {
        menu->addSeparator();
        const char *panelActions[] = {"view_toggledockers", "settings_detach_panel_menu", "settings_attach_panel_menu", "settings_toggle_right_panels"};
        for (const char *name : panelActions) {
            if (QAction *action = actionCollection()->action(QLatin1String(name))) {
                menu->addAction(action);
            }
        }
        menu->addSeparator();
        // The View menu got its toolbar list from a KDE merge point; the same list is built here, fresh each time it opens.
        QMenu *toolbarsMenu = menu->addMenu(i18nc("@action:inmenu", "&Toolbars"));
        connect(toolbarsMenu, &QMenu::aboutToShow, this, [this, toolbarsMenu]() {
            toolbarsMenu->clear();
            // Q_FOREACH (KToolBar *toolBar, toolBars()) {
            // (KisToolBar: this program's own toolbar class. The KDE name KToolBar does not exist here.)
            Q_FOREACH (KisToolBar *toolBar, toolBars()) {
                const QString title = toolBar->windowTitle().isEmpty() ? toolBar->objectName() : toolBar->windowTitle();
                QAction *toggle = toolbarsMenu->addAction(title);
                toggle->setCheckable(true);
                toggle->setChecked(toolBar->isVisible());
                connect(toggle, &QAction::toggled, toolBar, &QWidget::setVisible);
            }
        });
        if (QAction *lockToolbars = actionCollection()->action(QLatin1String("lock_toolbars"))) {
            menu->addAction(lockToolbars);
        }
        menu->addSeparator();
        const char *appearanceActions[] = {"theme_menu", "style_menu"};
        for (const char *name : appearanceActions) {
            if (QAction *action = actionCollection()->action(QLatin1String(name))) {
                menu->addAction(action);
            }
        }
    }
#endif

    // KRIMBLE 2026-10-06 (2nd): George: "If Window is constructed on the fly, use View menu instead." The former Settings
    // items above Presets are now in the View menu (krita5.xmlgui), so the block below is switched off (kept, not deleted).
#if 0
    // KRIMBLE 2026-10-06: the Settings menu is gone (George: "Move everything below Presets in the Settings menu to
    // the Help menu. Everything above that move to Window menu. Then remove Settings menu."). What was above
    // "Presets..." now ends the Window menu. ("Panels" is the entry added just above; the rest of the old Settings
    // entries below Presets are in the Help menu, see krita5.xmlgui.) The texts are the ones the Settings menu showed.
    {
        auto addNamed = [this, menu](const char *name, const QString &text = QString()) {
            if (QAction *action = actionCollection()->action(QLatin1String(name))) {
                if (!text.isEmpty()) {
                    action->setText(text);
                }
                menu->addAction(action);
            }
        };
        menu->addSeparator();
        addNamed("change_interface_scale", i18nc("@action:inmenu", "Interface Scale..."));
        menu->addSeparator();
        addNamed("theme_menu", i18nc("@action:inmenu", "&Themes"));
        addNamed("style_menu", i18nc("@action:inmenu", "&Styles"));
        addNamed("switch_application_language", i18nc("@action:inmenu", "Language..."));
        menu->addSeparator();
        addNamed("options_configure_toolbars", i18nc("@action:inmenu", "Customize Toolbar..."));
        // The Settings menu had a merge point for the automatic list of toolbars; the same list is built here.
        QMenu *toolbarsMenu = menu->addMenu(i18nc("@action:inmenu", "&Toolbars"));
        Q_FOREACH (KToolBar *toolBar, toolBars()) {
            const QString title = toolBar->windowTitle().isEmpty() ? toolBar->objectName() : toolBar->windowTitle();
            QAction *toggle = toolbarsMenu->addAction(title);
            toggle->setCheckable(true);
            toggle->setChecked(toolBar->isVisible());
            connect(toggle, &QAction::toggled, toolBar, &QWidget::setVisible);
        }
        addNamed("lock_toolbars", i18nc("@action:inmenu", "Lock Toolbars"));
        menu->addSeparator();
        addNamed("view_toggledockers", i18nc("@action:inmenu", "Toggle &Panels"));
        addNamed("settings_detach_panel_menu");
        addNamed("settings_attach_panel_menu");
        addNamed("settings_toggle_right_panels");
    }
#endif

    bool showMdiArea = windows.count( ) > 0;
    if (!showMdiArea) {
        showWelcomeScreen(true); // see workaround in function in header
    }

    // enable/disable the toolbox docker if there are no documents open
    Q_FOREACH (QObject* widget, children()) {
        if (widget->inherits("QDockWidget")) {
            QDockWidget* dw = static_cast<QDockWidget*>(widget);

            if ( dw->objectName() == "ToolBox") {
                dw->setEnabled(showMdiArea);
            }
        }
    }
}

void KisMainWindow::updateSubwindowFlags()
{
    bool onlyOne = false;
    if (d->mdiArea->subWindowList().size() == 1 && d->mdiArea->viewMode() == QMdiArea::SubWindowView) {
        onlyOne = true;
    }
    Q_FOREACH (QMdiSubWindow *subwin, d->mdiArea->subWindowList()) {
        if (onlyOne) {
            subwin->setWindowFlags(subwin->windowFlags() | Qt::FramelessWindowHint);
            subwin->showMaximized();
        } else {
            subwin->setWindowFlags((subwin->windowFlags() | Qt::FramelessWindowHint) ^ Qt::FramelessWindowHint);
        }
    }
}

void KisMainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window) {
        unsetActiveView();
        return;
    }
    QMdiSubWindow *subwin = qobject_cast<QMdiSubWindow *>(window);
    //dbgKrita << "setActiveSubWindow();" << subwin << d->activeSubWindow;

    if (subwin && subwin != d->activeSubWindow) {
        KisView *view = qobject_cast<KisView *>(subwin->widget());
        //dbgKrita << "\t" << view << activeView();
        if (view && view != activeView()) {
            setActiveView(view);
        }
        d->activeSubWindow = subwin;
    }
    updateWindowMenu();
    d->actionManager()->updateGUI();
}

void KisMainWindow::configChanged()
{
    KisConfig cfg(true);
    // KRIMBLE 2026-10-04: Subwindows (floating pictures) were tried as the default and went back to tabs the same
    // day ("the sub Windows are a little wonky"). The Subwindows line is kept for later.
    // QMdiArea::ViewMode viewMode = (QMdiArea::ViewMode)cfg.readEntry<int>("mdi_viewmode", (int)QMdiArea::SubWindowView);
    // QMdiArea::ViewMode viewMode = (QMdiArea::ViewMode)cfg.readEntry<int>("mdi_viewmode", (int)QMdiArea::TabbedView);
    // Krimble 2026-10-11: floating windows are the default again, so people know they are there. The Window menu has a
    // one tap switch to tabs (and back).
    QMdiArea::ViewMode viewMode = (QMdiArea::ViewMode)cfg.readEntry<int>("mdi_viewmode", (int)QMdiArea::SubWindowView);
    d->mdiArea->setViewMode(viewMode);
    Q_FOREACH (QMdiSubWindow *subwin, d->mdiArea->subWindowList()) {
        // subwin->setOption(QMdiSubWindow::RubberBandMove, cfg.readEntry<int>("mdi_rubberband", cfg.useOpenGL()));
        subwin->setOption(QMdiSubWindow::RubberBandMove, cfg.readEntry<int>("mdi_rubberband", krimbleRubberBandDefault(cfg)));
        // subwin->setOption(QMdiSubWindow::RubberBandResize, cfg.readEntry<int>("mdi_rubberband", cfg.useOpenGL()));
        subwin->setOption(QMdiSubWindow::RubberBandResize, cfg.readEntry<int>("mdi_rubberband", krimbleRubberBandDefault(cfg)));
        if (viewMode == QMdiArea::TabbedView) {
            subwin->setWindowState(Qt::WindowMaximized);
        }

        /**
         * Dirty workaround for a bug in Qt (checked on Qt 5.6.1):
         *
         * If you make a window "Show on top" and then switch to the tabbed mode
         * the window will continue to be painted in its initial "mid-screen"
         * position. It will persist here until you explicitly switch to its tab.
         */
        if (viewMode == QMdiArea::TabbedView) {
            Qt::WindowFlags oldFlags = subwin->windowFlags();
            Qt::WindowFlags flags = oldFlags;

            flags &= ~Qt::WindowStaysOnTopHint;
            flags &= ~Qt::WindowStaysOnBottomHint;

            if (flags != oldFlags) {
                subwin->setWindowFlags(flags);
                subwin->showMaximized();
            }
        }
    }
#ifdef Q_OS_MACOS
    updateSubwindowFlags();
#endif

    KConfigGroup group( KSharedConfig::openConfig(), "theme");
#ifndef Q_OS_HAIKU
    d->themeManager->setCurrentTheme(group.readEntry("Theme", "Krimble dark"));
#endif
    d->actionManager()->updateGUI();

    QString s = cfg.getMDIBackgroundColor();
    KoColor c = KoColor::fromXML(s);
    QBrush brush(c.toQColor());
    d->mdiArea->setBackground(brush);

    QString backgroundImage = cfg.getMDIBackgroundImage();
    if (backgroundImage != "") {
        QImage image(backgroundImage);
        QBrush brush(image);
        d->mdiArea->setBackground(brush);
    }

    d->mdiArea->update();

    qApp->setFont(KisUiFont::normalFont());

    Q_FOREACH (QObject* widget, children()) {
        if (widget->inherits("QDockWidget")) {
            QDockWidget* dw = static_cast<QDockWidget*>(widget);
            dw->setFont(KisUiFont::dockFont());
        }
    }
}

KisView* KisMainWindow::newView(QObject *document, QMdiSubWindow *subWindow)
{
    KisDocument *doc = qobject_cast<KisDocument*>(document);
    KisView *view = addViewAndNotifyLoadingCompleted(doc, subWindow);
    d->actionManager()->updateGUI();

    return view;
}

void KisMainWindow::newWindow()
{
#ifdef Q_OS_ANDROID
    // Check if current mainwindow exists, just to be sure.
    if (KisPart::instance()->currentMainwindow()) {
        QMessageBox::warning(this, i18nc("@title:window", "Krimble"),
                             "Creating a New Main Window is unsupported on Android");
        return;
    }
#endif
    KisMainWindow *mainWindow = KisPart::instance()->createMainWindow();
    mainWindow->initializeGeometry();
    mainWindow->show();
}

void KisMainWindow::closeCurrentWindow()
{
    if (d->mdiArea->currentSubWindow()) {
        d->mdiArea->currentSubWindow()->close();
        d->actionManager()->updateGUI();
    }
}

void KisMainWindow::checkSanity()
{
    // print error if the lcms engine is not available
    if (!KoColorSpaceEngineRegistry::instance()->contains("icc")) {
        // need to wait 1 event since exiting here would not work.
        m_errorMessage = i18n("The Krimble LittleCMS color management plugin is not installed. Krimble will quit now.");
        m_dieOnError = true;
        QTimer::singleShot(0, this, SLOT(showErrorAndDie()));
        return;
    }

    slotStoragesWarning();

    // window is created signal (used in Python)
    // there must be some asynchronous things happening in the constructor, because the window cannot
    // be referenced until after this timeout is done
    Q_EMIT KisPart::instance()->sigMainWindowCreated();
}

void KisMainWindow::showErrorAndDie()
{
    QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Installation error"), m_errorMessage);
    if (m_dieOnError) {
        exit(10);
    }
}

void KisMainWindow::showAboutApplication()
{
    KisAboutApplication dlg(this);
    dlg.exec();
}

void KisMainWindow::slotVisitFeatureRequests()
{
    QDesktopServices::openUrl(QUrl("https://forum.krimble.org/t/feature-requests"));
}

void KisMainWindow::slotVisitKrimbleWebsite()
{
    QDesktopServices::openUrl(QUrl("https://krimble.org"));
}

void KisMainWindow::slotVisitKrimbleForum()
{
    QDesktopServices::openUrl(QUrl("https://forum.krimble.org"));
}

QPointer<KisView> KisMainWindow::activeKisView()
{
    if (!d->mdiArea) return 0;
    QMdiSubWindow *activeSubWindow = d->mdiArea->activeSubWindow();
    //dbgKrita << "activeKisView" << activeSubWindow;
    if (!activeSubWindow) return 0;
    return qobject_cast<KisView*>(activeSubWindow->widget());
}

void KisMainWindow::newOptionWidgets(KoCanvasController *controller, const QList<QPointer<QWidget> > &optionWidgetList)
{
    KIS_ASSERT_RECOVER_NOOP(controller == KoToolManager::instance()->activeCanvasController());
    bool isOurOwnView = false;

    Q_FOREACH (QPointer<KisView> view, KisPart::instance()->views()) {
        if (view && view->canvasController() == controller) {
            isOurOwnView = view->mainWindow() == this;
        }
    }

    if (!isOurOwnView) return;

    Q_FOREACH (QWidget *w, optionWidgetList) {
#ifdef Q_OS_MACOS
        w->setAttribute(Qt::WA_MacSmallSize, true);
#endif
        w->setFont(KisUiFont::dockFont());
    }

    if (d->toolOptionsDocker) {
        d->toolOptionsDocker->setOptionWidgets(optionWidgetList);
    }
    else {
        d->viewManager->paintOpBox()->newOptionWidgets(optionWidgetList);
    }
}

void KisMainWindow::createActions()
{
    KisActionManager *actionManager = d->actionManager();



    actionManager->createStandardAction(KStandardAction::New, this, SLOT(slotFileNew()));
    actionManager->createStandardAction(KStandardAction::Open, this, SLOT(slotFileOpen()));
    actionManager->createStandardAction(KStandardAction::Quit, this, SLOT(slotFileQuit()));
    actionManager->createStandardAction(KStandardAction::ConfigureToolbars, this, SLOT(slotConfigureToolbars()));
    d->fullScreenMode = actionManager->createStandardAction(KStandardAction::FullScreen, this, SLOT(viewFullscreen(bool)));

    d->recentFiles = KStandardAction::openRecent(this, SLOT(slotFileOpenRecent(QUrl)), actionCollection());

    d->saveAction = actionManager->createStandardAction(KStandardAction::Save, this, SLOT(slotFileSave()));
    d->saveAction->setActivationFlags(KisAction::IMAGE_IS_WRITABLE);

    d->saveActionAs = actionManager->createStandardAction(KStandardAction::SaveAs, this, SLOT(slotFileSaveAs()));
    d->saveActionAs->setActivationFlags(KisAction::ACTIVE_IMAGE);

    d->undo = actionManager->createStandardAction(KStandardAction::Undo, this, SLOT(undo()));
    d->undo->setActivationFlags(KisAction::ACTIVE_IMAGE);

    d->redo = actionManager->createStandardAction(KStandardAction::Redo, this, SLOT(redo()));
    d->redo->setActivationFlags(KisAction::ACTIVE_IMAGE);

    d->undoActionsUpdateManager.reset(new KisUndoActionsUpdateManager(d->undo, d->redo));
    d->undoActionsUpdateManager->setCurrentDocument(d->activeView ? d->activeView->document() : 0);

    d->importAnimation  = actionManager->createAction("file_import_animation");
    connect(d->importAnimation, SIGNAL(triggered()), this, SLOT(importAnimation()));

    // Importing video requires ffmpeg, which is not available on Android.
#ifndef Q_OS_ANDROID
    d->importVideoAnimation = actionManager->createAction("file_import_video_animation");
    connect(d->importVideoAnimation, SIGNAL(triggered()), this, SLOT(importVideoAnimation()));
#endif

    d->renderAnimation = actionManager->createAction("render_animation");
    d->renderAnimation->setActivationFlags(KisAction::IMAGE_HAS_ANIMATION);
    connect( d->renderAnimation, SIGNAL(triggered()), this, SLOT(renderAnimation()));

    d->renderAnimationAgain = actionManager->createAction("render_animation_again");
    d->renderAnimationAgain->setActivationFlags(KisAction::IMAGE_HAS_ANIMATION);
    connect( d->renderAnimationAgain, SIGNAL(triggered()), this, SLOT(renderAnimationAgain()));

    d->closeAll = actionManager->createAction("file_close_all");
    connect(d->closeAll, SIGNAL(triggered()), this, SLOT(slotFileCloseAll()));

    d->visitFeatureRequests = actionManager->createAction("help_feature_request");
    connect(d->visitFeatureRequests, SIGNAL(triggered()), this, SLOT(slotVisitFeatureRequests()));

    d->visitKrimbleWebsite = actionManager->createAction("help_krimble_website");
    connect(d->visitKrimbleWebsite, SIGNAL(triggered()), this, SLOT(slotVisitKrimbleWebsite()));

    d->visitKrimbleForum = actionManager->createAction("help_krimble_forum");
    connect(d->visitKrimbleForum, SIGNAL(triggered()), this, SLOT(slotVisitKrimbleForum()));

    d->importFile  = actionManager->createAction("file_import_file");
    d->importFile->setActivationFlags(KisAction::IMAGE_IS_WRITABLE);
    connect(d->importFile, SIGNAL(triggered(bool)), this, SLOT(slotImportFile()));

    d->exportFile  = actionManager->createAction("file_export_file");
    connect(d->exportFile, SIGNAL(triggered(bool)), this, SLOT(slotExportFile()));

    d->exportFileAdvance  = actionManager->createAction("file_export_advanced");
    connect(d->exportFileAdvance, SIGNAL(triggered(bool)), this, SLOT(slotExportAdvance()));

    /* The following entry opens the document information dialog.  Since the action is named so it
        intends to show data this entry should not have a trailing ellipses (...).  */
    d->showDocumentInfo  = actionManager->createAction("file_documentinfo");
    connect(d->showDocumentInfo, SIGNAL(triggered(bool)), this, SLOT(slotDocumentInfo()));
#ifndef Q_OS_HAIKU
    d->themeManager->setThemeMenuAction(new KActionMenu(i18nc("@action:inmenu", "&Themes"), this));
    d->themeManager->registerThemeActions(actionCollection());
    connect(d->themeManager, SIGNAL(signalThemeChanged()), this, SLOT(slotThemeChanged()), Qt::QueuedConnection);
    connect(this, SIGNAL(themeChanged()), d->welcomePage, SLOT(slotUpdateThemeColors()), Qt::UniqueConnection);
#endif
    d->toggleDockers = actionManager->createAction("view_toggledockers");


    KisConfig(true).showDockers(true);
    d->toggleDockers->setChecked(true);
    connect(d->toggleDockers, SIGNAL(toggled(bool)), SLOT(toggleDockersVisibility(bool)));

    d->resetConfigurations  = actionManager->createAction("reset_configurations");
    connect(d->resetConfigurations, SIGNAL(triggered()), this, SLOT(slotResetConfigurations()));

#ifndef Q_OS_ANDROID
    d->toggleDetachCanvas = actionManager->createAction("view_detached_canvas");
    d->toggleDetachCanvas->setChecked(false);
    connect(d->toggleDetachCanvas, SIGNAL(toggled(bool)), SLOT(setCanvasDetached(bool)));
#endif
    setCanvasDetached(false);

    d->toggleDockerTitleBars = actionManager->createAction("view_toggledockertitlebars");
    d->toggleDockerTitleBars->setChecked(KisConfig(false).showDockerTitleBars());
    connect(d->toggleDockerTitleBars, SIGNAL(toggled(bool)), SLOT(showDockerTitleBars(bool)));

    actionCollection()->addAction("settings_dockers_menu", d->dockWidgetMenu);
    actionCollection()->addAction("window", d->windowMenu);

    actionCollection()->addAction("style_menu", d->styleMenu); // for widget styles: breeze, fusion, etc

    d->mdiCascade = actionManager->createAction("windows_cascade");
    connect(d->mdiCascade, SIGNAL(triggered()), d->mdiArea, SLOT(cascadeSubWindows()));

    d->mdiTile = actionManager->createAction("windows_tile");
    connect(d->mdiTile, SIGNAL(triggered()), d->mdiArea, SLOT(tileSubWindows()));

    d->mdiNextWindow = actionManager->createAction("windows_next");
    connect(d->mdiNextWindow, SIGNAL(triggered()), d->mdiArea, SLOT(activateNextSubWindow()));

    d->mdiPreviousWindow = actionManager->createAction("windows_previous");
    connect(d->mdiPreviousWindow, SIGNAL(triggered()), d->mdiArea, SLOT(activatePreviousSubWindow()));

#ifdef Q_OS_ANDROID
    d->showDonationManagementDialog = actionManager->createAction("manage_donations");
    connect(d->showDonationManagementDialog,
            &QAction::triggered,
            this,
            &KisMainWindow::slotShowDonationManagementDialog);

    KisAndroidDonations *androidDonations = KisAndroidDonations::instance();
    if (androidDonations) {
        d->manageSubscriptions = actionManager->createAction("manage_subscriptions");
        connect(d->manageSubscriptions,
                &QAction::triggered,
                androidDonations,
                &KisAndroidDonations::slotManageSubscriptions);
        connect(androidDonations,
                &KisAndroidDonations::sigShowDonationManagementDialogRequested,
                this,
                &KisMainWindow::slotShowDonationManagementDialog,
                Qt::QueuedConnection);
    }
#if KRITA_QT_HAS_ANDROID_QPLATFORMSCREEN_DENSITY_ADJUSTMENT
    KisAndroidScaling *androidScaling = KisAndroidScaling::instance();
    if (androidScaling && androidScaling->isSupported()) {
        d->changeInterfaceScale = actionManager->createAction("change_interface_scale");
        connect(d->changeInterfaceScale, &QAction::triggered, this, &KisMainWindow::slotChangeInterfaceScale);
    }
#endif
#else
    d->newWindow = actionManager->createAction("view_newwindow");
    connect(d->newWindow, SIGNAL(triggered(bool)), this, SLOT(newWindow()));
#endif

    d->close = actionManager->createStandardAction(KStandardAction::Close, this, SLOT(closeCurrentWindow()));

    d->showSessionManager = actionManager->createAction("file_sessions");
    connect(d->showSessionManager, SIGNAL(triggered(bool)), this, SLOT(slotShowSessionManager()));

    d->commandBarAction = actionManager->createAction("command_bar_open");
    connect(d->commandBarAction, SIGNAL(triggered(bool)), this, SLOT(openCommandBar()));

    actionManager->createStandardAction(KStandardAction::Preferences, this, SLOT(slotPreferences()));

    for (int i = 0; i < 2; i++) {
        d->expandingSpacers[i] = new KisAction(i18n("Expanding Spacer"));
        d->expandingSpacers[i]->setDefaultWidget(new QWidget(this));
        d->expandingSpacers[i]->defaultWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        actionManager->addAction(QString("expanding_spacer_%1").arg(i), d->expandingSpacers[i]);
    }

    {
        using namespace KisWidgetConnectionUtils;

        QAction *action = actionManager->createAction("lock_toolbars");
        connectControl(action, KisToolBar::toolBarStateModel(), "toolBarsLocked");
    }
}

void KisMainWindow::applyToolBarLayout()
{
    Q_FOREACH (KisToolBar *toolBar, toolBars()) {
        toolBar->layout()->setSpacing(4);
        toolBar->setStyleSheet("QToolBar { border: none }"); // has a border in "Fusion" style that people don't like

        // Hide text for buttons with an icon in the toolbar
        Q_FOREACH (QAction *ac, toolBar->actions()){
            if (ac->icon().pixmap(QSize(1,1)).isNull() == false){
                ac->setPriority(QAction::LowPriority);
            } else {
                ac->setIcon(QIcon());
            }
        }
    }
}

void KisMainWindow::initializeGeometry()
{
    // if the user didn's specify the geometry on the command line (does anyone do that still?),
    // we first figure out some good default size and restore the x,y position. See bug 285804Z.
    KConfigGroup cfg = d->windowStateConfig;
    QByteArray geom = QByteArray::fromBase64(cfg.readEntry("ko_geometry", QByteArray()));
    if (!restoreGeometry(geom)) {
        QRect desk = this->screen()->availableGeometry();

        quint32 x = desk.x();
        quint32 y = desk.y();
        quint32 w = 0;
        quint32 h = 0;

        // Default size -- maximize on small screens, something useful on big screens
        const int deskWidth = desk.width();
        if (deskWidth > 1024) {
            // a nice width, and slightly less than total available
            // height to compensate for the window decs
            w = (deskWidth / 3) * 2;
            h = (desk.height() / 3) * 2;
        }
        else {
            w = desk.width();
            h = desk.height();
        }

        x += (desk.width() - w) / 2;
        y += (desk.height() - h) / 2;

        move(x,y);
        setGeometry(geometry().x(), geometry().y(), w, h);
    }
#ifdef Q_OS_ANDROID
    // We handle full-screening differently on Android, see viewFullScreen.
    bool fullScreen = KisAndroidUtils::isInFullScreen();
#else
    bool fullScreen = isFullScreen();
#endif
    d->fullScreenMode->setChecked(fullScreen);
}

void KisMainWindow::showManual()
{
    QDesktopServices::openUrl(QUrl("https://krimble.org"));
}

void KisMainWindow::showDockerTitleBars(bool show)
{
    Q_FOREACH (QDockWidget *dock, dockWidgets()) {
        if (dock->titleBarWidget() && !dock->titleBarWidget()->inherits("KisUtilityTitleBar")) {
            dock->titleBarWidget()->setVisible(show || dock->isFloating());
        }
    }

    KisConfig cfg(true);
    cfg.setShowDockerTitleBars(show);
}

void KisMainWindow::slotXmlGuiMakingChanges(bool finished)
{
    if (finished) {
        subWindowActivated();
    }
}

void KisMainWindow::orientationChanged()
{
    QScreen *screen = QGuiApplication::primaryScreen();

    for (QWindow* window: QGuiApplication::topLevelWindows()) {
        // Android: we shouldn't transform Window managers independent of its child
        if ((window->type() == Qt::Popup)
            && (window->flags() & Qt::FramelessWindowHint) == 0
            && (window->geometry().topLeft() != QPoint(0, 0))) {
            // We are using reversed values. Because geometry returned is not the updated
            // one, but the previous one.
            int screenHeight = screen->geometry().width();
            int screenWidth = screen->geometry().height();

            // scaling
            int new_x = (window->position().x() * screenWidth) / screenHeight;
            int new_y = (window->position().y() * screenHeight) / screenWidth;

            // window width or height shouldn't change
            int winWidth = window->geometry().width();
            int winHeight = window->geometry().height();

            // Try best to not let the window go beyond screen.
            if (new_x > screenWidth - winWidth) {
                new_x = screenWidth - winWidth;
                if (new_x < 0)
                    new_x = 0;
            }
            if (new_y > screenHeight - winHeight) {
                new_y = screenHeight - winHeight;
                if (new_y < 0)
                    new_y = 0;
            }

            window->setPosition(QPoint(new_x, new_y));
        }
    }
}

bool KisMainWindow::checkActiveBundlesAvailable()
{
    KisStorageFilterProxyModel proxy;
    proxy.setSourceModel(KisStorageModel::instance());
    proxy.setFilter(KisStorageFilterProxyModel::ByStorageType,
                    QStringList()
                    << KisResourceStorage::storageTypeToUntranslatedString(KisResourceStorage::StorageType::Bundle));

    return (proxy.rowCount() > 0);
}

bool KisMainWindow::checkPaintOpAvailable()
{
    KisPaintOpPresetResourceServer * rserver = KisResourceServerProvider::instance()->paintOpPresetServer();
    return (rserver->resourceCount() > 0);
}

// One action has only one icon override, local XML contains ActionIconOverrides like so:
// <ActionIconOverrides>
//    <Action name="erase_action" icon="wheel-sectors"/>
//    <Action name="about_krita" icon="application-exit"/>
// </ActionIconOverrides>
// This function will rerender the actions with icons in UI (toolbars, shortcuts options etc.)
void KisMainWindow::applyActionIconOverridesFromLocalXML()
{
    QString xmlPath = KoResourcePaths::locateLocal("data", "krita5.xmlgui");
    QFile file(xmlPath);
    if (!file.exists()) {
        return;
    }

    QDomDocument doc;
    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file)) {
        return;
    }

    QDomNodeList overridesList = doc.elementsByTagName(QStringLiteral("ActionIconOverrides"));
    if (!overridesList.isEmpty()) {
        QDomElement overridesElement = overridesList.at(0).toElement();
        QDomNodeList actions = overridesElement.elementsByTagName(QStringLiteral("Action"));
        for (int i = 0; i < actions.count(); ++i) {
            QDomElement actionElement = actions.at(i).toElement();
            QString name = actionElement.attribute(QStringLiteral("name"));
            QString icon = actionElement.attribute(QStringLiteral("icon"));
            if (!name.isEmpty() && !icon.isEmpty()) {
                QAction *qa = actionCollection()->action(name);
                if (qa) {
                    qa->setIcon(KisIconUtils::loadIcon(icon));
                }
            }
        }
    }
}

void KisMainWindow::synchronizeDynamicActions()
{
    // Add all actions with a menu property to the main window
    Q_FOREACH(QAction *action, this->actionCollection()->actions()) {
        QString menuLocation = action->property("menulocation").toString();
        if (!menuLocation.isEmpty()) {
            QAction *found = 0;
            QList<QAction *> candidates = this->menuBar()->actions();
            Q_FOREACH(const QString &name, menuLocation.split("/")) {
                Q_FOREACH(QAction *candidate, candidates) {
                    if (candidate->objectName().toLower() == name.toLower()) {
                        found = candidate;
                        candidates = candidate->menu()->actions();
                        break;
                    }
                }
                if (candidates.isEmpty()) {
                    break;
                }
            }

            if (found && found->menu()) {
                QList<QAction *> existingActions = found->menu()->actions();

                if (std::find_if(existingActions.begin(),
                                 existingActions.end(),
                                 kismpl::mem_equal_to(&QAction::objectName, action->objectName()))
                    == existingActions.end()) {

                    if (std::is_sorted(existingActions.begin(),
                                       existingActions.end(),
                                       kismpl::mem_less(&QAction::objectName))) {

                        auto it = std::upper_bound(existingActions.begin(),
                                                   existingActions.end(),
                                                   action->objectName(),
                                                   kismpl::mem_less(&QAction::objectName));
                        found->menu()->insertAction(it != existingActions.end() ? *it : nullptr, action);

                    } else {
                        found->menu()->addAction(action);
                    }
                }
            }
        }
    }
}

#include <moc_KisMainWindow.cpp>
