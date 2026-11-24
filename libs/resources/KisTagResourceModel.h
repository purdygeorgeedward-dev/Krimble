/*
 * SPDX-FileCopyrightText: 2020 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KISTAGRESOURCEMODEL_H
#define KISTAGRESOURCEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

#include <KisTag.h>
#include <KoResource.h>
#include <KisResourceModel.h>

#include "kritaresources_export.h"

class KRITARESOURCES_EXPORT KisAbstractTagResourceModel
{
public:
    virtual ~KisAbstractTagResourceModel() {}

    virtual bool tagResources(const KisTagSP tag, const QVector<int> &resourceIds) = 0;
    virtual bool untagResources(const KisTagSP tag, const QVector<int> &resourceIds) = 0;

    virtual int isResourceTagged(const KisTagSP tag, const int resourceId) = 0;
};

class KRITARESOURCES_EXPORT KisAllTagResourceModel
        : public QAbstractTableModel
        , public KisAbstractTagResourceModel
{
    Q_OBJECT
private:

    friend class KisResourceModelProvider;
    friend class TestTagResourceModel;
    friend class KisTagResourceModel;

    KisAllTagResourceModel(const QString &resourceType, QObject *parent = 0);

public:
    ~KisAllTagResourceModel() override;

public:

    enum Columns {
        TagId = KisAbstractResourceModel::BrokenStatusMessage + 1,
        ResourceId,
        Tag,
        Resource,
        ResourceActive,
        TagActive,
        ResourceStorageActive,
        ResourceName,
        TagName
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    bool tagResources(const KisTagSP tag, const QVector<int> &resourceIds) override;
    bool untagResources(const KisTagSP tag, const QVector<int> &resourceId) override;
    int isResourceTagged(const KisTagSP tag, const int resourceId) override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private Q_SLOTS:
    void addStorage(const QString &location);
    void removeStorage(const QString &location);

    void slotResourceActiveStateChanged(const QString &resourceType, int resourceId);

private:

    QString createQuery(bool onlyAchieve = true, bool returnADbIndexToo = false);
    bool resetQuery();
    void closeQuery();

    struct Private;
    Private* const d;
};

class KRITARESOURCES_EXPORT KisTagResourceModel : public QSortFilterProxyModel
    , public KisAbstractTagResourceModel
    , public KisAbstractResourceModel
    , public KisAbstractResourceFilterInterface
{
    Q_OBJECT

public:

    KisTagResourceModel(const QString &resourceType, QObject *parent = 0);
    ~KisTagResourceModel() override;

public:

    enum TagFilter {
        ShowInactiveTags = 0,
        ShowActiveTags,
        ShowAllTags
    };

    void setTagFilter(TagFilter filter);

    void setResourceFilter(ResourceFilter filter) override;
    void setStorageFilter(StorageFilter filter) override;

    void setTagsFilter(const QVector<int> tagIds);
    void setResourcesFilter(const QVector<int> resourceIds);

    void setTagsFilter(const QVector<KisTagSP> tags);
    void setResourcesFilter(const QVector<KoResourceSP> resources);

    bool tagResources(const KisTagSP tag, const QVector<int> &resourceIds) override;
    bool untagResources(const KisTagSP tag, const QVector<int> &resourceIds) override;
    int isResourceTagged(const KisTagSP tag, const int resourceId) override;

    KoResourceSP resourceForIndex(QModelIndex index) const override;
    QModelIndex indexForResource(KoResourceSP resource) const override;
    QModelIndex indexForResourceId(int resourceId) const override;
    bool setResourceActive(const QModelIndex &index, bool value) override;
    KoResourceSP importResourceFile(const QString &filename, const bool allowOverwrite, const QString &storageId = QString()) override;
    KoResourceSP importResource(const QString &filename, QIODevice *device, const bool allowOverwrite, const QString &storageId = QString()) override;
    bool importWillOverwriteResource(const QString &fileName, const QString &storageLocation) const override;
    bool exportResource(KoResourceSP resource, QIODevice *device) override;
    bool addResource(KoResourceSP resource, const QString &storageId) override;
    bool addResourceDeduplicateFileName(KoResourceSP resource, const QString &storageId) override;
    bool updateResource(KoResourceSP resource) override;
    bool reloadResource(KoResourceSP resource) override;
    bool renameResource(KoResourceSP resource, const QString &name) override;
    bool setResourceMetaData(KoResourceSP resource, QMap<QString, QVariant> metadata) override;
    QHash<int, QByteArray> roleNames() const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

protected Q_SLOTS:
    void storageChanged(const QString &location);

private:
    struct Private;
    Private* const d;
};

#endif
