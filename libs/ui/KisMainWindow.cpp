/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2000-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Thomas zander <zander@kde.org>
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KisMainWindow.h"
#include "KrimbleBatchDialog.h"   // Krimble 2026-10-25: Batch Process with recipes and WebP
#include "KrimbleHelpDialog.h"    // Krimble 2026-10-25: Help > Krimble Help
#include "KrimbleLogDialog.h"     // Krimble 2026-10-25: Help > Show Krimble Log...

#include <KoConfig.h>

// qt includes
#include <QApplication>
#include <QByteArray>
#include <QCloseEvent>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QScreen>
#include <QRegularExpression>   // Krimble 2026-10-19: krimbleInstallDockEdgeGrabs
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
#include <QCheckBox>
#include <QSpinBox>
#include <QSignalBlocker>
#include "kis_image_manager.h"
#include <kis_filter_strategy.h>
#include <QDialogButtonBox>
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
#include <QAbstractButton>   // Krimble 2026-10-30: the Quick Adjust Preview switch
#include <cmath>              // Krimble 2026-10-30: the Quick Adjust row pictures
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
#include <QWidgetAction>
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
#include <QAndroidJniEnvironment>
#include <QAndroidActivityResultReceiver>
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
#include "kis_canvas_decoration.h"
#include <QtMath>
#include <QElapsedTimer>
#include <QTouchEvent>
#include "kis_node_manager.h"
#include "kis_coordinates_converter.h"
#include "flake/kis_shape_layer.h"
#include <KoPathShape.h>
#include <KoShapeRegistry.h>      // Krimble 2026-10-30: Layer > New > Star...
#include <QGraphicsDropShadowEffect>   // Krimble 2026-10-30: the blue glow of a selected button
#include <kis_paint_layer.h>      // Krimble 2026-10-30: QuickAdjust bakes its layers into the paint layer below
#include <KoShapeFactoryBase.h>
#include <KoProperties.h>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <KoShapeStroke.h>
#include <KoUnit.h>
#include <QColorDialog>
#include <QDoubleSpinBox>
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
// Krimble 2026-10-13 (4th): THE REAL CAUSE OF THE WIDE MENUS (found by reading Qt 5.15's own source, `QMenuPrivate::updateActionRects`).
// For a menu opened from the MENU BAR, Qt always measures the keyboard shortcut of every item and adds a column as wide as the longest
// one ("max_column_width += tabWidth"). The switch `setShortcutVisibleInContextMenu(false)` (used since 2026-10-12) only matters for
// right-click menus, so it did nothing here, and no style can remove that column after the fact. The only way is for the items not to have a
// shortcut at the moment the menu is measured. So the shortcuts of all items of a menu and its submenus are taken off just before the
// menu is shown (`aboutToShow` comes before the measuring) and put back as soon as it closes (`aboutToHide`). They still work on a keyboard.
typedef QList<QPair<QPointer<QAction>, QList<QKeySequence> > > KrimbleSavedShortcuts;
static void krimbleStripShortcuts(QMenu *menu, KrimbleSavedShortcuts *saved, int depth)
{
    if (!menu || depth > 6) return;
    Q_FOREACH (QAction *action, menu->actions()) {
        const QList<QKeySequence> keys = action->shortcuts();
        if (!keys.isEmpty()) {
            saved->append(qMakePair(QPointer<QAction>(action), keys));
            action->setShortcuts(QList<QKeySequence>());
        }
        if (action->menu()) krimbleStripShortcuts(action->menu(), saved, depth + 1);
    }
}

static void krimbleInstallMenuShortcutStripping(QMainWindow *win)
{
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QMenu *menu = top->menu();
        if (!menu || menu->property("krimbleShortcutsStripped").toBool()) continue;
        menu->setProperty("krimbleShortcutsStripped", true);
        QSharedPointer<KrimbleSavedShortcuts> saved(new KrimbleSavedShortcuts());
        QObject::connect(menu, &QMenu::aboutToShow, menu, [menu, saved]() {
            krimbleStripShortcuts(menu, saved.data(), 0);
        });
        QObject::connect(menu, &QMenu::aboutToHide, menu, [saved]() {
            for (int i = 0; i < saved->size(); ++i) {
                if (saved->at(i).first) saved->at(i).first->setShortcuts(saved->at(i).second);
            }
            saved->clear();
        });
        // Krimble 2026-10-15: George (1.0.36): "Got another stuck menu ... Can we move open menus down so we can still see the selected menu title?"
        // Qt puts every menu with two or more columns at the very top of the screen, over the menu bar, on purpose (QMenu::popup:
        // `if (ncols > 1) pos.setY(screen.top() + desktopFrame)`), so it cannot be moved down without cutting off its bottom items. George chose
        // (a): the menu's own name as a heading on its first line. Only for menus of 12 or more items (the others sit under the menu bar), and
        // added each time the menu is about to show (the Window menu is rebuilt then), after Krita's own handler has run.
        // Krimble 2026-10-18 (5th): George (1.0.37, fc126d8): "The menus, instead of just being right below the menubar so you can see the menu title, has an additional
        // title for some reason. We don't need that." The name heading of the 136th entry is off: the menus now open under the menu bar, so their own title is not
        // covered. The code is kept below, not compiled.
#if 0
        QString menuTitle = top->text();
        menuTitle.remove(QLatin1Char('&'));
        QObject::connect(menu, &QMenu::aboutToShow, menu, [menu, menuTitle]() {
            if (menu->findChild<QWidgetAction*>(QStringLiteral("krimbleMenuHeading"))) {
                // still there from the last time: make sure it is the first one
                QAction *headingAction = menu->findChild<QWidgetAction*>(QStringLiteral("krimbleMenuHeading"));
                if (menu->actions().value(0) == headingAction) return;
            }
            int itemCount = 0;
            Q_FOREACH (QAction *a, menu->actions()) {
                if (!a->isSeparator() && a->isVisible()) ++itemCount;
            }
            if (itemCount < 12) return;
            QWidgetAction *heading = menu->findChild<QWidgetAction*>(QStringLiteral("krimbleMenuHeading"));
            QAction *divider = menu->findChild<QAction*>(QStringLiteral("krimbleMenuHeadingDivider"));
            if (!heading) {
                heading = new QWidgetAction(menu);
                heading->setObjectName(QStringLiteral("krimbleMenuHeading"));
                QLabel *label = new QLabel(menuTitle);
                label->setStyleSheet(QStringLiteral("QLabel { font-weight: bold; padding: 8px 14px; color: #e6e6e6; background-color: #373952; }"));
                heading->setDefaultWidget(label);
                divider = new QAction(menu);
                divider->setObjectName(QStringLiteral("krimbleMenuHeadingDivider"));
                divider->setSeparator(true);
            }
            menu->removeAction(heading);
            menu->removeAction(divider);
            QAction *firstAction = menu->actions().value(0);
            menu->insertAction(firstAction, heading);
            menu->insertAction(firstAction, divider);
        });
#endif
    }
}
#endif

#ifdef Q_OS_ANDROID
// Krimble 2026-10-13 (6th): George: "There is a minimum width on the side panel. There should not be." and "Eliminate all minimum widths."
// Qt's own code (qdockwidget.cpp, `QDockWidgetItem::minimumSize` and `QDockWidgetLayout::minimumSize`) gives a docked panel as its smallest
// width the smallest width of the widget inside it (or of its title bar, if that is wider) plus the frame; the main window cannot make it
// narrower. A widget that has a minimum width of its own set (greater than 0) is measured by that number and not by what it asks for
// (qSmartMinSize), so the panels' content and title bars, and the canvas area, are given a minimum of 1 px. Whatever does not fit is cut
// off instead of forcing the panel wide. Done for every panel, the toolbox included, and again every 2 s for panels made later.
static void krimbleRemoveMinimumWidths(QMainWindow *win)
{
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        // George: "Minimum width for the toolbox should be one column." The toolbox is left out: its smallest width stays what its layout
        // gives (one column of icons).
        if (dock->objectName() == QLatin1String("ToolBox")) continue;
        if (QWidget *content = dock->widget()) {
            if (content->minimumWidth() != 1) content->setMinimumWidth(1);
        }
        if (QWidget *title = dock->titleBarWidget()) {
            if (title->minimumWidth() != 1) title->setMinimumWidth(1);
        }
    }
    if (QWidget *central = win->centralWidget()) {
        if (central->minimumWidth() != 1) central->setMinimumWidth(1);
    }
    // Krimble 2026-10-17: George (1.0.37): "it's too easy to accidentally grab the layer type selection drop-down list when grabbing the edge. Is
    // there a way to scoot that selection area away from the edge so that layer menu doesn't appear so often by accident?" The Layers panel's
    // contents (its blending-mode drop-down is the first thing on the left) are moved 16 logical px (about 54 on the screen) away from the panel's
    // left edge, where the finger lands when it looks for the edge. Done once per panel.
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (dock->objectName() != QLatin1String("KisLayerBox")) continue;
        QWidget *content = dock->widget();
        if (!content || content->property("krimbleEdgeMargin").toBool()) continue;
        content->setProperty("krimbleEdgeMargin", true);
        // QMargins margins = content->contentsMargins();
        // margins.setLeft(margins.left() + 16);
        // content->setContentsMargins(margins);
        // Krimble 2026-10-18: George (1.0.37): "6. Layers panel has a big strip on the side instead of just moving the layer type selector over."
        // Only the row with the blending-mode drop-down gets the 16 px space in front of it; the rest of the panel is as it was.
        QComboBox *blendCombo = content->findChild<QComboBox*>();
        if (blendCombo) {
            QBoxLayout *rowLayout = nullptr;
            std::function<QBoxLayout*(QLayout*)> findRow = [&](QLayout *layout) -> QBoxLayout* {
                if (!layout) return nullptr;
                for (int i = 0; i < layout->count(); ++i) {
                    QLayoutItem *item = layout->itemAt(i);
                    if (item->widget() == blendCombo) return qobject_cast<QBoxLayout*>(layout);
                    if (QLayout *child = item->layout()) {
                        if (QBoxLayout *found = findRow(child)) return found;
                    }
                }
                return nullptr;
            };
            rowLayout = findRow(content->layout());
            if (rowLayout && (rowLayout->direction() == QBoxLayout::LeftToRight)) {
                rowLayout->insertSpacing(0, 16);
            }
        }
    }
}
#endif

#ifdef Q_OS_ANDROID
// Krimble 2026-10-13 (5th): an X in the top right corner that leaves Canvas Only mode (George: "Just put a X in the top right corner."
// "Just the X by itself. Black."). Canvas Only hides the menu bar and the toolbox, and a phone has no Tab key, so nothing on the screen
// could turn it off. The X is drawn only while Canvas Only is on; tapping it turns the mode off (the same switch as View > Screen Mode >
// Canvas Only). It is not a window or picture close button: it does not close anything.
class KrimbleCanvasOnlyExit : public QWidget
{
public:
    KrimbleCanvasOnlyExit(QMainWindow *win, QAction *canvasOnly) : QWidget(win), m_win(win), m_action(canvasOnly)
    {
        win->installEventFilter(this);
        QObject::connect(canvasOnly, &QAction::toggled, this, [this](bool) { reposition(); });
        hide();
        reposition();
    }
    void reposition()
    {
        if (!m_action || !m_action->isChecked()) {
            hide();
            return;
        }
        const int size = 36;     // logical px: about 12 mm on the screen
        setGeometry(m_win->width() - size - 4, 4, size, size);
        show();
        raise();
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_win && (event->type() == QEvent::Resize || event->type() == QEvent::Show)) {
            reposition();
        }
        return false;
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setPen(QPen(Qt::black, 3.5, Qt::SolidLine, Qt::RoundCap));
        const int m = int(width() * 0.28);
        p.drawLine(m, m, width() - m, height() - m);
        p.drawLine(width() - m, m, m, height() - m);
    }
    void mousePressEvent(QMouseEvent *e) override
    {
        e->accept();
    }
    void mouseReleaseEvent(QMouseEvent *e) override
    {
        if (rect().contains(e->pos()) && m_action) {
            m_action->setChecked(false);
        }
        e->accept();
    }

private:
    QPointer<QMainWindow> m_win;
    QPointer<QAction> m_action;
};

static void krimbleInstallCanvasOnlyExit(KisMainWindow *win)
{
    if (!win->viewManager() || win->findChild<QWidget*>(QStringLiteral("krimbleCanvasOnlyExit"), Qt::FindDirectChildrenOnly)) return;
    QAction *canvasOnly = win->viewManager()->actionCollection()->action(QStringLiteral("view_show_canvas_only"));
    if (!canvasOnly) return;
    KrimbleCanvasOnlyExit *exitX = new KrimbleCanvasOnlyExit(win, canvasOnly);
    exitX->setObjectName(QStringLiteral("krimbleCanvasOnlyExit"));
}
#endif

#ifdef Q_OS_ANDROID
// Krimble 2026-10-19: George (1.0.39, test list, item 2): "Using the crop tool: It was hard to select from the canvas because the resize
// tool for the toolbox got in the way. Can the resize tools be temporarily disabled during selection?" and "I think any selection tool
// should disable resize gadgets temporarily for ease of use." True while the Crop tool or any selection tool is the active tool; the
// panel edge grab zones (KrimbleDockEdgeGrab, below) hide themselves then, and come back when another tool is chosen.
static bool krimbleSelectionToolActive()
{
    const QString id = KoToolManager::instance()->activeToolId();
    return id.startsWith(QLatin1String("KisToolSelect")) || id == QLatin1String("KisToolCrop");
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
            m_reason = QStringLiteral("hidden: panel floating or not visible");   // Krimble 2026-10-23 (2nd): the reasons are shown by the layout-info overlay
            hide();
            return;
        }
        // Krimble 2026-10-19: no grab zone while the Crop tool or a selection tool is in use (see krimbleSelectionToolActive above):
        // the zone lies over the first 30 px of the canvas, where it took the finger that was drawing the crop or selection box.
        if (krimbleSelectionToolActive()) {
            m_reason = QStringLiteral("hidden: crop or selection tool active");
            hide();
            return;
        }
        const Qt::DockWidgetArea area = m_win->dockWidgetArea(m_dock);
        if (area != Qt::LeftDockWidgetArea && area != Qt::RightDockWidgetArea) {
            m_reason = QStringLiteral("hidden: panel is not on the left or right (area %1)").arg(int(area));
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
        // const int w = 7;    // Krimble 2026-10-12 (5th): George: "Handle on toolbox is still too big. Should just be a slight curve outward." 7 x 40, a gentle bulge
        // const int h = 40;
        // Krimble 2026-10-13 (2nd): George (1.0.35): "Handle is now too small. Hard to grab in portrait." (and it could not be seen: its color was
        // the canvas's gray). The bulge drawn is still 7 px deep, but its zone is 22 px wide and 64 px tall, and it is the panel's own
        // color with a thin light edge, so it looks like the panel bulging out.
        const int w = 22;
        const int h = 64;
        QRect column;                       // the area covered by all docked panels on this side
        bool topmost = true;
        Q_FOREACH (QDockWidget *other, m_win->findChildren<QDockWidget*>()) {
            if (other->isFloating() || !other->isVisible() || m_win->dockWidgetArea(other) != area) continue;
            column = column.isNull() ? other->geometry() : column.united(other->geometry());
            if (other->geometry().top() < m_dock->geometry().top()) topmost = false;
        }
        if (!topmost || column.isNull()) {
            m_reason = column.isNull() ? QStringLiteral("hidden: no visible panels on this side") : QStringLiteral("hidden: another panel on this side is higher");
            hide();
            return;
        }
        // const QRect g = m_dock->geometry();
        // setGeometry(area == Qt::LeftDockWidgetArea ? g.right() + 1 : g.left() - w, g.top(), w, g.height());   (the whole edge, the second version)
        // const int y = column.top() + (column.height() - h) / 2;
        // setGeometry(area == Qt::LeftDockWidgetArea ? column.right() + 1 : column.left() - w, y, w, h);
        // Krimble 2026-10-13 (6th): George: "Lose the visible handle. Make an invisible grab area extend out from the side of the edge." (and,
        // about the first list, "4 replaced"). The zone is now 24 logical px wide (about 5 mm), as tall as the whole column of panels on
        // that side, and lies OUTSIDE the panels' edge, over the canvas (to the right of the toolbox, to the left of the right panels).
        // Nothing is drawn.
        // const int stripWidth = 24;
        // const int stripWidth = 28;
        const int stripWidth = 30;   // Krimble 2026-10-17 (2nd): George: "Try just one more pixel? Or 2?" 28 -> 30. Before: 28 // Krimble 2026-10-17: George (1.0.37): "It's also sometimes still hard to grab edges for resize." "Try a small change first." 24 -> 28
        // setGeometry(area == Qt::LeftDockWidgetArea ? column.right() + 1 : column.left() - stripWidth, column.top(), stripWidth, column.height());
        Q_UNUSED(stripWidth);
        // Krimble 2026-10-23: George (1.0.41 test list item 27): "Handles on toolbox and side panel still too small to grab easy, but close. I drew an
        // idea." His drawing: a tab sticking out of the middle of the toolbox's edge onto the canvas, the toolbox's own color, angled top and bottom
        // ends, a dotted line down its middle. The zone is now only that tab (not the whole edge, which took fingers that were drawing), centred on
        // the column of panels on that side. kTabDepth is what is drawn; the zone is kTabZone wide so a finger can catch it.
        const int zoneH = qMin(kTabHeight, column.height());
        const int y = column.top() + (column.height() - zoneH) / 2;
        setGeometry(area == Qt::LeftDockWidgetArea ? column.right() + 1 : column.left() - kTabZone, y, kTabZone, zoneH);
        show();
        raise();
        m_reason = QStringLiteral("shown at x%1 y%2, %3x%4").arg(x()).arg(y).arg(width()).arg(height());
    }
    // Krimble 2026-10-23 (2nd): one line for the layout-info overlay (Window > ... QA overlay): is this panel's edge tab shown, and if not, why
    QString status() const { return QStringLiteral("edge tab %1: %2").arg(m_dock ? m_dock->objectName() : QString(), m_reason); }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_dock && (event->type() == QEvent::Resize || event->type() == QEvent::Show
                                  || event->type() == QEvent::Move || event->type() == QEvent::ParentChange)) {
            reposition();
        }
        return false;
    }

