/* This file is part of the KDE project
 *  SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KISBASEACCESSOR_H_
#define _KISBASEACCESSOR_H_

#include <kritaimage_export.h>
#include <kis_shared.h>

class KRITAIMAGE_EXPORT KisBaseConstAccessor : public KisShared
{
    Q_DISABLE_COPY(KisBaseConstAccessor)
public:
    KisBaseConstAccessor() {}
    virtual ~KisBaseConstAccessor();
    virtual const quint8 * oldRawData() const = 0;

    virtual const quint8 * rawDataConst() const = 0;

    virtual qint32 x() const = 0;
    virtual qint32 y() const = 0;
};

class KRITAIMAGE_EXPORT KisBaseAccessor
{
    Q_DISABLE_COPY(KisBaseAccessor)
public:
    KisBaseAccessor() {}
    virtual ~KisBaseAccessor();
    virtual quint8 * rawData() = 0;
};

#endif
