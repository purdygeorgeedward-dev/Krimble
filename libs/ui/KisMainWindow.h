/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2000-2004 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KIS_MAIN_WINDOW_H
#define KIS_MAIN_WINDOW_H

#include "kritaui_export.h"

#include <QPointer>
#include <QPrinter>
#include <QUuid>
#include <QUrl>

#include <xmlgui/kxmlguiwindow.h>
#include <KoCanvasObserverBase.h>
#include <KoCanvasSupervisor.h>
#include "KisView.h"
#include <kis_workspace_resource.h>
#include <KoResource.h>

class QCloseEvent;
class QMoveEvent;

class KoCanvasResourceProvider;

class KisDocument;
class KoDockFactoryBase;
class QDockWidget;
class KisView;
class KisViewManager;
class KoCanvasController;

class KRITAUI_EXPORT KisMainWindow : public KXmlGuiWindow, public KoCanvasSupervisor
{
    Q_OBJECT

public:
    enum OpenFlag {
        None = 0,
        Import = 0x1,
        BatchMode = 0x2,
        RecoveryFile = 0x4
    };
    Q_DECLARE_FLAGS(OpenFlags, OpenFlag)

public:

    explicit KisMainWindow(QUuid id = QUuid());

    ~KisMainWindow() override;

    QMenu *createPopupMenu() override;

    QUuid id() const;

    virtual void showView(KisView *view, QMdiSubWindow *subWindow = 0);

    KisView *activeView() const;

    bool openDocument(const QString &path, OpenFlags flags);

    void showDocument(KisDocument *document);

    void showWelcomeScreen(bool show);

    bool saveDocument(KisDocument *document, bool saveas, bool isExporting, bool isAdvancedExporting = false);

    QList<QDockWidget*> dockWidgets() const;

    QDockWidget* dockWidget(const QString &id);

    QList<KoCanvasObserverBase*> canvasObservers() const override;

    KoCanvasResourceProvider *resourceManager() const;

    int viewCount() const;

    void saveWindowState(bool restoreNormalState =false);

    const KConfigGroup &windowStateConfig() const;

    bool restoreWorkspace(KoResourceSP res);
    bool restoreWorkspaceState(const QByteArray &state);

    static void swapWorkspaces(KisMainWindow *a, KisMainWindow *b);

    KisViewManager *viewManager() const;

    KisView *addViewAndNotifyLoadingCompleted(KisDocument *document,
                                              QMdiSubWindow *subWindow = 0);

    QStringList showOpenFileDialog(bool isImporting);

    QWidget *canvasWindow() const;
    bool canvasDetached() const;

    bool hackIsSaving() const;

    bool installBundle(const QString &fileName) const;

    QImage layoutThumbnail();

Q_SIGNALS:

    void documentSaved();

    void loadCompleted();

    void restoringDone();

    void themeChanged();

    void keyBindingsChanged();

    void guiLoadingFinished();

    void activeViewChanged();

public Q_SLOTS:

    void clearRecentFiles();

    void removeRecentFile(QString url);

    void slotFileNew();

    void slotFileOpen(bool isImporting = false);

    void slotFileOpenRecent(const QUrl &);

    void slotPreferences();

    void slotFileSave();

    void slotFileRevert();

    void slotChangeInterfaceScale();

    void slotShowSessionManager();

#ifdef Q_OS_ANDROID
    void slotShowDonationManagementDialog();
    void slotFlashWindowHack();
#endif

    void newOptionWidgets(KoCanvasController *controller, const QList<QPointer<QWidget> > & optionWidgetList);

    KisView *newView(QObject *document, QMdiSubWindow *subWindow = 0);

    void notifyChildViewDestroyed(KisView *view);

    void setActiveView(KisView *view);
    void unsetActiveView();

    void subWindowActivated();

    void windowFocused();

    void setCanvasDetached(bool detached);

    void viewFullscreen(bool fullScreen);

    bool checkActiveBundlesAvailable();
    bool checkPaintOpAvailable();

    void synchronizeDynamicActions();

private Q_SLOTS:
    void slotLoadCompleted();
    void slotLoadCanceled(const QString &);
    void slotSaveCompleted();
    void slotSaveCanceled(const QString &);
    void forceDockTabFonts();

    void slotUpdateWidgetStyle();

    void slotUpdateSaveActionTitle(const QString &documentPath);
    void slotUpdateReadWriteMode(bool readWrite);

    void slotFileSaveAs();

    void importAnimation();

    void importVideoAnimation();

    void renderAnimation();

    void renderAnimationAgain();

    void slotDocumentInfo();

    bool slotFileCloseAll();
    void slotVisitFeatureRequests();
    void slotVisitKrimbleWebsite();
    void slotVisitKrimbleForum();

    virtual void showAboutApplication();

    void slotFileQuit();

    void slotConfigureToolbars();

    void slotNewToolbarConfig();

    void slotResetConfigurations();

    void slotToolbarToggled(bool toggle);

    void showDockerTitleBars(bool show);

    void slotImportFile();

    void slotExportFile();

    void slotExportAdvance();

    void toggleDockersVisibility(bool visible, bool onWelcomePage = false);

    void updateTheme();
    void slotThemeChanged();

    void undo();
    void redo();
    void updateWindowMenu();
    void updateSubwindowFlags();
    void setActiveSubWindow(QWidget *window);
    void configChanged();

    void newWindow();
    void closeCurrentWindow();
    void checkSanity();

    void showErrorAndDie();

    void initializeGeometry();
    void showManual();
    void switchTab(int index);

    void slotXmlGuiMakingChanges(bool finished);

    void orientationChanged();

    void restoreWorkspace();

    void openCommandBar();

    void slotStoragesWarning(const QString &location = QString());

protected:

    void closeEvent(QCloseEvent * e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    void resizeEvent(QResizeEvent * e) override;
    void showEvent(QShowEvent *event) override;

    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

    bool windowsLayoutSavingAllowed() const override;

private:

    friend class KisWelcomePageWidget;
    void dragMove(QDragMoveEvent *event);
    void dragLeave();

private:

    void addView(KisView *view, QMdiSubWindow *subWindow = 0);

    friend class KisPart;

    QDockWidget* createDockWidget(KoDockFactoryBase* factory);

    bool openDocumentInternal(const QString &path, KisMainWindow::OpenFlags f = KisMainWindow::OpenFlags());

    void saveWindowSettings();

    QPointer<KisView> activeKisView();

    void createActions();

    void applyToolBarLayout();

    QByteArray borrowWorkspace(KisMainWindow *borrower);

    void customizeTabBar();

    void setMainWindowLayoutForCurrentMainWidget(int widgetIndex, bool widgetIndexChanged);
    void adjustLayoutForWelcomePage();
    void applyActionIconOverridesFromLocalXML();

private:
    class Private;
    Private * const d;

    QString m_errorMessage;
    bool m_dieOnError;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KisMainWindow::OpenFlags)

#endif
