/*
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISSTORAGEFILTERPROXYMODEL_H
#define KISSTORAGEFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QObject>
#include <KisResourceStorage.h>
#include <KisStorageModel.h>
#include "kritaresources_export.h"

class KRITARESOURCES_EXPORT KisStorageFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    KisStorageFilterProxyModel(QObject *parent = 0);
    ~KisStorageFilterProxyModel() override;

    enum FilterType {
        ByFileName = 0,
        ByStorageType,
        ByActive
    };

    KisResourceStorageSP storageForIndex(QModelIndex index = QModelIndex()) const;

    void setFilter(FilterType filterType, QVariant filter);

protected:

    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private Q_SLOTS:
    void slotModelReset();

private:
    struct Private;
    Private *const d;

    Q_DISABLE_COPY(KisStorageFilterProxyModel)
};

#endif
