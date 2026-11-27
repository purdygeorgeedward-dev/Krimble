/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2000-2005 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@valdyas.org>
   SPDX-FileCopyrightText: 2015 Michael Abrahams <miabraha@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KIS_PART_H
#define KIS_PART_H

#include <QList>
#include <QPointer>
#include <QUrl>
#include <QUuid>

#include <KisSessionResource.h>

#include "kritaui_export.h"
#include <kconfiggroup.h>
#include <KoConfig.h>
#include <kis_types.h>

namespace KIO {
}

class KisAction;
class KisDocument;
class KisView;
class KisDocument;
class KisIdleWatcher;
class KisAnimationCachePopulator;
class KisMainWindow;
class KisInputManager;
class KisViewManager;

class KRITAUI_EXPORT KisPart : public QObject
{
    Q_OBJECT

public:

    static KisPart *instance();

    explicit KisPart();

    ~KisPart() override;

    KisDocument *createDocument() const;

    KisDocument *createTemporaryDocument() const;

    void addDocument(KisDocument *document, bool notify = true);

    QList<QPointer<KisDocument> > documents() const;

    int documentCount() const;

    void removeDocument(KisDocument *document, bool deleteDocument = true);

    KisMainWindow *createMainWindow(QUuid id = QUuid());

    void notifyMainWindowIsBeingCreated(KisMainWindow *mainWindow);

    void removeMainWindow(KisMainWindow *mainWindow);

    const QList<QPointer<KisMainWindow> >& mainWindows() const;

    int mainwindowCount() const;

    void addRecentURLToAllMainWindows(QUrl url, QUrl oldUrl = QUrl());

    void queueAddRecentURLToAllMainWindowsOnFileSaved(QUrl url, QUrl oldUrl = QUrl());

    KisMainWindow *currentMainwindow() const;

    QWidget *currentMainwindowAsQWidget() const;

    KisMainWindow *windowById(QUuid id) const;

    KisIdleWatcher *idleWatcher() const;

    KisAnimationCachePopulator *cachePopulator() const;

    class KisPlaybackEngine* playbackEngine() const;

    void prioritizeFrameForCache(KisImageSP image, int frame);

public Q_SLOTS:

    void openExistingFile(const QString &path);

    void openTemplate(const QUrl &url);

    void startCustomDocument(KisDocument *doc);

private Q_SLOTS:

    void updateIdleWatcherConnections();

    void updateShortcuts();

Q_SIGNALS:
    void documentOpened(const QString &ref);

    void documentClosed(const QString &ref);

   void playbackEngineChanged(KisPlaybackEngine *newPlaybackEngine);

    void sigViewAdded(KisView *view);
    void sigViewRemoved(KisView *view);
    void sigDocumentAdded(KisDocument *document);
    void sigDocumentSaved(const QString &url);
    void sigDocumentRemoved(const QString &filename);
    void sigMainWindowIsBeingCreated(KisMainWindow *window);
    void sigMainWindowCreated();

public:

    KisInputManager *currentInputManager();

    KisView *createView(KisDocument *document,
                        KisViewManager *viewManager,
                        QWidget *parent);

    void addView(KisView *view);

    void removeView(KisView *view);

    QList<QPointer<KisView> > views() const;

    int viewCount(KisDocument *doc) const;

    void showSessionManager();

    void startBlankSession();

    bool restoreSession(const QString &sessionName);
    bool restoreSession(KisSessionResourceSP session);

    void setCurrentSession(KisSessionResourceSP session);

    bool closeSession(bool keepWindows = false);

    bool closingSession() const;

    static bool exists();

public:
    void upgradeToPlaybackEngineMLT(class KoCanvasBase *canvas);

    void unloadPlaybackEngine();

private:

    void setPlaybackEngine(KisPlaybackEngine *p_playbackEngine);

private Q_SLOTS:

    void slotDocumentSaved(const QString &filePath);

private:

    Q_DISABLE_COPY(KisPart)

    class Private;
    Private *const d;

};

#endif
