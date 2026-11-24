/*
 *  SPDX-FileCopyrightText: 2021 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KORESOURCELOADRESULT_H
#define KORESOURCELOADRESULT_H

#include <QSharedPointer>
#include <QDebug>
#include <KoResourceSignature.h>
#include <KoEmbeddedResource.h>

class KoResource;
typedef QSharedPointer<KoResource> KoResourceSP;

class KRITARESOURCES_EXPORT KoResourceLoadResult
{
public:
    enum Type {
        ExistingResource,
        EmbeddedResource,
        FailedLink
    };
public:
    KoResourceLoadResult(KoResourceSP resource);
    KoResourceLoadResult(KoEmbeddedResource embeddedRresource);
    KoResourceLoadResult(KoResourceSignature signature);

    template <typename T, typename = typename std::is_convertible<T*, KoResource*>::type>
    KoResourceLoadResult(QSharedPointer<T> resource)
        : KoResourceLoadResult(KoResourceSP(resource))
    {
    }

    KoResourceLoadResult(const KoResourceLoadResult &rhs);
    KoResourceLoadResult& operator=(const KoResourceLoadResult &rhs);

    ~KoResourceLoadResult();

    KoResourceSP resource() const noexcept;

    template <typename T>
    QSharedPointer<T> resource() const {
        return this->resource().dynamicCast<T>();
    }

    KoEmbeddedResource embeddedResource() const noexcept;

    KoResourceSignature signature() const;

    Type type() const;

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

KRITARESOURCES_EXPORT QDebug operator<<(QDebug debug, const KoResourceLoadResult &result);

#endif
