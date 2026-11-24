/*
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISFOLDERSTORAGE_H
#define KISFOLDERSTORAGE_H

#include <KisStoragePlugin.h>

#include <kritaresources_export.h>

class KRITARESOURCES_EXPORT KisFolderStorage : public KisStoragePlugin
{
public:
    KisFolderStorage(const QString &location);
    virtual ~KisFolderStorage();

    bool saveAsNewVersion(const QString &resourceType, KoResourceSP resource) override;

    KisResourceStorage::ResourceItem resourceItem(const QString &url) override;
    bool loadVersionedResource(KoResourceSP resource) override;
    QSharedPointer<KisResourceStorage::ResourceIterator> resources(const QString &resourceType) override;
    QSharedPointer<KisResourceStorage::TagIterator> tags(const QString &resourceType) override;
    bool importResource(const QString &url, QIODevice *device) override;
    bool exportResource(const QString &url, QIODevice *device) override;
    bool addResource(const QString  &resourceType, KoResourceSP resource) override;

    QStringList metaDataKeys() const override;
    QVariant metaData(const QString &key) const override;

    QString resourceMd5(const QString &url) override;
    QString resourceFilePath(const QString &url) override;
private:
    friend class FolderIterator;

};

#endif
