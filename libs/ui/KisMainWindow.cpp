/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2000-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Thomas zander <zander@kde.org>
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KisMainWindow.h"
#include "KrimbleBatchDialog.h"
#include "KrimbleHelpDialog.h"
#include "KrimbleLogDialog.h"
#include "KrimbleCustomCmykDialog.h"

#include <KoConfig.h>

#include <QKeyEvent>
#include <QApplication>
#include <QByteArray>
#include <QCloseEvent>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QScreen>
#include <QRegularExpression>
#include <QLinearGradient>
#include <QBrush>
#include <QPair>
#include <QVector>
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
#include <QSettings>
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
#include <QFrame>
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
#include <QPainter>
#include <QFontMetrics>
#include <QAbstractButton>
#include <cmath>
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
#include <KoShapeRegistry.h>
#include <QGraphicsDropShadowEffect>
#include <kis_paint_layer.h>
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

#include "krimble/KrimbleDockHelpers.inc"
#include "krimble/KrimbleMenus.inc"
#include "krimble/KrimbleDockEdges.inc"
#include "kis_zoom_manager.h"
#include <KoGroupButton.h>
#include <QStyleOptionToolButton>
#include <QStyleOptionButton>
#include <QStyleOptionSlider>
#include <QScrollBar>
#include <QToolButton>
#include "KrimbleAquaPaint.h"
#include "krimble/KrimbleMenuBarAndQuickAdjust.inc"
#include "krimble/KrimbleFileAndHelpExtras.inc"
#include "krimble/KrimblePanelsAndLayout.inc"
#include "krimble/KrimbleBevelsAndTabs.inc"

static void krimbleInstallMenuBarFilter(QMenuBar *bar)
{
    if (!bar || bar->property("krimbleCornerGuard").toBool()) return;
    bar->setProperty("krimbleCornerGuard", true);
    KrimbleMenuBarFilter *kFilter = new KrimbleMenuBarFilter(bar);
    bar->installEventFilter(kFilter);
    KrimbleMenuBarFilter::hideSettingsMenu(bar);
    Q_FOREACH (QWidget *child, bar->findChildren<QWidget *>()) {
        if (QByteArray(child->metaObject()->className()).endsWith("ControlLabel")) {
            KrimbleMenuBarFilter::adopt(child, bar);
            child->installEventFilter(kFilter);
        }
    }
    if (QWidget *corner = bar->cornerWidget(Qt::TopLeftCorner)) {
        if (QByteArray(corner->metaObject()->className()).endsWith("ControlLabel")) {
            KrimbleMenuBarFilter::adopt(corner, bar);
            corner->installEventFilter(kFilter);
        }
    }
    KrimbleMenuBarFilter::schedulePlaceK(bar);
}
#include "krimble/KrimbleHexGrid.inc"
#include "krimble/KrimbleMenuColumnDividers.h"
#include "krimble/KrimbleGlowAndDocking.inc"
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
#ifdef Q_OS_ANDROID
        {
            const qreal dpr = qMax<qreal>(1.0, qApp->devicePixelRatio());
            const int bar = qCeil(48.0 / dpr);
            welcomeScroller->setStyleSheet(QStringLiteral("QScrollBar:vertical { width: %1px; } QScrollBar:horizontal { height: %1px; }").arg(bar));
        }
#endif

        widgetStack->addWidget(welcomeScroller);
        widgetStack->addWidget(mdiArea);
        mdiArea->setTabsMovable(true);
        mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
        mdiArea->setDocumentMode(true);
        mdiArea->setOption(QMdiArea::DontMaximizeSubWindowOnActivation);
#ifdef HAVE_QMDIAREA_ALWAYS_SHOW_SUBWINDOW_TITLE
        mdiArea->setOption(QMdiArea::AlwaysShowSubwindowNameInTitleBar);
#endif

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

static void krimbleStackPanels(QMainWindow *win);

KisMainWindow::KisMainWindow(QUuid uuid)
    : KXmlGuiWindow()
    , d(new Private(this, uuid))
{
    KAcceleratorManager::setNoAccel(this);

    {
        KConfigGroup columnsGroup(KSharedConfig::openConfig(), "krimble");
        if (!columnsGroup.readEntry("ToolBoxDockedLeftReset", false)) {
            columnsGroup.writeEntry("ToolBoxColumns", 2);
            columnsGroup.sync();
        }
    }

    d->workspacemodel = new KisResourceModel(ResourceType::Workspaces, this);
    connect(d->workspacemodel, SIGNAL(modelReset()), this, SLOT(updateWindowMenu()));
    connect(d->workspacemodel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateWindowMenu()));
    connect(d->workspacemodel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(updateWindowMenu()));

    d->viewManager = new KisViewManager(this, actionCollection());
    {
        KConfigGroup toolboxStart(KSharedConfig::openConfig(), "krimble");
        if (!toolboxStart.readEntry("ToolboxStartV58", false)) {
            toolboxStart.writeEntry("ToolBoxColumns", 2);
            toolboxStart.sync();
        }
    }
    KConfigGroup group( KSharedConfig::openConfig(), "theme");
#ifndef Q_OS_HAIKU
    d->themeManager = new Digikam::ThemeManager(group.readEntry("Theme", "Krimble Blue-Gray"), this);
#ifndef Q_OS_HAIKU
    d->themeManager->setCurrentTheme(group.readEntry("Theme", "Krimble Blue-Gray"));
#endif
#endif
    d->windowStateConfig = KSharedConfig::openConfig()->group("MainWindow");

    setStandardToolBarMenuEnabled(true);
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
    setDockNestingEnabled(true);
#ifdef Q_OS_ANDROID
    setAnimated(false);
