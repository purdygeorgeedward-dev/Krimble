/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>
 * SPDX-FileCopyrightText: 2000-2005 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2010-2012 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2011 Inge Wallin <ingwa@kogmbh.com>
 * SPDX-FileCopyrightText: 2015 Michael Abrahams <miabraha@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KisPart.h"

#include <config-mlt.h>

#include "KoProgressProxy.h"
#include <KoCanvasController.h>
#include <KoCanvasControllerWidget.h>
#include <KoColorSpaceEngine.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoShapeControllerBase.h>
#include <KoResourceServerProvider.h>
#include <kis_icon.h>

#include "KisApplication.h"
#include "KisMainWindow.h"
#include "KisDocument.h"
#include "KisView.h"
#include "KisViewManager.h"
#include "KisImportExportManager.h"
#include "KoDocumentInfo.h"
#include "KisUsageLogger.h"

#include <kis_debug.h>
#include <KoResourcePaths.h>
#include <KoDialog.h>
#include <QMessageBox>
#include <QMenu>
#include <QScopedPointer>
#include <QMap>
#include <QRegularExpression>

#include <QMenuBar>
#include <klocalizedstring.h>
#include <kactioncollection.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <QKeySequence>

#include <QApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QGlobalStatic>
#include <KisMimeDatabase.h>
#include <dialogs/KisSessionManagerDialog.h>

#include <kis_group_layer.h>
#include "kis_config.h"
#include "kis_shape_controller.h"
#include "KisResourceServerProvider.h"
#include "kis_animation_cache_populator.h"
#include "kis_image_animation_interface.h"
#include "kis_time_span.h"
#include "kis_idle_watcher.h"
#include "kis_image.h"
#include "KisTranslateLayerNamesVisitor.h"
#include "kis_color_manager.h"

#include <KisCursorOverrideLock.h>
#include "kis_action.h"
#include "kis_action_registry.h"
#include "KisSessionResource.h"
#include "KisBusyWaitBroker.h"
#include "dialogs/kis_delayed_save_dialog.h"
#include "kis_memory_statistics_server.h"
#include "KisRecentFilesManager.h"
#include "KisRecentFileIconCache.h"
#include "KisPlaybackEngine.h"
#include "KisPlaybackEngineQT.h"

#ifdef HAVE_MLT
#include "KisPlaybackEngineMLT.h"
#endif

Q_GLOBAL_STATIC(KisPart, s_instance)

class Q_DECL_HIDDEN KisPart::Private
{
public:
    Private(KisPart *_part)
        : part(_part)
        , idleWatcher(2500)
        , animationCachePopulator(_part)
        , playbackEngine(nullptr)
    {
    }

    ~Private()
    {
    }

    KisPart *part;

    QList<QPointer<KisView> > views;
    QList<QPointer<KisMainWindow> > mainWindows;
    QList<QPointer<KisDocument> > documents;
    KisIdleWatcher idleWatcher;
    KisAnimationCachePopulator animationCachePopulator;
    std::unique_ptr<KisPlaybackEngine> playbackEngine;

    KisSessionResourceSP currentSession;
    bool closingSession{false};
    QPointer<KisSessionManagerDialog> sessionManager;

    QMap<QUrl, QUrl> pendingAddRecentUrlMap;

    bool queryCloseDocument(KisDocument *document) {
        Q_FOREACH(auto view, views) {
            if (view && view->isVisible() && view->document() == document) {
                return view->queryClose();
            }
        }

        return true;
    }
};

KisPart* KisPart::instance()
{
    return s_instance;
}

namespace {
void busyWaitWithFeedback(KisImageSP image)
{
    const int busyWaitDelay = 1000;
    if (KisPart::instance()->currentMainwindow()) {
        KisDelayedSaveDialog dialog(image, KisDelayedSaveDialog::ForcedDialog, busyWaitDelay, KisPart::instance()->currentMainwindow());
        dialog.blockIfImageIsBusy();
    }
}
}

KisPart::KisPart()
    : d(new Private(this))
{
    Q_UNUSED(KoResourceServerProvider::instance());
    Q_UNUSED(KisResourceServerProvider::instance());
    Q_UNUSED(KisColorManager::instance());

    connect(this, SIGNAL(documentOpened(QString)),
            this, SLOT(updateIdleWatcherConnections()));

    connect(this, SIGNAL(documentClosed(QString)),
            this, SLOT(updateIdleWatcherConnections()));

    connect(KisActionRegistry::instance(), SIGNAL(shortcutsUpdated()),
            this, SLOT(updateShortcuts()));
    connect(&d->idleWatcher, SIGNAL(startedIdleMode()),
            &d->animationCachePopulator, SLOT(slotRequestRegeneration()));
    connect(&d->idleWatcher, SIGNAL(startedIdleMode()),
            KisMemoryStatisticsServer::instance(), SLOT(tryForceUpdateMemoryStatisticsWhileIdle()));

    setPlaybackEngine(new KisPlaybackEngineQT(this));

    d->animationCachePopulator.slotRequestRegeneration();
    KisBusyWaitBroker::instance()->setFeedbackCallback(&busyWaitWithFeedback);
}

