/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_SWAPPED_DATA_STORE_H
#define __KIS_SWAPPED_DATA_STORE_H

#include "kritaimage_export.h"

#include <QMutex>
#include <QByteArray>

class QMutex;
class KisTileData;
class KisAbstractTileCompressor;
class KisChunkAllocator;
class KisMemoryWindow;

class KRITAIMAGE_EXPORT KisSwappedDataStore
{
public:
    KisSwappedDataStore();
    ~KisSwappedDataStore();

    quint64 numTiles() const;

    bool trySwapOutTileData(KisTileData *td);

    void swapInTileData(KisTileData *td);

    void forgetTileData(KisTileData *td);

    qint64 totalSwapMemoryUsed() const;

    void debugStatistics();

private:
    QByteArray m_buffer;
    KisAbstractTileCompressor *m_compressor;

    KisChunkAllocator *m_allocator;
    KisMemoryWindow *m_swapSpace;

    QMutex m_lock;

    qint64 m_totalSwapMemoryUsed;
};

#endif

