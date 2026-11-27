/*
 * SPDX-FileCopyrightText: 2019 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2023 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISSTORAGEMODEL_H
#define KISSTORAGEMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QScopedPointer>

#include "KisResourceStorage.h"
#include "kritaresources_export.h"

class QSqlQuery;

class KRITARESOURCES_EXPORT KisStorageModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    enum Columns {
        Id = 0,
        StorageType,
        Location,
        TimeStamp,
        PreInstalled,
        Active,
        Thumbnail,
        DisplayName,
        MetaData
    };

    enum StorageImportOption {
        None,
        Overwrite,
        Rename,
    };

    KisStorageModel(QObject *parent = 0);
    ~KisStorageModel() override;

    static KisStorageModel * instance();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    KisResourceStorageSP storageForIndex(const QModelIndex &index) const;
    KisResourceStorageSP storageForId(const int storageId) const;

    bool importStorage(const QString &filename, StorageImportOption importOption) const;
    bool importStorageData(const QString &filename, StorageImportOption importOption, const QByteArray &data) const;
    bool canImportStorage(const QString &filename) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

Q_SIGNALS:

    void storageEnabled(const QString &storage);
    void storageDisabled(const QString &storage);

    void storageResynchronized(const QString &storage, bool isBulkResynchronization);

    void storagesBulkSynchronizationFinished();

private Q_SLOTS:

    void addStorage(const QString &location);

    void removeStorage(const QString &location);

    void slotStoragesBulkSynchronizationFinished();

private :
    static bool importStorageInternal(const QString &filename,
                                      StorageImportOption importOption,
                                      bool dryRun,
                                      const QByteArray &data);

    void resetQuery();

    static QImage getThumbnailFromQuery(const QSqlQuery &query);

    struct Private;
    QScopedPointer<Private> d;
};

#endif
