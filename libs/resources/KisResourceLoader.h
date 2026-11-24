/*
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISRESOURCELOADER_H
#define KISRESOURCELOADER_H

#include <QString>
#include <QStringList>
#include <QSharedPointer>
#include <QIODevice>

#include <KoResource.h>
#include <KoID.h>

#include <kritaresources_export.h>

class KRITARESOURCES_EXPORT KisResourceLoaderBase
{
public:

    KisResourceLoaderBase(const QString &resourceSubType, const QString &resourceType, const QString &name, const QStringList &mimetypes)
    {
        m_resourceSubType = resourceSubType;
        m_resourceType = resourceType;
        m_mimetypes = mimetypes;
        m_name = name;
    }

    virtual ~KisResourceLoaderBase()
    {
    }

    QStringList filters() const;

    QStringList mimetypes() const
    {
        return m_mimetypes;
    }

    QString resourceType() const
    {
        return m_resourceType;
    }

    QString resourceSubType() const
    {
        return id();
    }

    QString id() const
    {
        return m_resourceSubType;
    }

    QString name() const
    {
        return m_name;
    }

    virtual KoResourceSP create(const QString &name) = 0;

    bool load(KoResourceSP resource, QIODevice &dev, KisResourcesInterfaceSP resourcesInterface)
    {
        Q_ASSERT(dev.isOpen() && dev.isReadable());
        return resource->loadFromDevice(&dev, resourcesInterface);
    }

    KoResourceSP load(const QString &name, QIODevice &dev, KisResourcesInterfaceSP resourcesInterface)
    {
        KoResourceSP resource = create(name);
        return load(resource, dev, resourcesInterface) ? resource : 0;
    }

private:
    QString m_resourceSubType;
    QString m_resourceType;
    QStringList m_mimetypes;
    QString m_name;

};

template<typename T>
class KisResourceLoader : public KisResourceLoaderBase {
public:
    KisResourceLoader(const QString &id, const QString &folder, const QString &name, const QStringList &mimetypes)
        : KisResourceLoaderBase(id, folder, name, mimetypes)
    {
    }

    virtual KoResourceSP create(const QString &name) override
    {
        QSharedPointer<T> resource = QSharedPointer<T>::create(name);
        return resource;
    }
};

#endif
