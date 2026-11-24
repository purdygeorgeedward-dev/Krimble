/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISINPUTPROFILEMODEL_H
#define KISINPUTPROFILEMODEL_H

#include <QStringListModel>

class KisInputProfileModel : public QStringListModel
{
    Q_OBJECT
public:
    KisInputProfileModel(QObject *parent = 0);
    ~KisInputProfileModel() override;

    bool setData(const QModelIndex &index, const QVariant &value, int = Qt::EditRole) override;

    QString profileName(const QModelIndex &index);
    QModelIndex find(const QString &name);

private Q_SLOTS:
    void profileNamesChanged();
};

#endif
