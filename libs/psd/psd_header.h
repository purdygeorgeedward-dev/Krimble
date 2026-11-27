/*
 *  SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2021 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef PSD_HEADER_H
#define PSD_HEADER_H

#include "kritapsd_export.h"

#include <QtGlobal>
#include <kis_debug.h>
#include <psd.h>

class QIODevice;

class KRITAPSD_EXPORT PSDHeader
{
public:
    PSDHeader();

    bool read(QIODevice &device);

    bool write(QIODevice &device);

    bool valid();

    QString signature;
    quint16 version;
    quint16 nChannels;
    quint32 height;
    quint32 width;
    quint16 channelDepth;
    psd_color_mode colormode;
    psd_byte_order byteOrder;
    bool tiffStyleLayerBlock;

    QString error;
};

KRITAPSD_EXPORT QDebug operator<<(QDebug dbg, const PSDHeader &header);

#endif