KisPart::~KisPart()
{
    while (!d->documents.isEmpty()) {
        delete d->documents.takeFirst();
    }

    while (!d->views.isEmpty()) {
        delete d->views.takeFirst();
    }

    while (!d->mainWindows.isEmpty()) {
        delete d->mainWindows.takeFirst();
    }

    delete d;
}

void KisPart::updateIdleWatcherConnections()
{
    QVector<KisImageSP> images;

    Q_FOREACH (QPointer<KisDocument> document, documents()) {
        if (document->image()) {
            images << document->image();
        }
    }

    d->idleWatcher.setTrackedImages(images);

    d->idleWatcher.forceImageModified();
}

void KisPart::addDocument(KisDocument *document, bool notify)
{
    Q_ASSERT(document);
    if (!d->documents.contains(document)) {
        d->documents.append(document);
        if (notify){
            Q_EMIT documentOpened('/'+ objectName());
            Q_EMIT sigDocumentAdded(document);
        }
        connect(document, SIGNAL(sigSavingFinished(QString)), SLOT(slotDocumentSaved(QString)));
    }
}

QList<QPointer<KisDocument> > KisPart::documents() const
{
    return d->documents;
}

KisDocument *KisPart::createDocument() const
{
    KisDocument *doc = new KisDocument();
    return doc;
}

KisDocument *KisPart::createTemporaryDocument() const
{
    KisDocument *doc = new KisDocument(false);
    return doc;
}

int KisPart::documentCount() const
{
    return d->documents.size();
}

void KisPart::removeDocument(KisDocument *document, bool deleteDocument)
{
    if (document) {
        d->documents.removeAll(document);
        Q_EMIT documentClosed('/' + objectName());
        Q_EMIT sigDocumentRemoved(document->path());
        if (deleteDocument) {
            document->deleteLater();
        }
    }
}

KisMainWindow *KisPart::createMainWindow(QUuid id)
{
    KisMainWindow *mw = new KisMainWindow(id);
    dbgUI <<"mainWindow" << (void*)mw << "added to view" << this;
    d->mainWindows.append(mw);

    mw->synchronizeDynamicActions();

    return mw;
}

void KisPart::notifyMainWindowIsBeingCreated(KisMainWindow *mainWindow)
{
    Q_EMIT sigMainWindowIsBeingCreated(mainWindow);
}

KisView *KisPart::createView(KisDocument *document,
                             KisViewManager *viewManager,
                             QWidget *parent)
{
    KisConfig cfg(false);
    KConfigGroup grp( KSharedConfig::openConfig(), "crashprevention");
    if (grp.readEntry("CreatingCanvas", false)) {
        cfg.disableOpenGL();
    }
    if (cfg.canvasState() == "OPENGL_FAILED") {
        cfg.disableOpenGL();
    }
    grp.writeEntry("CreatingCanvas", true);
    grp.sync();

    KisView *view = nullptr;
    {
        KisCursorOverrideLock cursorLock(Qt::WaitCursor);
        view = new KisView(document,  viewManager, parent);
    }

    grp.writeEntry("CreatingCanvas", false);
    grp.sync();

    addView(view);

    return view;
}

void KisPart::addView(KisView *view)
{
    if (!view)
        return;

    if (!d->views.contains(view)) {
        d->views.append(view);
    }

    Q_EMIT sigViewAdded(view);
}

void KisPart::removeView(KisView *view)
{
    if (!view) return;

    KIS_ASSERT_RECOVER_RETURN(!view->mainWindow()->hackIsSaving());

    Q_EMIT sigViewRemoved(view);

    QPointer<KisDocument> doc = view->document();
    d->views.removeAll(view);

    if (doc) {
        bool found = false;
        Q_FOREACH (QPointer<KisView> view, d->views) {
            if (view && view->document() == doc) {
                found = true;
                break;
            }
        }
        if (!found) {
            removeDocument(doc);
        }
    }
}

QList<QPointer<KisView> > KisPart::views() const
{
    return d->views;
}

int KisPart::viewCount(KisDocument *doc) const
{
    if (!doc) {
        return d->views.count();
    }
    else {
        int count = 0;
        Q_FOREACH (QPointer<KisView> view, d->views) {
            if (view && view->isVisible() && view->document() == doc) {
                count++;
            }
        }
        return count;
    }
}

