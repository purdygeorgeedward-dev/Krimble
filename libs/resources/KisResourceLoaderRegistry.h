/*
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISRESOURCELOADERREGISTRY_H
#define KISRESOURCELOADERREGISTRY_H

#include <QObject>
#include <QStringList>

#include <KoGenericRegistry.h>
#include "KisResourceLoader.h"

#include <kritaresources_export.h>

class KRITARESOURCES_EXPORT KisResourceLoaderRegistry : public QObject, public KoGenericRegistry<KisResourceLoaderBase*>
{
    Q_OBJECT
public:
    ~KisResourceLoaderRegistry() override;

    static KisResourceLoaderRegistry *instance();

    void registerLoader(KisResourceLoaderBase* loader);

    KisResourceLoaderBase *loader(const QString &resourceType, const QString &mimetype) const;

    QStringList filters(const QString &resourceType) const;

    QStringList mimeTypes(const QString &resourceType) const;

    QStringList resourceTypes() const;

    QVector<KisResourceLoaderBase*> resourceTypeLoaders(const QString &resourceType) const;

    struct ResourceCacheFixup {
        virtual ~ResourceCacheFixup() {};
        virtual QStringList executeFix() = 0;
    };

    void registerFixup(int priority, ResourceCacheFixup *fixup);
    QStringList executeAllFixups();

private:

    KisResourceLoaderRegistry(QObject *parent);
    KisResourceLoaderRegistry(const KisResourceLoaderRegistry&);
    KisResourceLoaderRegistry operator=(const KisResourceLoaderRegistry&);
private:

    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
