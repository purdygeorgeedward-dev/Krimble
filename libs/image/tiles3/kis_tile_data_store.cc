/*
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2018 Andrey Kamakin <a.kamakin@icloud.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "config-memory-leak-tracker.h"

#include <QGlobalStatic>

#include "kis_tile_data_store.h"
#include "kis_tile_data.h"
#include "kis_debug.h"

#include "kis_tile_data_store_iterators.h"

Q_GLOBAL_STATIC(KisTileDataStore, s_instance)

#ifdef DEBUG_PRECLONE
#include <stdio.h>
#define DEBUG_PRECLONE_ACTION(action, oldTD, newTD) \
    printf("!!! %s:\t\t\t  0x%X -> 0x%X    \t\t!!!\n",  \
           action, (quintptr)oldTD, (quintptr) newTD)
#define DEBUG_FREE_ACTION(td)                   \
    printf("Tile data free'd \t(0x%X)\n", td)
#else
#define DEBUG_PRECLONE_ACTION(action, oldTD, newTD)
#define DEBUG_FREE_ACTION(td)
#endif

#ifdef DEBUG_HIT_MISS
qint64 __preclone_miss = 0;
qint64 __preclone_hit = 0;

qint64 __preclone_miss_user_count = 0;
qint64 __preclone_miss_age = 0;

#define DEBUG_COUNT_PRECLONE_HIT(td) __preclone_hit++
#define DEBUG_COUNT_PRECLONE_MISS(td) __preclone_miss++; __preclone_miss_user_count+=td->numUsers(); __preclone_miss_age+=td->age()
#define DEBUG_REPORT_PRECLONE_EFFICIENCY()                      \
    dbgKrita << "Hits:" << __preclone_hit                       \
             << "of" << __preclone_hit + __preclone_miss        \
             << "("                                             \
             << qreal(__preclone_hit) / (__preclone_hit + __preclone_miss)       \
             << ")"                                             \
             << "miss users" << qreal(__preclone_miss_user_count) / __preclone_miss \
             << "miss age" << qreal(__preclone_miss_age) / __preclone_miss
#else
#define DEBUG_COUNT_PRECLONE_HIT(td)
#define DEBUG_COUNT_PRECLONE_MISS(td)
#define DEBUG_REPORT_PRECLONE_EFFICIENCY()
#endif

KisTileDataStore::KisTileDataStore()
    : m_pooler(this),
      m_swapper(this),
      m_numTiles(0),
      m_memoryMetric(0),
      m_counter(1),
      m_clockIndex(1)
{
    m_pooler.start();
    m_swapper.start();
}

KisTileDataStore::~KisTileDataStore()
{
    m_pooler.terminatePooler();
    m_swapper.terminateSwapper();

    if (numTiles() > 0) {
        errKrita << "Warning: some tiles have leaked:";
        errKrita << "\tTiles in memory:" << numTilesInMemory() << "\n"
                 << "\tTotal tiles:" << numTiles();
    }
}

KisTileDataStore* KisTileDataStore::instance()
{
    return s_instance;
}

KisTileDataStore::MemoryStatistics KisTileDataStore::memoryStatistics()
{
    QReadLocker lock(&m_iteratorLock);

    MemoryStatistics stats;

    const qint64 metricCoeff = qint64(KisTileData::WIDTH) * KisTileData::HEIGHT;

    stats.realMemorySize = m_pooler.lastRealMemoryMetric() * metricCoeff;
    stats.historicalMemorySize = m_pooler.lastHistoricalMemoryMetric() * metricCoeff;
    stats.poolSize = m_pooler.lastPoolMemoryMetric() * metricCoeff;

    stats.totalMemorySize = memoryMetric() * metricCoeff + stats.poolSize;

    stats.swapSize = m_swappedStore.totalSwapMemoryUsed();

    return stats;
}

void KisTileDataStore::tryForceUpdateMemoryStatisticsWhileIdle()
{
    if (!m_pooler.isRunning()) {
        m_pooler.forceUpdateMemoryStats();
    }
}

inline void KisTileDataStore::registerTileDataImp(KisTileData *td)
{
    int index = m_counter.fetchAndAddOrdered(1);
    td->m_tileNumber = index;

    m_tileDataMap.getGC().lockRawPointerAccess();
    m_tileDataMap.assign(index, td);
    m_tileDataMap.getGC().unlockRawPointerAccess();
    m_tileDataMap.getGC().update();

    m_numTiles.ref();
    m_memoryMetric += td->pixelSize();
}

void KisTileDataStore::registerTileData(KisTileData *td)
{
    QReadLocker lock(&m_iteratorLock);
    registerTileDataImp(td);
}

inline void KisTileDataStore::unregisterTileDataImp(KisTileData *td)
{
    m_tileDataMap.getGC().lockRawPointerAccess();

    if (m_clockIndex == td->m_tileNumber) {
        do {
            m_clockIndex.ref();
        } while (!m_tileDataMap.get(m_clockIndex.loadAcquire()) && m_clockIndex < m_counter);
    }

    int index = td->m_tileNumber;
    td->m_tileNumber = -1;
    m_tileDataMap.erase(index);
    m_numTiles.deref();
    m_memoryMetric -= td->pixelSize();

    m_tileDataMap.getGC().unlockRawPointerAccess();
    m_tileDataMap.getGC().update();
}

void KisTileDataStore::unregisterTileData(KisTileData *td)
{
    QReadLocker lock(&m_iteratorLock);
    unregisterTileDataImp(td);
}

KisTileData *KisTileDataStore::allocTileData(qint32 pixelSize, const quint8 *defPixel)
{
    KisTileData *td = new KisTileData(pixelSize, defPixel, this);
    registerTileData(td);
    return td;
}

KisTileData *KisTileDataStore::duplicateTileData(KisTileData *rhs)
{
    KisTileData *td = 0;

    if (rhs->m_clonesStack.pop(td)) {
        DEBUG_PRECLONE_ACTION("+ Pre-clone HIT", rhs, td);
        DEBUG_COUNT_PRECLONE_HIT(rhs);
    } else {
        rhs->blockSwapping();
        td = new KisTileData(*rhs);
        rhs->unblockSwapping();
        DEBUG_PRECLONE_ACTION("- Pre-clone #MISS#", rhs, td);
        DEBUG_COUNT_PRECLONE_MISS(rhs);
    }

    registerTileData(td);
    return td;
}

void KisTileDataStore::freeTileData(KisTileData *td)
{
    Q_ASSERT(td->m_store == this);

    DEBUG_FREE_ACTION(td);

    m_iteratorLock.lockForRead();
    td->m_swapLock.lockForWrite();

    if (!td->data()) {
        m_swappedStore.forgetTileData(td);
    } else {
        unregisterTileDataImp(td);
    }

    td->m_swapLock.unlock();
    m_iteratorLock.unlock();

    delete td;
}

void KisTileDataStore::ensureTileDataLoaded(KisTileData *td)
{
    checkFreeMemory();

    td->m_swapLock.lockForRead();

    while (!td->data()) {
        td->m_swapLock.unlock();

        m_iteratorLock.lockForWrite();

        if (!td->data()) {
            td->m_swapLock.lockForWrite();

            m_swappedStore.swapInTileData(td);
            registerTileDataImp(td);

            td->m_swapLock.unlock();
        }

        m_iteratorLock.unlock();

        td->m_swapLock.lockForRead();
    }
}

bool KisTileDataStore::trySwapTileData(KisTileData *td)
{

    bool result = false;
    if (!td->m_swapLock.tryLockForWrite()) return result;

    if (td->data()) {
        if (m_swappedStore.trySwapOutTileData(td)) {
            unregisterTileDataImp(td);
            result = true;
        }
    }
    td->m_swapLock.unlock();

    return result;
}

KisTileDataStoreIterator* KisTileDataStore::beginIteration()
{
    m_iteratorLock.lockForWrite();
    return new KisTileDataStoreIterator(m_tileDataMap, this);
}
void KisTileDataStore::endIteration(KisTileDataStoreIterator* iterator)
{
    delete iterator;
    m_iteratorLock.unlock();
}

KisTileDataStoreReverseIterator* KisTileDataStore::beginReverseIteration()
{
    m_iteratorLock.lockForWrite();
    return new KisTileDataStoreReverseIterator(m_tileDataMap, this);
}
void KisTileDataStore::endIteration(KisTileDataStoreReverseIterator* iterator)
{
    delete iterator;
    m_iteratorLock.unlock();
    DEBUG_REPORT_PRECLONE_EFFICIENCY();
}

KisTileDataStoreClockIterator* KisTileDataStore::beginClockIteration()
{
    m_iteratorLock.lockForWrite();
    return new KisTileDataStoreClockIterator(m_tileDataMap, m_clockIndex.loadAcquire(), this);
}

void KisTileDataStore::endIteration(KisTileDataStoreClockIterator* iterator)
{
    m_clockIndex = iterator->getFinalPosition();
    delete iterator;
    m_iteratorLock.unlock();
}

void KisTileDataStore::debugPrintList()
{
    KisTileDataStoreIterator* iter = beginIteration();
    KisTileData *item = 0;

    while (iter->hasNext()) {
        item = iter->next();
        dbgTiles << "-------------------------\n"
                 << "TileData:\t\t\t" << item
                 << "\n  refCount:\t" << item->m_refCount;
    }

    endIteration(iter);
}

void KisTileDataStore::debugSwapAll()
{
    KisTileDataStoreIterator* iter = beginIteration();
    KisTileData *item = 0;

    while (iter->hasNext()) {
        item = iter->next();
        iter->trySwapOut(item);
    }

    endIteration(iter);

}

void KisTileDataStore::debugClear()
{
    QWriteLocker l(&m_iteratorLock);
    ConcurrentMap<int, KisTileData*>::Iterator iter(m_tileDataMap);

    while (iter.isValid()) {
        delete iter.getValue();
        iter.next();
    }

    m_counter = 1;
    m_clockIndex = 1;
    m_numTiles = 0;
    m_memoryMetric = 0;
}

void KisTileDataStore::testingRereadConfig()
{
    m_pooler.testingRereadConfig();
    m_swapper.testingRereadConfig();
    kickPooler();
}

void KisTileDataStore::testingSuspendPooler()
{
    m_pooler.terminatePooler();
}

void KisTileDataStore::testingResumePooler()
{
    m_pooler.start();
}
