/*
 *  SPDX-FileCopyrightText: 2020 Saurabh Kumar <saurabhk660@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef COMMENT_MODEL
#define COMMENT_MODEL

#include "StoryboardItem.h"

#include <QAbstractListModel>
#include <QAbstractButton>

#include <kritastoryboarddocker_export.h>

class StoryboardModel;

class KRITASTORYBOARDDOCKER_EXPORT StoryboardCommentModel : public QAbstractListModel
{

    Q_OBJECT

public:
    StoryboardCommentModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                    const QModelIndex &destinationParent, int destinationChild) override;

    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    Qt::DropActions supportedDropActions() const override;
    Qt::DropActions supportedDragActions() const override;

    void resetData(QVector<StoryboardComment> list);
    QVector<StoryboardComment> getData();

Q_SIGNALS:
    void sigCommentListChanged();
private:
    QVector<StoryboardComment> m_commentList;
    friend class StoryboardModel;
};

#endif
