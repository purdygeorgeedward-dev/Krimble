/*
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2018 Andrey Kamakin <a.kamakin@icloud.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TILE_DATA_INTERFACE_H_
#define KIS_TILE_DATA_INTERFACE_H_

#include <QReadWriteLock>
#include <QAtomicInt>

#include "kis_lockless_stack.h"
#include "swap/kis_chunk_allocator.h"

class KisTileData;
class KisTileDataStore;

#define __TILE_DATA_WIDTH 64
#define __TILE_DATA_HEIGHT 64

typedef KisLocklessStack<KisTileData*> KisTileDataCache;

typedef QLinkedList<KisTileData*> KisTileDataList;
typedef KisTileDataList::iterator KisTileDataListIterator;
typedef KisTileDataList::const_iterator KisTileDataListConstIterator;

class SimpleCache
{
public:
    SimpleCache() = default;
    ~SimpleCache();

    bool push(int pixelSize, quint8 *&ptr)
    {
        QReadLocker l(&m_cacheLock);
        switch (pixelSize) {
        case 4:
            m_4Pool.push(ptr);
            break;
        case 8:
            m_8Pool.push(ptr);
            break;
        case 16:
            m_16Pool.push(ptr);
            break;
        default:
            return false;
        }

        return true;
    }

    bool pop(int pixelSize, quint8 *&ptr)
    {
        QReadLocker l(&m_cacheLock);
        switch (pixelSize) {
        case 4:
            return m_4Pool.pop(ptr);
        case 8:
            return m_8Pool.pop(ptr);
        case 16:
            return m_16Pool.pop(ptr);
        default:
            return false;
        }
    }

    void clear();

private:
    QReadWriteLock m_cacheLock;
    KisLocklessStack<quint8*> m_4Pool;
    KisLocklessStack<quint8*> m_8Pool;
    KisLocklessStack<quint8*> m_16Pool;
};

class KRITAIMAGE_EXPORT KisTileData
{
public:
    KisTileData(qint32 pixelSize, const quint8 *defPixel, KisTileDataStore *store, bool checkFreeMemory = true);

private:
    KisTileData(const KisTileData& rhs, bool checkFreeMemory = true);

public:
    ~KisTileData();

    enum EnumTileDataState {
        NORMAL = 0,
        COMPRESSED,
        SWAPPED
    };

    inline quint8* data() const;
    inline void setData(const quint8 *data);
    inline quint32 pixelSize() const;

    inline bool acquire();

    inline bool release();

    inline bool ref() const;

    inline bool deref();

    inline KisTileData* clone();

    inline void blockSwapping();
    inline void unblockSwapping();

    inline KisChunk swapChunk() const;
    inline void setSwapChunk(KisChunk chunk);

    inline bool mementoed() const;
    inline void setMementoed(bool value);

    inline int age() const;
    inline void resetAge();
    inline void markOld();

    inline qint32 numUsers() const;

    inline bool historical() const;

    void releaseMemory();

    void allocateMemory();

    static void releaseInternalPools();

private:
    void fillWithPixel(const quint8 *defPixel);

    static quint8* allocateData(const qint32 pixelSize);
    static void freeData(quint8 *ptr, const qint32 pixelSize);
private:
    friend class KisTileDataPooler;
    friend class KisTileDataPoolerTest;
    KisTileDataCache m_clonesStack;

private:
    friend class KisTile;
    friend class KisTileDataStore;

    friend class KisTileDataStoreIterator;
    friend class KisTileDataStoreReverseIterator;
    friend class KisTileDataStoreClockIterator;

    mutable EnumTileDataState m_state;

    int m_tileNumber = -1;

private:
    KisChunk m_swapChunk;

    qint32 m_mementoFlag;

    int m_age;

    QReadWriteLock m_swapLock;

private:
    friend class KisLowMemoryTests;

    mutable quint8* m_data;

    mutable QAtomicInt m_usersCount;

    mutable QAtomicInt m_refCount;

    qint32 m_pixelSize;

    KisTileDataStore *m_store;
    static SimpleCache m_cache;

public:
    static const qint32 WIDTH;
    static const qint32 HEIGHT;
};

#endif
