/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_BOOKMARKED_CONFIGURATIONS_MODEL_H_
#define _KIS_BOOKMARKED_CONFIGURATIONS_MODEL_H_

#include <QAbstractListModel>

#include <kritaui_export.h>

class KLocalizedString;

class KisBookmarkedConfigurationManager;
#include <kis_serializable_configuration.h>

class KRITAUI_EXPORT KisBookmarkedConfigurationsModel : public QAbstractListModel
{
public:
    KisBookmarkedConfigurationsModel(KisBookmarkedConfigurationManager*);
    ~KisBookmarkedConfigurationsModel() override;
    KisBookmarkedConfigurationManager* bookmarkedConfigurationManager();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    KisSerializableConfigurationSP configuration(const QModelIndex &index) const;
    QModelIndex indexFor(const QString& name) const;
    virtual bool isIndexDeletable(const QModelIndex &index) const;
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    virtual void newConfiguration(KLocalizedString baseName, const KisSerializableConfigurationSP config);
    virtual void saveConfiguration(const QString & name, const KisSerializableConfigurationSP config);
    virtual void deleteIndex(const QModelIndex &index);
private:
    struct Private;
    Private* const d;
};

#endif