#endif

    qApp->setStartDragDistance(25);

    connect(this, SIGNAL(restoringDone()), this, SLOT(forceDockTabFonts()));
    connect(this, SIGNAL(themeChanged()), d->viewManager, SLOT(updateIcons()));
    connect(this, SIGNAL(themeChanged()), KoToolManager::instance(), SLOT(themeChanged()));
    connect(KisPart::instance(), SIGNAL(documentClosed(QString)), SLOT(updateWindowMenu()));
    connect(KisPart::instance(), SIGNAL(documentOpened(QString)), SLOT(updateWindowMenu()));
    connect(KisConfigNotifier::instance(), SIGNAL(configChanged()), this, SLOT(configChanged()));

    actionCollection()->addAssociatedWidget(this);
    KoPluginLoader::instance()->load("Krita/ViewPlugin", KoPluginLoader::PluginsConfig(), d->viewManager, false);

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

    {
        QMenu *panelsMenu = d->dockWidgetMenu->menu();
        connect(panelsMenu, &QMenu::aboutToShow, this, [this, panelsMenu]() {
            Q_FOREACH (QAction *extra, panelsMenu->actions()) {
                if (!extra->property("krimblePanelsExtra").toBool()) continue;
                panelsMenu->removeAction(extra);
                if (extra->parent() == panelsMenu) extra->deleteLater();
            }
            auto mark = [](QAction *a) { a->setProperty("krimblePanelsExtra", true); return a; };
            QAction *firstPanel = panelsMenu->actions().isEmpty() ? nullptr : panelsMenu->actions().first();
            const char *names[] = {"view_toggledockers", "settings_detach_panel_menu", "settings_attach_panel_menu", "settings_toggle_right_panels"};
            for (const char *name : names) {
                if (QAction *action = actionCollection()->action(QLatin1String(name))) panelsMenu->insertAction(firstPanel, mark(action));
            }
            QAction *hideAllPanels = mark(new QAction(d->toggleDockers->isChecked() ? i18n("Hide All Panels") : i18n("Show All Panels"), panelsMenu));
            panelsMenu->insertAction(firstPanel, hideAllPanels);
            connect(hideAllPanels, &QAction::triggered, this, [this]() { d->toggleDockers->setChecked(!d->toggleDockers->isChecked()); });
            QAction *stackPanelsAction = mark(new QAction(i18n("Stack Panels"), panelsMenu));
            panelsMenu->insertAction(firstPanel, stackPanelsAction);
            connect(stackPanelsAction, &QAction::triggered, this, [this]() { krimbleStackPanels(this); });
            if (firstPanel) mark(panelsMenu->insertSeparator(firstPanel));
        });
    }

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
    actionCollection()->addAction("settings_detach_panel_menu", detachPanelAction);

