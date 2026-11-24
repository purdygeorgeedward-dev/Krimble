/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2007 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOJSONTRADER_H
#define KOJSONTRADER_H

#include <QList>
#include <QString>
#include <QMutex>
#include <QSharedPointer>
#include <kis_pointer_utils.h>
#include "kritaplugin_export.h"

class QPluginLoader;

class KRITAPLUGIN_EXPORT KoJsonTrader
{
public:

    static KoJsonTrader *instance();

    struct KRITAPLUGIN_EXPORT Plugin {
        Plugin() = default;
        Plugin(QSharedPointer<QPluginLoader> loader, QMutex *mutex);
        ~Plugin();

        QObject *instance() const;

        QJsonObject metaData() const;
        QString fileName() const;
        QString errorString() const;

    private:
        QSharedPointer<QPluginLoader> m_loader;
        QMutex *m_mutex = 0;
    };

     QList<Plugin> query(const QString &servicetype, const QString &mimetype);

     KoJsonTrader();
     ~KoJsonTrader();

private:
     void initializePluginLoaderCache();

private:
     QString m_pluginPath;
     mutable QMutex m_mutex;

     struct PluginCacheEntry;

     QList<PluginCacheEntry> m_pluginLoaderCache;
};

#endif
