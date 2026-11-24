/*
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISRESOURCECACHEDB_H
#define KISRESOURCECACHEDB_H

#include <QObject>

#include <kritaresources_export.h>

#include <KisResourceStorage.h>

class KRITARESOURCES_EXPORT KisResourceCacheDb
{
public:

    static const QString resourceCacheDbFilename;
    static const QString databaseVersion;
    static QStringList storageTypes;
    static QStringList disabledBundles;

    static bool isValid();

    static QString lastError();

    static bool initialize(const QString &location);

    static void deleteTemporaryResources();

    static void performHouseKeepingOnExit();

    static void setForeignKeysStateImpl(bool isEnabled);

    static bool getForeignKeysStateImpl();

    static void synchronizeForeignKeysState();

private:

    friend class KisResourceLocator;
    friend class TestResourceLocator;
    friend class TestResourceCacheDb;
    friend class KisAllTagsModel;
    friend class KisResourceLoaderRegistry;
    friend class KisResourceUserOperations;
    friend class KisDocument;
    friend class KisAllResourcesModel;

    explicit KisResourceCacheDb();
    ~KisResourceCacheDb();
    KisResourceCacheDb operator=(const KisResourceCacheDb&);
    static bool registerResourceType(const QString &resourceType);

    static std::pair<QVector<int>,QVector<int>> tagsForStorage(const QString &resourceType, const QString &storageLocation);
    static QVector<int> resourcesForStorage(const QString &resourceType, const QString &storageLocation);
    static int resourceIdForResource(const QString &resourceFileName, const QString &resourceType, const QString &storageLocation);
    static bool resourceNeedsUpdating(int resourceId, QDateTime timestamp);

    static bool addResourceVersion(int resourceId, QDateTime timestamp, KisResourceStorageSP storage, KoResourceSP resource);

    static bool addResourceVersionImpl(int resourceId, QDateTime timestamp, KisResourceStorageSP storage, KoResourceSP resource);
    static bool removeResourceVersionImpl(int resourceId, int version, KisResourceStorageSP storage);

    static bool updateResourceTableForResourceIfNeeded(int resourceId, const QString &resourceType, KisResourceStorageSP storage);
    static bool makeResourceTheCurrentVersion(int resourceId, KoResourceSP resource);
    static bool removeResourceCompletely(int resourceId);

    static bool getResourceIdFromFilename(QString filename, QString resourceType, QString storageLocation, int &outResourceId);
    static bool getResourceIdFromVersionedFilename(QString filename, QString resourceType, QString storageLocation, int& outResourceId);
    static bool getAllVersionsLocations(int resourceId, QStringList &outVersionsLocationsList);

    static bool addResource(KisResourceStorageSP storage, QDateTime timestamp, KoResourceSP resource, const QString &resourceType);
    static bool addResources(KisResourceStorageSP storage, QString resourceType);

    static bool setResourceActive(int resourceId, bool active = false);

    static bool tagResource(const QString &resourceFileName, KisTagSP tag, const QString &resourceType);
    static bool hasTag(const QString &url, const QString &resourceType);
    static bool linkTagToStorage(const QString &url, const QString &resourceType, const QString &storageLocation);
    static bool addTag(const QString &resourceType, const QString storageLocation, KisTagSP tag);
    static bool addTags(KisResourceStorageSP storage, QString resourceType);

    static bool registerStorageType(const KisResourceStorage::StorageType storageType);
    static bool addStorage(KisResourceStorageSP storage, bool preinstalled);
    static bool addStorageTags(KisResourceStorageSP storage);

    static bool deleteStorage(KisResourceStorageSP storage);
    static bool deleteStorage(QString location);
    static bool synchronizeStorage(KisResourceStorageSP storage);

    static QMap<QString, QVariant> metaDataForId(int id, const QString &tableName);

    static bool updateMetaDataForId(const QMap<QString, QVariant> map, int id, const QString &tableName);
    static bool addMetaDataForId(const QMap<QString, QVariant> map, int id, const QString &tableName);

    static bool removeOrphanedMetaData();

    static bool s_valid;
    static QString s_lastError;
};

#endif
