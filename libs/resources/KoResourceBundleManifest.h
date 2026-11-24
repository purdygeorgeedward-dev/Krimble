/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2014 Victor Lafon <metabolic.ewilan@hotmail.fr>

   SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KORESOURCEBUNDLEMANIFEST_H
#define KORESOURCEBUNDLEMANIFEST_H

#include <QString>
#include <QMap>
#include <QMultiMap>
#include <QDomElement>

#include <kritaresources_export.h>

class QIODevice;

class KRITARESOURCES_EXPORT  KoResourceBundleManifest
{
public:

    struct ResourceReference {

        ResourceReference() {}

        ResourceReference(const QString &_resourcePath, const QList<QString> &_tagList, const QString &_fileTypeName,
                          const QString &_md5, const int _resourceId = -1, const QString _filenameInBundle = "") {
            resourcePath = _resourcePath;
            tagList = _tagList;
            fileTypeName = _fileTypeName;
            md5sum = _md5;
            resourceId = _resourceId;
            filenameInBundle = _filenameInBundle.isEmpty() ? resourcePath : _filenameInBundle;
        }

        QString resourcePath;
        QList<QString> tagList;
        QString fileTypeName;
        QString md5sum;
        int resourceId;
        QString filenameInBundle;
    };

    KoResourceBundleManifest();

    virtual ~KoResourceBundleManifest();

    bool load(QIODevice *device);

    bool save(QIODevice *device);

    void addResource(const QString &fileType, const QString &fileName, const QStringList &tagFileList, const QString &md5, const int resourceId = -1, const QString filenameInBundle = "");
    void removeResource(ResourceReference &resource);

    QStringList types() const;

    QStringList tags() const;

    QList<ResourceReference> files(const QString &type = QString()) const;

    void removeFile(QString fileName);

private:
    QMap<QString, QMap<QString, ResourceReference>> m_resources;
    bool parseFileEntry(const QDomElement &e);
};

#endif
