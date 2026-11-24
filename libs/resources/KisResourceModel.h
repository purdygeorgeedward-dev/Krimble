/*
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISRESOURCEMODEL_H
#define KISRESOURCEMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

#include <kritaresources_export.h>

#include <KoResource.h>
#include <KisTag.h>

class KRITARESOURCES_EXPORT KisAbstractResourceModel {

public:

    enum Columns {
        Id = 0,
        StorageId,
        Name,
        Filename,
        Tooltip,
        Thumbnail,
        Status,
        Location,
        ResourceType,
        Tags,
        MD5,
        LargeThumbnail,
        Dirty,
        MetaData,
        ResourceActive,
        StorageActive,
        BrokenStatus,
        BrokenStatusMessage,
    };

    virtual ~KisAbstractResourceModel(){}

    virtual KoResourceSP resourceForIndex(QModelIndex index = QModelIndex()) const = 0;

    virtual QModelIndex indexForResource(KoResourceSP resource) const = 0;

    virtual QModelIndex indexForResourceId(int resourceId) const = 0;

    virtual bool setResourceActive(const QModelIndex &index, bool value) = 0;

    inline bool setResourceInactive(const QModelIndex &index) {
        return setResourceActive(index, false);
    }

    virtual KoResourceSP importResourceFile(const QString &filename, const bool allowOverwrite, const QString &storageId = QString("")) = 0;

    virtual KoResourceSP importResource(const QString &filename, QIODevice *device, const bool allowOverwrite, const QString &storageId = QString("")) = 0;

    virtual bool importWillOverwriteResource(const QString &fileName, const QString &storageLocation = QString()) const = 0;

    virtual bool exportResource(KoResourceSP resource, QIODevice *device) = 0;

    virtual bool addResource(KoResourceSP resource, const QString &storageId = QString("")) = 0;

    virtual bool addResourceDeduplicateFileName(KoResourceSP resource, const QString &storageId) = 0;

    virtual bool updateResource(KoResourceSP resource) = 0;

    virtual bool reloadResource(KoResourceSP resource) = 0;

    virtual bool renameResource(KoResourceSP resource, const QString &name) = 0;

    virtual bool setResourceMetaData(KoResourceSP resource, QMap<QString, QVariant> metadata) = 0;
};

class KRITARESOURCES_EXPORT KisAbstractResourceFilterInterface
{
public:
    virtual ~KisAbstractResourceFilterInterface() {}

    enum ResourceFilter {
        ShowInactiveResources = 0,
        ShowActiveResources,
        ShowAllResources
    };

    enum StorageFilter {
        ShowInactiveStorages = 0,
        ShowActiveStorages,
        ShowAllStorages
    };
public:

    virtual void setResourceFilter(ResourceFilter filter) = 0;

    virtual void setStorageFilter(StorageFilter filter) = 0;
};

class KRITARESOURCES_EXPORT KisAllResourcesModel : public QAbstractTableModel, public KisAbstractResourceModel
{
    Q_OBJECT

private:
    friend class KisResourceModelProvider;
    friend class KisResourceModel;
    friend class KisResourceQueryMapper;
    KisAllResourcesModel(const QString &resourceType, QObject *parent = 0);

public:

    ~KisAllResourcesModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    KoResourceSP resourceForIndex(QModelIndex index = QModelIndex()) const override;
    QModelIndex indexForResource(KoResourceSP resource) const override;
    QModelIndex indexForResourceId(int resourceId) const override;
    bool setResourceActive(const QModelIndex &index, bool value) override;
    KoResourceSP importResourceFile(const QString &filename, const bool allowOverwrite, const QString &storageId = QString("")) override;
    KoResourceSP importResource(const QString &filename, QIODevice *device, const bool allowOverwrite, const QString &storageId = QString("")) override;
    bool importWillOverwriteResource(const QString &fileName, const QString &storageLocation = QString()) const override;
    bool exportResource(KoResourceSP resource, QIODevice *device) override;
    bool addResource(KoResourceSP resource, const QString &storageId = QString("")) override;
    bool addResourceDeduplicateFileName(KoResourceSP resource, const QString &storageId = QString("")) override;
    bool updateResource(KoResourceSP resource) override;
    bool reloadResource(KoResourceSP resource) override;
    bool renameResource(KoResourceSP resource, const QString &name) override;
    bool setResourceMetaData(KoResourceSP resource, QMap<QString, QVariant> metadata) override;

private Q_SLOTS:

    void storageActiveStateChanged(const QString &location);
    void storageResynchronized(const QString &storage, bool isBulkResynchronization);
    void storagesBulkSynchronizationFinished();

    void beginExternalResourceImport(const QString &resourceType, int numResources);

    void endExternalResourceImport(const QString &resourceType);

    void beginExternalResourceRemove(const QString &resourceType, const QVector<int> &resourceId);

    void endExternalResourceRemove(const QString &resourceType);

    void slotResourceActiveStateChanged(const QString &resourceType, int resourceId);

public:

    KoResourceSP resourceForId(int id) const;

    bool resourceExists(const QString &md5, const QString &filename, const QString &name);

    QVector<KoResourceSP> resourcesForFilename(QString filename) const;

    QVector<KoResourceSP> resourcesForName(const QString &name) const;
    QVector<KoResourceSP> resourcesForMD5(const QString &md5sum) const;
    QVector<KisTagSP> tagsForResource(int resourceId) const;

private:

    bool prepareQuery();
    bool resetQuery();
    void closeQuery();

    struct Private;
    Private *const d;

};

class KRITARESOURCES_EXPORT KisResourceModel : public QSortFilterProxyModel, public KisAbstractResourceModel, public KisAbstractResourceFilterInterface
{
    Q_OBJECT

public:

    KisResourceModel(const QString &type, QObject *parent = 0);
    ~KisResourceModel() override;

    void setResourceFilter(ResourceFilter filter) override;
    void setStorageFilter(StorageFilter filter) override;

    void showOnlyUntaggedResources(bool showOnlyUntagged);

public:

    KoResourceSP resourceForIndex(QModelIndex index = QModelIndex()) const override;
    QModelIndex indexForResource(KoResourceSP resource) const override;
    QModelIndex indexForResourceId(int resourceId) const override;
    bool setResourceActive(const QModelIndex &index, bool value) override;
    KoResourceSP importResourceFile(const QString &filename, const bool allowOverwrite, const QString &storageId = QString("")) override;
    KoResourceSP importResource(const QString &filename, QIODevice *device, const bool allowOverwrite, const QString &storageId = QString("")) override;
    bool importWillOverwriteResource(const QString &fileName, const QString &storageLocation = QString()) const override;
    bool exportResource(KoResourceSP resource, QIODevice *device) override;
    bool addResource(KoResourceSP resource, const QString &storageId = QString("")) override;
    bool addResourceDeduplicateFileName(KoResourceSP resource, const QString &storageId = QString("")) override;
    bool updateResource(KoResourceSP resource) override;
    bool reloadResource(KoResourceSP resource) override;
    bool renameResource(KoResourceSP resource, const QString &name) override;
    bool setResourceMetaData(KoResourceSP resource, QMap<QString, QVariant> metadata) override;
    QHash<int, QByteArray> roleNames() const override;

public:

    KoResourceSP resourceForId(int id) const;

    QVector<KoResourceSP> resourcesForFilename(QString fileName) const;

    QVector<KoResourceSP> resourcesForName(QString name) const;
    QVector<KoResourceSP> resourcesForMD5(const QString md5sum) const;
    QVector<KisTagSP> tagsForResource(int resourceId) const;

protected:

    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    QVector<KoResourceSP> filterByColumn(const QString filter, KisAllResourcesModel::Columns column) const;
    bool filterResource(const QModelIndex &idx) const;

    struct Private;
    Private *const d;

    Q_DISABLE_COPY(KisResourceModel)

};

#endif
