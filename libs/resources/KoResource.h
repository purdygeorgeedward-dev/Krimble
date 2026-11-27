/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2003 Patrick Julien <freak@codepimps.org>
    SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KORESOURCE_H
#define KORESOURCE_H

#include <QImage>
#include <QString>
#include <QSharedPointer>
#include <QDebug>

#include "KoResourceSignature.h"

#include "KisResourceTypes.h"
#include <boost/operators.hpp>

#include <kritaresources_export.h>

class QDomDocument;
class QDomElement;

class KoResource;
typedef QSharedPointer<KoResource> KoResourceSP;

class KisResourcesInterface;
typedef QSharedPointer<KisResourcesInterface> KisResourcesInterfaceSP;

class KoResourceLoadResult;

namespace ResourceTestHelper {
void overrideResourceVersion(KoResourceSP resource, int version);
}

class KRITARESOURCES_EXPORT KoResource : public boost::equality_comparable<KoResource>
{
public:

    KoResource();
    explicit KoResource(const QString &filename);
    virtual ~KoResource();
    KoResource(const KoResource &rhs);
    KoResource &operator=(const KoResource &rhs) = delete;

    virtual KoResourceSP clone() const = 0;

public:
    bool load(KisResourcesInterfaceSP resourcesInterface);
    virtual bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) = 0;

    bool save();
    virtual bool saveToDevice(QIODevice* dev) const;

    QImage image() const;
    void setImage(const QImage &image);

    virtual void updateThumbnail();

    virtual QImage thumbnail() const;

    virtual QString thumbnailPath() const;

    QString md5Sum(bool generateIfEmpty = true) const;

    void setMD5Sum(const QString &md5sum);

    QString filename() const;
    void setFilename(const QString& filename);

    virtual QString name() const;
    void setName(const QString& name);

    bool valid() const;
    void setValid(bool valid);

    bool active() const;
    void setActive(bool active);

    virtual QString defaultFileExtension() const;

    bool permanent() const;
    void setPermanent(bool permanent);

    QString storageLocation() const;
    void setStorageLocation(const QString &location);

    void setDirty(bool value);

    bool isDirty() const;

    void addMetaData(QString key, QVariant value);

    QMap<QString, QVariant> metadata() const;

    int version() const;
    void setVersion(int version);

    int resourceId() const;
    void setResourceId(int id);

    virtual QPair<QString, QString> resourceType() const = 0;

    KoResourceSignature signature() const;

    virtual bool isEphemeral() const;

    virtual bool isSerializable() const;

    QList<KoResourceLoadResult> requiredResources(KisResourcesInterfaceSP globalResourcesInterface) const;

    virtual QList<KoResourceLoadResult> linkedResources(KisResourcesInterfaceSP globalResourcesInterface) const;

    virtual QList<KoResourceLoadResult> embeddedResources(KisResourcesInterfaceSP globalResourcesInterface) const;

    QList<KoResourceLoadResult> takeSideLoadedResources(KisResourcesInterfaceSP globalResourcesInterface);

    virtual QList<KoResourceLoadResult> sideLoadedResources(KisResourcesInterfaceSP globalResourcesInterface) const;

    virtual void clearSideLoadedResources();

    virtual QList<int> requiredCanvasResources() const;

private:
    struct Private;
    Private* const d;
};

static inline bool operator==(const KoResource &resource1, const KoResource &resource2)
{
    return (resource1.md5Sum() == resource2.md5Sum());
}

static inline uint qHash(const KoResource &resource)
{
    return qHash(resource.md5Sum());
}

Q_DECLARE_METATYPE(QSharedPointer<KoResource>)

inline QDebug operator<<(QDebug dbg, const KoResourceSP res)
{
    if (!res) {
        dbg.noquote() << "NULL Resource";
    }
    else {
        dbg.nospace() << "[RESOURCE] Name: " << res->name()
                      << " Version: " << res->version()
                      << " Filename: " << res->filename()
                      << " MD5: " << res->md5Sum(false)
                      << " Type: " << res->resourceType()
                      << " Valid: " << res->valid()
                      << " Storage: " << res->storageLocation();
    }
    return dbg.space();
}

#endif

