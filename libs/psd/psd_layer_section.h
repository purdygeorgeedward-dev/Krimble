/*
 *  SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2021 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef PSD_LAYER_SECTION_H
#define PSD_LAYER_SECTION_H

#include "kritapsd_export.h"

#include <QString>

class QIODevice;

#include <kis_types.h>
#include <psd.h>

#include "psd_header.h"
#include "psd_layer_record.h"

class KRITAPSD_EXPORT PSDLayerMaskSection
{
public:
    PSDLayerMaskSection(const PSDHeader &header);
    ~PSDLayerMaskSection();

    bool read(QIODevice &io);
    bool write(QIODevice &io, KisNodeSP rootLayer, psd_compression_type compressionType);

    QString error;

    bool hasTransparency{false};

    qint16 nLayers{0};
    QVector<PSDLayerRecord *> layers;

    struct GlobalLayerMaskInfo {
        quint16 overlayColorSpace{0};
        quint16 colorComponents[4]{0, 0, 0, 0};
        quint16 opacity{0};
        quint8 kind{0};
    };
    GlobalLayerMaskInfo globalLayerMaskInfo;
    PsdAdditionalLayerInfoBlock globalInfoSection;

private:
    template<psd_byte_order byteOrder = psd_byte_order::psdBigEndian>
    bool readLayerInfoImpl(QIODevice &io);
    bool readPsdImpl(QIODevice &io);
    template<psd_byte_order byteOrder = psd_byte_order::psdBigEndian>
    bool readTiffImpl(QIODevice &io);
    template<psd_byte_order byteOrder = psd_byte_order::psdBigEndian>
    bool readGlobalMask(QIODevice &io);
    void writePsdImpl(QIODevice &io, KisNodeSP rootLayer, psd_compression_type compressionType);
    template<psd_byte_order byteOrder = psd_byte_order::psdBigEndian>
    void writeTiffImpl(QIODevice &io, KisNodeSP rootLayer, psd_compression_type compressionType);
private:
    const PSDHeader m_header;
};

#endif
