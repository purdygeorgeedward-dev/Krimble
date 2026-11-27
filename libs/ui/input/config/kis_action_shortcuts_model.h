/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISACTIONSHORTCUTSMODEL_H
#define KISACTIONSHORTCUTSMODEL_H

#include <QAbstractListModel>

class KisAbstractInputAction;
class KisInputProfile;

class KisActionShortcutsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit KisActionShortcutsModel(QObject *parent = 0);
    ~KisActionShortcutsModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role =
                                    Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    KisAbstractInputAction *action() const;
    KisInputProfile *profile() const;

    bool canRemoveRow(int row) const;

public Q_SLOTS:
    void setAction(KisAbstractInputAction *action);
    void setProfile(KisInputProfile *profile);

private Q_SLOTS:
    void currentProfileChanged();

private:
    class Private;
    Private *const d;
};

#endif
