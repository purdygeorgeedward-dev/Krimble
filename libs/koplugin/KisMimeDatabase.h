    /*
 * SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KISMIMEDATABASE_H
#define KISMIMEDATABASE_H

#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QList>

#include "kritaplugin_export.h"

class KRITAPLUGIN_EXPORT KisMimeDatabase
{
public:

    static QString mimeTypeForFile(const QString &file, bool checkExistingFiles = true);
    static QString mimeTypeForSuffix(const QString &suffix);
    static QString mimeTypeForData(const QByteArray ba);
    static QString descriptionForMimeType(const QString &mimeType);
    static QStringList suffixesForMimeType(const QString &mimeType);
    static QString iconNameForMimeType(const QString &mimeType);

private:

    struct KisMimeType {
        QByteArray mimeType;
        QStringList suffixes;
        QString description;
    };

    static QList<KisMimeType> s_mimeDatabase;
    static void fillMimeData();

};

#endif
