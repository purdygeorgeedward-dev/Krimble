/*
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KISRESOURCEITERATOR_H
#define KISRESOURCEITERATOR_H

#include <QImage>
#include <QString>
#include <QScopedPointer>
#include <QModelIndex>

#include <KoResource.h>

class KisResourceModel;

#include <kritaresources_export.h>

class KRITARESOURCES_EXPORT KisResourceItem {
private:
    friend class KisResourceIterator;
    KisResourceItem(KisResourceModel *resourceModel, const QModelIndex &index);
public:
    int id();
    QString resourceType();
    QString name();
    QString filename();
    QString tooltip();
    QString md5sum();
    QImage thumbnail();
    KoResourceSP resource();
private:
    KisResourceModel *m_resourceModel;
    QModelIndex m_index;
};

typedef QSharedPointer<KisResourceItem> KisResourceItemSP;

class KRITARESOURCES_EXPORT KisResourceIterator
{
public:
    KisResourceIterator(KisResourceModel *resourceModel);
    ~KisResourceIterator();

    bool hasNext() const;
    bool hasPrevious() const;
    const KisResourceItemSP next();
    const KisResourceItemSP peekNext() const;
    const KisResourceItemSP peekPrevious() const;
    const KisResourceItemSP previous();
    void toFront();
    void toBack();

private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
