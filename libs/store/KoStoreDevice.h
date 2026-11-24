/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef koStoreDevice_h
#define koStoreDevice_h

#include "KoStore.h"
#include <kritastore_export.h>

class KRITASTORE_EXPORT KoStoreDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit KoStoreDevice(KoStore * store) : m_store(store) {
        setOpenMode(m_store->mode() == KoStore::Read ? QIODevice::ReadOnly : QIODevice::WriteOnly);
    }
    ~KoStoreDevice() override;

    bool isSequential() const override {
        return true;
    }

    bool open(OpenMode m) override {
        setOpenMode(m);
        if (m & QIODevice::ReadOnly)
            return (m_store->mode() == KoStore::Read);
        if (m & QIODevice::WriteOnly)
            return (m_store->mode() == KoStore::Write);
        return false;
    }
    void close() override {}

    qint64 size() const override {
        if (m_store->mode() == KoStore::Read)
            return m_store->size();
        else
            return 0xffffffff;
    }

    qint64 pos() const override {
        return m_store->pos();
    }
    bool seek(qint64 pos) override {
        return m_store->seek(pos);
    }
    bool atEnd() const override {
        return m_store->atEnd();
    }

protected:
    KoStore *m_store;

    qint64 readData(char *data, qint64 maxlen) override {
        return m_store->read(data, maxlen);
    }

    qint64 writeData(const char *data, qint64 len) override {
        return m_store->write(data, len);
    }

};

#endif
