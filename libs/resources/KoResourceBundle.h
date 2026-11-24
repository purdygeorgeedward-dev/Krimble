/*
 *  SPDX-FileCopyrightText: 2014 Victor Lafon metabolic.ewilan @hotmail.fr
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KORESOURCEBUNDLE_H
#define KORESOURCEBUNDLE_H

#include <QSet>
#include <QList>
#include <QSharedPointer>
#include <QDomDocument>

#include <KoXmlWriter.h>

#include <KoResource.h>
#include "KoResourceBundleManifest.h"

#include "kritaresources_export.h"

#include <KisTag.h>

class KoStore;

class KRITARESOURCES_EXPORT KoResourceBundle
{

public:
    KoResourceBundle(QString const& fileName);

    virtual ~KoResourceBundle();

    QString defaultFileExtension() const;

    bool load();
    bool loadFromDevice(QIODevice *dev);

    bool save();

    bool saveToDevice(QIODevice* dev) const;

    void setMetaData(const QString &key, const QString &value);
    const QString metaData(const QString &key, const QString &defaultValue = QString()) const;

    void addResource(QString fileType, QString filePath, QVector<KisTagSP> fileTagList, const QString md5sum, const int resourceId = -1, const QString filenameInBundle = "");

    QList<QString> getTagsList();

    void setThumbnail(QImage);

    void saveMetadata(QScopedPointer<KoStore> &store);

    void saveManifest(QScopedPointer<KoStore> &store);

    QStringList resourceTypes() const;
    int resourceCount() const;

    KoResourceBundleManifest &manifest();

    KoResourceSP resource(const QString &resourceType, const QString &filepath);
    bool exportResource(const QString &resourceType, const QString &fileName, QIODevice *device);
    bool loadResource(KoResourceSP resource);

    QImage image() const;

    QString filename() const;

    QString resourceMd5(const QString &url);
private:

    void writeMeta(const QString &metaTag, KoXmlWriter *writer);
    void writeUserDefinedMeta(const QString &metaTag, KoXmlWriter *writer);
    bool readMetaData(KoStore *resourceStore);

private:
    QImage m_thumbnail;
    KoResourceBundleManifest m_manifest;
    QMap<QString, QString> m_metadata;
    QSet<QString> m_bundletags;
    QList<QByteArray> m_gradientsMd5Installed;
    QList<QByteArray> m_patternsMd5Installed;
    QList<QByteArray> m_brushesMd5Installed;
    QList<QByteArray> m_palettesMd5Installed;
    QList<QByteArray> m_workspacesMd5Installed;
    QList<QByteArray> m_presetsMd5Installed;
    QString m_filename;
    QString m_bundleVersion;

};

typedef QSharedPointer<KoResourceBundle> KoResourceBundleSP;

#endif
