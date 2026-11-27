/* This file is part of the KDE project
 *
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_GENERIC_REGISTRY_MODEL_H_
#define _KO_GENERIC_REGISTRY_MODEL_H_

#include <QAbstractListModel>
#include "KoGenericRegistry.h"

template<typename T>
class KoGenericRegistryModel : public QAbstractListModel
{

public:

    KoGenericRegistryModel(KoGenericRegistry<T>* registry);

    ~KoGenericRegistryModel() override;

public:

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    T get(const QModelIndex &index) const;

private:

    KoGenericRegistry<T>* m_registry;
};

template<typename T>
KoGenericRegistryModel<T>::KoGenericRegistryModel(KoGenericRegistry<T>* registry) : m_registry(registry)
{
}

template<typename T>
KoGenericRegistryModel<T>::~KoGenericRegistryModel()
{
}

template<typename T>
int KoGenericRegistryModel<T>::rowCount(const QModelIndex & ) const
{
    return m_registry->keys().size();
}

template<typename T>
QVariant KoGenericRegistryModel<T>::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return QVariant(get(index)->name());
    }
    return QVariant();
}

template<typename T>
T KoGenericRegistryModel<T>::get(const QModelIndex &index) const
{
    return m_registry->get(m_registry->keys()[index.row()]);
}

#endif
