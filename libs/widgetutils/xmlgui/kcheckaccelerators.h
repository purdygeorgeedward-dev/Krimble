/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1997 Matthias Kalle Dalheimer (kalle@kde.org)
    SPDX-FileCopyrightText: 1998, 1999, 2000 KDE Team
    SPDX-FileCopyrightText: 2008 Nick Shaforostoff <shaforostoff@kde.ru>

    SPDX-License-Identifier: LGPL-2.0-or-later
        */

#ifndef KCHECKACCELERATORS_H_
#define KCHECKACCELERATORS_H_

#include <QObject>
#include <QPointer>

#include <QTimer>

class QDialog;
class QTextBrowser;

class KisKCheckAccelerators: public QObject
{
    Q_OBJECT
public:
    KisKCheckAccelerators(QObject *parent, int key, bool autoCheck, bool copyWidgetText);
    bool eventFilter(QObject *, QEvent *e) override;

private:
    void checkAccelerators(bool automatic);
    int key {0};
    bool block {false};
    bool alwaysShow {false};
    bool autoCheck {false};

    bool copyWidgetText {false};
    QString copyWidgetTextCommand;

    QTimer autoCheckTimer;
    void createDialog(QWidget *parent, bool automatic);
    QPointer<QDialog> drklash;
    QTextBrowser *drklash_view {nullptr};

private Q_SLOTS:
    void autoCheckSlot();
    void slotDisableCheck(bool);
};

#endif
