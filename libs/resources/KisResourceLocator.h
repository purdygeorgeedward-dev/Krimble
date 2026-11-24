/*
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISRESOURCELOCATOR_H
#define KISRESOURCELOCATOR_H

#include <QObject>
#include <QScopedPointer>
#include <QStringList>
#include <QString>

#include "kritaresources_export.h"

#include <KisResourceStorage.h>

class KRITARESOURCES_EXPORT KisResourceLocator : public QObject
{
    Q_OBJECT
public:

    static const QString resourceLocationKey;

    static KisResourceLocator *instance();

    ~KisResourceLocator();

    enum class LocatorError {
        Ok,
        LocationReadOnly,
        CannotCreateLocation,
        CannotInitializeDb,
        CannotSynchronizeDb
    };

    LocatorError initialize(const QString &installationResourcesLocation);

    QStringList errorMessages() const;

    QString resourceLocationBase() const;

    void purge(const QString &storageLocation, const QVector<int> &removedTagIds);

    bool addStorage(const QString &storageLocation, KisResourceStorageSP storage);

    bool removeStorage(const QString &storageLocation);

    bool hasStorage(const QString &storageLocation);

    static void saveTags();

    void purgeTag(const QString tagUrl, const QString resourceType);

    QString filePathForResource(KoResourceSP resource);

    void updateFontStorage();

Q_SIGNALS:

    void progressMessage(const QString&);

    void storageAdded(const QString &location);

    void storageRemoved(const QString &location);

    void beginExternalResourceImport(const QString &resourceType, int numResources);

    void endExternalResourceImport(const QString &resourceType);

    void beginExternalResourceRemove(const QString &resourceType, const QVector<int> resourceIds);

    void endExternalResourceRemove(const QString &resourceType);

    void resourceActiveStateChanged(const QString &resourceType, int resourceId);

    void storageResynchronized(const QString &storage, bool isBulkResynchronization);

    void storagesBulkSynchronizationFinished();

private:

    friend class KisAllTagsModel;
    friend class KisTagResourceModel;
    friend class KisAllResourcesModel;
    friend class KisAllTagResourceModel;
    friend class KisStorageModel;
    friend class TestResourceLocator;
    friend class TestResourceModel;
    friend class Resource;
    friend class KisResourceCacheDb;
    friend class KisStorageFilterProxyModel;
    friend class KisResourceQueryMapper;
    friend class KisResourceUserOperations;
    friend class KisBrushTypeMetaDataFixup;
    friend class KisResourceThumbnailCache;

    bool resourceCached(QString storageLocation, const QString &resourceType, const QString &filename) const;

    KoResourceSP resource(QString storageLocation, const QString &resourceType, const QString &filename);

    KoResourceSP resourceForId(int resourceId);

    bool setResourceActive(int resourceId, bool active);

    KoResourceSP importResourceFromFile(const QString &resourceType, const QString &fileName, const bool allowOverwrite, const QString &storageLocation = QString());

    KoResourceSP importResource(const QString &resourceType, const QString &fileName, QIODevice *device, const bool allowOverwrite, const QString &storageLocation = QString());

    KoResourceSP importResourceDeduplicateFileName(const QString &resourceType, const QString &proposedFileName, QIODevice *device, const QString &storageLocation = QString());

    bool addResourceDeduplicateFileName(const QString &resourceType, const KoResourceSP resource, const QString &storageLocation);

    bool importWillOverwriteResource(const QString &resourceType, const QString &fileName, const QString &storageLocation = QString()) const;

    bool exportResource(KoResourceSP resource, QIODevice *device);

    bool addResource(const QString &resourceType, const KoResourceSP resource, const QString &storageLocation = QString());

    bool updateResource(const QString &resourceType, const KoResourceSP resource);

    bool reloadResource(const QString &resourceType, const KoResourceSP resource);

    QMap<QString, QVariant> metaDataForResource(int id) const;

    bool setMetaDataForResource(int id, QMap<QString, QVariant> map) const;

    QMap<QString, QVariant> metaDataForStorage(const QString &storageLocation) const;

    void setMetaDataForStorage(const QString &storageLocation, QMap<QString, QVariant> map) const;

    void loadRequiredResources(KoResourceSP resource);

    KisTagSP tagForUrl(const QString &tagUrl, const QString resourceType);

    static KisTagSP tagForUrlNoCache(const QString &tagUrl, const QString resourceType);

    KisResourceLocator(QObject *parent);
    KisResourceLocator(const KisResourceLocator&);
    KisResourceLocator operator=(const KisResourceLocator&);

    enum class InitializationStatus {
        Unknown,
        Initialized,
        FirstRun,
        FirstUpdate,
        Updating
    };

    LocatorError firstTimeInstallation(InitializationStatus initializationStatus, const QString &installationResourcesLocation);

    bool synchronizeDb();

    void findStorages();
    QList<KisResourceStorageSP> storages() const;

    KisResourceStorageSP storageByLocation(const QString &location) const;
    KisResourceStorageSP folderStorage() const;
    KisResourceStorageSP memoryStorage() const;
    KisResourceStorageSP fontStorage() const;

    struct ResourceStorage {
        QString storageLocation;
        QString resourceType;
        QString resourceFileName;
     };

    friend class KisMyPaintPaintOpPreset;

    ResourceStorage getResourceStorage(int resourceId) const;
    QString makeStorageLocationAbsolute(QString storageLocation) const;
    QString makeStorageLocationRelative(QString location) const;

    class Private;
    QScopedPointer<Private> d;
};

#endif