#if 0
    {
        KConfigGroup krimbleGroup = KSharedConfig::openConfig()->group("Krimble");
        const bool attachOn = krimbleGroup.readEntry("AttachPanels", true);
        setProperty("krimbleAttachPanels", attachOn);
        QAction *attachPanelsAction = new QAction(i18nc("@action:inmenu", "Attach Panels"), this);
        attachPanelsAction->setCheckable(true);
        attachPanelsAction->setChecked(attachOn);
        actionCollection()->addAction("settings_attach_panel_menu", attachPanelsAction);
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

    QAction *toggleRightPanelsAction = new QAction(i18nc("@action:inmenu", "Hide Right Panels"), this);
    actionCollection()->addAction("settings_toggle_right_panels", toggleRightPanelsAction);
    connect(toggleRightPanelsAction, &QAction::triggered, this, [this]() {
        krimbleSetRightPanelsHidden(this, !krimbleRightDocks(this, true).isEmpty());
        krimbleUpdateRightPanelUi(this);
    });
#if 0
    {
        struct DragState { int startWidth {0}; };
        auto dragState = std::make_shared<DragState>();
        d->mdiArea->setObjectName(QStringLiteral("krimbleCanvasArea"));
        new KrimbleSideBarButton(d->mdiArea,
            [this]() {
                if (krimbleRightDocks(this, true).isEmpty()) {
                    krimbleSetRightPanelsHidden(this, false);
                }
                krimbleUpdateRightPanelUi(this);
            },
            [this, dragState]() {
                if (krimbleRightDocks(this, true).isEmpty()) {
                    krimbleSetRightPanelsHidden(this, false);
                    QApplication::processEvents();
                }
                dragState->startWidth = krimbleRightPanelsWidth(this);
            },
            [this, dragState](int dx) {
                krimbleSetRightPanelsWidth(this, dragState->startWidth - dx);
                krimbleUpdateRightPanelUi(this);
            },
            [this](int globalX) {
                const int rightEdge = mapToGlobal(QPoint(width(), 0)).x();
                if (globalX >= rightEdge - 56 || krimbleRightPanelsWidth(this) < 120) {
                    krimbleSetRightPanelsHidden(this, true);
                    krimbleUpdateRightPanelUi(this);
                }
            });
    }
#endif
    {
        KrimbleRightPanelsFilter *rightPanelsFilter = new KrimbleRightPanelsFilter(this);
        installEventFilter(rightPanelsFilter);
        d->mdiArea->installEventFilter(rightPanelsFilter);
    }
    krimbleInstallMenuBarFilter(menuBar());
#ifdef Q_OS_ANDROID
    qApp->installEventFilter(new KrimbleEdgeCollapseFilter(this));
    Q_FOREACH (int delay, QList<int>({300, 1000, 2000})) QTimer::singleShot(delay, this, [this]() { menuBar()->update(); });
    KisUsageLogger::log(QStringLiteral("KRIMBLE-TABS the tab tear-off filter is installed (application level)"));
    QTimer::singleShot(2500, this, [this]() { krimbleStackToolBars(this); krimbleInstallHelpExtras(this); krimbleInstallFileExtras(this);
        krimbleInstallDockEdgeGrabs(this);
        QTimer *grabTimer = new QTimer(this);
        connect(grabTimer, &QTimer::timeout, this, [this, grabTimer]() {
            static QElapsedTimer passClock;
            if (!passClock.isValid()) passClock.start();
            if (passClock.elapsed() < 30000) grabTimer->setInterval(2000);
            else grabTimer->setSingleShot(true);
            krimbleInstallDockEdgeGrabs(this);
            Q_FOREACH (QAction *top, menuBar()->actions()) krimbleHideMenuShortcuts(top->menu(), 0);
            krimbleInstallMenuShortcutStripping(this);
            krimbleInstallCanvasOnlyExit(this);
            krimbleRemoveMinimumWidths(this);
            krimbleInstallBevels(this);
            krimbleInstallHexMenus(this);
            krimbleInstallSelectionGlow(this);
            {
                static QElapsedTimer brushesClock;
                if (!brushesClock.isValid()) brushesClock.start();
                if (brushesClock.elapsed() < 30000) {
                    Q_FOREACH (QDockWidget *dock, findChildren<QDockWidget*>()) {
                        if ((dock->objectName() == QLatin1String("PresetDocker") || dock->windowTitle() == QLatin1String("Brushes")) && dock->isVisible()) dock->hide();
                    }
                }
            }
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
        connect(this, &KisMainWindow::activeViewChanged, grabTimer, [grabTimer]() { grabTimer->start(500); });
        Q_FOREACH (QAction *top, menuBar()->actions()) krimbleHideMenuShortcuts(top->menu(), 0);
        {
            KisConfig floatCfg(false);
            {
                KConfigGroup toolboxStart(KSharedConfig::openConfig(), "krimble");
                if (!toolboxStart.readEntry("ToolboxStartV58", false)) {
                    toolboxStart.writeEntry("ToolboxStartV58", true);
                    toolboxStart.sync();
                    if (QDockWidget *toolBoxDock = findChild<QDockWidget*>(QStringLiteral("ToolBox"))) {
                        if (toolBoxDock->isFloating()) toolBoxDock->setFloating(false);
                        addDockWidget(Qt::LeftDockWidgetArea, toolBoxDock);
                    }
                }
            }
            {
                KConfigGroup resetGroup(KSharedConfig::openConfig(), "krimble");
                if (!resetGroup.readEntry("ToolBoxDockedLeftReset", false)) {
                    resetGroup.writeEntry("ToolBoxDockedLeftReset", true);
                    resetGroup.sync();
                    if (QDockWidget *toolBoxDock = findChild<QDockWidget*>(QStringLiteral("ToolBox"))) {
                        if (toolBoxDock->isFloating()) toolBoxDock->setFloating(false);
                        addDockWidget(Qt::LeftDockWidgetArea, toolBoxDock);
                        toolBoxDock->show();
                    }
                }
            }
            if (false) {
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
        {
            KisConfig panelsCfg(false);
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
        if (QWidget *central = centralWidget()) {
            central->setMinimumWidth(40);
        }
        {
            KConfigGroup layoutGroup = KSharedConfig::openConfig()->group("KrimbleLayouts");
            if (!layoutGroup.hasKey("default")) {
                layoutGroup.writeEntry("default", saveState().toBase64());
                layoutGroup.sync();
            }
            layoutGroup.deleteEntry("portrait_last");
            layoutGroup.deleteEntry("landscape_last");
            layoutGroup.sync();
            krimbleRestoreSavedLayout(this);
            krimbleInstallLayerExtras(this);
            krimbleInstallViewExtras(this);
            krimbleInstallColorSettings(this);
            krimbleInstallImageExtras(this);
        }
    });
    installEventFilter(new KrimbleClosePopupsOnResizeFilter(this));
    connect(qApp, &QGuiApplication::applicationStateChanged, this, [](Qt::ApplicationState state) {
        if (state != Qt::ApplicationActive) return;
        while (QWidget *popup = QApplication::activePopupWidget()) {
            popup->close();
        }
    });
#endif

    QTimer::singleShot(0, this, [this]() {
        krimbleApplyXmlGuiLabels(this);
        Q_FOREACH (QMenu *menu, this->findChildren<QMenu*>()) {
            connect(menu, &QMenu::aboutToShow, this, [this]() { krimbleApplyXmlGuiLabels(this); });
        }
    });
    connect(KoToolManager::instance(), &KoToolManager::changedTool, this,
            [this](KoCanvasController*) { krimbleApplyXmlGuiLabels(this); });
#ifdef Q_OS_ANDROID
    connect(KoToolManager::instance(), &KoToolManager::changedTool, this,
            [this](KoCanvasController*) { krimbleInstallDockEdgeGrabs(this); });
#endif

    d->styleActions = new QActionGroup(this);
    QAction * action;

#ifndef Q_OS_ANDROID

    Q_FOREACH (QString styleName, QStyleFactory::keys()) {

        action = new QAction(styleName, d->styleActions);
        action->setCheckable(true);
        d->actionMap.insert(styleName, action);
        d->styleMenu->addAction(d->actionMap.value(styleName));
    }

    QString styleFromConfig = cfg.widgetStyle().toLower();
    QString styleToSelect = styleFromConfig.isEmpty() ? style()->objectName().toLower() : styleFromConfig;

    Q_FOREACH (auto key, d->actionMap.keys()) {
        if(key.toLower() == styleToSelect) {
            d->actionMap.value(key)->setChecked(true);
        }
    }

    connect(d->styleActions, SIGNAL(triggered(QAction*)),
            this, SLOT(slotUpdateWidgetStyle()));
#endif

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

    themeChanged();

    setCentralWidget(d->widgetStack);
    d->widgetStack->setCurrentIndex(0);

    connect(d->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(subWindowActivated()));
    connect(d->windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));
    connect(d->documentMapper, SIGNAL(mapped(QObject*)), this, SLOT(newView(QObject*)));

    d->canvasWindow = new KisCanvasWindow(this);
    actionCollection()->addAssociatedWidget(d->canvasWindow);

    createActions();

    d->welcomePage->setMainWindow(this);

    d->recentFiles->setRecentFilesModel(&KisRecentDocumentsModelWrapper::instance()->model());

    subWindowActivated();
    updateWindowMenu();

    if (isHelpMenuEnabled() && !d->helpMenu) {
        d->helpMenu = new KisKHelpMenu(this, KAboutData::applicationData(), false);

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

    QAction *helpAction = actionCollection()->action("help_contents");
    helpAction->disconnect();
    helpAction->setText(i18n("Krimble Help"));
    connect(helpAction, &QAction::triggered, this, [this]() {
        (new KrimbleHelpDialog(this, KrimbleHelpDialog::handbookText()))->show();
    });

#if 0
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

    KisPart::instance()->notifyMainWindowIsBeingCreated(this);

    setLocalXMLFile(KoResourcePaths::locateLocal("data", "krita5.xmlgui"));
    setXMLFile(":/kxmlgui5/krita5.xmlgui");

    guiFactory()->addClient(this);
    applyActionIconOverridesFromLocalXML();
    connect(guiFactory(), SIGNAL(makingChanges(bool)), SLOT(slotXmlGuiMakingChanges(bool)));

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
        using namespace std::placeholders;
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

    this->winId();

#ifdef Q_OS_ANDROID
    setFixedSize(KisApplication::primaryScreen()->availableGeometry().size());

    QScreen *s = QGuiApplication::primaryScreen();
    s->setOrientationUpdateMask(Qt::LandscapeOrientation|Qt::InvertedLandscapeOrientation|Qt::PortraitOrientation|Qt::InvertedPortraitOrientation);
    connect(s, SIGNAL(orientationChanged(Qt::ScreenOrientation)), this, SLOT(orientationChanged()));

#if KRITA_QT_HAS_ANDROID_QPLATFORMSCREEN_DENSITY_ADJUSTMENT
    KisAndroidScaling *androidScaling = KisAndroidScaling::instance();
    if (androidScaling) {
        connect(androidScaling,
                &KisAndroidScaling::sigInterfaceScaleChanged,
                this,
                &KisMainWindow::slotFlashWindowHack,
                Qt::QueuedConnection);
    }
#endif

    QAndroidJniObject::callStaticMethod<void>("org/qtproject/qt5/android/QtNative", "setApplicationState", "(I)V", Qt::ApplicationActive);
#endif

    setAcceptDrops(true);
    QTabBar *tabBar = d->findTabBarHACK();
    if (tabBar) {
        tabBar->setElideMode(Qt::ElideRight);
        customizeTabBar();
        tabBar->setExpanding(true);
        tabBar->setAcceptDrops(true);
        tabBar->setChangeCurrentOnDrag(true);
    }

    applyMainWindowSettings(d->windowStateConfig);
}

KisMainWindow::~KisMainWindow()
{

    Q_FOREACH (QMdiSubWindow *subwin, d->mdiArea->subWindowList()) {
        if (subwin) {
            (void) subwin->close();
            delete subwin;
        }
    }

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

    viewManager()->inputManager()->addTrackedCanvas(view->canvasBase());

    showView(view, subWindow);
    Q_EMIT restoringDone();

}

void KisMainWindow::notifyChildViewDestroyed(KisView *view)
{
    viewManager()->inputManager()->removeTrackedCanvas(view->canvasBase());
    if (view->canvasBase() == viewManager()->canvasBase()) {
        viewManager()->setCurrentView(0);
    }
}

static bool s_krimbleQuitting = false;
static void krimbleSaveOpenList()
{
    if (s_krimbleQuitting) return;
    QStringList paths;
    Q_FOREACH (QPointer<KisDocument> doc, KisPart::instance()->documents()) {
        if (doc && !doc->path().isEmpty() && !paths.contains(doc->path())) paths << doc->path();
    }
    KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleSession");
    g.writeEntry("openFiles", paths);
    g.sync();
}

static void krimbleInstallReopen(KisMainWindow *win)
{
    static bool installed = false;
    if (installed) return;
    installed = true;
    QTimer::singleShot(2000, win, [win]() {
        KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleSession");
        const QStringList paths = g.readEntry("openFiles", QStringList());
        const bool nothingOpen = KisPart::instance()->documents().isEmpty();
        g.writeEntry("openFiles", QStringList());
        g.sync();
        if (nothingOpen) {
            Q_FOREACH (const QString &path, paths) {
                qWarning() << "Krimble: opening again after a restart:" << path;
                win->openDocument(path, KisMainWindow::None);
            }
        }
        krimbleSaveOpenList();
        QObject::connect(KisPart::instance(), &KisPart::sigDocumentAdded, win, [win](KisDocument *) { QTimer::singleShot(0, win, []() { krimbleSaveOpenList(); }); });
        QObject::connect(KisPart::instance(), &KisPart::sigDocumentRemoved, win, [win](const QString &) { QTimer::singleShot(0, win, []() { krimbleSaveOpenList(); }); });
        QObject::connect(qApp, &QCoreApplication::aboutToQuit, qApp, []() {
            s_krimbleQuitting = true;
            KConfigGroup quitGroup = KSharedConfig::openConfig()->group("KrimbleSession");
            quitGroup.writeEntry("openFiles", QStringList());
            quitGroup.sync();
        });
        QObject::connect(qApp, &QGuiApplication::applicationStateChanged, win, [](Qt::ApplicationState state) {
            if (state != Qt::ApplicationActive) krimbleSaveOpenList();
        });
    });
}

class KrimbleFitOnOpen : public QObject
{
public:
    KrimbleFitOnOpen(KisView *view, QWidget *canvasWidget) : QObject(canvasWidget), m_view(view)
    {
        canvasWidget->installEventFilter(this);
        QTimer::singleShot(1500, this, [this]() { nudge(); });
        QTimer::singleShot(3000, this, [this]() { nudge(); });
    }
    void fit()
    {
        if (!m_view || !m_view->zoomManager()) return;
        m_view->zoomManager()->slotZoomToFit();
        if (m_view->canvasBase()) {
            m_view->canvasBase()->refetchDataFromImage();
            m_view->canvasBase()->updateCanvas();
        }
        nudge();
    }
    void nudge()
    {
        if (!m_view || !m_view->canvasController() || m_nudging) return;
        m_nudging = true;
        m_view->canvasController()->pan(QPoint(1, 0));
        QTimer::singleShot(60, this, [this]() {
            if (m_view && m_view->canvasController()) m_view->canvasController()->pan(QPoint(-1, 0));
            m_nudging = false;
        });
    }
protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        switch (event->type()) {
        case QEvent::Resize: {
            QResizeEvent *re = static_cast<QResizeEvent *>(event);
            if (!re->size().isEmpty() && re->size() != re->oldSize()) {
                QTimer::singleShot(0, this, [this]() { fit(); });
            }
            break;
        }
        case QEvent::Show:
            QTimer::singleShot(0, this, [this]() { fit(); });
            break;
        case QEvent::MouseButtonPress:
        case QEvent::TouchBegin:
        case QEvent::TabletPress:
        case QEvent::Wheel:
            watched->removeEventFilter(this);
            deleteLater();
            break;
        default:
            break;
        }
        return false;
    }
private:
    QPointer<KisView> m_view;
    bool m_nudging {false};
};

void KisMainWindow::showView(KisView *imageView, QMdiSubWindow *subwin)
{
    if (imageView && activeView() != imageView) {
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
        krimbleInstallMenuBarFilter(menuBar());
        {
            QPointer<KisView> krimbleFitView(imageView);
            if (imageView->canvasBase() && imageView->canvasBase()->canvasWidget()) {
                new KrimbleFitOnOpen(imageView, imageView->canvasBase()->canvasWidget());
            }
            QTimer::singleShot(1000, this, [krimbleFitView]() {
                if (krimbleFitView && krimbleFitView->zoomManager()) krimbleFitView->zoomManager()->slotZoomToFit();
                if (krimbleFitView && krimbleFitView->canvasBase()) {
                    krimbleFitView->canvasBase()->refetchDataFromImage();
                    krimbleFitView->canvasBase()->updateCanvas();
                }
            });
        }
#ifdef Q_OS_ANDROID
        if (QTabBar *krimbleTabs = d->findTabBarHACK()) {
            if (!krimbleTabs->property("krimbleTabK").toBool()) {
                krimbleTabs->setProperty("krimbleTabK", true);
                new KrimbleTabKFilter(krimbleTabs);
            }
        }
#endif

        KisConfig cfg(true);
        subwin->setOption(QMdiSubWindow::RubberBandMove, cfg.readEntry<int>("mdi_rubberband", krimbleRubberBandDefault(cfg)));
        subwin->setOption(QMdiSubWindow::RubberBandResize, cfg.readEntry<int>("mdi_rubberband", krimbleRubberBandDefault(cfg)));
        subwin->setWindowIcon(qApp->windowIcon());
#ifdef Q_OS_ANDROID
        {
            KrimbleMdiTitleStyle *titleStyle = new KrimbleMdiTitleStyle();
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

        KoToolManager::instance()->initializeCurrentToolForCanvas();

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

        Q_FOREACH (QPointer<KisView> koview, KisPart::instance()->views()) {
            KisViewManager *view = qobject_cast<KisViewManager*>(koview);
            if (view) {
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

    group.writeEntry("Theme", d->themeManager->currentThemeName());
#endif
    updateTheme();

    Q_FOREACH (QWidget* topLevelWidget, qApp->topLevelWidgets()) {
        if (topLevelWidget == this) {
            continue;
        }
        if (topLevelWidget->isHidden()) {
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
    {
        static const QStringList krimbleTextFormats = {QStringLiteral("kra"), QStringLiteral("krz"), QStringLiteral("ora"), QStringLiteral("psd"),
                                                       QStringLiteral("svg"), QStringLiteral("svgz"), QStringLiteral("pdf"), QStringLiteral("tif"), QStringLiteral("tiff")};
        if (krimbleTextFormats.contains(QFileInfo(path).suffix().toLower())) krimbleEnsureBundledFonts();
    }
    ScopedWidgetDisabler disabler(d->welcomePage->dropFrameBorder);
    QApplication::processEvents();

    if (!QFile(path).exists()) {
        if (!(flags & BatchMode)) {
            QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Krimble"), i18n("The file %1 does not exist.", path));
        }
        KisRecentFilesManager::instance()->remove(QUrl::fromLocalFile(path));
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
    if (flags & RecoveryFile) {
        openFlags |= KisDocument::RecoveryFile;
    }

    bool openRet = !(flags & Import) ? newdoc->openPath(path, openFlags) : newdoc->importDocument(path);

    if (!openRet) {
        delete newdoc;
        return false;
    }

    KisPart::instance()->addDocument(newdoc);

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
    showWelcomeScreen(false);

    KisView *view = KisPart::instance()->createView(document, d->viewManager, this);
    addView(view, subWindow);

    Q_EMIT guiLoadingFinished();

#ifdef Q_OS_ANDROID
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
    if (!errMsg.isEmpty()) {
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

    std::unique_lock<QMutex> l(d->savingEntryMutex, std::try_to_lock);
    if (!l.owns_lock()) return false;

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
    if (!target.exists()) {
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

        QString suggestedFilename = QFileInfo(suggestedURL.toLocalFile()).completeBaseName();

        if (!suggestedFilename.isEmpty()) {
            suggestedFilename = suggestedFilename + "." + KisMimeDatabase::suffixesForMimeType(KIS_MIME_TYPE).first();
            suggestedURL = suggestedURL.adjusted(QUrl::RemoveFilename);
            suggestedURL.setPath(suggestedURL.path() + suggestedFilename);
        }

        saveas = true;
    }

    bool ret = false;

    if (document->path().isEmpty() || isExporting || saveas) {
        bool justChangingFilterOptions = false;

        KoFileDialog dialog(this, KoFileDialog::SaveFile, "SaveAs");
        dialog.setCaption(isExporting ? i18n("Exporting") : i18n("Saving As"));

        if (isExporting && !d->lastExportLocation.isEmpty() && !d->lastExportLocation.contains(QDir::tempPath())) {

            QString proposedPath = QFileInfo(d->lastExportLocation).absolutePath();
            QString proposedFileName = suggestedURL.isEmpty() ? document->documentInfo()->aboutInfo("title") :  QFileInfo(suggestedURL.toLocalFile()).completeBaseName();
            QString proposedMimeType =  d->lastExportedFormat.isEmpty() ? "" : d->lastExportedFormat;
            QString proposedExtension = KisMimeDatabase::suffixesForMimeType(proposedMimeType).first().remove("*,");

            dialog.setDefaultDir(proposedPath + "/" + proposedFileName + "." + proposedExtension, true);
            dialog.setMimeTypeFilters(mimeFilter, proposedMimeType);
        }
        else {
            KisConfig cfg(true);
            QByteArray default_mime_type = cfg.exportMimeType(false).toUtf8();
            QString proposedMimeType = QString::fromLatin1(default_mime_type);

            KConfigGroup group =  KSharedConfig::openConfig()->group("File Dialogs");
            QString proposedPath = group.readEntry("SaveAs", "");
            if (proposedPath.isEmpty()) {
                proposedPath = group.readEntry("OpenDocument", "");
            }
            if (proposedPath.isEmpty()) {
                proposedPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
            }

            if(default_mime_type == "all/mime"){
                dialog.setDefaultDir(suggestedURL.isEmpty() ? proposedPath : proposedPath + "/" + QFileInfo(suggestedURL.toLocalFile()).completeBaseName(), true);
            }

            if (default_mime_type != "all/mime" && !default_mime_type.isEmpty()) {
                QString proposedExtension = KisMimeDatabase::suffixesForMimeType(proposedMimeType).first().remove("*,");
                dialog.setDefaultDir(suggestedURL.isEmpty() ? proposedPath :  proposedPath + "/" + QFileInfo(suggestedURL.toLocalFile()).completeBaseName() + "." + proposedExtension, true);
            }

            if (!isExporting) {
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

            if (!justChangingFilterOptions) {
                if (!document->isNativeFormat(outputFormat))
                    wantToSave = true;
            }

            if (wantToSave) {
                if (!isExporting) {
                    ret = document->saveAs(newFilePath, outputFormat, true);
                    if (ret) {
                        dbgUI << "Successful Save As!";
                        KisPart::instance()->queueAddRecentURLToAllMainWindowsOnFileSaved(QUrl::fromLocalFile(newFilePath));
                    } else {
                        dbgUI << "Failed Save As!";

                    }
                }
                else {
                    ret = document->exportDocument(newFilePath, outputFormat, isAdvancedExporting, true);
                    if (ret) {
                        d->lastExportLocation = newFilePath;
                        d->lastExportedFormat = outputFormat;
                    }
                }

            }
            else
                ret = false;
        }
        else
            ret = false;
    } else {
        KIS_ASSERT_RECOVER_NOOP(!isExporting);

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

void KisMainWindow::keyPressEvent(QKeyEvent *e)
{
#ifdef Q_OS_ANDROID
    if (e->key() == Qt::Key_Back) {
        e->accept();
        return;
    }
#endif
    KXmlGuiWindow::keyPressEvent(e);
}

void KisMainWindow::keyReleaseEvent(QKeyEvent *e)
{
#ifdef Q_OS_ANDROID
    if (e->key() == Qt::Key_Back) {
        e->accept();
        QAndroidJniObject activity = QtAndroid::androidActivity();
        if (activity.isValid()) {
            QAndroidJniEnvironment env;
            activity.callMethod<jboolean>("moveTaskToBack", "(Z)Z", jboolean(true));
            if (env->ExceptionCheck()) env->ExceptionClear();
        }
        return;
    }
#endif
    KXmlGuiWindow::keyReleaseEvent(e);
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

        KConfigGroup group = d->windowStateConfig;
        saveMainWindowSettings(group);

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
    resetAutoSaveSettings();

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
    if (!event->spontaneous()) {
        setMainWindowLayoutForCurrentMainWidget(d->widgetStack->currentIndex(), false);
    }
#ifdef Q_OS_ANDROID
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

            if (d->mdiArea->subWindowList().isEmpty()) {
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
    krimbleApplyXmlGuiLabels(this);

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
    doc->setModified(false);
    view->closeView();
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
    QWidget *win = qApp->activeWindow();
    if (win && !qobject_cast<KisMainWindow *>(win) && (win->isModal() || win->windowModality() != Qt::NonModal)) {
        return;
    }

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
    if (d->widgetStack->currentIndex() == 0) {
        return;
    }

    if (restoreNormalState) {
        QAction *showCanvasOnly = d->viewManager->actionCollection()->action("view_show_canvas_only");

        if (showCanvasOnly && showCanvasOnly->isChecked()) {
            showCanvasOnly->setChecked(false);
        }

        d->windowStateConfig.writeEntry("ko_geometry", saveGeometry().toBase64());
        d->windowStateConfig.writeEntry("State", saveState().toBase64());

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

    Q_FOREACH (QDockWidget *dock, dockWidgets()) {
        if (dock) {
            dock->setProperty("Locked", false);
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

    d->commandBar->activateWindow();

}

void KisMainWindow::slotStoragesWarning(const QString & )
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

}

bool KisMainWindow::restoreWorkspace(KoResourceSP res)
{
    KisWorkspaceResourceSP workspace = res.dynamicCast<KisWorkspaceResource>();

    bool success = restoreWorkspaceState(workspace->dockerState());

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
            d->workspaceBorrowedBy = QUuid();
            return currentWorkspace;
        } else {
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
        KisImportExportErrorCode status = importer.import(files, firstFrame, step, autoAddHoldframes, startFrom1, isAscending);

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

    if (!d->activeView)
        return;

    plugActionList("toolbarlist", d->toolbarList);

    applyActionIconOverridesFromLocalXML();
    applyToolBarLayout();
}

void KisMainWindow::slotToolbarToggled(bool toggle)
{
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
    KisAndroidUtils::setFullScreen(fullScreen);
    d->fullScreenMode->setChecked(fullScreen);
#else
    KisConfig cfg(false);
    cfg.setFullscreenMode(fullScreen);

    if (fullScreen) {
        setWindowState(windowState() | Qt::WindowFullScreen);
    } else {
        setWindowState(windowState() & ~Qt::WindowFullScreen);
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

        if (!dockWidget) {
            warnKrita << "Could not create docker for" << factory->id();
            return 0;
        }

        KoDockWidgetTitleBar *titleBar = dynamic_cast<KoDockWidgetTitleBar*>(dockWidget->titleBarWidget());

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
        if (!showTitlebars) {
            if (dockWidget->titleBarWidget() && !dockWidget->titleBarWidget()->inherits("KisUtilityTitleBar")) {
                dockWidget->titleBarWidget()->setVisible(false);
            }
            dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
        }
        if (dockWidget->widget() && dockWidget->widget()->layout()
            && dockWidget->objectName() != QLatin1String("ToolBox") && dockWidget->objectName() != QLatin1String("ColorSelectorNg")) {
            const qreal marginDpr = qMax<qreal>(1.0, qApp->devicePixelRatio());
            dockWidget->widget()->layout()->setContentsMargins(qRound(28.0 / marginDpr), 1, qRound(12.0 / marginDpr), 1);
        }

        Qt::DockWidgetArea side = Qt::RightDockWidgetArea;
        bool visible = true;

        switch (factory->defaultDockPosition()) {
        case KoDockFactoryBase::DockTornOff:
            dockWidget->setFloating(true);
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

        bool locked = false;
        if (titleBar && locked) {
            titleBar->setLocked(true);
        }
        else if ( dockWidget->titleBarWidget()->inherits("KisUtilityTitleBar") && locked){
            KisUtilityTitleBar *utilityTitleBar = dynamic_cast<KisUtilityTitleBar*>(dockWidget->titleBarWidget());
            utilityTitleBar->setLocked(true);
        }

#ifdef Q_OS_ANDROID
        dockWidget->installEventFilter(new KisDockedSizeTracker(dockWidget));
        dockWidget->installEventFilter(new KrimbleDockTolerance(this, dockWidget));
        if (dockWidget->objectName() == QLatin1String("PresetDocker") || dockWidget->windowTitle() == QLatin1String("Brushes")) {
            dockWidget->setProperty("krimbleUserOpened", false);
            connect(dockWidget->toggleViewAction(), &QAction::triggered, dockWidget, [dockWidget](bool on) {
                dockWidget->setProperty("krimbleUserOpened", on);
            });
            connect(dockWidget, &QDockWidget::visibilityChanged, dockWidget, [dockWidget](bool visible) {
                if (visible) {
                    QTimer::singleShot(0, dockWidget, [dockWidget]() {
                        if (!dockWidget->property("krimbleUserOpened").toBool() && dockWidget->isVisible()) dockWidget->hide();
                    });
                } else if (dockWidget->isHidden()) {
                    dockWidget->setProperty("krimbleUserOpened", false);
                }
            });
        }
        connect(dockWidget, &QDockWidget::topLevelChanged, this, [this, dockWidget](bool floating) {
            dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);

            if (floating) {
                dockWidget->setProperty("krimbleWasFloated", true);
            } else if (dockWidget->property("krimbleWasFloated").toBool() && isVisible()
                       && dockWidget->objectName() != QLatin1String("ToolBox") && !dockWidget->property("krimbleAttaching").toBool()) {
                dockWidget->setProperty("krimbleWasFloated", false);
                QTimer::singleShot(0, this, [this, dockWidget]() {
                    if (dockWidget->isFloating()) return;
                    const Qt::DockWidgetArea area = dockWidgetArea(dockWidget);
                    QDockWidget *target = nullptr;
                    Q_FOREACH (QDockWidget *other, findChildren<QDockWidget*>()) {
                        if (other == dockWidget || other->isFloating() || !other->isVisible()) continue;
                        if (other->objectName() == QLatin1String("ToolBox") || dockWidgetArea(other) != area) continue;
                        if (tabifiedDockWidgets(other).contains(dockWidget)) { target = nullptr; break; }
                        if (!target) target = other;
                    }
                    if (target) tabifyDockWidget(target, dockWidget);
                    dockWidget->show();
                    dockWidget->raise();
                });
            }

            if (false && !floating && dockWidget->isVisible() && !dockWidget->property("krimbleAttaching").toBool()) {
                const QVariant floatingCenter = dockWidget->property("krimbleFloatingCenter");
                if (floatingCenter.isValid()) {
                    krimbleAttachDockWhereItIs(this, dockWidget, floatingCenter.toPoint());
                }
            }

            if (QWidget *bar = dockWidget->titleBarWidget()) {
                if (QLayout *barLayout = bar->layout()) {
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
                    if (dockWidget->objectName() != QLatin1String("ToolBox")) {
                        content->setMinimumSize(150, 120);
                    }
                } else if (dockWidget->property("krimbleContentMin").isValid()) {
                    content->setMinimumSize(dockWidget->property("krimbleContentMin").toSize());
                    if (contentLayout) {
                        contentLayout->setSizeConstraint(QLayout::SizeConstraint(dockWidget->property("krimbleLayoutConstraint").toInt()));
                    }
                    dockWidget->setProperty("krimbleContentMin", QVariant());
                }
            }

            const int gripStrip = 20;
            const bool stripped = dockWidget->property("krimbleGripStrip").toBool();
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

            KisFloatingDockSizeGrip *grip = static_cast<KisFloatingDockSizeGrip*>(
                dockWidget->findChild<QWidget*>(QStringLiteral("krimbleResizeHandle"), Qt::FindDirectChildrenOnly));
            if (floating && isToolBox) {
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
                const QSize dockedSize = dockWidget->property("krimbleDockedSize").toSize();
                if (dockWidget->objectName() == QLatin1String("ToolBox")) {
                    if (dockedSize.isValid()) {
                        dockWidget->resize(dockedSize);
                    }
                } else if (QScreen *scr = QGuiApplication::primaryScreen()) {
                    const QSize screenSize = scr->availableGeometry().size();
                    const int shortSide = qMin(screenSize.width(), screenSize.height());
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

     Q_FOREACH (auto key, d->actionMap.keys()) {
         if(d->actionMap.value(key)->isChecked()) {
            cfg.setWidgetStyle(key);
            qApp->setProperty(currentUnderlyingStyleNameProperty, key);
            qApp->setStyle(key);

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

static void krimbleStackPanels(QMainWindow *win)
{
    QList<QDockWidget*> docks;
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (dock->objectName() == QLatin1String("ToolBox")) continue;
        if (dock->isFloating()) {
            if (!dock->isVisible()) continue;
        } else if (win->dockWidgetArea(dock) != Qt::RightDockWidgetArea) {
            continue;
        }
        if (!dock->toggleViewAction()->isChecked()) continue;
        docks << dock;
    }
    if (docks.size() < 2) return;
    std::stable_sort(docks.begin(), docks.end(), [](QDockWidget *a, QDockWidget *b) {
        const int ya = a->isVisible() ? a->mapToGlobal(QPoint(0, 0)).y() : 1 << 20;
        const int yb = b->isVisible() ? b->mapToGlobal(QPoint(0, 0)).y() : 1 << 20;
        return ya < yb;
    });
    int totalHeight = 0;
    Q_FOREACH (QDockWidget *dock, docks) {
        if (dock->isVisible()) totalHeight += dock->height();
    }
    totalHeight = qMin(totalHeight, qMax(200, win->height() - 140));
    Q_FOREACH (QDockWidget *dock, docks) {
        win->removeDockWidget(dock);
    }
    Q_FOREACH (QDockWidget *dock, docks) {
        win->addDockWidget(Qt::RightDockWidgetArea, dock);
        if (dock->isFloating()) dock->setFloating(false);
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

    {
        KisConfig windowCfg(true);
        const bool tabbed = windowCfg.readEntry<int>("mdi_viewmode", (int)QMdiArea::TabbedView) == (int)QMdiArea::TabbedView;
        QAction *viewModeAction = menu->addAction(tabbed ? i18n("Switch to Floating Windows") : i18n("Switch to Tabs"));
        connect(viewModeAction, &QAction::triggered, this, [this, tabbed]() {
            KisConfig cfg(false);
            cfg.writeEntry<int>("mdi_viewmode", tabbed ? (int)QMdiArea::SubWindowView : (int)QMdiArea::TabbedView);
            KisConfigNotifier::instance()->notifyConfigChanged();
            if (tabbed) {
                QTimer::singleShot(0, this, [this]() { d->mdiArea->cascadeSubWindows(); });
            }
        });
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

        d->viewManager->canvasResourceProvider()->notifySavingWorkspace(workspace);
        workspace->setValid(true);

        workspace->setFilename(name.replace(" ", "_") + workspace->defaultFileExtension());
        workspace->setName(name);

        KisResourceUserOperations::addResourceWithUserInput(this, workspace);
    });

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

    {
        auto addActionByName = [this, menu](const char *name) {
            if (QAction *action = actionCollection()->action(QLatin1String(name))) {
                menu->addAction(action);
            }
        };
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
            toolbarsMenu->addSeparator();
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
                    removeToolBar(toolBar);
                    addToolBar(Qt::TopToolBarArea, toolBar);
                    toolBar->setOrientation(Qt::Horizontal);
                    toolBar->show();
                }
                QTimer::singleShot(0, this, [this]() { krimbleStackToolBars(this); });
            });
        });
        addActionByName("options_configure_toolbars");
        addActionByName("lock_toolbars");
        menu->addSeparator();
#if 0
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
        menu->addSeparator();
        addActionByName("switch_application_language");
    }

#if 0
    {
        menu->addSeparator();
        const char *panelActions[] = {"view_toggledockers", "settings_detach_panel_menu", "settings_attach_panel_menu", "settings_toggle_right_panels"};
        for (const char *name : panelActions) {
            if (QAction *action = actionCollection()->action(QLatin1String(name))) {
                menu->addAction(action);
            }
        }
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
        });
        if (QAction *lockToolbars = actionCollection()->action(QLatin1String("lock_toolbars"))) {
            menu->addAction(lockToolbars);
        }
        menu->addSeparator();
        const char *appearanceActions[] = {"theme_menu"};
        for (const char *name : appearanceActions) {
            if (QAction *action = actionCollection()->action(QLatin1String(name))) {
                menu->addAction(action);
            }
        }
    }
#endif

#if 0
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
        showWelcomeScreen(true);
    }

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

    if (subwin && subwin != d->activeSubWindow) {
        KisView *view = qobject_cast<KisView *>(subwin->widget());
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
    if (!cfg.readEntry<bool>("krimbleTabsRestored724", false)) {
        KisConfig krimbleWrite(false);
        krimbleWrite.writeEntry<int>("mdi_viewmode", (int)QMdiArea::TabbedView);
        krimbleWrite.writeEntry<bool>("krimbleTabsRestored724", true);
    }
    QMdiArea::ViewMode viewMode = (QMdiArea::ViewMode)KisConfig(true).readEntry<int>("mdi_viewmode", (int)QMdiArea::TabbedView);
    d->mdiArea->setViewMode(viewMode);
    Q_FOREACH (QMdiSubWindow *subwin, d->mdiArea->subWindowList()) {
        subwin->setOption(QMdiSubWindow::RubberBandMove, cfg.readEntry<int>("mdi_rubberband", krimbleRubberBandDefault(cfg)));
        subwin->setOption(QMdiSubWindow::RubberBandResize, cfg.readEntry<int>("mdi_rubberband", krimbleRubberBandDefault(cfg)));
        if (viewMode == QMdiArea::TabbedView) {
            subwin->setWindowState(Qt::WindowMaximized);
        }

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
    d->themeManager->setCurrentTheme(group.readEntry("Theme", "Krimble Blue-Gray"));
#endif
    new KrimbleCanvasOverlay(this);
    krimbleInstallBevels(this);
    krimbleInstallReopen(this);
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
    if (!KoColorSpaceEngineRegistry::instance()->contains("icc")) {
        m_errorMessage = i18n("The Krimble LittleCMS color management plugin is not installed. Krimble will quit now.");
        m_dieOnError = true;
        QTimer::singleShot(0, this, SLOT(showErrorAndDie()));
        return;
    }

    slotStoragesWarning();

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

    actionCollection()->addAction("style_menu", d->styleMenu);

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
    connect(d->showDonationManagementDialog, &QAction::triggered, this, []() {
        QDesktopServices::openUrl(QUrl(QStringLiteral("https://www.buymeacoffee.com/GeorgeEdwardPurdy")));
    });

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
        toolBar->setStyleSheet("QToolBar { border: none }");

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
    KConfigGroup cfg = d->windowStateConfig;
    QByteArray geom = QByteArray::fromBase64(cfg.readEntry("ko_geometry", QByteArray()));
    if (!restoreGeometry(geom)) {
        QRect desk = this->screen()->availableGeometry();

        quint32 x = desk.x();
        quint32 y = desk.y();
        quint32 w = 0;
        quint32 h = 0;

        const int deskWidth = desk.width();
        if (deskWidth > 1024) {
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
        if ((window->type() == Qt::Popup)
            && (window->flags() & Qt::FramelessWindowHint) == 0
            && (window->geometry().topLeft() != QPoint(0, 0))) {
            int screenHeight = screen->geometry().width();
            int screenWidth = screen->geometry().height();

            int new_x = (window->position().x() * screenWidth) / screenHeight;
            int new_y = (window->position().y() * screenHeight) / screenWidth;

            int winWidth = window->geometry().width();
            int winHeight = window->geometry().height();

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
