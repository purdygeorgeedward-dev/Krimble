/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_ABSTRACT_COMPRESSION_H
#define __KIS_ABSTRACT_COMPRESSION_H

#include "kritaimage_export.h"
#include <QtGlobal>

class KRITAIMAGE_EXPORT KisAbstractCompression
{
public:
    KisAbstractCompression();
    virtual ~KisAbstractCompression();

    virtual qint32 compress(const quint8* input, qint32 inputLength, quint8* output, qint32 outputLength) = 0;

    virtual qint32 decompress(const quint8* input, qint32 inputLength, quint8* output, qint32 outputLength) = 0;

    virtual qint32 outputBufferSize(qint32 dataSize) = 0;

    virtual void adjustForDataSize(qint32 dataSize);

public:

    static void linearizeColors(quint8 *input, quint8 *output,
                                qint32 dataSize, qint32 pixelSize);
    static void delinearizeColors(quint8 *input, quint8 *output,
                                  qint32 dataSize, qint32 pixelSize);
};

#endif

