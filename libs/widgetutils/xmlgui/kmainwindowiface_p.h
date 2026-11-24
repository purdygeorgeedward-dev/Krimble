/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Ian Reinhart Geiser <geiseri@yahoo.com>
   SPDX-FileCopyrightText: 2006 Thiago Macieira <thiago@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KMAINWINDOWIFACE_P_H
#define KMAINWINDOWIFACE_P_H

#include <QDBusAbstractAdaptor>
#include <QMap>

class KXmlGuiWindow;

class KisKMainWindowInterface : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.KisKMainWindow")

public:
    KisKMainWindowInterface(KXmlGuiWindow *mainWindow);
    ~KisKMainWindowInterface() override;

public Q_SLOTS:
    QStringList actions();

    bool activateAction(const QString &action);

    bool disableAction(const QString &action);

    bool enableAction(const QString &action);

    bool actionIsEnabled(const QString &action);

    QString actionToolTip(const QString &action);

    qlonglong winId();
    void grabWindowToClipBoard();
private:
    KXmlGuiWindow *m_MainWindow;
};

#endif

