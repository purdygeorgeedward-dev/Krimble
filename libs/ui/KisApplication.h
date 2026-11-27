/*
 * SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
 * SPDX-FileCopyrightText: 2012 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KIS_APPLICATION_H
#define KIS_APPLICATION_H

#include <QScopedPointer>
#include <qtsingleapplication/qtsingleapplication.h>
#include "kritaui_export.h"

class KisMainWindow;
class KisApplicationPrivate;
class QWidget;
class KisApplicationArguments;
class KisAutoSaveRecoveryDialog;
class KisExtendedModifiersMapperPluginInterface;
class KisAndroidDonations;
class KisAndroidScaling;

#include <KisImportExportManager.h>

KRITAUI_EXPORT void krimbleEnsureBundledFonts();

class KRITAUI_EXPORT KisApplication : public QtSingleApplication
{
    Q_OBJECT

public:
    explicit KisApplication(const QString &key, int &argc, char **argv);

    ~KisApplication() override;

    virtual bool start(const KisApplicationArguments &args);

    bool event(QEvent *event) override;

    void askResetConfig();

    void setSplashScreen(QWidget *splash);
    void hideSplashScreen();

    bool notify(QObject *receiver, QEvent *event) override;

    void addResourceTypes();
    bool registerResources();
    void loadPlugins();
    void initializeGlobals(const KisApplicationArguments &args);
    void processPostponedSynchronizationEvents();

    bool isStoreApplication();

    static void verifyMetatypeRegistration();

    KisExtendedModifiersMapperPluginInterface* extendedModifiersPluginInterface();

#ifdef Q_OS_ANDROID
    KisAndroidDonations *androidDonations();
    KisAndroidScaling *androidScaling();
#endif

public Q_SLOTS:

    void executeRemoteArguments(QByteArray message, KisMainWindow *mainWindow);
    void remoteArguments(const QString &message);
    void fileOpenRequested(const QString & url);
    void setSplashScreenLoadingText(const QString&);

private Q_SLOTS:
    void slotSetLongPress(bool enabled);

private:
    void checkAutosaveFiles();
    bool createNewDocFromTemplate(const QString &fileName, KisMainWindow *m_mainWindow);
    void resetConfig();

private:
    class Private;
    QScopedPointer<Private> d;
    class ResetStarting;
    friend class ResetStarting;
};

#endif
