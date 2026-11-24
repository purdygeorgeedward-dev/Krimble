/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 2002 Simon Hausmann <hausmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KTOOLBARHANDLER_H
#define KTOOLBARHANDLER_H

#include <QLinkedList>
#include <QObject>

#include <kxmlguiclient.h>

class KXmlGuiWindow;

namespace KDEPrivate
{

class ToolBarHandler : public QObject, public KisKXMLGUIClient
{
    Q_OBJECT

public:
    explicit ToolBarHandler(KXmlGuiWindow *mainWindow);

    ToolBarHandler(KXmlGuiWindow *mainWindow, QObject *parent);

    ~ToolBarHandler() override;

    QAction *toolBarMenuAction();

public Q_SLOTS:
    void setupActions();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void clientAdded(KisKXMLGUIClient *))
};

}

#endif
