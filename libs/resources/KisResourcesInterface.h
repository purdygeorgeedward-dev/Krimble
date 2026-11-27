/*
 *  SPDX-FileCopyrightText: 2020 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISRESOURCESINTERFACE_H
#define KISRESOURCESINTERFACE_H

#include "kritaresources_export.h"

#include <QScopedPointer>
#include <KoResource.h>
#include <KoResourceLoadResult.h>

class QString;
class QByteArray;
class KisResourcesInterfacePrivate;

class KRITARESOURCES_EXPORT KisResourcesInterface
{
public:
    class KRITARESOURCES_EXPORT ResourceSourceAdapter
    {
    public:
        ResourceSourceAdapter(const QString &type);
        virtual ~ResourceSourceAdapter();
        friend class KisResourcesInterface;
        virtual QVector<KoResourceSP> resourcesForFilename(const QString& filename) const = 0;
        virtual QVector<KoResourceSP> resourcesForName(const QString& name) const = 0;
        virtual QVector<KoResourceSP> resourcesForMD5(const QString& md5) const = 0;
public:

        KoResourceSP bestMatch(const QString md5, const QString filename, const QString name);

        KoResourceSP exactMatch(const QString md5, const QString filename, const QString name);

        KoResourceLoadResult bestMatchLoadResult(const QString md5, const QString filename, const QString name);

        virtual KoResourceSP fallbackResource() const = 0;

    private:
        KoResourceSP findResource(const QString md5, const QString filename, const QString name, bool exactMatch);
    private:
        Q_DISABLE_COPY(ResourceSourceAdapter);
        const QString m_type;
    };

    template <typename T>
    class TypedResourceSourceAdapter
    {
    public:
        TypedResourceSourceAdapter(ResourceSourceAdapter *adapter)
            : m_source(adapter)
        {
        }
private:
        QVector<QSharedPointer<T>> resourcesForFilename(const QString& filename) const
        {
            QVector<QSharedPointer<T>> r;
            Q_FOREACH(KoResourceSP resource, m_source->resourcesForFilename(filename)) {
                r << resource.dynamicCast<T>();
            }
            return r;
        }

        QVector<QSharedPointer<T>> resourcesForName(const QString& name) const
        {
            QVector<QSharedPointer<T>> r;
            Q_FOREACH(KoResourceSP resource, m_source->resourcesForName(name)) {
                r << resource.dynamicCast<T>();
            }
            return r;
        }

        QVector<QSharedPointer<T>> resourcesForMD5(const QString& md5) const
        {
            QVector<QSharedPointer<T>> r;
            Q_FOREACH(KoResourceSP resource, m_source->resourcesForMD5(md5)) {
                r << resource.dynamicCast<T>();
            }
            return r;
        }
public:

        QSharedPointer<T>  bestMatch(const QString md5, const QString filename, const QString name) {
            return m_source->bestMatch(md5, filename, name).dynamicCast<T>();
        }

        QSharedPointer<T>  exactMatch(const QString md5, const QString filename, const QString name) {
            return m_source->exactMatch(md5, filename, name).dynamicCast<T>();
        }

        KoResourceLoadResult bestMatchLoadResult(const QString md5, const QString filename, const QString name) {
            return m_source->bestMatchLoadResult(md5, filename, name);
        }

        QSharedPointer<T> fallbackResource() const
        {
            return m_source->fallbackResource().dynamicCast<T>();
        }

    protected:
        ResourceSourceAdapter *m_source;
    };

public:
    KisResourcesInterface();
    virtual ~KisResourcesInterface();

    ResourceSourceAdapter& source(const QString &type) const;

    template<typename T>
    TypedResourceSourceAdapter<T> source(const QString &type) const {
        return TypedResourceSourceAdapter<T>(&this->source(type));
    }

protected:
    KisResourcesInterface(KisResourcesInterfacePrivate *dd);
    virtual ResourceSourceAdapter* createSourceImpl(const QString &type) const = 0;

protected:
    QScopedPointer<KisResourcesInterfacePrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(KisResourcesInterface)
};

using KisResourcesInterfaceSP = QSharedPointer<KisResourcesInterface>;

#endif