protected:
    // Krimble 2026-10-23: the tab from George's drawing (see reposition). Tuning values.
    static constexpr int kTabDepth = 10;    // logical px the tab sticks out from the panel's edge
    static constexpr int kTabZone = 20;     // logical px wide touch zone (the tab plus room for a finger)
    // static constexpr int kTabHeight = 140;  // logical px tall, at most the height of the panels' column
    // Krimble 2026-10-26: George (1.0.44, with a screenshot): "Way too hard to grab the side panel sizing gadget. Try to grab sizing gadget and I get this from color panel" (the black
    // square: the color selector's pop-up, opened by a touch that fell on the Color panel just below the zone: the zone covered y 425 to 615 and the touch was at about y 624). With the tab
    // no longer drawn (kTabVisible = false) nothing shows where the zone is, so it is 140 again, the height of which he said in 1.0.42: "it works great" (the tab itself was too big then).
    static constexpr int kTabHeight = 140;  // 70 -> 140 (2026-10-26)
    // static constexpr int kTabHeight = 70;   // Krimble 2026-10-23 (2nd): George (1.0.42): "The handle on the toolbox is about 2x the height it should be, but it works great." 140 -> 70

    // Krimble 2026-10-25: George (1.0.43): "Now we have the little dots and the handle and more dots. As long as we have the little dots there I think we can make the handle
    // invisible (as long as it works)." The tab (its shape and its dotted line) is not drawn any more; its touch zone (kTabZone wide, kTabHeight tall, in the middle of the
    // column of panels) is exactly as before, so a finger on it still resizes that side. Krita's own dotted handle in the separator (the "little dots", 212th entry) stays.
    // Set this to true to see the tab again.
    static constexpr bool kTabVisible = false;

    void paintEvent(QPaintEvent *) override
    {
        if (!kTabVisible) return;
        // Krimble 2026-10-13 (6th): nothing is drawn any more (the grab zone is invisible). The old drawing is kept below, not run.
        // return;
        // Krimble 2026-10-23: the tab from George's drawing: panel color, angled ends, dotted line down the middle.
        {
            QPainter p(this);
            p.setRenderHint(QPainter::Antialiasing, true);
            const bool leftArea = m_win->dockWidgetArea(m_dock) == Qt::LeftDockWidgetArea;
            const qreal h = height();
            const qreal d = kTabDepth;
            const qreal slope = d * 1.6;                      // how far down the angled end runs
            const qreal x0 = leftArea ? 0 : width();          // the panel's edge
            const qreal x1 = leftArea ? d : width() - d;      // the tab's outer edge
            QPainterPath tab;
            tab.moveTo(x0, 0);
            tab.lineTo(x1, slope);
            tab.lineTo(x1, h - slope);
            tab.lineTo(x0, h);
            tab.closeSubpath();
            // p.setPen(Qt::NoPen);
            // Krimble 2026-10-23 (2nd): on the right the tab lies over the canvas's scroll bar, which has the panel's own color, so the tab could not be seen there; a thin light edge
            // makes it show. The toolbox side (George: "it works great") is drawn as before.
            QColor edgeColor = m_dock->palette().color(QPalette::WindowText);
            edgeColor.setAlpha(140);
            p.setPen(leftArea ? QPen(Qt::NoPen) : QPen(edgeColor, 1.2));
            p.setBrush(m_dock->palette().color(QPalette::Window));
            p.drawPath(tab);
            QColor dots = m_dock->palette().color(QPalette::WindowText);
            dots.setAlpha(170);
            QPen pen(dots, 1.6);
            pen.setStyle(Qt::DotLine);
            pen.setCapStyle(Qt::RoundCap);
            p.setPen(pen);
            const qreal mid = (x0 + x1) / 2.0;
            p.drawLine(QPointF(mid, slope + 4), QPointF(mid, h - slope - 4));
            return;
        }
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        // QColor fill = palette().color(QPalette::Mid);   (the canvas's gray: it could not be seen)
        // fill.setAlpha(215);
        QColor fill = m_dock->palette().color(QPalette::Window);
        QColor edge = m_dock->palette().color(QPalette::WindowText);
        edge.setAlpha(150);
        p.setPen(QPen(edge, 1.5));
        p.setBrush(fill);
        // (the rounded tab with three grip lines, kept:)
        // p.drawRoundedRect(QRectF(rect()), 7, 7);
        // Krimble 2026-10-12 (5th): a slight curve outward from the panel edge, no grip lines.
        const qreal w = width();
        const qreal h = height();
        const qreal depth = 7;     // how far the curve sticks out; the rest of the zone is only a bigger place to put a finger
        QPainterPath bulge;
        if (m_win->dockWidgetArea(m_dock) == Qt::LeftDockWidgetArea) {     // the panel is on the left: the curve bulges to the right
            bulge.moveTo(0, h * 0.1);
            bulge.cubicTo(depth * 1.3, h * 0.28, depth * 1.3, h * 0.72, 0, h * 0.9);
        } else {                                                           // the panels are on the right: the curve bulges to the left
            bulge.moveTo(w, h * 0.1);
            bulge.cubicTo(w - depth * 1.3, h * 0.28, w - depth * 1.3, h * 0.72, w, h * 0.9);
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
    QString m_reason {QStringLiteral("not placed yet")};   // Krimble 2026-10-23 (2nd)
};

static void krimbleInstallDockEdgeGrabs(QMainWindow *win)
{
    // Krimble 2026-10-19: George (1.0.39): "it's not just selection tools. Any tool, including text, when you draw will grab the edge of the
    // toolbox. We should go back to the original handle concept from Krita." The invisible grab zones over the canvas (KrimbleDockEdgeGrab)
    // are switched off: any that exist are hidden and no new ones are made. Panels are resized with Krita's own handle again, the separator
    // between a panel and the canvas (widened for fingers by KisWideDockSeparatorStyle, not changed). The code below is kept, not run.
    // Krimble 2026-10-23: no longer hidden here: this runs on a timer, and each tab decides for itself whether it shows (reposition()).
    // Q_FOREACH (QWidget *existing, win->findChildren<QWidget*>(QRegularExpression(QStringLiteral("^krimbleEdgeGrab_")), Qt::FindDirectChildrenOnly)) {
    //     existing->hide();
    // }
    // return;
    // Krimble 2026-10-23: back on, as the small visible tab from George's drawing (1.0.41 test list item 27), not the full-edge strip.
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
// Krimble 2026-10-30: George: "new QuickAdjust dialog was supposed to already be built by ChatGPT. Either complete it or replace it." The drawing
// (ChatGPT's mockup) has a small picture at the start of each slider row, a rainbow Hue slider and a Preview switch that says ON or OFF. These are the
// missing parts, drawn in code (no picture files). The mockup's "Krimble" logo in the heading is left out: George, "QuickAdjust does not need to say
// Krimble in it."
static QPixmap krimbleQuickAdjustIcon(int which, int px)
{
    QPixmap pm(px, px);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    const QRectF r(3, 3, px - 6, px - 6);
    const QPointF c = r.center();
    const QColor ink(0xf4, 0xf6, 0xff);
    p.setPen(QPen(ink, 2));
    p.setBrush(Qt::NoBrush);
    switch (which) {
    case 0: {   // Brightness: a sun
        const qreal core = r.width() * 0.22;
        p.drawEllipse(c, core, core);
        for (int k = 0; k < 8; ++k) {
            const qreal a = k * M_PI / 4.0;
            const QPointF dir(std::cos(a), std::sin(a));
            p.drawLine(c + dir * (core + 3), c + dir * (r.width() * 0.46));
        }
        break; }
    case 1:     // Contrast: a circle, the left half filled
        p.drawEllipse(r);
        p.setBrush(ink);
        p.drawPie(r, 90 * 16, 180 * 16);
        break;
    case 2: {   // Saturation: three overlapping drops of color
        const qreal d = r.width() * 0.58;
        p.setPen(Qt::NoPen);
        const QColor cols[3] = { QColor(40, 190, 255, 215), QColor(255, 90, 170, 215), QColor(255, 200, 40, 215) };
        const QPointF pts[3] = { QPointF(r.left() + d / 2, r.top() + d / 2), QPointF(r.right() - d / 2, r.top() + d / 2), QPointF(c.x(), r.bottom() - d / 2) };
        for (int k = 0; k < 3; ++k) { p.setBrush(cols[k]); p.drawEllipse(pts[k], d / 2, d / 2); }
        break; }
    case 3: {   // Hue: a ring of all the colors
        QConicalGradient g(c, 0);
        for (int k = 0; k <= 6; ++k) g.setColorAt(k / 6.0, QColor::fromHsv((k * 60) % 360, 255, 255));
        const qreal w = r.width() * 0.22;
        p.setPen(QPen(QBrush(g), w));
        p.drawEllipse(r.adjusted(w / 2, w / 2, -w / 2, -w / 2));
        break; }
    case 4: {   // Lightness: a square, the left half light
        const QRectF sq = r.adjusted(2, 2, -2, -2);
        p.drawRoundedRect(sq, 3, 3);
        p.setBrush(ink);
        p.drawRect(QRectF(sq.left(), sq.top(), sq.width() / 2, sq.height()));
        break; }
    case 5: {   // Vibrance: a four-point sparkle
        const qreal R = r.width() * 0.5, q = r.width() * 0.14;
        QPainterPath path;
        path.moveTo(c.x(), c.y() - R); path.lineTo(c.x() + q, c.y() - q); path.lineTo(c.x() + R, c.y());
        path.lineTo(c.x() + q, c.y() + q); path.lineTo(c.x(), c.y() + R); path.lineTo(c.x() - q, c.y() + q);
        path.lineTo(c.x() - R, c.y()); path.lineTo(c.x() - q, c.y() - q); path.closeSubpath();
        p.setPen(QPen(ink, 1.5));
        p.setBrush(QColor(0x9d, 0x8c, 0xff));
        p.drawPath(path);
        break; }
    }
    return pm;
}

// the Preview switch of the drawing: a pill that says ON or OFF, with a round knob, and a blue glow when it is on
class KrimbleToggleSwitch : public QAbstractButton
{
public:
    explicit KrimbleToggleSwitch(QWidget *parent = nullptr) : QAbstractButton(parent)
    {
        setCheckable(true);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }
    QSize sizeHint() const override { return QSize(84, 40); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QRectF r = QRectF(rect()).adjusted(5, 5, -5, -5);
        const qreal radius = r.height() / 2.0;
        if (isChecked()) {
            p.setBrush(Qt::NoBrush);
            for (int i = 4; i >= 1; --i) {
                p.setPen(QPen(QColor(56, 198, 255, 20 * (5 - i)), i * 2.0));
                p.drawRoundedRect(r, radius, radius);
            }
        }
        QLinearGradient bg(r.topLeft(), r.bottomLeft());
        if (isChecked()) { bg.setColorAt(0, QColor(0x38, 0xc6, 0xff)); bg.setColorAt(1, QColor(0x0b, 0x6f, 0xd6)); }
        else             { bg.setColorAt(0, QColor(0x4a, 0x52, 0x76)); bg.setColorAt(1, QColor(0x2a, 0x30, 0x48)); }
        p.setPen(QPen(isChecked() ? QColor(0x9f, 0xe3, 0xff) : QColor(0x6b, 0x74, 0x98), 1.5));
        p.setBrush(bg);
        p.drawRoundedRect(r, radius, radius);
        const qreal knob = r.height() - 8;
        const qreal x = isChecked() ? r.right() - knob - 4 : r.left() + 4;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0xf4, 0xf6, 0xff));
        p.drawEllipse(QRectF(x, r.top() + 4, knob, knob));
        p.setPen(QColor(0xf4, 0xf6, 0xff));
        QFont f = font(); f.setBold(true); p.setFont(f);
        const QRectF textRect = isChecked() ? QRectF(r.left() + 6, r.top(), r.width() - knob - 14, r.height())
                                            : QRectF(r.left() + knob + 10, r.top(), r.width() - knob - 14, r.height());
        p.drawText(textRect, Qt::AlignCenter, isChecked() ? QStringLiteral("ON") : QStringLiteral("OFF"));
    }
};

