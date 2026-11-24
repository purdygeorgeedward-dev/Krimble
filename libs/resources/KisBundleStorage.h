/*
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISBUNDLESTORAGE_H
#define KISBUNDLESTORAGE_H

#include <KisStoragePlugin.h>
#include "kritaresources_export.h"

class KRITARESOURCES_EXPORT KisBundleStorage : public KisStoragePlugin
{
public:
    KisBundleStorage(const QString &location);
    virtual ~KisBundleStorage();

    KisResourceStorage::ResourceItem resourceItem(const QString &url) override;

    bool loadVersionedResource(KoResourceSP resource) override;
    QString resourceMd5(const QString &url) override;
    QSharedPointer<KisResourceStorage::ResourceIterator> resources(const QString &resourceType) override;
    QSharedPointer<KisResourceStorage::TagIterator> tags(const QString &resourceType) override;
    QImage thumbnail() const override;
    QStringList metaDataKeys() const override;
    QVariant metaData(const QString &key) const override;

    bool saveAsNewVersion(const QString &resourceType, KoResourceSP resource) override;

    bool exportResource(const QString &url, QIODevice *device) override;

private:
    friend class BundleIterator;

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif
