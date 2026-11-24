/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QMutexLocker>
#include "kis_swapped_data_store.h"
#include "kis_memory_window.h"
#include "kis_image_config.h"

#include "kis_tile_compressor_2.h"

KisSwappedDataStore::KisSwappedDataStore()
    : m_totalSwapMemoryUsed(0)
{
    KisImageConfig config(true);
    const quint64 maxSwapSize = config.maxSwapSize() * MiB;
    const quint64 swapSlabSize = config.swapSlabSize() * MiB;
    const quint64 swapWindowSize = config.swapWindowSize() * MiB;

    m_allocator = new KisChunkAllocator(swapSlabSize, maxSwapSize);
    m_swapSpace = new KisMemoryWindow(config.swapDir(), swapWindowSize);

    m_compressor = new KisTileCompressor2();
}

KisSwappedDataStore::~KisSwappedDataStore()
{
    delete m_compressor;
    delete m_swapSpace;
    delete m_allocator;
}

quint64 KisSwappedDataStore::numTiles() const
{

    return m_allocator->numChunks();
}

bool KisSwappedDataStore::trySwapOutTileData(KisTileData *td)
{
    Q_ASSERT(td->data());
    QMutexLocker locker(&m_lock);

    const qint32 expectedBufferSize = m_compressor->tileDataBufferSize(td);
    if(m_buffer.size() < expectedBufferSize)
        m_buffer.resize(expectedBufferSize);

    qint32 bytesWritten;
    m_compressor->compressTileData(td, (quint8*) m_buffer.data(), m_buffer.size(), bytesWritten);

    KisChunk chunk = m_allocator->getChunk(bytesWritten);
    quint8 *ptr = m_swapSpace->getWriteChunkPtr(chunk);
    if (!ptr) {
        qWarning() << "swap out of tile failed";
        return false;
    }
    memcpy(ptr, m_buffer.data(), bytesWritten);

    td->releaseMemory();
    td->setSwapChunk(chunk);

    m_totalSwapMemoryUsed += chunk.size();

    return true;
}

void KisSwappedDataStore::swapInTileData(KisTileData *td)
{
    Q_ASSERT(!td->data());
    QMutexLocker locker(&m_lock);

    KisChunk chunk = td->swapChunk();
    m_totalSwapMemoryUsed -= chunk.size();

    td->allocateMemory();
    td->setSwapChunk(KisChunk());

    quint8 *ptr = m_swapSpace->getReadChunkPtr(chunk);
    Q_ASSERT(ptr);
    m_compressor->decompressTileData(ptr, chunk.size(), td);
    m_allocator->freeChunk(chunk);
}

void KisSwappedDataStore::forgetTileData(KisTileData *td)
{
    QMutexLocker locker(&m_lock);

    m_totalSwapMemoryUsed -= td->swapChunk().size();

    m_allocator->freeChunk(td->swapChunk());
    td->setSwapChunk(KisChunk());
}

qint64 KisSwappedDataStore::totalSwapMemoryUsed() const
{
    return m_totalSwapMemoryUsed;
}

void KisSwappedDataStore::debugStatistics()
{
    m_allocator->sanityCheck();
    m_allocator->debugFragmentation();
}
