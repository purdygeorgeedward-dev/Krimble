#
#ifndef KOMD5GENERATOR_H
#define KOMD5GENERATOR_H

#include <QByteArray>
#include <QString>

#include <kritaresources_export.h>

class QIODevice;

class KRITARESOURCES_EXPORT KoMD5Generator
{
public:
    static QString generateHash(const QString &filename);

    static QString generateHash(const QByteArray &array);

    static QString generateHash(QIODevice *device);
};

#endif
