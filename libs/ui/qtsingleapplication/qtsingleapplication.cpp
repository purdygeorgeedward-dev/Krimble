// Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
// SPDX-License-Identifier: BSD-3-Clause

#include "qtsingleapplication.h"
#include "qtlocalpeer.h"
#include <QWidget>

void QtSingleApplication::sysInit(const QString &appId)
{
    actWin = 0;
    peer = new QtLocalPeer(this, appId);
    connect(peer, SIGNAL(messageReceived(const QString&)), SIGNAL(messageReceived(const QString&)));
}

QtSingleApplication::QtSingleApplication(int &argc, char **argv, bool GUIenabled)
    : QApplication(argc, argv, GUIenabled)
{
    sysInit();
}

QtSingleApplication::QtSingleApplication(const QString &appId, int &argc, char **argv)
    : QApplication(argc, argv)
{
    sysInit(appId);
}

#if QT_VERSION < 0x050000

QtSingleApplication::QtSingleApplication(int &argc, char **argv, Type type)
    : QApplication(argc, argv, type)
{
    sysInit();
}

#  if defined(Q_WS_X11)
QtSingleApplication::QtSingleApplication(Display* dpy, Qt::HANDLE visual, Qt::HANDLE cmap)
    : QApplication(dpy, visual, cmap)
{
    sysInit();
}

QtSingleApplication::QtSingleApplication(Display *dpy, int &argc, char **argv, Qt::HANDLE visual, Qt::HANDLE cmap)
    : QApplication(dpy, argc, argv, visual, cmap)
{
    sysInit();
}

QtSingleApplication::QtSingleApplication(Display* dpy, const QString &appId, int argc, char **argv, Qt::HANDLE visual, Qt::HANDLE cmap)
    : QApplication(dpy, argc, argv, visual, cmap)
{
    sysInit(appId);
}
#  endif
#endif

bool QtSingleApplication::isRunning()
{
    return peer->isClient();
}

bool QtSingleApplication::sendMessage(const QString &message, int timeout)
{
    return peer->sendMessage(message, timeout);
}

QString QtSingleApplication::id() const
{
    return peer->applicationId();
}

void QtSingleApplication::setActivationWindow(QWidget* aw, bool activateOnMessage)
{
    actWin = aw;
    if (activateOnMessage)
        connect(peer, SIGNAL(messageReceived(const QString&)), this, SLOT(activateWindow()));
    else
        disconnect(peer, SIGNAL(messageReceived(const QString&)), this, SLOT(activateWindow()));
}

QWidget* QtSingleApplication::activationWindow() const
{
    return actWin;
}

void QtSingleApplication::activateWindow()
{
    if (actWin) {
        actWin->setWindowState(actWin->windowState() & ~Qt::WindowMinimized);
        actWin->raise();
        actWin->activateWindow();
    }
}

