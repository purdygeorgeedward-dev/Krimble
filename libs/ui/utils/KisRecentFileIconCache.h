/*
 *  SPDX-FileCopyrightText: 2022 Alvin Wong <alvin@alvinhc.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_RECENT_FILE_ICON_CACHE_H
#define KIS_RECENT_FILE_ICON_CACHE_H

#include <QFuture>
#include <QIcon>
#include <QMap>
#include <QThreadPool>
#include <QUrl>

class KisRecentFileIconCache : public QObject
{
    Q_OBJECT

    struct CacheItem;

    QMap<QUrl, CacheItem> m_iconCacheMap;
    QThreadPool m_iconFetchThreadPool;
    float m_devicePixelRatioF {1.0};

public:
    KisRecentFileIconCache();
    ~KisRecentFileIconCache();

public:
    static KisRecentFileIconCache *instance();

    QIcon getOrQueueFileIcon(const QUrl &url);

    void invalidateFileIcon(const QUrl &url);

    void reloadFileIcon(const QUrl &url);

private Q_SLOTS:
    void cleanupOnQuit();
    void iconFetched();
    void futureCanceled();

Q_SIGNALS:
    void fileIconChanged(const QUrl &url, const QIcon &icon);
};

#endif
