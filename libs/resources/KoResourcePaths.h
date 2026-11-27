/*
 * SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KORESOURCEPATHS_H
#define KORESOURCEPATHS_H

#include <QScopedPointer>
#include <QString>
#include <QStringList>

#include <kritaresources_export.h>

class KRITARESOURCES_EXPORT KoResourcePaths
{
public:

    KoResourcePaths();
    virtual ~KoResourcePaths();

    enum SearchOption { NoSearchOptions = 0,
                        Recursive = 1,
                        IgnoreExecBit = 4
                      };
    Q_DECLARE_FLAGS(SearchOptions, SearchOption)

    static QString getApplicationRoot();

    static QString s_overrideAppDataLocation;
    static QString getAppDataLocation();

    static void getAllUserResourceFoldersLocationsForWindowsStore(QString& standardLocation, QString& privateLocation);

    static void addAssetType(const QString &type, const char *basetype,
                                const QString &relativeName, bool priority = true);

    static void addAssetDir(const QString &type, const QString &dir, bool priority = true);

    static QString findAsset(const QString &type, const QString &fileName);

    static QStringList findDirs(const QString &type);

    static QStringList findAllAssets(const QString &type,
                                        const QString &filter = QString(),
                                        SearchOptions options = NoSearchOptions);

    static QStringList assetDirs(const QString &type);

    static QString saveLocation(const QString &type, const QString &suffix = QString(), bool create = true);

    static QString locate(const QString &type, const QString &filename);

    static QString locateLocal(const QString &type, const QString &filename, bool createDir = false);

private:

    void addResourceTypeInternal(const QString &type, const QString &basetype,
                                 const QString &relativeName, bool priority);

    void addResourceDirInternal(const QString &type, const QString &absdir, bool priority);

    QString findResourceInternal(const QString &type, const QString &fileName);

    QStringList findDirsInternal(const QString &type);

    QStringList findAllResourcesInternal(const QString &type,
                                         const QString &filter = QString(),
                                         SearchOptions options = NoSearchOptions) const;

    QStringList resourceDirsInternal(const QString &type);

    QString saveLocationInternal(const QString &type, const QString &suffix = QString(), bool create = true);

    QString locateInternal(const QString &type, const QString &filename);

    QString locateLocalInternal(const QString &type, const QString &filename, bool createDir = false);

    QStringList findExtraResourceDirs() const;

    class Private;
    QScopedPointer<Private> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KoResourcePaths::SearchOptions)

#endif
