/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_GUI_CLIENT_H
#define KIS_GUI_CLIENT_H

#include <QQueue>
#include <QPointer>
#include <QMainWindow>

#include <KoToolManager.h>

#include <kritaui_export.h>
#include <kis_types.h>

#include "kis_floating_message.h"

class QPoint;
class KisView;

class KisCanvas2;
class KisCanvasResourceProvider;
class KisDocument;
class KisFilterManager;
class KisGridManager;
class KisGuidesManager;
class KisImageManager;
class KisNodeManager;
class KisDecorationsManager;
class KisPaintopBox;
class KisSelectionManager;
class KisStatusBar;
class KisUndoAdapter;
class KisZoomManager;
class KisPaintopBox;
class KisActionManager;
class KisInputManager;
class KoUpdater;
class KoProgressUpdater;
class KisMainWindow;
class KoCanvasResourceProvider;
class KisIdleTasksManager;
class KisTextPropertiesManager;

class KRITAUI_EXPORT KisViewManager : public QObject
{

    Q_OBJECT

public:
    KisViewManager(QWidget *parent, KisKActionCollection *actionCollection);
    ~KisViewManager() override;

    virtual KisKActionCollection* actionCollection() const;

public:

    void setCurrentView(KisView *view);

    KisImageWSP image() const;

    KisCanvasResourceProvider *canvasResourceProvider();

    KisCanvas2 *canvasBase() const;

    QWidget* canvas() const;

    KisStatusBar *statusBar() const;

    KisPaintopBox* paintOpBox() const;

    QPointer<KoUpdater> createUnthreadedUpdater(const QString &name);
    QPointer<KoUpdater> createThreadedUpdater(const QString &name);

    KisSelectionManager *selectionManager();

    KisNodeManager *nodeManager() const;

    KisActionManager *actionManager() const;

    KisNodeSP activeNode();

    KisLayerSP activeLayer();

    KisPaintDeviceSP activeDevice();

    KisFilterManager *filterManager();

    KisImageManager *imageManager();

    KisInputManager *inputManager() const;

    KisIdleTasksManager *idleTasksManager();

    KisTextPropertiesManager *textPropertyManager() const;

    KisSelectionSP selection();

    bool selectionEditable();

    KisUndoAdapter *undoAdapter();

    KisDocument *document() const;

    int viewCount() const;

    bool blockUntilOperationsFinished(KisImageSP image);

    void blockUntilOperationsFinishedForced(KisImageSP image);

    void notifyWorkspaceLoaded();

public:

    KisGridManager * gridManager() const;
    KisGuidesManager * guidesManager() const;

    void enableControls();
    void disableControls();

    void showFloatingMessage(const QString &message, const QIcon& icon, int timeout = 4500,
                             KisFloatingMessage::Priority priority = KisFloatingMessage::Medium,
                             int alignment = Qt::AlignCenter | Qt::TextWordWrap);

    void showFloatingZoomMessage(const QString &message);
    void showFloatingRotationMessage(const QString &message);

    KisMainWindow *mainWindow() const;

    QWidget *mainWindowAsQWidget() const;

    QMainWindow* qtMainWindow() const;

    void setQtMainWindow(QMainWindow* newMainWindow);

    static void initializeResourceManager(KoCanvasResourceProvider *resourceManager);
    static void testingInitializeOpacityToPresetResourceConverter(KoCanvasResourceProvider *resourceManager);

public Q_SLOTS:

    void switchCanvasOnly(bool toggled);
    void setShowFloatingMessage(bool show);
    void showHideScrollbars();

    void updateGUI();

    void updateIcons();

    void slotViewAdded(KisView *view);
    void slotViewRemoved(KisView *view);

    void slotActivateTransformTool();

    void changeAuthorProfile(const QString &profileName);
    void slotUpdateAuthorProfileActions();

    void slotToggleFgBg();

    void slotToggleBrushOutline();

Q_SIGNALS:

    void floatingMessageRequested(const QString &message, const QString &iconName);
    void viewChanged();

    void brushOutlineToggled();

private Q_SLOTS:

    void slotCreateTemplate();
    void slotCreateCopy();
    void slotDocumentSaved();
    void slotSaveIncremental();
    void slotSaveIncrementalBackup();
    void showStatusBar(bool toggled);
    void toggleTabletLogger();
    void openResourcesDirectory();
    void guiUpdateTimeout();
    void slotUpdatePixelGridAction();
    void slotSaveShowRulersState(bool value);
    void slotSaveRulersTrackMouseState(bool value);
    void slotResetRotation();
    void slotResetDisplay();
    void slotCreateOpacityResource(bool isOpacityPresetMode, KoToolBase *tool);
private:
    void createActions();
    void setupManagers();

    QString canonicalPath();

    KisZoomManager * zoomManager();

    void handleFloatingZoomRotationMessage(QString &messageToClear);

    void updateCanvasOnlyActionState();

private:
    class KisViewManagerPrivate;
    KisViewManagerPrivate * const d;
};

#endif
