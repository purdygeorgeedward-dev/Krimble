/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __koStore_h_
#define __koStore_h_

#include <QByteArray>
#include <QIODevice>
#include "kritastore_export.h"

class QWidget;
class QUrl;
class KoStorePrivate;

class KRITASTORE_EXPORT KoStore
{
public:

    enum Mode { Read, Write };
    enum Backend { Auto, Zip, Directory };

    static KoStore *createStore(const QString &fileName, Mode mode,
                                const QByteArray &appIdentification = QByteArray(),
                                Backend backend = Auto, bool writeMimetype = true);

    static KoStore *createStore(QIODevice *device, Mode mode,
                                const QByteArray &appIdentification = QByteArray(),
                                Backend backend = Auto, bool writeMimetype = true);

    virtual ~KoStore();

    bool open(const QString &name);

    bool isOpen() const;

    bool close();

    QIODevice *device() const;

    QByteArray read(qint64 max);

    qint64 write(const QByteArray &data);

    qint64 read(char *buffer, qint64 length);

    virtual qint64 write(const char* data, qint64 length);

    qint64 size() const;

    bool bad() const;

    Mode mode() const;

    virtual QStringList directoryList() const;

    virtual bool enterDirectory(const QString &directory);

    bool leaveDirectory();

    QString currentPath() const;

    void pushDirectory();

    void popDirectory();

    bool hasFile(const QString &fileName) const;

    bool hasDirectory(const QString &directoryName);

    bool extractFile(const QString &sourceName, QByteArray &data);

    bool seek(qint64 pos);
    qint64 pos() const;
    bool atEnd() const;

    bool finalize();

    virtual void setCompressionEnabled(bool e);

    void setSubstitution(const QString &name, const QString &substitution);

protected:
    KoStore(Mode mode, bool writeMimetype = true);

    virtual bool doFinalize() {
        return true;
    }

    virtual bool openWrite(const QString &name) = 0;
    virtual bool openRead(const QString &name) = 0;

    virtual bool closeRead() = 0;
    virtual bool closeWrite() = 0;

    virtual bool enterRelativeDirectory(const QString &dirName) = 0;

    virtual bool enterAbsoluteDirectory(const QString &path) = 0;

    virtual bool fileExists(const QString &absPath) const = 0;

protected:
    KoStorePrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(KoStore)

private:
    KoStore(const KoStore& store);
    KoStore& operator=(const KoStore& store);
};

#endif
