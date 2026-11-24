/*
 *  SPDX-FileCopyrightText: 2024 Deif Lou <ginoba@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QApplication>
#include <QWidget>
#include <QDebug>

#include "KisGrabKeyboardFocusRecoveryWorkaround.h"

class KisGrabKeyboardFocusRecoveryWorkaround::Private
{
public:

    QScopedPointer<QWidget> dummyFocusRecoveryWidget {nullptr};

    Private()
    {
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
        dummyFocusRecoveryWidget.reset(new QWidget);
        dummyFocusRecoveryWidget->setObjectName("dummyFocusRecoveryWidget");
#endif
    }
};

KisGrabKeyboardFocusRecoveryWorkaround::KisGrabKeyboardFocusRecoveryWorkaround()
    : m_d(new Private)
{}

KisGrabKeyboardFocusRecoveryWorkaround* KisGrabKeyboardFocusRecoveryWorkaround::instance()
{
    static KisGrabKeyboardFocusRecoveryWorkaround *instance_ {nullptr};
    if (!instance_) {
        instance_ = new KisGrabKeyboardFocusRecoveryWorkaround;
    }
    return instance_;
}

void KisGrabKeyboardFocusRecoveryWorkaround::recoverFocus()
{
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    QWidget *activeWindow = qApp->activeWindow();
    QWidget *mainWindow = activeWindow;
    while (mainWindow->parentWidget()) {
        mainWindow = mainWindow->parentWidget();
    }
    m_d->dummyFocusRecoveryWidget->setParent(mainWindow, Qt::Dialog);
    for (int i = 0; i < 2; ++i) {
        m_d->dummyFocusRecoveryWidget->show();
        m_d->dummyFocusRecoveryWidget->close();
        activeWindow->activateWindow();
    }
#endif
}
