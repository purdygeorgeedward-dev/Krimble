/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Nicolas GOUTTE <goutte@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __koStore_p_h_
#define __koStore_p_h_

#include "KoStore.h"

#include <QStringList>
#include <QStack>

#include <QUrl>

class QWidget;

class KoStorePrivate
{
public:
    explicit KoStorePrivate(KoStore *qq, KoStore::Mode _mode, bool _writeMimetype)
        : q(qq),
        window(0),
        mode(_mode),
        size(0),
        stream(0),
        isOpen(false),
        good(false),
        finalized(false),
        writeMimetype(_writeMimetype)
    {
    }

    QString toExternalNaming(const QString &internalNaming) const;

    bool enterDirectoryInternal(const QString &directory);

    bool extractFile(const QString &sourceName, QIODevice &buffer);

    KoStore *q;

    QString localFileName;
    QWidget *window;

    KoStore::Mode mode;

    QStringList filesList;

    QStringList currentPath;

    QString fileName;
    qint64 size;

    QIODevice *stream;

    bool isOpen;
    bool good;
    bool finalized;

    QStack<QString> directoryStack;

    bool writeMimetype;

    QString substituteThis;
    QString substituteWith;

    QUrl url;
};

#endif
