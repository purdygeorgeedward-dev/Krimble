/****************************************************************************
**
** Copyright (C) 2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Concurrent project on Trolltech Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "modeltest.h"

Q_DECLARE_METATYPE(QModelIndex)

ModelTest::ModelTest(QAbstractItemModel *_model, QObject *parent) : QObject(parent), model(_model), fetchingMore(false)
{
    Q_ASSERT(model);

    connect(model, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(runAllTests()));
    connect(model, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(runAllTests()));
    connect(model, SIGNAL(columnsInserted(QModelIndex,int,int)),
            this, SLOT(runAllTests()));
    connect(model, SIGNAL(columnsRemoved(QModelIndex,int,int)),
            this, SLOT(runAllTests()));
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(runAllTests()));
    connect(model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
            this, SLOT(runAllTests()));
    connect(model, SIGNAL(layoutAboutToBeChanged()), this, SLOT(runAllTests()));
    connect(model, SIGNAL(layoutChanged()), this, SLOT(runAllTests()));
    connect(model, SIGNAL(modelReset()), this, SLOT(runAllTests()));
    connect(model, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(runAllTests()));
    connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(runAllTests()));
    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(runAllTests()));
    connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(runAllTests()));

    connect(model, SIGNAL(layoutAboutToBeChanged()),
            this, SLOT(layoutAboutToBeChanged()));
    connect(model, SIGNAL(layoutChanged()),
            this, SLOT(layoutChanged()));

    connect(model, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(rowsAboutToBeInserted(QModelIndex,int,int)));
    connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(rowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(rowsInserted(QModelIndex,int,int)));
    connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(rowsRemoved(QModelIndex,int,int)));

    runAllTests();
}

void ModelTest::runAllTests()
{
    if (fetchingMore)
        return;
    nonDestructiveBasicTest();
    rowCount();
    columnCount();
    hasIndex();
    index();
    parent();
    data();
}

void ModelTest::nonDestructiveBasicTest()
{
    Q_ASSERT(model->buddy(QModelIndex()) == QModelIndex());
    model->canFetchMore(QModelIndex());
    Q_ASSERT(model->columnCount(QModelIndex()) >= 0);
    Q_ASSERT(model->data(QModelIndex()) == QVariant());
    fetchingMore = true;
    model->fetchMore(QModelIndex());
    fetchingMore = false;
    Qt::ItemFlags flags = model->flags(QModelIndex());
    Q_ASSERT(flags == Qt::ItemIsDropEnabled || flags == 0);
    Q_UNUSED(flags);
    model->hasChildren(QModelIndex());
    model->hasIndex(0, 0);
    model->headerData(0, Qt::Horizontal);
    model->index(0, 0);
    model->itemData(QModelIndex());
    QVariant cache;
    model->match(QModelIndex(), -1, cache);
    model->mimeTypes();
    Q_ASSERT(model->parent(QModelIndex()) == QModelIndex());
    Q_ASSERT(model->rowCount() >= 0);
    QVariant variant;
    model->setData(QModelIndex(), variant, -1);
    model->setHeaderData(-1, Qt::Horizontal, QVariant());
    model->setHeaderData(0, Qt::Horizontal, QVariant());
    model->setHeaderData(999999, Qt::Horizontal, QVariant());
    QMap<int, QVariant> roles;
    model->sibling(0, 0, QModelIndex());
    model->span(QModelIndex());
    model->supportedDropActions();
}

void ModelTest::rowCount()
{
    QModelIndex topIndex = model->index(0, 0, QModelIndex());
    int rows = model->rowCount(topIndex);
    Q_ASSERT(rows >= 0);
    if (rows > 0)
        Q_ASSERT(model->hasChildren(topIndex) == true);

    QModelIndex secondLevelIndex = model->index(0, 0, topIndex);
    if (secondLevelIndex.isValid()) {
        rows = model->rowCount(secondLevelIndex);
        Q_ASSERT(rows >= 0);
        if (rows > 0)
            Q_ASSERT(model->hasChildren(secondLevelIndex) == true);
    }

}

void ModelTest::columnCount()
{
    QModelIndex topIndex = model->index(0, 0, QModelIndex());
    Q_ASSERT(model->columnCount(topIndex) >= 0);

    QModelIndex childIndex = model->index(0, 0, topIndex);
    if (childIndex.isValid())
        Q_ASSERT(model->columnCount(childIndex) >= 0);

}

void ModelTest::hasIndex()
{
    Q_ASSERT(model->hasIndex(-2, -2) == false);
    Q_ASSERT(model->hasIndex(-2, 0) == false);
    Q_ASSERT(model->hasIndex(0, -2) == false);

    int rows = model->rowCount();
    int columns = model->columnCount();
    Q_UNUSED(columns);
    Q_ASSERT(model->hasIndex(rows, columns) == false);
    Q_ASSERT(model->hasIndex(rows + 1, columns + 1) == false);

    if (rows > 0)
        Q_ASSERT(model->hasIndex(0, 0) == true);

}

void ModelTest::index()
{
    Q_ASSERT(model->index(-2, -2) == QModelIndex());
    Q_ASSERT(model->index(-2, 0) == QModelIndex());
    Q_ASSERT(model->index(0, -2) == QModelIndex());

    int rows = model->rowCount();
    int columns = model->columnCount();
    Q_UNUSED(columns);
    if (rows == 0)
        return;

    Q_ASSERT(model->index(rows, columns) == QModelIndex());
    Q_ASSERT(model->index(0, 0).isValid() == true);

    QModelIndex a = model->index(0, 0);
    QModelIndex b = model->index(0, 0);
    Q_ASSERT(a == b);

}

void ModelTest::parent()
{
    Q_ASSERT(model->parent(QModelIndex()) == QModelIndex());

    if (model->rowCount() == 0)
        return;

    QModelIndex topIndex = model->index(0, 0, QModelIndex());
    Q_ASSERT(model->parent(topIndex) == QModelIndex());

    if (model->rowCount(topIndex) > 0) {
        QModelIndex childIndex = model->index(0, 0, topIndex);
        Q_ASSERT(model->parent(childIndex) == topIndex);
    }

    QModelIndex topIndex1 = model->index(0, 1, QModelIndex());
    if (model->rowCount(topIndex1) > 0) {
        QModelIndex childIndex = model->index(0, 0, topIndex);
        QModelIndex childIndex1 = model->index(0, 0, topIndex1);
        Q_ASSERT(childIndex != childIndex1);
    }

    checkChildren(QModelIndex());
}

void ModelTest::checkChildren(const QModelIndex &parent, int currentDepth)
{
    QModelIndex p = parent;
    while (p.isValid())
        p = p.parent();

    if (model->canFetchMore(parent)) {
        fetchingMore = true;
        model->fetchMore(parent);
        fetchingMore = false;
    }

    int rows = model->rowCount(parent);
    int columns = model->columnCount(parent);

    if (rows > 0)
        Q_ASSERT(model->hasChildren(parent));

    Q_ASSERT(rows >= 0);
    Q_ASSERT(columns >= 0);
    if (rows > 0)
        Q_ASSERT(model->hasChildren(parent) == true);

    Q_ASSERT(model->hasIndex(rows + 1, 0, parent) == false);
    for (int r = 0; r < rows; ++r) {
        if (model->canFetchMore(parent)) {
            fetchingMore = true;
            model->fetchMore(parent);
            fetchingMore = false;
        }
        Q_ASSERT(model->hasIndex(r, columns + 1, parent) == false);
        for (int c = 0; c < columns; ++c) {
            Q_ASSERT(model->hasIndex(r, c, parent) == true);
            QModelIndex index = model->index(r, c, parent);
            Q_ASSERT(index.isValid() == true);

            QModelIndex modifiedIndex = model->index(r, c, parent);
            Q_ASSERT(index == modifiedIndex);

            QModelIndex a = model->index(r, c, parent);
            QModelIndex b = model->index(r, c, parent);
            Q_ASSERT(a == b);

            Q_ASSERT(index.model() == model);
            Q_ASSERT(index.row() == r);
            Q_ASSERT(index.column() == c);
            Q_ASSERT(model->data(index, Qt::DisplayRole).isValid() == true);

            Q_ASSERT(model->parent(index) == parent);

            if (model->hasChildren(index) && currentDepth < 10) {
                checkChildren(index, ++currentDepth);
            }

            QModelIndex newerIndex = model->index(r, c, parent);
            Q_ASSERT(index == newerIndex);
        }
    }
}

void ModelTest::data()
{
    Q_ASSERT(!model->data(QModelIndex()).isValid());

    if (model->rowCount() == 0)
        return;

    Q_ASSERT(model->index(0, 0).isValid());

    Q_ASSERT(model->setData(QModelIndex(), QLatin1String("foo"), Qt::DisplayRole) == false);

    QVariant variant = model->data(model->index(0, 0), Qt::ToolTipRole);
    if (variant.isValid()) {
        Q_ASSERT(variant.canConvert<QString>());
    }
    variant = model->data(model->index(0, 0), Qt::StatusTipRole);
    if (variant.isValid()) {
        Q_ASSERT(variant.canConvert<QString>());
    }
    variant = model->data(model->index(0, 0), Qt::WhatsThisRole);
    if (variant.isValid()) {
        Q_ASSERT(variant.canConvert<QString>());
    }

    variant = model->data(model->index(0, 0), Qt::SizeHintRole);
    if (variant.isValid()) {
        Q_ASSERT(variant.canConvert<QSize>());
    }

    QVariant fontVariant = model->data(model->index(0, 0), Qt::FontRole);
    if (fontVariant.isValid()) {
        Q_ASSERT(fontVariant.canConvert<QFont>());
    }

    QVariant textAlignmentVariant = model->data(model->index(0, 0), Qt::TextAlignmentRole);
    if (textAlignmentVariant.isValid()) {
        int alignment = textAlignmentVariant.toInt();
        Q_UNUSED(alignment);
        Q_ASSERT(alignment == Qt::AlignLeft ||
                 alignment == Qt::AlignRight ||
                 alignment == Qt::AlignHCenter ||
                 alignment == Qt::AlignJustify ||
                 alignment == Qt::AlignTop ||
                 alignment == Qt::AlignBottom ||
                 alignment == Qt::AlignVCenter ||
                 alignment == Qt::AlignCenter ||
                 alignment == Qt::AlignAbsolute ||
                 alignment == Qt::AlignLeading ||
                 alignment == Qt::AlignTrailing);
    }

    QVariant colorVariant = model->data(model->index(0, 0), Qt::BackgroundRole);
    if (colorVariant.isValid()) {
        Q_ASSERT(colorVariant.canConvert<QColor>());
    }

    colorVariant = model->data(model->index(0, 0), Qt::ForegroundRole);
    if (colorVariant.isValid()) {
        Q_ASSERT(colorVariant.canConvert<QColor>());
    }

    QVariant checkStateVariant = model->data(model->index(0, 0), Qt::CheckStateRole);
    if (checkStateVariant.isValid()) {
        int state = checkStateVariant.toInt();
        Q_UNUSED(state);
        Q_ASSERT(state == Qt::Unchecked ||
                 state == Qt::PartiallyChecked ||
                 state == Qt::Checked);
    }
}

void ModelTest::rowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(end);
    Changing c;
    c.parent = parent;
    c.oldSize = model->rowCount(parent);
    c.last = model->data(model->index(start - 1, 0, parent));
    c.next = model->data(model->index(start, 0, parent));
    insert.push(c);
}

void ModelTest::rowsInserted(const QModelIndex & parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);

    Changing c = insert.pop();
    Q_ASSERT(c.parent == parent);
    Q_ASSERT(c.oldSize + (end - start + 1) == model->rowCount(parent));
    Q_ASSERT(c.last == model->data(model->index(start - 1, 0, c.parent)));
    Q_ASSERT(c.next == model->data(model->index(end + 1, 0, c.parent)));
}

void ModelTest::layoutAboutToBeChanged()
{
    for (int i = 0; i < qBound(0, model->rowCount(), 100); ++i)
        changing.append(QPersistentModelIndex(model->index(i, 0)));
}

void ModelTest::layoutChanged()
{
    for (int i = 0; i < changing.count(); ++i) {
        QPersistentModelIndex p = changing[i];
        Q_ASSERT(p == model->index(p.row(), p.column(), p.parent()));
    }
    changing.clear();
}

void ModelTest::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Changing c;
    c.parent = parent;
    c.oldSize = model->rowCount(parent);
    c.last = model->data(model->index(start - 1, 0, parent));
    c.next = model->data(model->index(end + 1, 0, parent));
    remove.push(c);
}

void ModelTest::rowsRemoved(const QModelIndex & parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);

    Changing c = remove.pop();
    Q_ASSERT(c.parent == parent);
    Q_ASSERT(c.oldSize - (end - start + 1) == model->rowCount(parent));
    Q_ASSERT(c.last == model->data(model->index(start - 1, 0, c.parent)));
    Q_ASSERT(c.next == model->data(model->index(start, 0, c.parent)));
}