bool KisPart::closingSession() const
{
    return d->closingSession;
}

bool KisPart::exists()
{
    return s_instance.exists();
}

bool KisPart::closeSession(bool keepWindows)
{
    d->closingSession = true;

    Q_FOREACH(auto document, d->documents) {
        if (!d->queryCloseDocument(document.data())) {
            d->closingSession = false;
            return false;
        }
    }

    if (d->currentSession) {
        KisConfig kisCfg(false);
        if (kisCfg.saveSessionOnQuit(false)) {

            d->currentSession->storeCurrentWindows();
            KisResourceModel model(ResourceType::Sessions);
            bool result = model.updateResource(d->currentSession);
            Q_UNUSED(result);

            KConfigGroup cfg = KSharedConfig::openConfig()->group("session");
            cfg.writeEntry("previousSession", d->currentSession->name());
        }

        d->currentSession = nullptr;
    }

    if (!keepWindows) {
        Q_FOREACH (auto window, d->mainWindows) {
            window->close();
        }

        if (d->sessionManager) {
            d->sessionManager->close();
        }
    }

    d->closingSession = false;
    return true;
}

void KisPart::slotDocumentSaved(const QString &filePath)
{
    Q_EMIT sigDocumentSaved(filePath);

    QUrl url = QUrl::fromLocalFile(filePath);
    KisRecentFileIconCache::instance()->reloadFileIcon(url);
    if (!d->pendingAddRecentUrlMap.contains(url)) {
        return;
    }
    QUrl oldUrl = d->pendingAddRecentUrlMap.take(url);
    addRecentURLToAllMainWindows(url, oldUrl);
}

void KisPart::removeMainWindow(KisMainWindow *mainWindow)
{
    dbgUI <<"mainWindow" << (void*)mainWindow <<"removed from doc" << this;
    if (mainWindow) {
        d->mainWindows.removeAll(mainWindow);
    }
}

const QList<QPointer<KisMainWindow> > &KisPart::mainWindows() const
{
    return d->mainWindows;
}

int KisPart::mainwindowCount() const
{
    return d->mainWindows.count();
}

KisMainWindow *KisPart::currentMainwindow() const
{
    QWidget *widget = qApp->activeWindow();
    KisMainWindow *mainWindow = qobject_cast<KisMainWindow*>(widget);
    while (!mainWindow && widget) {
        widget = widget->parentWidget();
        mainWindow = qobject_cast<KisMainWindow*>(widget);
    }

    if (!mainWindow && mainWindows().size() > 0) {
        mainWindow = mainWindows().first();
    }
    return mainWindow;

}

QWidget *KisPart::currentMainwindowAsQWidget() const
{
    return currentMainwindow();
}

KisMainWindow * KisPart::windowById(QUuid id) const
{
    Q_FOREACH(QPointer<KisMainWindow> mainWindow, d->mainWindows) {
        if (mainWindow->id() == id) {
            return mainWindow;
        }
    }

    return nullptr;
}

KisIdleWatcher* KisPart::idleWatcher() const
{
    return &d->idleWatcher;
}

KisAnimationCachePopulator* KisPart::cachePopulator() const
{
    return &d->animationCachePopulator;
}

KisPlaybackEngine *KisPart::playbackEngine() const
{
    return d->playbackEngine.get();
}

void KisPart::prioritizeFrameForCache(KisImageSP image, int frame) {
    KisImageAnimationInterface* animInterface = image->animationInterface();
    if ( animInterface && animInterface->documentPlaybackRange().contains(frame)) {
        d->animationCachePopulator.requestRegenerationWithPriorityFrame(image, frame);
    }
}

void KisPart::openExistingFile(const QString &path)
{

    KisMainWindow *mw = currentMainwindow();
    KIS_SAFE_ASSERT_RECOVER_RETURN(mw);

    mw->openDocument(path, KisMainWindow::None);
}

void KisPart::updateShortcuts()
{
    Q_FOREACH (KisMainWindow *mainWindow, d->mainWindows) {
        KisKActionCollection *ac = mainWindow->actionCollection();

        ac->updateShortcuts();
    }
}