#if 0   // Krimble 2026-10-30: switched off: George: "not drawn in code. I will provide a graphic." The code is kept, not run.
// Krimble 2026-10-30: George: "QuickAdjust should have its own logo, a big bold stylize QuickAdjust." (not the Krimble K and paw: "no.") A logo made of the
// word itself, drawn in code: heavy italic letters, "Quick" light with a blue glow and "Adjust" in Krimble orange (#FD6001) with a dark edge, and a thin
// slider line under them with a round knob. It shrinks to fit the width of the window.
class KrimbleQuickAdjustLogo : public QWidget
{
public:
    explicit KrimbleQuickAdjustLogo(QWidget *parent = nullptr) : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setMinimumHeight(66);
        setAccessibleName(QStringLiteral("QuickAdjust"));
    }
    QSize sizeHint() const override { return QSize(300, 70); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QString quick = QStringLiteral("Quick"), adjust = QStringLiteral("Adjust");
        QFont f = font();
        f.setBold(true);
        f.setItalic(true);
        f.setWeight(QFont::Black);
        f.setLetterSpacing(QFont::PercentageSpacing, 96);
        f.setPixelSize(44);
        QFontMetricsF fm(f);
        const qreal total = fm.horizontalAdvance(quick) + fm.horizontalAdvance(adjust);
        const qreal avail = width() - 28.0;
        if (total > avail) {
            f.setPixelSize(qMax(18, int(44.0 * avail / total)));
            fm = QFontMetricsF(f);
        }
        const qreal quickWidth = fm.horizontalAdvance(quick);
        const qreal textWidth = quickWidth + fm.horizontalAdvance(adjust);
        const qreal x = (width() - textWidth) / 2.0;
        const qreal y = 8.0 + fm.ascent();
        QPainterPath quickPath, adjustPath;
        quickPath.addText(x, y, f, quick);
        adjustPath.addText(x + quickWidth, y, f, adjust);
        // glow behind the letters
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(56, 198, 255, 80), 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPath(quickPath);
        p.setPen(QPen(QColor(253, 96, 1, 90), 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPath(adjustPath);
        // the letters
        QLinearGradient quickFill(0, y - fm.ascent(), 0, y + fm.descent());
        quickFill.setColorAt(0.0, QColor(0xff, 0xff, 0xff));
        quickFill.setColorAt(1.0, QColor(0xb4, 0xc2, 0xff));
        p.setPen(QPen(QColor(8, 12, 36), 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(quickFill);
        p.drawPath(quickPath);
        QLinearGradient adjustFill(0, y - fm.ascent(), 0, y + fm.descent());
        adjustFill.setColorAt(0.0, QColor(0xff, 0x8d, 0x42));
        adjustFill.setColorAt(0.5, QColor(0xfd, 0x60, 0x01));
        adjustFill.setColorAt(1.0, QColor(0xc4, 0x4a, 0x00));
        p.setBrush(adjustFill);
        p.drawPath(adjustPath);
        // the slider line under the word, with its knob
        const qreal lineY = y + fm.descent() + 7.0;
        p.setPen(QPen(QColor(0x53, 0x5c, 0x80), 3, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(x, lineY), QPointF(x + textWidth, lineY));
        p.setPen(QPen(QColor(0xfd, 0x60, 0x01), 3, Qt::SolidLine, Qt::RoundCap));
        const qreal knobX = x + textWidth * 0.62;
        p.drawLine(QPointF(x, lineY), QPointF(knobX, lineY));
        p.setPen(QPen(QColor(0xf4, 0xf6, 0xff), 1.5));
        p.setBrush(QColor(0xf4, 0xf6, 0xff));
        p.drawEllipse(QPointF(knobX, lineY), 5.0, 5.0);
    }
};
#endif   // Krimble 2026-10-30: end of the logo drawn in code (switched off)

// Krimble 2026-10-30: "Keep as layers" in the Quick Adjust window (off at first; kept while the program runs)
static bool g_krimbleQuickKeepLayers = false;

class KrimbleQuickAdjust : public QObject
{
public:
    KrimbleQuickAdjust(KisMainWindow *win, QWidget *content) : QObject(content), m_win(win)
    {
        QVBoxLayout *layout = new QVBoxLayout(content);
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setSpacing(8);
        QHBoxLayout *options = new QHBoxLayout;
        QLabel *title = new QLabel(i18n("Adjustments"), content);
        QFont font = title->font(); font.setBold(true); title->setFont(font);
        options->addWidget(title);
        options->addStretch();
        // m_preview = new QCheckBox(i18n("Preview"), content);
        // Krimble 2026-10-30: the Preview switch of the drawing (a pill that says ON or OFF), with its label
        options->addWidget(new QLabel(i18n("Preview"), content));
        m_preview = new KrimbleToggleSwitch(content);
        m_preview->setChecked(true);
        options->addWidget(m_preview);
        layout->addLayout(options);
        // Krimble 2026-10-30: George: "I didn't expect operations as layers. Offer it as a toggle." The adjustments are applied to the layer when OK is
        // pressed (merged down into the paint layer below), unless this is checked: then they stay as adjustment layers (the old behaviour).
        m_keepLayers = new QCheckBox(i18n("Keep as layers"), content);
        m_keepLayers->setChecked(g_krimbleQuickKeepLayers);
        connect(m_keepLayers, &QCheckBox::toggled, this, [](bool on) { g_krimbleQuickKeepLayers = on; });
        layout->addWidget(m_keepLayers);
        for (int i = 0; i < SliderCount; ++i) {
            QHBoxLayout *row = new QHBoxLayout;
            row->setSpacing(8);
            // Krimble 2026-10-30: the small picture at the start of the row (the drawing's sun, half circle, drops, color ring, half square, sparkle)
            QLabel *rowIcon = new QLabel(content);
            rowIcon->setPixmap(krimbleQuickAdjustIcon(i, 34));
            rowIcon->setFixedSize(34, 34);
            row->addWidget(rowIcon);
            m_labels[i] = new QLabel(i18n(def(i).name), content);
            m_labels[i]->setMinimumWidth(90);
            row->addWidget(m_labels[i]);
            m_numbers[i] = new QSpinBox(content);
            m_numbers[i]->setRange(def(i).min, def(i).max);
            m_numbers[i]->setMinimumWidth(72);
            row->addWidget(m_numbers[i]);
            layout->addLayout(row);
            m_sliders[i] = new QSlider(Qt::Horizontal, content);
            m_sliders[i]->setRange(def(i).min, def(i).max);
            m_sliders[i]->setMinimumWidth(1);
            m_sliders[i]->setMinimumHeight(38);
            m_sliders[i]->setObjectName(QStringLiteral("krimbleQuickAdjustSlider"));
            if (i == Hue) {
                // Krimble 2026-10-30: the drawing's rainbow Hue slider: the groove shows the colors the slider goes through (and has no orange fill)
                m_sliders[i]->setStyleSheet(QStringLiteral(
                    "QSlider::groove:horizontal { height: 9px; border: 1px solid #535c80; border-radius: 4px;"
                    " background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #ff00c8, stop:0.2 #4a3cff, stop:0.4 #00c8ff,"
                    " stop:0.6 #1fdc3f, stop:0.8 #ffe600, stop:1 #ff2a00); }"
                    "QSlider::sub-page:horizontal { background: transparent; border: none; }"));
            }
            layout->addWidget(m_sliders[i]);
            connect(m_sliders[i], &QSlider::valueChanged, this, [this, i](int value) {
                m_values[i] = value;
                QSignalBlocker blocker(m_numbers[i]);
                m_numbers[i]->setValue(value);
                m_timer.start(90);
            });
            connect(m_numbers[i], qOverload<int>(&QSpinBox::valueChanged), this, [this, i](int value) {
                m_sliders[i]->setValue(value);
            });
        }
        QPushButton *reset = new QPushButton(i18n("Reset"), content);
        layout->addWidget(reset);
        layout->addStretch(1);
        connect(reset, &QPushButton::clicked, this, [this]() {
            for (int i = 0; i < SliderCount; ++i) m_sliders[i]->setValue(0);
        });
        m_timer.setSingleShot(true);
        connect(&m_timer, &QTimer::timeout, this, [this]() { apply(); });
        for (int i = 0; i < SliderCount; ++i) m_values[i] = 0;
        connect(m_preview, &QAbstractButton::toggled, this, [this](bool enabled) {
            m_previewEnabled = enabled;
            flush();
        });
    }

    // Krimble 2026-10-13 (11th): for the dialog version (George: "Build Quick Adjust as a dialog window launched from Image Adjust."):
    // OK applies any slider move that is still waiting; Cancel takes back the adjustment layers made while the dialog was open.
    void flush()
    {
        m_timer.stop();
        apply();
    }
    void commitVisible()
    {
        m_previewEnabled = true;
        if (m_preview) m_preview->setChecked(true);
        flush();
    }
    // Krimble 2026-10-30: called when OK is pressed, after commitVisible(). Not checked "Keep as layers": the adjustment layers are merged down, bottom one first,
    // each into the paint layer directly below it (a layer that is not on a paint layer stays as a layer). Either way the layer that was active before
    // Quick Adjust is the active layer again (George: Edit > Fill > Foreground Color pasted a weird image while the adjustment layers were active: the fill
    // went into an adjustment layer).
    void finish()
    {
        KisViewManager *view = m_win->viewManager();
        if (!view || !view->nodeManager()) return;
        KisImageSP image = view->image();
        if (image && !(m_keepLayers && m_keepLayers->isChecked())) {
            QList<KisNodeSP> made;
            KisNodeWSP *refs[3] = { &m_bcLayer, &m_hsvLayer, &m_vibLayer };
            for (KisNodeWSP *ref : refs) {
                KisNodeSP node = *ref;
                if (node && node->parent()) made.append(node);
            }
            std::sort(made.begin(), made.end(), [](const KisNodeSP &a, const KisNodeSP &b) {
                return a->parent() == b->parent() && a->parent()->index(a) < b->parent()->index(b);
            });
            QAction *mergeDown = view->actionCollection()->action(QStringLiteral("merge_layer"));
            Q_FOREACH (KisNodeSP node, made) {
                KisNodeSP below = node->prevSibling();
                if (!below || !dynamic_cast<KisPaintLayer*>(below.data()) || !mergeDown) continue;
                view->nodeManager()->slotNonUiActivatedNode(node);
                mergeDown->trigger();
                image->waitForDone();
            }
        }
        if (m_activeBefore && m_activeBefore->parent()) view->nodeManager()->slotNonUiActivatedNode(m_activeBefore);
    }
    void removeCreatedLayers()
    {
        m_timer.stop();
        KisViewManager *view = m_win->viewManager();
        if (!view) return;
        KisNodeCommandsAdapter adapter(view);
        KisNodeWSP *refs[3] = { &m_bcLayer, &m_hsvLayer, &m_vibLayer };
        for (int i = 0; i < 3; ++i) {
            KisNodeSP node = *refs[i];
            if (node && node->parent()) adapter.removeNode(node);
            *refs[i] = KisNodeWSP();
        }
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
        KisNodeWSP *refs[3] = { &m_bcLayer, &m_hsvLayer, &m_vibLayer };
        for (KisNodeWSP *ref : refs) {
            KisNodeSP node = *ref;
            if (node && node->parent()) node->setVisible(m_previewEnabled);
        }
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
            if (!m_activeBefore) m_activeBefore = active;   // Krimble 2026-10-30: finish() makes it the active layer again
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
    QSpinBox *m_numbers[SliderCount];
    // QCheckBox *m_preview = nullptr;
    QAbstractButton *m_preview = nullptr;   // Krimble 2026-10-30: the Preview switch
    QCheckBox *m_keepLayers = nullptr;     // Krimble 2026-10-30
    KisNodeWSP m_activeBefore;             // Krimble 2026-10-30: the layer that was active when the first adjustment layer was made
    bool m_previewEnabled = true;
    int m_values[SliderCount];
    QTimer m_timer;
    KisImageWSP m_image;
    KisNodeWSP m_bcLayer;
    KisNodeWSP m_hsvLayer;
    KisNodeWSP m_vibLayer;
};

// Krimble 2026-10-13 (11th): Quick Adjust as a dialog window (George: "Quick Adjust should be a dialog window, not a growth of some kind." and
// "Build Quick Adjust as a dialog window launched from Image Adjust."). One window, not tied to any other: six sliders, Reset, OK and Cancel.
// While it is open, the picture shows the change live (adjustment layers are made as the sliders move); Cancel takes those layers away, OK keeps
// them. The window sits at the bottom of the screen so the picture stays visible above it.
static void krimbleShowQuickAdjustDialog(KisMainWindow *win)
{
    KisViewManager *view = win->viewManager();
    if (!view || !view->image()) {
        QMessageBox::information(win, i18n("Quick Adjust"), i18n("Open a picture first."));
        return;
    }
    QDialog *dialog = new QDialog(win);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(i18n("Quick Adjust"));
    dialog->setModal(false);
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(4, 4, 4, 4);
    QWidget *content = new QWidget(dialog);
    KrimbleQuickAdjust *adjust = new KrimbleQuickAdjust(win, content);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
    // Krimble 2026-10-14: George (1.0.36): "Quick Adjust is perfect size. Has no title bar?" The window has none on the phone, so its name is
    // written at the top of it.
    // Krimble 2026-10-30: George: "QuickAdjust does not need to say Krimble in it." The heading shows the tool name only.
    // QLabel *heading = new QLabel(i18n("Krimble  |  QuickAdjust"), dialog);
    // Krimble 2026-10-30: George: "not drawn in code. I will provide a graphic." The logo drawn in code (KrimbleQuickAdjustLogo, switched off above with #if 0)
    // is not used; the plain heading is back until his graphic of QuickAdjust arrives.
    QLabel *heading = new QLabel(i18n("QuickAdjust"), dialog);
    QFont headingFont = heading->font();
    headingFont.setBold(true);
    heading->setFont(headingFont);
    layout->addWidget(heading);
    // layout->addWidget(new KrimbleQuickAdjustLogo(dialog));
    layout->addWidget(content);
    layout->addWidget(buttons);
    QObject::connect(buttons, &QDialogButtonBox::accepted, dialog, [adjust, dialog]() {
        adjust->commitVisible();
        adjust->finish();     // Krimble 2026-10-30: merge down unless "Keep as layers"; the earlier layer is active again
        dialog->accept();
    });
    QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, [adjust, dialog]() {
        adjust->removeCreatedLayers();
        dialog->reject();
    });
    // closing the window another way (Android's back button) counts as Cancel
    QObject::connect(dialog, &QDialog::rejected, dialog, [adjust]() { adjust->removeCreatedLayers(); });
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QRect avail = scr->availableGeometry();
        const int width = int(avail.width() * 0.94);
        const int height = qMin(dialog->sizeHint().height() + 20, int(avail.height() * 0.90));
        dialog->resize(width, height);
        dialog->move(avail.left() + (avail.width() - width)/2,
                     avail.bottom() - height - 8);
    }
    dialog->show();
}

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
    dock->setAllowedAreas(Qt::NoDockWidgetArea);   // Krimble 2026-10-12: Quick Adjust is a floating tool; dragging it never docks it
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

// Krimble 2026-10-13 (10th): "Share..." (George's design, in his own words and by his choices: "What if the menu item for sharing is just Share...
// and it opens a dialog window to get the details?", "And that universal share symbol.", "After export options.", "All of the things you
// indicated.", "PNG has a quality slider too.", "No warnings. Build it."). The dialog has: Format (JPG or PNG); a slider, "Quality" for JPG
// (10 to 100) and "Compression" for PNG (0 to 9, where more makes a smaller file and the picture stays the same); Size (original, or a
// longest side of 1920, 1280 or 1080 px); and "Flatten onto white" (JPG only). Share then opens Android's share sheet.
static void krimbleShareWithOptions(KisMainWindow *win, bool asJpg, int level, int maxSide, bool flatten)
{
#ifdef Q_OS_ANDROID
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
    if (maxSide > 0 && qMax(flat.width(), flat.height()) > maxSide) {
        flat = flat.scaled(maxSide, maxSide, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    const QString dirPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/share");
    QDir().mkpath(dirPath);
    const QString base = QStringLiteral("Krimble-%1").arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss")));
    const QString extension = asJpg ? QStringLiteral(".jpg") : QStringLiteral(".png");
    const QString path = dirPath + QLatin1Char('/') + base + extension;
    bool saved;
    if (asJpg) {
        QImage toSave = flat;
        if (flatten) {
            toSave = QImage(flat.size(), QImage::Format_RGB32);
            toSave.fill(Qt::white);
            QPainter painter(&toSave);
            painter.drawImage(0, 0, flat);
            painter.end();
        }
        saved = toSave.save(path, "JPG", level);
    } else {
        // Qt's PNG quality: 0 is the smallest file, 100 the largest; the slider's 9 is the smallest file
        saved = flat.save(path, "PNG", (9 - qBound(0, level, 9)) * 100 / 9);
    }
    if (!saved) {
        QMessageBox::warning(win, i18n("Share"), i18n("The image could not be saved for sharing."));
        return;
    }
    QAndroidJniObject jPath = QAndroidJniObject::fromString(path);
    QAndroidJniObject jMime = QAndroidJniObject::fromString(asJpg ? QStringLiteral("image/jpeg") : QStringLiteral("image/png"));
    QAndroidJniObject jName = QAndroidJniObject::fromString(base + extension);
    const jboolean ok = QAndroidJniObject::callStaticMethod<jboolean>("org/krimble/android/ShareHelper", "shareFile",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", jPath.object<jstring>(), jMime.object<jstring>(), jName.object<jstring>());
    if (!ok) {
        QMessageBox::warning(win, i18n("Share"), i18n("Sharing did not work on this phone."));
    }
#else
    Q_UNUSED(win); Q_UNUSED(asJpg); Q_UNUSED(level); Q_UNUSED(maxSide); Q_UNUSED(flatten);
#endif
}

static void krimbleShowShare(KisMainWindow *win)
{
    if (!win->viewManager() || !win->viewManager()->image()) {
        QMessageBox::information(win, i18n("Share"), i18n("Open an image first."));
        return;
    }
    QDialog *dialog = new QDialog(win);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(i18n("Share"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(8, 8, 8, 8);
    QComboBox *format = new QComboBox(dialog);
    format->addItem(i18n("JPG"));
    format->addItem(i18n("PNG"));
    QLabel *levelLabel = new QLabel(dialog);
    QSlider *level = new QSlider(Qt::Horizontal, dialog);
    QComboBox *size = new QComboBox(dialog);
    size->addItem(i18n("Original size"), 0);
    size->addItem(i18n("Longest side 1920 px"), 1920);
    size->addItem(i18n("Longest side 1280 px"), 1280);
    size->addItem(i18n("Longest side 1080 px"), 1080);
    QCheckBox *flatten = new QCheckBox(i18n("Flatten onto white"), dialog);
    flatten->setChecked(true);
    auto updateLevelLabel = [format, levelLabel, level]() {
        levelLabel->setText(format->currentIndex() == 0 ? i18n("Quality: %1", level->value()) : i18n("Compression: %1", level->value()));
    };
    auto updateFormat = [format, level, flatten, updateLevelLabel]() {
        const bool jpg = format->currentIndex() == 0;
        flatten->setEnabled(jpg);
        if (jpg) {
            level->setRange(10, 100);
            level->setValue(90);
        } else {
            level->setRange(0, 9);
            level->setValue(5);
        }
        updateLevelLabel();
    };
    QObject::connect(format, QOverload<int>::of(&QComboBox::currentIndexChanged), dialog, [updateFormat](int) { updateFormat(); });
    QObject::connect(level, &QSlider::valueChanged, dialog, [updateLevelLabel](int) { updateLevelLabel(); });
    updateFormat();
    QDialogButtonBox *buttons = new QDialogButtonBox(dialog);
    QPushButton *shareButton = buttons->addButton(i18n("Share"), QDialogButtonBox::AcceptRole);
    buttons->addButton(QDialogButtonBox::Cancel);
    Q_UNUSED(shareButton);
    QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    QObject::connect(buttons, &QDialogButtonBox::accepted, dialog, [win, dialog, format, level, size, flatten]() {
        const bool jpg = format->currentIndex() == 0;
        const int value = level->value();
        const int maxSide = size->currentData().toInt();
        const bool flat = flatten->isChecked();
        dialog->accept();
        krimbleShareWithOptions(win, jpg, value, maxSide, flat);
    });
    layout->addWidget(format);
    layout->addWidget(levelLabel);
    layout->addWidget(level);
    layout->addWidget(size);
    layout->addWidget(flatten);
    layout->addWidget(buttons);
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        dialog->resize(int(scr->availableGeometry().width() * 0.9), dialog->sizeHint().height() + 20);
    }
    dialog->show();
}

// "Batch Process...": resize or convert many pictures with one tap (Claude's idea; George: "Add batch presets.").
// Pick pictures, pick a preset, and the results are saved in Pictures/Krimble/Batch (the originals are not touched).
// Works on pictures Qt can read (JPG, PNG, WebP, BMP and the like), not on layered files.
static void krimbleShowBatch(KisMainWindow *win)
{
#ifdef Q_OS_ANDROID
    // Krimble 2026-10-25: George: "I'm working a lot with webp. I would like to add this feature." Batch Process is now KrimbleBatchDialog (KrimbleBatchDialog.cpp, with the engine
    // in KrimbleBatch.cpp): recipes (resize, format JPG / PNG / WebP lossless / WebP lossy, quality, name) that can be changed and saved, WebP read and written with its transparency.
    // Only the two things that need Android are here: asking for the pictures, and saving each result in Pictures/Krimble/Batch.
    auto pickPictures = [win]() -> QStringList {
        KoFileDialog fileDialog(win, KoFileDialog::OpenFiles, "KrimbleBatch");
        fileDialog.setCaption(i18n("Choose pictures"));
        fileDialog.setMimeTypeFilters(QStringList() << QStringLiteral("image/jpeg") << QStringLiteral("image/png")
                                                    << QStringLiteral("image/webp") << QStringLiteral("image/bmp"));
        return fileDialog.filenames();
    };
    auto savePicture = [](const QString &temp, const QString &mime, const QString &name, const QString &folder) -> bool {
        QAndroidJniObject jPath = QAndroidJniObject::fromString(temp);
        QAndroidJniObject jMime = QAndroidJniObject::fromString(mime);
        QAndroidJniObject jName = QAndroidJniObject::fromString(name);
        QAndroidJniObject jFolder = QAndroidJniObject::fromString(folder);   // "Batch", or the folder a script names
        return QAndroidJniObject::callStaticMethod<jboolean>("org/krimble/android/ShareHelper", "saveToPictures",
            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z",
            jPath.object<jstring>(), jMime.object<jstring>(), jName.object<jstring>(), jFolder.object<jstring>());
    };
    KrimbleBatchDialog *batchDialog = new KrimbleBatchDialog(win, pickPictures, savePicture, QStringLiteral("Pictures/Krimble"));
    batchDialog->show();
#if 0   // Krimble 2026-10-25: the Batch Process window before the recipes (2026-10-12 to 2026-10-24: six presets, JPG and PNG only), kept
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
#endif
#else
    Q_UNUSED(win);
#endif
}

#ifdef Q_OS_ANDROID
// Krimble 2026-10-13: "Take Photo..." (Claude's idea; George: "Yes. Go for it."): opens the phone's own camera app (with its zoom, HDR, flash
// and night mode) and, when the photo is taken, opens it in Krimble. The photo is also kept in Pictures/Krimble/Camera. No camera
// permission is needed, because the phone's camera app takes the picture, not Krimble.
class KrimbleCameraReceiver : public QAndroidActivityResultReceiver
{
public:
    KrimbleCameraReceiver(KisMainWindow *win, const QString &uri) : m_win(win), m_uri(uri) {}
    void handleActivityResult(int requestCode, int resultCode, const QAndroidJniObject &data) override
    {
        Q_UNUSED(data);
        const bool taken = (requestCode == 4711 && resultCode == -1);    // -1 is RESULT_OK
        if (taken) {
            // if (m_win) m_win->openDocument(m_uri, KisMainWindow::OpenFlags());
            // Krimble 2026-10-13 (2nd): George: "Take photo is a failure. You take a pic and then crash." The camera's answer arrives on
            // Android's own thread, not on the thread that runs Krimble's windows; opening a picture from there crashed the app. The
            // opening is now queued to the window's own thread.
            // Krimble 2026-10-15: George (1.0.36): the camera opened, the photo was taken, no crash, but "The file
            // content://media/external/images/media/1000356140 does not exist." The picture is now copied into the app's cache first
            // (ShareHelper.copyCameraPhoto, on this thread) and the copy is opened like any file; if there is nothing to copy, the reason is shown.
            // KisMainWindow *window = m_win.data();
            // const QString uri = m_uri;
            // if (window) { QMetaObject::invokeMethod(window, [window, uri]() { window->openDocument(uri, KisMainWindow::OpenFlags()); }, Qt::QueuedConnection); }
            KisMainWindow *window = m_win.data();
            const QString uri = m_uri;
            QAndroidJniObject jUriString = QAndroidJniObject::fromString(uri);
            QAndroidJniObject jResult = QAndroidJniObject::callStaticObjectMethod("org/krimble/android/ShareHelper", "copyCameraPhoto",
                "(Ljava/lang/String;)Ljava/lang/String;", jUriString.object<jstring>());
            const QString result = jResult.isValid() ? jResult.toString() : QStringLiteral("ERROR: no answer from the helper");
            if (!result.startsWith(QLatin1String("ERROR")) && result != QLatin1String("EMPTY")) {
                if (window) {
                    QMetaObject::invokeMethod(window, [window, result]() {
                        window->openDocument(result, KisMainWindow::OpenFlags());
                    }, Qt::QueuedConnection);
                }
            } else {
                if (result == QLatin1String("EMPTY")) {
                    QAndroidJniObject::callStaticMethod<void>("org/krimble/android/ShareHelper", "deleteUri", "(Ljava/lang/String;)V", jUriString.object<jstring>());
                }
                if (window) {
                    const QString message = (result == QLatin1String("EMPTY"))
                        ? i18n("The camera app said it took the photo but saved nothing to the place Krimble gave it.")
                        : result;
                    QMetaObject::invokeMethod(window, [window, message]() {
                        QMessageBox::warning(window, i18n("Take Photo"), message);
                    }, Qt::QueuedConnection);
                }
            }
        } else {
            QAndroidJniObject jUri = QAndroidJniObject::fromString(m_uri);
            QAndroidJniObject::callStaticMethod<void>("org/krimble/android/ShareHelper", "deleteUri", "(Ljava/lang/String;)V", jUri.object<jstring>());
        }
        // (the receiver is not deleted here: a few bytes per photo are left; the camera app has finished with it)
    }
private:
    QPointer<KisMainWindow> m_win;
    QString m_uri;
};

static void krimbleTakePhoto(KisMainWindow *win)
{
    QAndroidJniObject jUri = QAndroidJniObject::callStaticObjectMethod("org/krimble/android/ShareHelper", "prepareCameraUri", "()Ljava/lang/String;");
    const QString uri = jUri.isValid() ? jUri.toString() : QString();
    if (uri.isEmpty()) {
        QMessageBox::warning(win, i18n("Take Photo"), i18n("The camera cannot be started on this phone (it needs Android 10 or newer)."));
        return;
    }
    QAndroidJniObject action = QAndroidJniObject::getStaticObjectField<jstring>("android/provider/MediaStore", "ACTION_IMAGE_CAPTURE");
    QAndroidJniObject intent("android/content/Intent", "(Ljava/lang/String;)V", action.object<jstring>());
    QAndroidJniObject extraOutput = QAndroidJniObject::getStaticObjectField<jstring>("android/provider/MediaStore", "EXTRA_OUTPUT");
    QAndroidJniObject uriObject = QAndroidJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", jUri.object<jstring>());
    intent.callObjectMethod("putExtra", "(Ljava/lang/String;Landroid/os/Parcelable;)Landroid/content/Intent;", extraOutput.object<jstring>(), uriObject.object());
    intent.callObjectMethod("addFlags", "(I)Landroid/content/Intent;", 3);      // read and write access to that address
    QtAndroid::startActivity(intent, 4711, new KrimbleCameraReceiver(win, uri));
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {          // no camera app
        env->ExceptionClear();
        QAndroidJniObject::callStaticMethod<void>("org/krimble/android/ShareHelper", "deleteUri", "(Ljava/lang/String;)V", jUri.object<jstring>());
        QMessageBox::warning(win, i18n("Take Photo"), i18n("No camera app was found on this phone."));
    }
}
#endif

// "Upscale...": makes the picture 2, 3 or 4 times larger with the resize method of your choice (Claude's idea; George: "Yes. Go for it.").
// Lanczos3 is the default; the other methods are the ones of Image > Image Size. Optionally a light sharpen layer is put on top
// (a layer, so it can be hidden or removed).
static void krimbleShowUpscale(KisMainWindow *win)
{
    KisViewManager *view = win->viewManager();
    if (!view || !view->image()) {
        QMessageBox::information(win, i18n("Upscale"), i18n("Open a picture first."));
        return;
    }
    KisImageSP image = view->image();
    QDialog *dialog = new QDialog(win);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(i18n("Upscale"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(8, 8, 8, 8);
    QLabel *current = new QLabel(i18n("Now: %1 x %2 px", image->width(), image->height()), dialog);
    QComboBox *factor = new QComboBox(dialog);
    factor->addItem(i18n("2 times larger"), 2);
    factor->addItem(i18n("3 times larger"), 3);
    factor->addItem(i18n("4 times larger"), 4);
    QComboBox *method = new QComboBox(dialog);
    KisFilterStrategyRegistry *strategies = KisFilterStrategyRegistry::instance();
    int lanczosIndex = 0;
    Q_FOREACH (const QString &id, strategies->keys()) {
        if (KisFilterStrategy *strategy = strategies->get(id)) {
            method->addItem(strategy->name(), id);
            if (id == QLatin1String("Lanczos3")) lanczosIndex = method->count() - 1;
        }
    }
    method->setCurrentIndex(lanczosIndex);
    QCheckBox *sharpen = new QCheckBox(i18n("Add a light sharpen layer on top"), dialog);
    sharpen->setChecked(true);
    QLabel *result = new QLabel(dialog);
    auto updateResult = [image, factor, result]() {
        const int f = factor->currentData().toInt();
        result->setText(i18n("Will be: %1 x %2 px", image->width() * f, image->height() * f));
    };
    QObject::connect(factor, QOverload<int>::of(&QComboBox::currentIndexChanged), dialog, updateResult);
    updateResult();
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
    QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    // (the lambda is "mutable": it keeps its own copy of the image pointer and calls waitForDone() on it, which a const copy does not allow)
    QObject::connect(buttons, &QDialogButtonBox::accepted, dialog, [win, view, image, dialog, factor, method, sharpen]() mutable {
        const int f = factor->currentData().toInt();
        const qint64 newWidth = qint64(image->width()) * f;
        const qint64 newHeight = qint64(image->height()) * f;
        if (newWidth * newHeight > 100LL * 1000 * 1000) {
            QMessageBox::warning(dialog, i18n("Upscale"), i18n("The result would be larger than 100 million pixels, which a phone cannot hold. Choose a smaller factor."));
            return;
        }
        KisFilterStrategy *strategy = KisFilterStrategyRegistry::instance()->get(method->currentData().toString());
        if (!strategy) return;
        const bool addSharpen = sharpen->isChecked();
        dialog->accept();
        view->imageManager()->scaleCurrentImage(QSize(int(newWidth), int(newHeight)), image->xRes(), image->yRes(), strategy);
        if (addSharpen) {
            image->waitForDone();
            KisFilterSP filter = KisFilterRegistry::instance()->value(QStringLiteral("unsharp"));
            if (filter) {
                KisFilterConfigurationSP config = filter->defaultConfiguration(KisGlobalResourcesInterface::instance());
                KisAdjustmentLayerSP sharpenLayer = new KisAdjustmentLayer(image, i18n("Upscale Sharpen"), config, KisSelectionSP());
                KisNodeCommandsAdapter adapter(view);
                adapter.addNode(sharpenLayer, image->root(), image->root()->lastChild());
            }
        }
    });
    layout->addWidget(current);
    layout->addWidget(factor);
    layout->addWidget(method);
    layout->addWidget(sharpen);
    layout->addWidget(result);
    layout->addWidget(buttons);
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        dialog->resize(int(scr->availableGeometry().width() * 0.9), dialog->sizeHint().height() + 20);
    }
    dialog->show();
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
        // Krimble 2026-10-13 (10th): Share... (George's design): right after Export Options..., with the share symbol.
        {
            QAction *shareAction = new QAction(KisIconUtils::loadIcon(QStringLiteral("krimble-share")), i18n("Share..."), file);
            QObject::connect(shareAction, &QAction::triggered, win, [win]() { krimbleShowShare(win); });
            QAction *afterExport = nullptr;
            const QList<QAction*> fileActions = file->actions();
            for (int i = 0; i < fileActions.size(); ++i) {
                QString text = fileActions.at(i)->text();
                text.remove(QLatin1Char('&'));
                if (text.startsWith(QLatin1String("Export Options")) && i + 1 < fileActions.size()) afterExport = fileActions.at(i + 1);
            }
            // file->insertAction(afterExport ? afterExport : before, shareAction);
            // Krimble 2026-10-18 (3rd): George (1.0.37, c666301): "7. Share seems to work. Let's put Share... above Exit." It is inserted below, after Take Photo..., just above Exit.
            Q_UNUSED(afterExport);
            Q_UNUSED(shareAction);
        }
        QAction *shareJpg = new QAction(i18n("Share as JPG..."), file);
        QObject::connect(shareJpg, &QAction::triggered, win, [win]() { krimbleShareImage(win, true); });
        QAction *sharePng = new QAction(i18n("Share as PNG..."), file);
        QObject::connect(sharePng, &QAction::triggered, win, [win]() { krimbleShareImage(win, false); });
        // Krimble 2026-10-12: Batch Process... (Claude's idea)
        // Krimble 2026-10-12 (2nd): George: "I think it should be in the image menu, not below exit." Moved to the Image menu
        // (krimbleInstallImageExtras); the File menu lines are kept here, commented out.
        // QAction *batch = new QAction(i18n("Batch Process..."), file);
        // QObject::connect(batch, &QAction::triggered, win, [win]() { krimbleShowBatch(win); });
        // Krimble 2026-10-13 (9th): George: "1. comment out." (Share as JPG / Share as PNG, to be tried again after other things work.)
        // QAction *sep = new QAction(file);
        // sep->setSeparator(true);
        // file->insertAction(before, sep);
        // file->insertAction(before, shareJpg);
        // file->insertAction(before, sharePng);
        // Krimble 2026-10-13: Take Photo... (Claude's idea)
        // Krimble 2026-10-13 (3rd): George: "Remove take photo. It's a failure." (1.0.35 crashed after the photo was taken.) Removed from the
        // File menu; the code (krimbleTakePhoto, KrimbleCameraReceiver, the Java helper) is kept, with the thread fix of the 115th entry.
        // QAction *takePhoto = new QAction(i18n("Take Photo..."), file);
        // QObject::connect(takePhoto, &QAction::triggered, win, [win]() { krimbleTakePhoto(win); });
        // file->insertAction(before, takePhoto);
        // Krimble 2026-10-13 (12th): George: "Re-enable and I will test your fix." Back in the File menu, in the place it had in 1.0.35 (just above
        // Exit), to test the thread fix of the 115th entry.
        // Krimble 2026-10-14: George (1.0.36): "The camera generated an error, but it didn't crash! Not crashing is good! We should still comment it
        // out temporarily." The error was: The file content://media/external/images/media/1000356140 does not exist. Out again; the code stays.
        // QAction *takePhoto = new QAction(i18n("Take Photo..."), file);
        // QObject::connect(takePhoto, &QAction::triggered, win, [win]() { krimbleTakePhoto(win); });
        // file->insertAction(before, takePhoto);
        // Krimble 2026-10-15: George: "I would like to troubleshoot Take Photo if possible." Back in the File menu, just above Exit, with the copy-first
        // fix of the 138th entry.
        QAction *takePhoto = new QAction(i18n("Take Photo..."), file);
        QObject::connect(takePhoto, &QAction::triggered, win, [win]() { krimbleTakePhoto(win); });
        file->insertAction(before, takePhoto);
        // Krimble 2026-10-18 (3rd): Share... just above Exit (below Take Photo...).
        QAction *shareLast = new QAction(KisIconUtils::loadIcon(QStringLiteral("krimble-share")), i18n("Share..."), file);
        QObject::connect(shareLast, &QAction::triggered, win, [win]() { krimbleShowShare(win); });
        file->insertAction(before, shareLast);
        // file->insertAction(before, batch);
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
        QWidget *bevelOverlay = dock->findChild<QWidget*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly);
        const QString bevelState = !bevelOverlay ? QStringLiteral("no bevel") : (bevelOverlay->isVisible() ? QStringLiteral("bevel on") : QStringLiteral("bevel hidden"));
        r += QStringLiteral("%1 %2x%3  min %4  max %5  %6  %7\n").arg(dock->objectName()).arg(dock->width()).arg(dock->height())
                .arg(dock->minimumSizeHint().width()).arg(dock->maximumWidth() > 100000 ? QStringLiteral("-") : QString::number(dock->maximumWidth())).arg(where).arg(bevelState);
    }
    Q_FOREACH (QToolBar *bar, win->findChildren<QToolBar*>()) {
        if (!bar->isVisible()) continue;
        r += QStringLiteral("bar %1 %2x%3\n").arg(bar->objectName()).arg(bar->width()).arg(bar->height());
    }
#ifdef Q_OS_ANDROID
    // Krimble 2026-10-23 (2nd): the panels' edge tabs (KrimbleDockEdgeGrab): shown, or why not (George, 1.0.42: the right panel has none)
    Q_FOREACH (QWidget *grab, win->findChildren<QWidget*>(QRegularExpression(QStringLiteral("^krimbleEdgeGrab_")), Qt::FindDirectChildrenOnly)) {
        r += static_cast<KrimbleDockEdgeGrab*>(grab)->status() + QLatin1Char('\n');
    }
#endif
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
        // Krimble 2026-10-13 (3rd): George: "Where is? Is unpredictable and has to go." Removed from the Help menu; the code
        // (krimbleShowWhereIs, the search window) is kept.
        // QAction *whereIs = help->addAction(i18n("Where is...?"));
        // QObject::connect(whereIs, &QAction::triggered, win, [win]() { krimbleShowWhereIs(win); });
        // Krimble 2026-10-12: removed (George): choosing a line in the Cheat Sheet crashed the app in 1.0.33. The code is kept.
        // QAction *cheatSheet = help->addAction(i18n("Cheat Sheet"));
        // QObject::connect(cheatSheet, &QAction::triggered, win, [win]() { krimbleShowCheatSheet(win); });
        // QAction *feedback = help->addAction(i18n("Send Feedback"));
        // QObject::connect(feedback, &QAction::triggered, win, [win]() { krimbleSendFeedback(win); });
        // Krimble 2026-10-18: George (1.0.37): "7. Send feedback does nothing. Might as well remove it." Out of the Help menu; the code stays.
        // Krimble 2026-10-13 (6th): George: "5 no": Menu Diagnostics is removed from the Help menu; the code is kept here.
#if 0
        // Krimble 2026-10-13 (2nd): Menu Diagnostics (for George's "Menus are still too wide"): shows numbers about the Edit menu
        QAction *menuDiag = help->addAction(i18n("Menu Diagnostics"));
        QObject::connect(menuDiag, &QAction::triggered, win, [win]() {
            QMenu *edit = nullptr;
            Q_FOREACH (QAction *top, win->menuBar()->actions()) {
                QString title = top->text();
                title.remove(QLatin1Char('&'));
                if (top->menu() && title == QLatin1String("Edit")) edit = top->menu();
            }
            if (!edit) return;
            const QFontMetrics fm(edit->font());
            int widest = 0, withTab = 0, shortcutVisible = 0, withShortcut = 0, withIcon = 0;
            Q_FOREACH (QAction *a, edit->actions()) {
                widest = qMax(widest, fm.horizontalAdvance(a->text().remove(QLatin1Char('&'))));
                if (a->text().contains(QLatin1Char('\t'))) ++withTab;
                if (a->isShortcutVisibleInContextMenu()) ++shortcutVisible;
                if (!a->shortcut().isEmpty()) ++withShortcut;
                if (!a->icon().isNull()) ++withIcon;
            }
            const QString report = QStringLiteral(
                "Edit menu\nsizeHint width: %1\nwidest text: %2\nactions: %3\nwith a tab in the text: %4\nshortcut shown: %5\nwith a shortcut: %6\nwith an icon: %7\n"
                "menu style: %8\napp style: %9\napp stylesheet length: %10\nmenu stylesheet length: %11\ndevice pixel ratio: %12\nfont px: %13")
                .arg(edit->sizeHint().width()).arg(widest).arg(edit->actions().size()).arg(withTab).arg(shortcutVisible).arg(withShortcut).arg(withIcon)
                .arg(QString::fromLatin1(edit->style()->metaObject()->className())).arg(QString::fromLatin1(qApp->style()->metaObject()->className()))
                .arg(qApp->styleSheet().length()).arg(edit->styleSheet().length()).arg(qApp->devicePixelRatio()).arg(edit->font().pixelSize());
            QMessageBox::information(win, i18n("Menu Diagnostics"), report);
        });
#endif
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

// Adds "Batch Process..." at the end of the Image menu, after a divider (once, when the menu first opens). It was at the end of the
// File menu, below Exit (George: "I think it should be in the image menu, not below exit.").
static void krimbleInstallImageExtras(KisMainWindow *win)
{
    QMenu *imageMenu = nullptr;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        if (top->menu() && title == QLatin1String("Image")) imageMenu = top->menu();
    }
    if (!imageMenu || imageMenu->property("krimbleImageExtras").toBool()) return;
    imageMenu->setProperty("krimbleImageExtras", true);
    QObject::connect(imageMenu, &QMenu::aboutToShow, win, [win, imageMenu]() {
        // Krimble 2026-10-30: George: "QuickAdjust should be the first item in the Image menu. It's a shortcut to adjustments." First item, then a divider
        // (the same item stays first in Image > Adjustments).
        if (!imageMenu->findChild<QAction*>(QStringLiteral("krimbleQuickAdjustTop"))) {
            QAction *quickTop = new QAction(i18n("Quick Adjust..."), imageMenu);
            quickTop->setObjectName(QStringLiteral("krimbleQuickAdjustTop"));
            QObject::connect(quickTop, &QAction::triggered, win, [win]() { krimbleShowQuickAdjustDialog(win); });
            QAction *topFirst = imageMenu->actions().value(0);
            QAction *topDivider = new QAction(imageMenu);
            topDivider->setSeparator(true);
            imageMenu->insertAction(topFirst, quickTop);
            imageMenu->insertAction(topFirst, topDivider);
        }
        if (imageMenu->findChild<QAction*>(QStringLiteral("krimbleBatchProcess"))) return;
        // Krimble 2026-10-13 (11th): Quick Adjust... at the bottom of the Image > Adjustments submenu (George: "launched from Image Adjust";
        // the place inside the submenu, the bottom, is my choice).
        Q_FOREACH (QAction *action, imageMenu->actions()) {
            QString text = action->text();
            text.remove(QLatin1Char('&'));
            if (text == QLatin1String("Adjustments") && action->menu()) {
                QMenu *adjustments = action->menu();
                QObject::connect(adjustments, &QMenu::aboutToShow, win, [win, adjustments]() {
                    if (adjustments->findChild<QAction*>(QStringLiteral("krimbleQuickAdjust"))) return;
                    QAction *quick = new QAction(i18n("Quick Adjust..."), adjustments);
                    quick->setObjectName(QStringLiteral("krimbleQuickAdjust"));
                    QObject::connect(quick, &QAction::triggered, win, [win]() { krimbleShowQuickAdjustDialog(win); });
                    // adjustments->addSeparator();
                    // adjustments->addAction(quick);          (bottom of the submenu, 1.0.36)
                    // Krimble 2026-10-14: George (1.0.36): "Should be first instead of last under Image - Adjust menu." First, then a divider.
                    QAction *firstAction = adjustments->actions().value(0);
                    QAction *divider = new QAction(adjustments);
                    divider->setSeparator(true);
                    adjustments->insertAction(firstAction, quick);
                    adjustments->insertAction(firstAction, divider);
                });
            }
        }
        // Krimble 2026-10-15: George (1.0.36): "I want Free rotation in the image rotation menu." and "Rotate the graphic like the old default transform
        // behavior. Free transform - rotate." A new item, "Free Rotate", first in Image > Image Rotation (the place inside the submenu is my choice),
        // starts the Free Transform tool, where dragging outside the box turns the graphic by any angle.
        Q_FOREACH (QAction *action, imageMenu->actions()) {
            QString text = action->text();
            text.remove(QLatin1Char('&'));
            if (text == QLatin1String("Image Rotation") && action->menu()) {
                QMenu *rotation = action->menu();
                QObject::connect(rotation, &QMenu::aboutToShow, win, [win, rotation]() {
                    if (rotation->findChild<QAction*>(QStringLiteral("krimbleFreeRotate"))) return;
                    QAction *freeRotate = new QAction(i18n("Free Rotate"), rotation);
                    freeRotate->setObjectName(QStringLiteral("krimbleFreeRotate"));
                    QObject::connect(freeRotate, &QAction::triggered, win, [win]() {
                        KisViewManager *view = win->viewManager();
                        if (!view) return;
                        // QAction *transform = view->actionCollection()->action(QStringLiteral("KisToolTransformFree"));
                        QAction *transform = view->actionCollection()->action(QStringLiteral("KisToolFreeRotate"));   // Krimble 2026-10-18: the tool that turns with two fingers
                        if (transform) transform->trigger();
                    });
                    QAction *firstAction = rotation->actions().value(0);
                    QAction *divider = new QAction(rotation);
                    divider->setSeparator(true);
                    rotation->insertAction(firstAction, freeRotate);
                    rotation->insertAction(firstAction, divider);
                });
            }
        }
        QAction *batch = new QAction(i18n("Batch Process..."), imageMenu);
        batch->setObjectName(QStringLiteral("krimbleBatchProcess"));
        QObject::connect(batch, &QAction::triggered, win, [win]() { krimbleShowBatch(win); });
        // Krimble 2026-10-13: Upscale... (Claude's idea)
        QAction *upscale = new QAction(i18n("Upscale..."), imageMenu);
        QObject::connect(upscale, &QAction::triggered, win, [win]() { krimbleShowUpscale(win); });
        imageMenu->addSeparator();
        imageMenu->addAction(upscale);
        imageMenu->addAction(batch);
    });
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

        // Krimble 2026-10-20: George (1.0.40): "Thumb tripped over the gadget on the bottom bar and blew up the image accidentally. Toggle off just
        // like the little rotate gadget on the bar." View > Zoom Gadget, right after Rotation Gadget: the zoom slider in the status bar on or off
        // (setting krimble_show_zoom_gadget, hidden by default).
        QAction *zoomGadget = viewMenu->findChild<QAction*>(QStringLiteral("krimbleZoomGadget"));
        if (!zoomGadget) {
            zoomGadget = new QAction(i18n("Zoom Gadget"), viewMenu);
            zoomGadget->setObjectName(QStringLiteral("krimbleZoomGadget"));
            zoomGadget->setCheckable(true);
            viewMenu->addAction(zoomGadget);
            QObject::connect(zoomGadget, &QAction::toggled, win, [win](bool on) {
                KisConfig cfg(false);
                cfg.writeEntry<int>("krimble_show_zoom_gadget", on ? 1 : 0);
                if (QWidget *slider = win->statusBar() ? win->statusBar()->findChild<QWidget*>(QStringLiteral("krimbleZoomGadget")) : nullptr) {
                    slider->setVisible(on);
                }
            });
        }
        QSignalBlocker zoomBlocker(zoomGadget);
        zoomGadget->setChecked(KisConfig(true).readEntry<int>("krimble_show_zoom_gadget", 0) != 0);
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
            // Krimble 2026-10-18 (5th): a menu that is on the screen but is not the active popup (the case of "stuck") is hidden too
            Q_FOREACH (QWidget *top, QApplication::topLevelWidgets()) {
                if (QMenu *shownMenu = qobject_cast<QMenu*>(top)) {
                    if (shownMenu->isVisible()) shownMenu->hide();
                }
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
// Krimble 2026-10-15: George (1.0.36): "Floating windows have very small buttons for close and stuff, and ugly aqua colored title bar. Make
// tiny adjustment to increase size, then make the bar blue." and, asked whether the color should match the UI: "I guess the blue gray." The title bar
// of an image window (QMdiSubWindow) is drawn by the style: its height, which also sizes its buttons, is `PM_TitleBarHeight`, and its color is the
// palette's highlight (the aqua). This style is put on the image windows only: the title bar is 25% higher (so its buttons are 25% bigger) and
// drawn in the app's blue-gray, rgb(55,57,82), "Krimble dark". It is my best guess that this is the bar George means; not seen on a phone.
// Krimble 2026-10-17: George (1.0.37): "1. Giant mega lines again for bevels instead of one pixel." and "8. Docked panels have pointless mega bevel lines.
// Detached panels have no bevel lines at all. See how the Tool Options panel disappears into the toolbar cluster because there are no hilight or
// shadow lines on the edges? And they should be only one pixel thick on each edge. Toolbars need them too." A transparent overlay is put on every
// panel and every toolbar. A docked panel gets a light line along its top and a dark line along its bottom; a detached panel and a toolbar get a
// light line along the top and left edges and a dark line along the bottom and right edges (a raised look). Each line is ONE DEVICE PIXEL thick.
// (This replaces the lines drawn by KoDockWidgetTitleBar of the 135th entry, which were 3 device px each and only on docked panels.)
class KrimbleBevelOverlay : public QWidget
{
public:
    explicit KrimbleBevelOverlay(QWidget *target) : QWidget(target), m_target(target)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setObjectName(QStringLiteral("krimbleBevelOverlay"));
        target->installEventFilter(this);
        reposition();
    }
    void reposition()
    {
        if (!m_target) return;
        setGeometry(m_target->rect());
        raise();
        show();
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_target) {
            switch (event->type()) {
            case QEvent::Resize:
            case QEvent::Show:
            case QEvent::LayoutRequest:
                reposition();
                break;
            case QEvent::ChildAdded:
                QTimer::singleShot(0, this, [this]() { raise(); });
                break;
            default:
                break;
            }
        }
        return false;
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, false);
        const qreal one = 1.0 / qMax<qreal>(1.0, devicePixelRatioF());   // one device pixel
        const QColor light(110, 114, 156);
        const QColor dark(18, 19, 34);
        const qreal w = width();
        const qreal h = height();
        QDockWidget *dock = qobject_cast<QDockWidget*>(m_target.data());
        const bool allEdges = !dock || dock->isFloating();
        p.fillRect(QRectF(0, 0, w, one), light);
        p.fillRect(QRectF(0, h - one, w, one), dark);
        if (allEdges) {
            p.fillRect(QRectF(0, 0, one, h), light);
            p.fillRect(QRectF(w - one, 0, one, h), dark);
        }
    }

private:
    QPointer<QWidget> m_target;
};

// Krimble 2026-10-18 (15th): the title text of a panel is hidden when the panel is in a tab group (docked, with other panels tabbed with it); it shows again when it is alone or floating
static void krimbleRefreshTabTitles(QMainWindow *win)
{
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        KoDockWidgetTitleBar *bar = dynamic_cast<KoDockWidgetTitleBar*>(dock->titleBarWidget());
        if (!bar) continue;
        const bool tabbed = !dock->isFloating() && !win->tabifiedDockWidgets(dock).isEmpty();
        bar->setTitleTextHidden(tabbed);
    }
}

static void krimbleInstallBevels(QMainWindow *win)
{
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (!dock->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(dock);
    }
    Q_FOREACH (QToolBar *bar, win->findChildren<QToolBar*>()) {
        if (!bar->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(bar);
    }
    // Krimble 2026-10-18 (4th): George (1.0.37, fc126d8): "I don't see any edges." (the one-pixel light and dark lines of menus, finding 11). The first way, the
    // PE_FrameMenu of the app's style, was never reached: the theme's style sheet draws menus itself. The same overlay as for panels and toolbars is put on every
    // menu and submenu (light on the top and left edges, dark on the bottom and right, one device pixel).
    Q_FOREACH (QMenu *menu, win->findChildren<QMenu*>()) {
        if (!menu->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(menu);
    }
    // Krimble 2026-10-18 (15th): the double titles of panels in tabs (see krimbleRefreshTabTitles); at once when a tab is chosen, and every 2 s with the lines above
    static bool s_tabTitlesHooked = false;
    if (!s_tabTitlesHooked) {
        s_tabTitlesHooked = true;
        QObject::connect(win, &QMainWindow::tabifiedDockWidgetActivated, win, [win](QDockWidget*) { krimbleRefreshTabTitles(win); });
    }
    krimbleRefreshTabTitles(win);
}

// Krimble 2026-10-18: HEX MAPS (George: "Can we offer a hex mapping feature?" and "View-show-hex and we use an svg to overlay a hex pattern which they
// can pinch zoom/scale to make it easier to make hex maps. Add a layer that's a hex pattern and the user sets the scale." and "Hexes are drawn as vector
// art. User can set the line thickness."). Two things: (1) View > Show > Hex: a hexagon pattern drawn over the canvas (a canvas decoration, like the
// grid), which follows the zoom; its size is set with View > Show > Hex Size...; (2) Layer > New > Hex Pattern...: a VECTOR layer (a KisShapeLayer) holding
// the pattern as one path shape, so the lines stay sharp at any zoom, with the size, the line thickness and the color chosen by the user.
// A hexagon is described by its "size" = the distance from the center to a corner, in image pixels. Pointy-top: columns sqrt(3) x size apart, rows 1.5 x size
// apart, every second row shifted by half a column. Flat-top: the same turned by a quarter turn.
static void krimbleHexCenters(qreal size, bool flatTop, const QRectF &area, const std::function<void(const QPointF&)> &visit)
{
    const qreal s3 = 1.7320508075688772;
    if (size <= 0.0) return;
    if (!flatTop) {
        const qreal colWidth = s3 * size;
        const qreal rowHeight = 1.5 * size;
        const int firstRow = int(qFloor(area.top() / rowHeight)) - 1;
        const int lastRow = int(qCeil(area.bottom() / rowHeight)) + 1;
        for (int row = firstRow; row <= lastRow; ++row) {
            const qreal shift = (row & 1) ? colWidth / 2.0 : 0.0;
            const int firstCol = int(qFloor((area.left() - shift) / colWidth)) - 1;
            const int lastCol = int(qCeil((area.right() - shift) / colWidth)) + 1;
            for (int col = firstCol; col <= lastCol; ++col) visit(QPointF(col * colWidth + shift, row * rowHeight));
        }
    } else {
        const qreal colWidth = 1.5 * size;
        const qreal rowHeight = s3 * size;
        const int firstCol = int(qFloor(area.left() / colWidth)) - 1;
        const int lastCol = int(qCeil(area.right() / colWidth)) + 1;
        for (int col = firstCol; col <= lastCol; ++col) {
            const qreal shift = (col & 1) ? rowHeight / 2.0 : 0.0;
            const int firstRow = int(qFloor((area.top() - shift) / rowHeight)) - 1;
            const int lastRow = int(qCeil((area.bottom() - shift) / rowHeight)) + 1;
            for (int row = firstRow; row <= lastRow; ++row) visit(QPointF(col * colWidth, row * rowHeight + shift));
        }
    }
}

static QPolygonF krimbleHexPolygon(const QPointF &center, qreal size, bool flatTop)
{
    QPolygonF polygon;
    for (int i = 0; i < 6; ++i) {
        const qreal angle = qDegreesToRadians((flatTop ? 0.0 : 30.0) + 60.0 * i);
        polygon << QPointF(center.x() + size * qCos(angle), center.y() + size * qSin(angle));
    }
    return polygon;
}

class KrimbleHexDecoration : public KisCanvasDecoration
{
public:
    explicit KrimbleHexDecoration(KisView *parent) : KisCanvasDecoration(QStringLiteral("krimblehex"), parent)
    {
        setPriority(1);
    }
    qreal size = 100.0;      // center to corner, in image pixels
    bool flatTop = false;
    bool adjusting = false;  // George: "Hex overlay should be pinch zoom then drop with a tap." While true, a pinch scales the hexes and a tap drops them.

protected:
    void drawDecoration(QPainter &gc, const QRectF &updateArea, const KisCoordinatesConverter *converter, KisCanvas2 *canvas) override
    {
        Q_UNUSED(canvas);
        const QTransform toWidget = converter->imageToWidgetTransform();
        const qreal scale = KoUnit::approxTransformScale(toWidget);
        if (scale * size < 6.0) return;                       // too small to be of any use: the same rule as the grid
        const QRectF imageArea = toWidget.inverted().mapRect(updateArea);
        gc.save();
        gc.setRenderHint(QPainter::Antialiasing, true);
        QPen pen(QColor(30, 30, 30, 210), 1.0);
        pen.setCosmetic(true);
        gc.setPen(pen);
        gc.setBrush(Qt::NoBrush);
        const qreal sz = size;
        const bool flat = flatTop;
        krimbleHexCenters(sz, flat, imageArea.adjusted(-sz, -sz, sz, sz), [&](const QPointF &center) {
            gc.drawPolygon(toWidget.map(krimbleHexPolygon(center, sz, flat)));
        });
        if (adjusting) {
            // a note while the pattern is being scaled
            const QString note = i18n("Pinch to scale the hexes, tap to drop");
            QFont noteFont = gc.font();
            noteFont.setPointSizeF(noteFont.pointSizeF() * 1.4);
            noteFont.setBold(true);
            gc.setFont(noteFont);
            const QRectF window = gc.window();
            QRectF plate = QFontMetricsF(noteFont).boundingRect(note);
            plate.adjust(-14, -8, 14, 8);
            plate.moveCenter(QPointF(window.center().x(), window.top() + plate.height()));
            gc.setPen(Qt::NoPen);
            gc.setBrush(QColor(18, 19, 34, 220));
            gc.drawRoundedRect(plate, 6, 6);
            gc.setPen(QColor(230, 230, 230));
            gc.drawText(plate, Qt::AlignCenter, note);
        }
        gc.restore();
    }
};

// the small window that asks for the size, the orientation and (for the layer) the line thickness and color
static bool krimbleAskHex(QMainWindow *win, bool forLayer, qreal *size, bool *flatTop, qreal *thickness, QColor *color)
{
    QDialog dialog(win);
    dialog.setWindowTitle(forLayer ? i18n("Hex Pattern") : i18n("Hex Size"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QLabel *sizeLabel = new QLabel(i18n("Hex size (center to corner, pixels):"), &dialog);
    QDoubleSpinBox *sizeBox = new QDoubleSpinBox(&dialog);
    sizeBox->setRange(4.0, 5000.0);
    sizeBox->setDecimals(1);
    sizeBox->setValue(*size);
    QCheckBox *flatBox = new QCheckBox(i18n("Flat-top (otherwise pointy-top)"), &dialog);
    flatBox->setChecked(*flatTop);
    layout->addWidget(sizeLabel);
    layout->addWidget(sizeBox);
    layout->addWidget(flatBox);
    QDoubleSpinBox *thicknessBox = nullptr;
    QPushButton *colorButton = nullptr;
    QColor chosen = color ? *color : QColor(Qt::black);
    if (forLayer) {
        QLabel *thicknessLabel = new QLabel(i18n("Line thickness (pixels):"), &dialog);
        thicknessBox = new QDoubleSpinBox(&dialog);
        thicknessBox->setRange(0.1, 200.0);
        thicknessBox->setDecimals(1);
        thicknessBox->setValue(*thickness);
        colorButton = new QPushButton(i18n("Line color..."), &dialog);
        auto paintColorButton = [colorButton, &chosen]() {
            colorButton->setStyleSheet(QStringLiteral("QPushButton { border-left: 24px solid %1; }").arg(chosen.name()));
        };
        paintColorButton();
        QObject::connect(colorButton, &QPushButton::clicked, &dialog, [&]() {
            const QColor picked = QColorDialog::getColor(chosen, &dialog, i18n("Line color"));
            if (picked.isValid()) {
                chosen = picked;
                paintColorButton();
            }
        });
        layout->addWidget(thicknessLabel);
        layout->addWidget(thicknessBox);
        layout->addWidget(colorButton);
    }
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        dialog.resize(int(scr->availableGeometry().width() * 0.9), dialog.sizeHint().height());
    }
    if (dialog.exec() != QDialog::Accepted) return false;
    *size = sizeBox->value();
    *flatTop = flatBox->isChecked();
    if (forLayer) {
        *thickness = thicknessBox->value();
        if (color) *color = chosen;
    }
    return true;
}

static qreal g_krimbleHexSize = 100.0;
static bool g_krimbleHexFlat = false;
static qreal g_krimbleHexThickness = 2.0;
// Krimble 2026-10-18 (8th): George: "The hexes disappeared at one point while I was working with them." Asked what he had just done: "3" (switched pictures or left the app). The overlay was
// kept by the picture window (KisView) it was switched on in, so another picture started without it, and an app that was closed by Android started without it too. Whether the hexes are
// on, and their size and orientation, are now kept for the whole program (these three) and saved in the settings; the overlay follows the picture that is open.
static bool g_krimbleHexOn = false;
static void krimbleHexSaveState()
{
    KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleHex");
    g.writeEntry("on", g_krimbleHexOn);
    g.writeEntry("size", double(g_krimbleHexSize));
    g.writeEntry("flat", g_krimbleHexFlat);
    g.sync();
}
static void krimbleHexLoadState()
{
    KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleHex");
    g_krimbleHexOn = g.readEntry("on", false);
    g_krimbleHexSize = g.readEntry("size", double(g_krimbleHexSize));
    g_krimbleHexFlat = g.readEntry("flat", g_krimbleHexFlat);
}
static QColor g_krimbleHexColor = QColor(Qt::black);

// George: "Hex overlay should be pinch zoom then drop with a tap." While the overlay is being adjusted, two fingers scale the hexes (the distance between
// them against its last value), a short single tap that does not move drops them (the size is kept; the pattern then follows the canvas as usual), and
// other touches on the canvas are held back so that the canvas does not zoom or paint at the same time.
class KrimbleHexAdjustFilter : public QObject
{
public:
    KrimbleHexAdjustFilter(QWidget *widget, KrimbleHexDecoration *deco, KisCanvas2 *canvas)
        : QObject(widget), m_widget(widget), m_deco(deco), m_canvas(canvas)
    {
        widget->installEventFilter(this);
    }
    // George: "Yes" to: when the overlay is dropped with a tap, ask "Make this a layer?" (a transparent vector layer with the hexes at that size)
    void setOnDrop(const std::function<void()> &onDrop) { m_onDrop = onDrop; }
    void finish(bool dropped = false)
    {
        krimbleHexSaveState();      // Krimble 2026-10-18 (8th): the size set by pinching is kept
        if (m_deco) m_deco->adjusting = false;
        if (m_widget) m_widget->removeEventFilter(this);
        if (m_canvas) m_canvas->updateCanvas();
        const std::function<void()> callback = m_onDrop;
        if (dropped && callback) {
            QTimer::singleShot(0, qApp, [callback]() { callback(); });
        }
        deleteLater();
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        Q_UNUSED(watched);
        if (!m_deco || !m_deco->adjusting) {
            finish();
            return false;
        }
        const QEvent::Type type = event->type();
        if (type != QEvent::TouchBegin && type != QEvent::TouchUpdate && type != QEvent::TouchEnd) return false;
        QTouchEvent *touch = static_cast<QTouchEvent*>(event);
        const QList<QTouchEvent::TouchPoint> points = touch->touchPoints();
        if (points.isEmpty()) return true;
        if (type == QEvent::TouchBegin) {
            m_pinched = false;
            m_moved = false;
            m_start = points.first().pos();
            m_lastDistance = 0.0;
            m_clock.start();
        }
        if (points.size() >= 2) {
            m_pinched = true;
            const qreal distance = QLineF(points.at(0).pos(), points.at(1).pos()).length();
            if (m_lastDistance > 0.0 && distance > 0.0) {
                m_deco->size = qBound<qreal>(8.0, m_deco->size * distance / m_lastDistance, 3000.0);
                g_krimbleHexSize = m_deco->size;
                if (m_canvas) m_canvas->updateCanvas();
            }
            m_lastDistance = distance;
        } else {
            m_lastDistance = 0.0;
            if (QLineF(points.first().pos(), m_start).length() > 24.0) m_moved = true;
            if (type == QEvent::TouchEnd && !m_pinched && !m_moved && m_clock.elapsed() < 600) {
                finish(true);
            }
        }
        return true;
    }

private:
    QPointer<QWidget> m_widget;
    QPointer<KrimbleHexDecoration> m_deco;
    QPointer<KisCanvas2> m_canvas;
    QPointF m_start;
    qreal m_lastDistance {0.0};
    bool m_pinched {false};
    bool m_moved {false};
    QElapsedTimer m_clock;
    std::function<void()> m_onDrop;
};

static KrimbleHexDecoration *krimbleHexDecorationOf(KisMainWindow *win, bool create)
{
    KisViewManager *manager = win->viewManager();
    KisView *view = win->activeView();   // Krimble 2026-10-18: KisMainWindow::activeView() (KisViewManager has none)
    if (!view || !view->canvasBase()) return nullptr;
    KisCanvasDecorationSP existing = view->canvasBase()->decoration(QStringLiteral("krimblehex"));
    KrimbleHexDecoration *hex = dynamic_cast<KrimbleHexDecoration*>(existing.data());
    if (!hex && create) {
        hex = new KrimbleHexDecoration(view);
        view->canvasBase()->addDecoration(hex);
    }
    return hex;
}

// puts the hex overlay on the picture that is open now, if the hexes are on
static void krimbleHexFollowView(KisMainWindow *win)
{
    if (!g_krimbleHexOn) return;
    KrimbleHexDecoration *deco = krimbleHexDecorationOf(win, true);
    if (!deco) return;
    deco->size = g_krimbleHexSize;
    deco->flatTop = g_krimbleHexFlat;
    deco->adjusting = false;
    deco->setVisible(true);
    if (win->activeView() && win->activeView()->canvasBase()) win->activeView()->canvasBase()->updateCanvas();
}

static void krimbleMakeHexLayer(KisMainWindow *win)
{
    KisViewManager *manager = win->viewManager();
    if (!manager || !manager->image() || !manager->nodeManager()) {
        QMessageBox::information(win, i18n("Hex Pattern"), i18n("Open a picture first."));
        return;
    }
    if (!krimbleAskHex(win, true, &g_krimbleHexSize, &g_krimbleHexFlat, &g_krimbleHexThickness, &g_krimbleHexColor)) return;
    KisImageSP image = manager->image();
    const QRectF bounds = image->bounds();
    const qreal size = g_krimbleHexSize;
    const qreal estimate = (bounds.width() / (1.7320508 * size) + 2.0) * (bounds.height() / (1.5 * size) + 2.0);
    if (estimate > 15000.0) {
        QMessageBox::information(win, i18n("Hex Pattern"), i18n("That would be %1 hexes. Choose a bigger size.", qRound(estimate)));
        return;
    }
    KisNodeSP node = manager->nodeManager()->createNode(QStringLiteral("KisShapeLayer"));
    KisShapeLayer *layer = dynamic_cast<KisShapeLayer*>(node.data());
    if (!layer) {
        QMessageBox::warning(win, i18n("Hex Pattern"), i18n("The vector layer could not be made."));
        return;
    }
    // a vector shape lives in points: image pixels per point is xRes()
    const qreal pointsPerPixel = image->xRes() > 0.0 ? 1.0 / image->xRes() : 1.0;
    KoPathShape *path = new KoPathShape();
    path->setShapeId(KoPathShapeId);
    const bool flat = g_krimbleHexFlat;
    krimbleHexCenters(size, flat, bounds.adjusted(-size, -size, size, size), [&](const QPointF &center) {
        const QPolygonF hex = krimbleHexPolygon(center, size, flat);
        path->moveTo(hex.at(0) * pointsPerPixel);
        for (int i = 1; i < hex.size(); ++i) path->lineTo(hex.at(i) * pointsPerPixel);
        path->close();
    });
    path->normalize();
    path->setStroke(toQShared(new KoShapeStroke(g_krimbleHexThickness * pointsPerPixel, g_krimbleHexColor)));
    path->setBackground(QSharedPointer<KoShapeBackground>());
    path->setName(QStringLiteral("Hex pattern"));
    layer->addShape(path);
    node->setName(image->nextLayerName(i18n("Hex Pattern")));
}

// Krimble 2026-10-30: Layer > New > Star... George: "The star tool in Illustrator is a cool tool. it's essentially two invisible circles with equidistant
// vertices on them and lines drawn between them to make the star shapes. The user changes the number of points to change the star and moves the inner
// circle inward or outward. The whole thing can be rotated as a vector object." and "Instead of a new tool in the already full toolbox, we can just have a menu
// item called Star or Create Star somewhere appropriate." and, of the plan (a vector layer, the Points and Inner Radius in a small dialog, the foreground color as
// the fill, no outline): "sounds great". The star is Krita's own StarShape (shape id "StarShape": corner count, tip radius = the outer circle, base radius = the
// inner circle), on a new vector layer in the middle of the picture. It can be moved, scaled and rotated with the Select Shapes tool, and its circles changed
// with the shape edit handles. Only the dialog's last values are remembered while the program runs.
static int g_krimbleStarPoints = 5;
static int g_krimbleStarInner = 40;      // percent of the outer circle
static int g_krimbleStarSize = 0;        // outer circle diameter in pixels; 0 = half of the picture's shorter side

static void krimbleMakeStarLayer(KisMainWindow *win)
{
    KisViewManager *manager = win->viewManager();
    if (!manager || !manager->image() || !manager->nodeManager()) {
        QMessageBox::information(win, i18n("Star"), i18n("Open a picture first."));
        return;
    }
    KisImageSP image = manager->image();
    const QRectF bounds = image->bounds();
    if (g_krimbleStarSize <= 0) g_krimbleStarSize = qMax(10, int(qMin(bounds.width(), bounds.height()) / 2.0));

    QDialog dialog(win);
    dialog.setWindowTitle(i18n("Star"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QSpinBox *pointsBox = new QSpinBox(&dialog);
    pointsBox->setRange(3, 100);
    pointsBox->setValue(g_krimbleStarPoints);
    QSpinBox *innerBox = new QSpinBox(&dialog);
    innerBox->setRange(5, 95);
    innerBox->setSuffix(QStringLiteral(" %"));
    innerBox->setValue(g_krimbleStarInner);
    QSpinBox *sizeBox = new QSpinBox(&dialog);
    sizeBox->setRange(10, qMax(100, int(qMax(bounds.width(), bounds.height()) * 2.0)));
    sizeBox->setSuffix(QStringLiteral(" px"));
    sizeBox->setValue(g_krimbleStarSize);
    layout->addWidget(new QLabel(i18n("Points:"), &dialog));
    layout->addWidget(pointsBox);
    layout->addWidget(new QLabel(i18n("Inner circle (percent of the outer circle):"), &dialog));
    layout->addWidget(innerBox);
    layout->addWidget(new QLabel(i18n("Size (width of the outer circle):"), &dialog));
    layout->addWidget(sizeBox);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;
    g_krimbleStarPoints = pointsBox->value();
    g_krimbleStarInner = innerBox->value();
    g_krimbleStarSize = sizeBox->value();

    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(QStringLiteral("StarShape"));
    if (!factory) {
        QMessageBox::warning(win, i18n("Star"), i18n("The star shape is not available in this build."));
        return;
    }
    KisNodeSP node = manager->nodeManager()->createNode(QStringLiteral("KisShapeLayer"));
    KisShapeLayer *layer = dynamic_cast<KisShapeLayer*>(node.data());
    if (!layer) {
        QMessageBox::warning(win, i18n("Star"), i18n("The vector layer could not be made."));
        return;
    }
    // a vector shape lives in points: image pixels per point is xRes() (as in the Hex Pattern layer)
    const qreal pointsPerPixel = image->xRes() > 0.0 ? 1.0 / image->xRes() : 1.0;
    const qreal tipRadius = (g_krimbleStarSize / 2.0) * pointsPerPixel;
    KoProperties props;
    props.setProperty("corners", g_krimbleStarPoints);
    props.setProperty("convex", false);
    props.setProperty("tipRadius", tipRadius);
    props.setProperty("baseRadius", tipRadius * g_krimbleStarInner / 100.0);
    props.setProperty("baseRoundness", 0.0);
    props.setProperty("tipRoundness", 0.0);
    QVariant fill;
    fill.setValue(manager->canvasResourceProvider()->fgColor().toQColor());   // the foreground color is the fill
    props.setProperty("background", fill);
    KoShape *shape = factory->createShape(&props, nullptr);
    if (!shape) {
        QMessageBox::warning(win, i18n("Star"), i18n("The star could not be made."));
        return;
    }
    shape->setStroke(KoShapeStrokeModelSP());   // no outline
    shape->setName(QStringLiteral("Star"));
    shape->setAbsolutePosition(QPointF(bounds.center().x() * pointsPerPixel, bounds.center().y() * pointsPerPixel), KoFlake::Center);
    layer->addShape(shape);
    node->setName(image->nextLayerName(i18n("Star")));
    // so that it can be moved, rotated and its circles changed at once
    QTimer::singleShot(0, win, []() { KoToolManager::instance()->switchToolRequested(QStringLiteral("InteractionTool")); });
}

// Krimble 2026-10-30: George: "All panels with selections like little buttons or whatever should have the blue glow if possible when selected." and "use that
// light up blue glow for selections like the mockups." A checked tool button or push button (the selected tool in the toolbox, a pressed-in option button in a
// panel or a dialog) gets a soft blue glow around it. The style sheet cannot blur, so this is a drop shadow effect with no offset, in light blue, put on the
// button when it is checked and taken off when it is not. Check boxes and radio buttons are left alone (they are not "little buttons"). Every button is found when
// it is first shown (the filter sees its Polish event) and the ones that exist already are found when the filter is installed.
static void krimbleSetButtonGlow(QAbstractButton *button, bool on)
{
    if (on) {
        if (button->graphicsEffect()) return;       // it has an effect already (this one, or another that is not ours)
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(button);
        glow->setColor(QColor(56, 198, 255, 230));  // the blue of the selected tab and checked button
        glow->setBlurRadius(18);
        glow->setOffset(0, 0);
        button->setGraphicsEffect(glow);
    } else if (qobject_cast<QGraphicsDropShadowEffect*>(button->graphicsEffect())) {
        button->setGraphicsEffect(nullptr);
    }
}

class KrimbleGlowFilter : public QObject
{
public:
    explicit KrimbleGlowFilter(QObject *parent) : QObject(parent) {}
    void hook(QAbstractButton *button)
    {
        if (!button || !button->isCheckable() || button->property("krimbleGlowHooked").toBool()) return;
        if (!qobject_cast<QToolButton*>(button) && !qobject_cast<QPushButton*>(button)) return;   // not check boxes or radio buttons
        button->setProperty("krimbleGlowHooked", true);
        QObject::connect(button, &QAbstractButton::toggled, button, [button](bool on) { krimbleSetButtonGlow(button, on); });
        krimbleSetButtonGlow(button, button->isChecked());
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::Polish) {
            if (QAbstractButton *button = qobject_cast<QAbstractButton*>(watched)) hook(button);
        }
        return false;
    }
};

static void krimbleInstallSelectionGlow(KisMainWindow *win)
{
    if (win->findChild<KrimbleGlowFilter*>(QStringLiteral("krimbleGlowFilter"))) return;
    KrimbleGlowFilter *filter = new KrimbleGlowFilter(win);
    filter->setObjectName(QStringLiteral("krimbleGlowFilter"));
    qApp->installEventFilter(filter);
    Q_FOREACH (QAbstractButton *button, win->findChildren<QAbstractButton*>()) {
        filter->hook(button);
    }
}

// View > Show > Hex (a check mark) and Hex Size...; Layer > New > Hex Pattern...
static void krimbleInstallHexMenus(KisMainWindow *win)
{
    // Krimble 2026-10-18 (8th): once: the saved state is read, and the overlay follows the open picture (see g_krimbleHexOn)
    static bool s_hexFollowHooked = false;
    if (!s_hexFollowHooked) {
        s_hexFollowHooked = true;
        krimbleHexLoadState();
        QObject::connect(win, &KisMainWindow::activeViewChanged, win, [win]() {
            QTimer::singleShot(300, win, [win]() { krimbleHexFollowView(win); });
        });
    }
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QMenu *menu = top->menu();
        if (!menu || menu->property("krimbleHexMenus").toBool()) continue;
        QString topTitle = top->text();
        topTitle.remove(QLatin1Char('&'));
        const bool isView = (topTitle == QLatin1String("View"));
        const bool isLayer = (topTitle == QLatin1String("Layer"));
        if (!isView && !isLayer) continue;
        menu->setProperty("krimbleHexMenus", true);
        QObject::connect(menu, &QMenu::aboutToShow, menu, [win, menu, isView, isLayer]() {
            Q_FOREACH (QAction *action, menu->actions()) {
                QString text = action->text();
                text.remove(QLatin1Char('&'));
                QMenu *sub = action->menu();
                if (!sub) continue;
                if (isView && text == QLatin1String("Show")) {
                    if (sub->findChild<QAction*>(QStringLiteral("krimbleHex"))) {
                        QAction *hexAction = sub->findChild<QAction*>(QStringLiteral("krimbleHex"));
                        KrimbleHexDecoration *deco = krimbleHexDecorationOf(win, false);
                        QSignalBlocker blocker(hexAction);
                        hexAction->setChecked(deco && deco->visible());
                        continue;
                    }
                    QAction *hexAction = new QAction(i18n("Hex"), sub);
                    hexAction->setObjectName(QStringLiteral("krimbleHex"));
                    hexAction->setCheckable(true);
                    QObject::connect(hexAction, &QAction::toggled, win, [win](bool on) {
                        g_krimbleHexOn = on;          // Krimble 2026-10-18 (8th): on for every picture, and saved
                        krimbleHexSaveState();
                        KrimbleHexDecoration *deco = krimbleHexDecorationOf(win, on);
                        if (!deco) return;
                        deco->size = g_krimbleHexSize;
                        deco->flatTop = g_krimbleHexFlat;
                        deco->setVisible(on);
                        KisViewManager *manager = win->viewManager();
                        KisCanvas2 *hexCanvas = (manager && win->activeView()) ? win->activeView()->canvasBase() : nullptr;
                        if (on && hexCanvas && hexCanvas->canvasWidget()) {
                            // George: "Hex overlay should be pinch zoom then drop with a tap."
                            deco->adjusting = true;
                            KrimbleHexAdjustFilter *adjustFilter = new KrimbleHexAdjustFilter(hexCanvas->canvasWidget(), deco, hexCanvas);
                            adjustFilter->setOnDrop([win]() {
                                if (QMessageBox::question(win, i18n("Hex Pattern"),
                                        i18n("Make this a layer? It is a transparent vector layer with the hexes at the size you set."),
                                        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                                    krimbleMakeHexLayer(win);
                                }
                            });
                        } else {
                            deco->adjusting = false;
                        }
                        if (hexCanvas) hexCanvas->updateCanvas();
                    });
                    QAction *sizeAction = new QAction(i18n("Hex Size..."), sub);
                    sizeAction->setObjectName(QStringLiteral("krimbleHexSize"));
                    QObject::connect(sizeAction, &QAction::triggered, win, [win]() {
                        qreal thickness = g_krimbleHexThickness;
                        QColor color = g_krimbleHexColor;
                        if (!krimbleAskHex(win, false, &g_krimbleHexSize, &g_krimbleHexFlat, &thickness, &color)) return;
                        krimbleHexSaveState();        // Krimble 2026-10-18 (8th)
                        KrimbleHexDecoration *deco = krimbleHexDecorationOf(win, false);
                        if (deco) {
                            deco->size = g_krimbleHexSize;
                            deco->flatTop = g_krimbleHexFlat;
                            KisViewManager *manager = win->viewManager();
                            if (manager && win->activeView() && win->activeView()->canvasBase()) win->activeView()->canvasBase()->updateCanvas();
                        }
                    });
                    sub->addAction(hexAction);
                    sub->addAction(sizeAction);
                } else if (isLayer && text == QLatin1String("New")) {
                    // Krimble 2026-10-30: Layer > New > Star... (see krimbleMakeStarLayer), above Hex Pattern...
                    if (!sub->findChild<QAction*>(QStringLiteral("krimbleStarLayer"))) {
                        QAction *starAction = new QAction(i18n("Star..."), sub);
                        starAction->setObjectName(QStringLiteral("krimbleStarLayer"));
                        QObject::connect(starAction, &QAction::triggered, win, [win]() { krimbleMakeStarLayer(win); });
                        sub->addAction(starAction);
                    }
                    if (sub->findChild<QAction*>(QStringLiteral("krimbleHexLayer"))) continue;
                    QAction *layerAction = new QAction(i18n("Hex Pattern..."), sub);
                    layerAction->setObjectName(QStringLiteral("krimbleHexLayer"));
                    QObject::connect(layerAction, &QAction::triggered, win, [win]() { krimbleMakeHexLayer(win); });
                    sub->addAction(layerAction);
                }
            }
        });
    }
}

class KrimbleMdiTitleStyle : public QProxyStyle
{
public:
    KrimbleMdiTitleStyle() : QProxyStyle(QApplication::style()->objectName()) {}
    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override
    {
        const int base = QProxyStyle::pixelMetric(metric, option, widget);
        if (metric == QStyle::PM_TitleBarHeight) return int(base * 1.25);
        return base;
    }
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override
    {
        if (control == QStyle::CC_TitleBar) {
            if (const QStyleOptionTitleBar *titleBar = qstyleoption_cast<const QStyleOptionTitleBar*>(option)) {
                QStyleOptionTitleBar copy(*titleBar);
                copy.palette.setColor(QPalette::Highlight, QColor(55, 57, 82));
                copy.palette.setColor(QPalette::HighlightedText, QColor(230, 230, 230));
                // QProxyStyle::drawComplexControl(control, &copy, painter, widget);
                // Krimble 2026-10-20: George (1.0.41, screenshot of an image window's title): "Image window title text is weird double image." The base style
                // (Fusion) draws the title twice, a light copy one logical pixel down and right as a shadow and the text on top; at this phone's density
                // (3.19) the shadow is 3 device pixels away and shows as a double image. The bar is drawn without its text, and the text is drawn once.
                const QString title = copy.text;
                copy.text = QString();
                QProxyStyle::drawComplexControl(control, &copy, painter, widget);
                const QRect textRect = subControlRect(control, titleBar, QStyle::SC_TitleBarLabel, widget);
                painter->save();
                QFont titleFont = painter->font();
                titleFont.setBold(true);
                painter->setFont(titleFont);
                painter->setPen(QColor(230, 230, 230));
                painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter,
                                  QFontMetrics(titleFont).elidedText(title, Qt::ElideRight, textRect.width()));
                painter->restore();
                return;
            }
        }
        QProxyStyle::drawComplexControl(control, option, painter, widget);
    }
};

class KrimbleDockTolerance : public QObject
{
public:
    // static const int Distance = 36;
    // Krimble 2026-10-12: George: "Panels should not try to snap into place until they are right on top of the edge they are going to
    // snap to." 8 logical px (about 27 on the screen), was 36.
    // static const int Distance = 8;
    // Krimble 2026-10-18 (2nd): George (1.0.37, fc126d8): "4. When a panel is detached by dragging it is still way too eager to reattach. Distance is an issue."
    // 8 -> 4 logical px; and a panel can join another panel (tabs) only after the finger has moved 120 logical px from where the drag began.
    static const int Distance = 4;
    static const int DwellMs = 350;   // Krimble 2026-10-30: how long the finger must stay within reach of a side before the panel may dock there

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
                if (!m_dragging) m_startPos = p;
                const QRect r(m_win->mapToGlobal(QPoint(0, 0)), m_win->size());
                Qt::DockWidgetAreas allowed = Qt::NoDockWidgetArea;
                if (p.x() - r.left() < Distance) allowed |= Qt::LeftDockWidgetArea;
                if (r.right() - p.x() < Distance) allowed |= Qt::RightDockWidgetArea;
                if (p.y() - r.top() < Distance) allowed |= Qt::TopDockWidgetArea;
                if (r.bottom() - p.y() < Distance) allowed |= Qt::BottomDockWidgetArea;
                // Krimble 2026-10-30: George: "I think there should be a proximity sensor that has a feel for when it's deliberate." (the Brushes panel attached to an
                // edge too eagerly). A side of the window is allowed only after the finger has stayed within reach of it for DwellMs (350 ms), so a finger
                // passing the edge on its way somewhere else does not dock the panel. "Within reach" is the zone used on release below (14 px, 28 px on the right).
                {
                    Qt::DockWidgetAreas reachAreas = Qt::NoDockWidgetArea;
                    if (p.x() - r.left() < 14) reachAreas |= Qt::LeftDockWidgetArea;
                    if (r.right() - p.x() < 28) reachAreas |= Qt::RightDockWidgetArea;
                    if (p.y() - r.top() < 14) reachAreas |= Qt::TopDockWidgetArea;
                    if (r.bottom() - p.y() < 14) reachAreas |= Qt::BottomDockWidgetArea;
                    if (reachAreas != m_reachMask) {
                        m_reachMask = reachAreas;
                        m_reachClock.restart();
                    }
                    if (m_reachMask == Qt::NoDockWidgetArea || m_reachClock.elapsed() < DwellMs) {
                        allowed = Qt::NoDockWidgetArea;       // not deliberate (yet): no side of the window (panels to tab with are added below)
                    }
                }
                // Krimble 2026-10-15: George (1.0.36): "Panels don't go into tabs at all now when you drag them on top of each other." and
                // "Love the tabs. Bring them back." and "Expand so they tab." Dropping a panel onto another panel needs the area of that panel to be
                // allowed, and only the window's edges were. While the finger is over a docked panel, that panel's area is allowed too, so the
                // panel can join it (as a tab, or beside it, as Qt decides from where on that panel the finger is).
                Q_FOREACH (QDockWidget *other, m_win->findChildren<QDockWidget*>()) {
                    if (other == m_dock || other->isFloating() || !other->isVisible()) continue;
                    // Krimble 2026-10-19: George (1.0.39): "The toolbox isn't close to an edge and it snaps to one aggressively." The toolbox
                    // does not join other panels (tabs); with the panels covering much of the screen, a finger passing over one docked it there.
                    if (m_dock->objectName() == QLatin1String("ToolBox")) continue;
                    if (QLineF(QPointF(p), QPointF(m_startPos)).length() < 120.0) continue;     // Krimble 2026-10-18 (2nd): not at once
                    const QRect otherRect(other->mapToGlobal(QPoint(0, 0)), other->size());
                    if (otherRect.contains(p)) allowed |= m_win->dockWidgetArea(other);
                }
                if (m_dock->allowedAreas() != allowed) {
                    m_dock->setAllowedAreas(allowed);
                }
                m_dragging = true;
            }
        } else if (event->type() == QEvent::MouseButtonRelease && m_dragging) {
            m_dragging = false;
            const QPoint releasePos = static_cast<QMouseEvent*>(event)->globalPos();
            // Krimble 2026-10-30: deliberate = the finger stayed within reach of a side for DwellMs (see the move handler above)
            const bool dwelled = m_reachMask != Qt::NoDockWidgetArea && m_reachClock.isValid() && m_reachClock.elapsed() >= DwellMs;
            m_reachMask = Qt::NoDockWidgetArea;
            QPointer<QDockWidget> dock(m_dock);
            QPointer<QMainWindow> window(m_win);
            QTimer::singleShot(0, m_dock, [dock, window, releasePos, dwelled]() {
                if (dock) {
                    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
                }
                // Krimble 2026-10-18 (16th): George (1.0.37, c666301): "4 and 13 in super close range they should attach, not bounce." (13: "I can't get panels to attach at bottom or top
                // any more unless I just tell them in menu where to dock. Sometimes Tool options panel bounces.") Qt's own drop test for the top and bottom edges needs a very small
                // spot, and with a finger (and the 4 px of Distance above) the panel was refused and stayed where it was. When the panel is let go within 14 logical px of a
                // side of the window and Qt has not docked it, it is docked there, on the nearest side.
                if (!dock || !window || !dock->isFloating()) return;
                if (!dwelled) return;     // Krimble 2026-10-30: not deliberate: it stays where it was let go
                const QRect r(window->mapToGlobal(QPoint(0, 0)), window->size());
                const int toLeft = releasePos.x() - r.left();
                const int toRight = r.right() - releasePos.x();
                const int toTop = releasePos.y() - r.top();
                const int toBottom = r.bottom() - releasePos.y();
                const int nearest = qMin(qMin(toLeft, toRight), qMin(toTop, toBottom));
                // const int reach = 14;
                // Krimble 2026-10-23 (2nd): George (1.0.42): "Right panels could stand to be a touch more eager to attach." The right side reaches twice as far (28 logical px).
                const int reach = (nearest == toRight) ? 28 : 14;
                if (nearest > reach) return;
                const Qt::DockWidgetArea area = (nearest == toLeft) ? Qt::LeftDockWidgetArea
                                              : (nearest == toRight) ? Qt::RightDockWidgetArea
                                              : (nearest == toTop) ? Qt::TopDockWidgetArea
                                              : Qt::BottomDockWidgetArea;
                window->addDockWidget(area, dock);
                dock->setFloating(false);
                dock->show();
            });
        }
        return false;
    }

private:
    QMainWindow *m_win;
    QDockWidget *m_dock;
    bool m_dragging {false};
    QPoint m_startPos;   // Krimble 2026-10-18 (2nd): where the finger was when this drag began
    Qt::DockWidgetAreas m_reachMask {Qt::NoDockWidgetArea};   // Krimble 2026-10-30: the sides the finger is within reach of
    QElapsedTimer m_reachClock;                              // since when
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
    // kept below as a comment). ON = the standard behaviour: a panel that floats docks at the
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
            krimbleInstallMenuShortcutStripping(this);   // Krimble 2026-10-13 (4th): the real fix for the wide menus (see above)
            krimbleInstallCanvasOnlyExit(this);          // Krimble 2026-10-13 (5th): the X that leaves Canvas Only mode
            krimbleRemoveMinimumWidths(this);            // Krimble 2026-10-13 (6th): no minimum widths (see above)
            krimbleInstallBevels(this);                  // Krimble 2026-10-17: one-pixel bevel lines on panels and toolbars (see above)
            krimbleInstallHexMenus(this);                // Krimble 2026-10-18: View > Show > Hex and Layer > New > Hex Pattern... (see above)
            krimbleInstallSelectionGlow(this);           // Krimble 2026-10-30: the blue glow of a selected (checked) button
            // Krimble 2026-10-18: George (1.0.37, git 41a4afb): "1. Brushes still loads at start." The hiding at start (2.5 s) came too early: opening
            // the first image brings the saved layout back, with Brushes. So it is hidden once more, once, the first time an image is open.
            // Krimble 2026-10-18 (2nd): George (1.0.37, fc126d8): "2. Brushes still loads by default." The two hidings above and below happen once, and a
            // saved layout brought it back again afterwards. For the first 30 s after the start it is hidden whenever it shows up.
            {
                static QElapsedTimer brushesClock;
                if (!brushesClock.isValid()) brushesClock.start();
                if (brushesClock.elapsed() < 30000) {
                    Q_FOREACH (QDockWidget *dock, findChildren<QDockWidget*>()) {
                        if ((dock->objectName() == QLatin1String("PresetDocker") || dock->windowTitle() == QLatin1String("Brushes")) && dock->isVisible()) dock->hide();
                    }
                }
            }
            // Krimble 2026-10-18 (3rd): George (1.0.37, fc126d8): "2. I don't think so." (about the 30 s rule: an image opened later than that still brings Brushes
            // back, through the saved layout / the Default workspace). When the first image of the session is opened, whenever that is, Brushes is hidden
            // again, twice (1.2 s and 3 s after), to come after the layout is restored.
            if (!property("krimbleBrushesViewHook").toBool()) {
                setProperty("krimbleBrushesViewHook", true);
                connect(this, &KisMainWindow::activeViewChanged, this, [this]() {
                    if (property("krimbleBrushesViewCount").toInt() >= 1) return;
                    setProperty("krimbleBrushesViewCount", 1);
                    auto hideBrushes = [this]() {
                        Q_FOREACH (QDockWidget *dock, findChildren<QDockWidget*>()) {
                            if ((dock->objectName() == QLatin1String("PresetDocker") || dock->windowTitle() == QLatin1String("Brushes")) && dock->isVisible()) dock->hide();
                        }
                    };
                    QTimer::singleShot(1200, this, hideBrushes);
                    QTimer::singleShot(3000, this, hideBrushes);
                });
            }
            if (!property("krimbleBrushesHiddenAfterImage").toBool() && d->widgetStack->currentIndex() != 0) {
                setProperty("krimbleBrushesHiddenAfterImage", true);
                Q_FOREACH (QDockWidget *dock, findChildren<QDockWidget*>()) {
                    if (dock->objectName() == QLatin1String("PresetDocker") || dock->windowTitle() == QLatin1String("Brushes")) dock->hide();
                }
            }
            // Krimble 2026-10-13 (13th): George: "The saved window state on close is the most reliable predictor of the next load state. If we can
            // set it, Brushes will behave." Krita writes the panel layout only when the last image is closed or a toolbar is changed, never when
            // Android stops the app (swiping it away, or low memory), so a panel closed in one session came back in the next. The layout is
            // now also written when the app goes to the background (as it does when the last image is closed), unless the welcome page is
            // showing (Krita does not save that layout) or Canvas Only mode is on (its layout hides everything).
            if (!property("krimbleSaveOnBackground").toBool()) {
                setProperty("krimbleSaveOnBackground", true);
                connect(qApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
                    if (state != Qt::ApplicationHidden && state != Qt::ApplicationSuspended) return;
                    if (d->widgetStack->currentIndex() == 0) return;
                    QAction *canvasOnly = d->viewManager ? d->viewManager->actionCollection()->action(QStringLiteral("view_show_canvas_only")) : nullptr;
                    if (canvasOnly && canvasOnly->isChecked()) return;
                    d->windowStateConfig.writeEntry("ko_geometry", saveGeometry().toBase64());
                    d->windowStateConfig.writeEntry("State", saveState().toBase64());
                    saveWindowSettings();
                });
            }
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
            // if (!panelsCfg.readEntry<int>("krimble_default_panels_done", 0)) {
            //     panelsCfg.writeEntry<int>("krimble_default_panels_done", 1);
            // Krimble 2026-10-17: George (1.0.37): "4. Brushes still auto loads on start." Hidden at every start, not only the first (the saved layout
            // of a phone that had run an older build kept bringing it back); it is shown again from Window > Panels.
            if (true) {
                Q_FOREACH (QDockWidget *dock, findChildren<QDockWidget*>()) {
                    const QString title = dock->windowTitle().remove(QLatin1Char('&')).trimmed();
                    if (title.compare(QStringLiteral("Brushes"), Qt::CaseInsensitive) == 0
                        || title.compare(QStringLiteral("Brush Presets"), Qt::CaseInsensitive) == 0) {
                        dock->hide();
                    }
                }
            }
        }
        // Krimble 2026-10-17: George (1.0.37): "7. Default side panel size is too wide on load. It squeezes the picture." In landscape the column of
        // panels on the right is not wider than 30% of the window at start (it was 254 of 665 logical px, 38%); it can still be dragged wider.
        {
            QDockWidget *firstRight = nullptr;
            int rightWidth = 0;
            Q_FOREACH (QDockWidget *dock, findChildren<QDockWidget*>()) {
                if (dock->isFloating() || !dock->isVisible()) continue;
                if (dockWidgetArea(dock) == Qt::RightDockWidgetArea) {
                    if (!firstRight) firstRight = dock;
                    rightWidth = qMax(rightWidth, dock->width());
                }
            }
            const int maxRightWidth = int(width() * 0.30);
            if (firstRight && width() >= 500 && rightWidth > maxRightWidth) {
                resizeDocks({firstRight}, {maxRightWidth}, Qt::Horizontal);
            }
        }
        // Krimble 2026-10-13 (2nd): George, portrait: "the toolbox reverts to one column ... the right panel will not resize in portrait at
        // all". The canvas area could not be narrower than about 147 logical px (about 500 on the screen), so with the toolbox and the
        // panels next to it there was no room in portrait and the program shrank the toolbox back to one column and refused to resize
        // the panels. The canvas area may now be as narrow as 40 logical px.
        if (QWidget *central = centralWidget()) {
            central->setMinimumWidth(40);
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
            // krimbleInstallLayerExtras(this);   // Krimble 2026-10-13 (9th): George: "5. Comment out pending testing." (Layer > Style Presets)
            krimbleInstallLayerExtras(this);      // Krimble 2026-10-13 (11th): George: "Re-enable 3, 4, 5 for testing." (Layer > Style Presets)
            krimbleInstallViewExtras(this);
            // krimbleInstallImageExtras(this);   // Krimble 2026-10-13 (9th): George: "3. Comment out. 4. Comment out." (Image > Upscale... and Batch Process...)
            krimbleInstallImageExtras(this);      // Krimble 2026-10-13 (11th): George: "Re-enable 3, 4, 5 for testing." (Image > Upscale... and Batch Process..., and Quick Adjust... in Image > Adjustments)
        }
    });
    // Krimble 2026-10-11: close open menus when the window changes size (rotation).
    installEventFilter(new KrimbleClosePopupsOnResizeFilter(this));
    // Krimble 2026-10-18 (5th): and when the program comes back to the front (after another app), no menu stays open
    connect(qApp, &QGuiApplication::applicationStateChanged, this, [](Qt::ApplicationState state) {
        if (state != Qt::ApplicationActive) return;
        while (QWidget *popup = QApplication::activePopupWidget()) {
            popup->close();
        }
    });
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
#ifdef Q_OS_ANDROID
    // Krimble 2026-10-19: the panel edge grab zones hide while the Crop tool or a selection tool is in use and come back for any other
    // tool (see krimbleSelectionToolActive); this places or hides them right away when the tool changes.
    connect(KoToolManager::instance(), &KoToolManager::changedTool, this,
            [this](KoCanvasController*) { krimbleInstallDockEdgeGrabs(this); });
#endif


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
        // Krimble 2026-10-25: George: "Krimble relaunches constantly on switching ... I want it to stop crashing." The log (krita.log) already says at every start why the run before ended; this shows it.
        {
            QAction *showLogAction = new QAction(i18n("Show Krimble Log..."), this);
            connect(showLogAction, &QAction::triggered, this, [this]() {
                QString extra;
#ifdef Q_OS_ANDROID
                {
                    QAndroidJniEnvironment env;
                    QAndroidJniObject activity = QtAndroid::androidActivity();
                    if (activity.isValid()) {
                        QAndroidJniObject record = activity.callObjectMethod("getLastDestroyRecord", "()Ljava/lang/String;");
                        if (env->ExceptionCheck()) {
                            env->ExceptionClear();
                        } else if (record.isValid()) {
                            extra = record.toString();
                        }
                    }
                }
#endif
                (new KrimbleLogDialog(this, QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/krita.log"), extra))->show();
            });
            actions->addAction(QStringLiteral("krimble_show_log"), showLogAction);
        }

        connect(d->helpMenu, SIGNAL(showAboutApplication()), SLOT(showAboutApplication()));
    }

    // KDE' libs 4''s help contents action is broken outside kde, for some reason... We can handle it just as easily ourselves
    QAction *helpAction = actionCollection()->action("help_contents");
    helpAction->disconnect();
    // connect(helpAction, SIGNAL(triggered()), this, SLOT(showManual()));   // the Krita manual on the web (taken out of the Help menu on 2026-10-13)
    // Krimble 2026-10-25: George: "Do we have a help file? We should." Help > Krimble Help shows the Krimble handbook (krita/data/aboutdata/krimble-help.md, in the app's resources).
    helpAction->setText(i18n("Krimble Help"));
    connect(helpAction, &QAction::triggered, this, [this]() {
        (new KrimbleHelpDialog(this, KrimbleHelpDialog::handbookText()))->show();
    });

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
#ifdef Q_OS_ANDROID
        {
            KrimbleMdiTitleStyle *titleStyle = new KrimbleMdiTitleStyle();   // Krimble 2026-10-15: bigger buttons, blue-gray title bar (see above)
            titleStyle->setParent(subwin);
            subwin->setStyle(titleStyle);
        }
#endif

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

// Krimble 2026-10-23: File > Revert (see the action's creation above)
void KisMainWindow::slotFileRevert()
{
    KisView *view = activeView();
    if (!view || !view->document()) return;
    KisDocument *doc = view->document();
    const QString path = doc->path();
    if (path.isEmpty() || !QFileInfo::exists(path)) {
        QMessageBox::information(this, i18n("Revert"), i18n("This picture has not been saved yet, so there is no saved file to go back to."));
        return;
    }
    if (doc->isModified()) {
        const QMessageBox::StandardButton answer = QMessageBox::question(this, i18n("Revert"),
            i18n("Go back to the saved file? All changes made since the last save will be lost."), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (answer != QMessageBox::Yes) return;
    }
    doc->setModified(false);   // so that closing it does not ask to save
    view->closeView();
    // opened again once the closing has finished (opening at once could find the old picture still open and just switch to it)
    QTimer::singleShot(200, this, [this, path]() { openDocument(path, None); });
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
        // dragged panel from docking itself back into a dock zone. That
        // broke docking entirely instead -- addDockWidget() below
        // immediately places the widget into `side`, an area it had
        // just been told it wasn't allowed in at all, a direct
        // self-contradiction fed to Qt's dock-layout engine. This is a
        // strong candidate for both bug #7 (window docking broken --
        // dragged panels can't be dropped back into ANY area, since
        // zero areas are allowed) and bug #12 (docker panels rendering
        // as unmanageable narrow columns -- plausible symptom of Qt's
        // internal dock-layout width allocation misbehaving for a
        // widget that declares itself allowed nowhere). Removed;
        // QDockWidget defaults to Qt::AllDockWidgetAreas when this is
        // never called, restoring normal dock/undock behavior.
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
        //    area docked it back in. While a panel floats it is now allowed in
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
        // if (dockWidget->objectName() == QLatin1String("ToolBox")) {
        //     dockWidget->installEventFilter(new KrimbleDockTolerance(this, dockWidget));
        // }
        // Krimble 2026-10-12: George: the Layers panel, once detached, "tried really hard to attach to edges that were far away from it".
        // Every floating panel, not only the toolbox, now docks only when the finger is close to a side of the window.
        dockWidget->installEventFilter(new KrimbleDockTolerance(this, dockWidget));
        // Krimble 2026-10-30: George: "I created a new file and the Brushes panel appeared. Why?" The hiding above (at the start, once after the first image,
        // for 30 s) did not cover a later new file, when a saved layout brings the panel back. The Brushes panel is now hidden every time it shows up by
        // itself. It stays only when George opens it from the Window menu (the panel's own show/hide action, which is triggered only by a person, not
        // by the layout code); it is forgotten when the panel is closed with its X, so the next appearance is hidden again.
        if (dockWidget->objectName() == QLatin1String("PresetDocker") || dockWidget->windowTitle() == QLatin1String("Brushes")) {
            dockWidget->setProperty("krimbleUserOpened", false);
            connect(dockWidget->toggleViewAction(), &QAction::triggered, dockWidget, [dockWidget](bool on) {
                dockWidget->setProperty("krimbleUserOpened", on);
            });
            connect(dockWidget, &QDockWidget::visibilityChanged, dockWidget, [dockWidget](bool visible) {
                if (visible) {
                    // a moment later: the menu action's "triggered" comes right after the panel is shown
                    QTimer::singleShot(0, dockWidget, [dockWidget]() {
                        if (!dockWidget->property("krimbleUserOpened").toBool() && dockWidget->isVisible()) dockWidget->hide();
                    });
                } else if (dockWidget->isHidden()) {
                    dockWidget->setProperty("krimbleUserOpened", false);
                }
            });
        }
        connect(dockWidget, &QDockWidget::topLevelChanged, this, [this, dockWidget](bool floating) {
            // KRIMBLE 2026-10-04: Settings > "Attach Panels" decides what a floating panel does.
            // ON (the default) = the standard behaviour: a panel dragged to the edge of the window
            // docks there. OFF = floating panels never dock anywhere. Old line:
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


// Krimble 2026-10-23: George (test list "Mercury's Shadow", item 29): "Window - Stack panels (in panels section to stack the side panels)." Puts the panels docked on the
// right side one above the other, each in its own part of the column (they may have been tabs), with equal heights. The toolbox is not touched.
static void krimbleStackPanels(QMainWindow *win)
{
    QList<QDockWidget*> docks;
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (dock->isFloating() || dock->objectName() == QLatin1String("ToolBox")) continue;
        if (win->dockWidgetArea(dock) != Qt::RightDockWidgetArea) continue;
        if (!dock->toggleViewAction()->isChecked()) continue;   // closed panels stay closed (a panel behind another tab still counts as open)
        docks << dock;
    }
    if (docks.size() < 2) return;
    // keep the order the panels have now, from top to bottom (the ones behind a tab are put after the one in front of them)
    std::stable_sort(docks.begin(), docks.end(), [](QDockWidget *a, QDockWidget *b) {
        const int ya = a->isVisible() ? a->mapToGlobal(QPoint(0, 0)).y() : 1 << 20;
        const int yb = b->isVisible() ? b->mapToGlobal(QPoint(0, 0)).y() : 1 << 20;
        return ya < yb;
    });
    int totalHeight = 0;
    Q_FOREACH (QDockWidget *dock, docks) {
        if (dock->isVisible()) totalHeight += dock->height();
    }
    // for (int i = 1; i < docks.size(); ++i) {
    //     win->splitDockWidget(docks.at(i - 1), docks.at(i), Qt::Vertical);
    //     docks.at(i)->show();
    // }
    // docks.first()->show();
    // Krimble 2026-10-23: George (1.0.42): "Stacking does not work." The result of the splitDockWidget version was the Color panel on top and the other two SIDE BY SIDE under
    // it (Qt's split goes the way the area is already divided, not the way it is asked). Each panel is now taken out of the window and added again to the right area, one after
    // the other: on the left and right sides Qt lays added panels out one above the other, and a panel that was a tab is no longer one.
    Q_FOREACH (QDockWidget *dock, docks) {
        win->removeDockWidget(dock);
    }
    Q_FOREACH (QDockWidget *dock, docks) {
        win->addDockWidget(Qt::RightDockWidgetArea, dock);
        dock->show();
    }
    QList<int> sizes;
    const int each = qMax(1, totalHeight / docks.size());
    for (int i = 0; i < docks.size(); ++i) sizes << each;
    win->resizeDocks(docks, sizes, Qt::Vertical);
}

void KisMainWindow::updateWindowMenu()
{
    QMenu *menu = d->windowMenu->menu();
    // Krimble 2026-10-18 (5th): George (1.0.38): "window menu is still sticky" and: "It won't close and won't allow selection and if you rotate the device it is stuck." This function empties
    // and fills the menu and its submenus (the documents, the workspaces). It is called whenever a document is opened or closed, a workspace is added or removed, or a window is destroyed;
    // when that happens while the menu is open on the screen, the items are removed from under the finger and the open popup can no longer be used or closed. While the menu is open the
    // rebuild is put off until it is closed.
    if (menu->isVisible() || QApplication::activePopupWidget() == menu) {
        menu->setProperty("krimbleRebuildPending", true);
        if (!menu->property("krimbleHideHooked").toBool()) {
            menu->setProperty("krimbleHideHooked", true);
            connect(menu, &QMenu::aboutToHide, this, [this, menu]() {
                if (menu->property("krimbleRebuildPending").toBool()) {
                    menu->setProperty("krimbleRebuildPending", false);
                    QTimer::singleShot(0, this, SLOT(updateWindowMenu()));
                }
            });
        }
        return;
    }
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
        // QAction *showDragAction = menu->addAction(i18n("Show Contents While Dragging"));
        // Krimble 2026-10-13 (12th): George: "Try ShowDrag." (a shorter label, to see how much narrower the Window menu gets)
        QAction *showDragAction = menu->addAction(i18n("ShowDrag"));
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
    // Krimble 2026-10-14: George (1.0.36): "Filename length for open image is stretching the width of Window menu. Could it have a set length ...
    // like so many characters?" At most 24 characters wide (the middle is left out: "content://c...41984e.png"), instead of 40% of the screen.
    fileStringWidth = qMin(fileStringWidth, fontMetrics.averageCharWidth() * 24);
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
        // Krimble 2026-10-24: Customize Toolbar... is here, with the other toolbar items (George, test list item 10); it was in the Edit menu
        addActionByName("options_configure_toolbars");
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
        // Krimble 2026-10-23: Stack Panels (George, test list item 29), right after Hide All Panels
        QAction *stackPanelsAction = menu->addAction(i18n("Stack Panels"));
        connect(stackPanelsAction, &QAction::triggered, this, [this]() { krimbleStackPanels(this); });
        // Krimble 2026-10-12: Mirror Layout (Left-Handed) (Claude's idea): checked while the toolbox is on the right
        // Krimble 2026-10-12: Mirror Layout removed from the menu (George): it corrupted the display in 1.0.33. The code is kept.
        // QAction *mirrorLayout = menu->addAction(i18n("Mirror Layout (Left-Handed)"));
        // mirrorLayout->setCheckable(true);
        // if (QDockWidget *toolBoxDock = findChild<QDockWidget*>(QStringLiteral("ToolBox"))) {
        //     mirrorLayout->setChecked(!toolBoxDock->isFloating() && dockWidgetArea(toolBoxDock) == Qt::RightDockWidgetArea);
        // }
        // connect(mirrorLayout, &QAction::triggered, this, [this]() { krimbleMirrorLayout(this); });
        // Krimble 2026-10-13 (9th): George: "8. Comment out." (Window > Layout Presets, Save Layout as Startup, Reset Layout)
#if 0
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
#endif
        menu->addSeparator();
        addActionByName("theme_menu");
        // addActionByName("style_menu");
        // Krimble 2026-10-18 (5th): George (1.0.37, c666301): "Window - Styles does nothing." On Android the Styles submenu is never filled (the loop that lists the widget styles is inside
        // #ifndef Q_OS_ANDROID, and the app has its own style), so the item opened an empty submenu. It is out of the Window menu; the action itself is kept.
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
        // const char *appearanceActions[] = {"theme_menu", "style_menu"};
        const char *appearanceActions[] = {"theme_menu"};   // Krimble 2026-10-18 (5th): no Styles item (see above)
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

    // Krimble 2026-10-23: George (test list "Mercury's Shadow"): "Where is File - Revert??? Put it back." The menu definition (krita5.xmlgui) still has the line
    // "file_reload_file" under Save As, but no action of that name was ever created, so nothing showed. It is created here, and named Revert.
    {
        KisAction *revertAction = actionManager->createAction("file_reload_file");
        revertAction->setText(i18n("Revert"));
        revertAction->setToolTip(i18n("Go back to the saved file; changes made since the last save are lost"));
        revertAction->setActivationFlags(KisAction::ACTIVE_IMAGE);
        connect(revertAction, SIGNAL(triggered()), this, SLOT(slotFileRevert()));
    }

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

    // Krimble 2026-10-23: George: "Can we have a toggle to reset all options to defaults on restart?" A checkable item next to Reset Preferences; the work is done at the next start
    // (krimbleResetToolOptionsIfWanted in KisApplication.cpp).
    {
        KisAction *resetToolOptionsOnStart = actionManager->createAction("krimble_reset_tool_options_on_start");
        resetToolOptionsOnStart->setText(i18n("Reset Tool Options on Start"));
        resetToolOptionsOnStart->setToolTip(i18n("When checked, the tools start with their default options every time the program starts"));
        resetToolOptionsOnStart->setCheckable(true);
        resetToolOptionsOnStart->setChecked(KSharedConfig::openConfig()->group(QStringLiteral("Krimble")).readEntry("resetToolOptionsOnStart", false));
        connect(resetToolOptionsOnStart, &QAction::toggled, this, [](bool on) {
            KConfigGroup group = KSharedConfig::openConfig()->group(QStringLiteral("Krimble"));
            group.writeEntry("resetToolOptionsOnStart", on);
            group.sync();
        });
    }

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
