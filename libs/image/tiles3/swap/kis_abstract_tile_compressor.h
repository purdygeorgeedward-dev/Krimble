/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_ABSTRACT_TILE_COMPRESSOR_H
#define __KIS_ABSTRACT_TILE_COMPRESSOR_H

#include "kritaimage_export.h"
#include "../kis_tile.h"
#include "../kis_tiled_data_manager.h"

class KisPaintDeviceWriter;

class KisAbstractTileCompressor;
typedef KisSharedPtr<KisAbstractTileCompressor> KisAbstractTileCompressorSP;

class KRITAIMAGE_EXPORT KisAbstractTileCompressor : public KisShared
{
public:
    KisAbstractTileCompressor();
    virtual ~KisAbstractTileCompressor();

public:

    virtual bool writeTile(KisTileSP tile, KisPaintDeviceWriter &store) = 0;

    virtual bool readTile(QIODevice *stream, KisTiledDataManager *dm) = 0;

    virtual void compressTileData(KisTileData *tileData,quint8 *buffer,
                                  qint32 bufferSize, qint32 &bytesWritten) = 0;

    virtual bool decompressTileData(quint8 *buffer, qint32 bufferSize,
                                    KisTileData *tileData) = 0;

    virtual qint32 tileDataBufferSize(KisTileData *tileData) = 0;

protected:
    inline qint32 xToCol(KisTiledDataManager *dm, qint32 x) {
        return dm->xToCol(x);
    }

    inline qint32 yToRow(KisTiledDataManager *dm, qint32 y) {
        return dm->yToRow(y);
    }

    inline qint32 pixelSize(KisTiledDataManager *dm) {
        return dm->pixelSize();
    }
};

#endif

