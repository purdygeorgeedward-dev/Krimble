/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2021 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KIS_META_DATA_IO_BACKEND_H_
#define _KIS_META_DATA_IO_BACKEND_H_

#include <kritametadata_export.h>

class QIODevice;
class QString;

namespace KisMetaData
{
class Store;
class KRITAMETADATA_EXPORT IOBackend
{
public:
    enum BackendType { Binary, Text };

    enum HeaderType {
        NoHeader,
        JpegHeader
    };

public:
    virtual ~IOBackend(){};

    virtual QString id() const = 0;

    virtual QString name() const = 0;

    virtual BackendType type() const = 0;

    virtual bool supportSaving() const = 0;

    virtual bool saveTo(const Store *store, QIODevice *ioDevice, HeaderType headerType = NoHeader) const = 0;

    virtual bool canSaveAllEntries(Store *store) const = 0;

    virtual bool supportLoading() const = 0;

    virtual bool loadFrom(Store *store, QIODevice *ioDevice) const = 0;
};
}

#endif