void KisPart::openTemplate(const QUrl &url)
{
    KisCursorOverrideLock cursorLock(Qt::BusyCursor);

    KisDocument *document = createDocument();

    bool ok = document->loadNativeFormat(url.toLocalFile());
    document->setModified(false);
    document->undoStack()->clear();
    document->documentInfo()->resetMetaData();

    if (ok) {
        QString mimeType = KisMimeDatabase::mimeTypeForFile(url.toLocalFile());
        mimeType.remove( QRegularExpression( "-template$" ) );
        document->setMimeTypeAfterLoading(mimeType);
        document->resetPath();
        document->setReadWrite(true);
    }
    else {
        if (document->errorMessage().isEmpty()) {
            QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Krimble"), i18n("Could not create document from template\n%1", document->localFilePath()));
        }
        else {
            QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Krimble"), i18n("Could not create document from template\n%1\nReason: %2", document->localFilePath(), document->errorMessage()));
        }
        delete document;
        return;
    }
    QMap<QString, QString> dictionary;
    KisTranslateLayerNamesVisitor v(dictionary);
    document->image()->rootLayer()->accept(v);

    addDocument(document);

    KisMainWindow *mw = currentMainwindow();
    mw->addViewAndNotifyLoadingCompleted(document);
}

void KisPart::addRecentURLToAllMainWindows(QUrl url, QUrl oldUrl)
{
    if (!url.isEmpty()) {
        bool ok = true;
        if (url.isLocalFile()) {
            QString path = url.adjusted(QUrl::StripTrailingSlash).toLocalFile();
            const QStringList tmpDirs = QStandardPaths::locateAll(QStandardPaths::TempLocation, "", QStandardPaths::LocateDirectory);
            for (QStringList::ConstIterator it = tmpDirs.begin() ; ok && it != tmpDirs.end() ; ++it) {
                if (path.contains(*it)) {
                    ok = false;
                }
            }

            const QStringList templateDirs = KoResourcePaths::findDirs("templates");
            for (QStringList::ConstIterator it = templateDirs.begin() ; ok && it != templateDirs.end() ; ++it) {
                if (path.contains(*it)) {
                    ok = false;
                    break;
                }
            }
        }
        if (ok) {
            if (!oldUrl.isEmpty()) {
                KisRecentFilesManager::instance()->remove(oldUrl);
            }
            KisRecentFilesManager::instance()->add(url);
        }
    }
}

void KisPart::queueAddRecentURLToAllMainWindowsOnFileSaved(QUrl url, QUrl oldUrl)
{
    d->pendingAddRecentUrlMap.insert(url, oldUrl);
}

void KisPart::startCustomDocument(KisDocument* doc)
{
    addDocument(doc);
    KisMainWindow *mw = currentMainwindow();
    mw->addViewAndNotifyLoadingCompleted(doc);

}

KisInputManager* KisPart::currentInputManager()
{
    KisMainWindow *mw = currentMainwindow();
    KisViewManager *manager = mw ? mw->viewManager() : 0;
    return manager ? manager->inputManager() : 0;
}

void KisPart::showSessionManager()
{
    if (d->sessionManager.isNull()) {
        d->sessionManager = new KisSessionManagerDialog();
        d->sessionManager->setAttribute(Qt::WA_DeleteOnClose);
    }

    d->sessionManager->show();
    d->sessionManager->activateWindow();
}

void KisPart::startBlankSession()
{
    KisMainWindow *window = createMainWindow();
    window->initializeGeometry();
    window->show();

}

bool KisPart::restoreSession(const QString &sessionName)
{
    if (sessionName.isNull()) return false;

    KoResourceServer<KisSessionResource> *rserver = KisResourceServerProvider::instance()->sessionServer();
    KisSessionResourceSP session = rserver->resource("", "", sessionName);
    if (!session || !session->valid()) return false;

    return restoreSession(session);
}

bool KisPart::restoreSession(KisSessionResourceSP session)
{
    session->restore();
    d->currentSession = session;
    return true;
}

void KisPart::setCurrentSession(KisSessionResourceSP session)
{
    d->currentSession = session;
}

void KisPart::upgradeToPlaybackEngineMLT(KoCanvasBase* canvas)
{
#ifdef HAVE_MLT

    if (d->playbackEngine->supportsAudio()) {
        return;
    }

    setPlaybackEngine(new KisPlaybackEngineMLT(this));

    if (canvas) {
        d->playbackEngine->setObservedCanvas(canvas);
    }

#endif
}

void KisPart::unloadPlaybackEngine()
{
#ifdef HAVE_MLT

    d->playbackEngine.reset();

#endif
}

void KisPart::setPlaybackEngine(KisPlaybackEngine *p_playbackEngine)
{
    std::unique_ptr<KisPlaybackEngine> backup(p_playbackEngine);
    d->playbackEngine.swap(backup);

    KisUsageLogger::log("Audio Playback Engine: " + QString(p_playbackEngine->metaObject()->className()));

    Q_EMIT playbackEngineChanged(p_playbackEngine);
}

#include "moc_KisPart.cpp"
