/*
 *  SPDX-FileCopyrightText: 2018 Andrey Kamakin <a.kamakin@icloud.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TILEHASHTABLE_2_H
#define KIS_TILEHASHTABLE_2_H

#include "kis_shared.h"
#include "kis_shared_ptr.h"
#include "3rdparty/lock_free_map/concurrent_map.h"
#include "kis_tile.h"
#include "kis_debug.h"

#define SANITY_CHECK

template <class T>
class KisTileHashTableIteratorTraits2;

template <class T>
class KisTileHashTableTraits2
{
    static constexpr bool isInherited = std::is_convertible<T*, KisShared*>::value;
    Q_STATIC_ASSERT_X(isInherited, "Template must inherit KisShared");

public:
    typedef T TileType;
    typedef KisSharedPtr<T> TileTypeSP;
    typedef KisWeakSharedPtr<T> TileTypeWSP;

    KisTileHashTableTraits2(KisMementoManager *mm);
    KisTileHashTableTraits2(const KisTileHashTableTraits2<T> &ht, KisMementoManager *mm);
    ~KisTileHashTableTraits2();

    bool isEmpty()
    {
        return !m_numTiles.loadRelaxed();
    }

    bool tileExists(qint32 col, qint32 row);

    TileTypeSP getExistingTile(qint32 col, qint32 row);

    TileTypeSP getTileLazy(qint32 col, qint32 row, bool& newTile);

    TileTypeSP getReadOnlyTileLazy(qint32 col, qint32 row, bool &existingTile);
    void addTile(TileTypeSP tile);
    bool deleteTile(TileTypeSP tile);
    bool deleteTile(qint32 col, qint32 row);

    void clear();

    void setDefaultTileData(KisTileData *defaultTileData);
    KisTileData* defaultTileData();

    KisTileData* refAndFetchDefaultTileData();

    qint32 numTiles()
    {
        return m_numTiles.loadRelaxed();
    }

    void debugPrintInfo();
    void debugMaxListLength(qint32 &min, qint32 &max);

    friend class KisTileHashTableIteratorTraits2<T>;

private:
    struct MemoryReclaimer {
        MemoryReclaimer(TileType *data) : d(data) {}

        void destroy()
        {
            TileTypeSP::deref(reinterpret_cast<TileTypeSP*>(this), d);
            delete this;
        }

    private:
        TileType *d;
    };

    inline quint32 calculateHashImpl(qint32 col, qint32 row)
    {
        if (col == 0 && row == 0) {
            col = 0x7FFF;
            row = 0x7FFF;
        }

        return ((static_cast<quint32>(row) << 16) | (static_cast<quint32>(col) & 0xFFFF));
    }

    inline quint32 calculateHash(qint32 col, qint32 row)
    {
#ifdef SANITY_CHECK
        KIS_ASSERT_RECOVER_NOOP(qAbs(row) < 0x7FFF && qAbs(col) < 0x7FFF);
#endif

        return calculateHashImpl(col, row);
    }

    inline quint32 calculateHashSafe(qint32 col, qint32 row)
    {
        KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(qAbs(row) < 0x7FFF && qAbs(col) < 0x7FFF, 0);
        return calculateHashImpl(col, row);
    }

    inline void insert(quint32 idx, TileTypeSP item)
    {
        TileTypeSP::ref(&item, item.data());
        TileType *tile = 0;

        {
            QReadLocker locker(&m_iteratorLock);
            m_map.getGC().lockRawPointerAccess();
            tile = m_map.assign(idx, item.data());
        }

        if (tile) {
            tile->notifyDeadWithoutDetaching();
            m_map.getGC().enqueue(&MemoryReclaimer::destroy, new MemoryReclaimer(tile));
        } else {
            m_numTiles.fetchAndAddRelaxed(1);
        }

        m_map.getGC().unlockRawPointerAccess();

        m_map.getGC().update();
    }

    inline bool erase(quint32 idx)
    {
        m_map.getGC().lockRawPointerAccess();

        bool wasDeleted = false;
        TileType *tile = m_map.erase(idx);

        if (tile) {
            tile->notifyDetachedFromDataManager();

            wasDeleted = true;
            m_numTiles.fetchAndSubRelaxed(1);
            m_map.getGC().enqueue(&MemoryReclaimer::destroy, new MemoryReclaimer(tile));
        }

        m_map.getGC().unlockRawPointerAccess();

        m_map.getGC().update();
        return wasDeleted;
    }

private:
    typedef ConcurrentMap<quint32, TileType*> LockFreeTileMap;
    typedef typename LockFreeTileMap::Mutator LockFreeTileMapMutator;
    mutable LockFreeTileMap m_map;

    QReadWriteLock m_defaultPixelDataLock;
    mutable QReadWriteLock m_iteratorLock;

    QAtomicInt m_numTiles;
    KisTileData *m_defaultTileData;
    KisMementoManager *m_mementoManager;
};

template <class T>
class KisTileHashTableIteratorTraits2
{
public:
    typedef T TileType;
    typedef KisSharedPtr<T> TileTypeSP;
    typedef typename ConcurrentMap<quint32, TileType*>::Iterator Iterator;

    KisTileHashTableIteratorTraits2(KisTileHashTableTraits2<T> *ht) : m_ht(ht)
    {
        m_ht->m_iteratorLock.lockForWrite();
        m_iter.setMap(m_ht->m_map);
    }

    ~KisTileHashTableIteratorTraits2()
    {
        m_ht->m_iteratorLock.unlock();
    }

    void next()
    {
        m_iter.next();
    }

    TileTypeSP tile() const
    {
        return TileTypeSP(m_iter.getValue());
    }

    bool isDone() const
    {
        return !m_iter.isValid();
    }

    void deleteCurrent()
    {
        m_ht->erase(m_iter.getKey());
        next();
    }

    void moveCurrentToHashTable(KisTileHashTableTraits2<T> *newHashTable)
    {
        TileTypeSP tile = m_iter.getValue();
        next();

        quint32 idx = m_ht->calculateHash(tile->col(), tile->row());
        m_ht->erase(idx);
        newHashTable->insert(idx, tile);
    }

private:
    KisTileHashTableTraits2<T> *m_ht;
    Iterator m_iter;
};

template <class T>
KisTileHashTableTraits2<T>::KisTileHashTableTraits2(KisMementoManager *mm)
    : m_numTiles(0), m_defaultTileData(0), m_mementoManager(mm)
{
}

template <class T>
KisTileHashTableTraits2<T>::KisTileHashTableTraits2(const KisTileHashTableTraits2<T> &ht, KisMementoManager *mm)
    : KisTileHashTableTraits2(mm)
{
    setDefaultTileData(ht.m_defaultTileData);

    QWriteLocker locker(&ht.m_iteratorLock);
    typename ConcurrentMap<quint32, TileType*>::Iterator iter(ht.m_map);

    while (iter.isValid()) {
        TileTypeSP tile = new TileType(*iter.getValue(), m_mementoManager);
        insert(iter.getKey(), tile);
        iter.next();
    }
}

template <class T>
KisTileHashTableTraits2<T>::~KisTileHashTableTraits2()
{
    clear();
    setDefaultTileData(0);
}

template<class T>
bool KisTileHashTableTraits2<T>::tileExists(qint32 col, qint32 row)
{
    return getExistingTile(col, row);
}

template <class T>
typename KisTileHashTableTraits2<T>::TileTypeSP KisTileHashTableTraits2<T>::getExistingTile(qint32 col, qint32 row)
{
    const quint32 idx = calculateHashSafe(col, row);
    if (!idx) {
        return TileTypeSP();
    }

    m_map.getGC().lockRawPointerAccess();
    TileTypeSP tile = m_map.get(idx);
    m_map.getGC().unlockRawPointerAccess();

    m_map.getGC().update();
    return tile;
}

template <class T>
typename KisTileHashTableTraits2<T>::TileTypeSP KisTileHashTableTraits2<T>::getTileLazy(qint32 col, qint32 row, bool &newTile)
{
    newTile = false;
    const quint32 idx = calculateHashSafe(col, row);
    if (!idx) {

        newTile = false;

        QReadLocker locker(&m_defaultPixelDataLock);
        return new TileType(col, row, m_defaultTileData, 0);
    }

    m_map.getGC().lockRawPointerAccess();

    TileTypeSP tile = m_map.get(idx);

    while (!tile) {
        m_map.getGC().unlockRawPointerAccess();

        {
            QReadLocker locker(&m_defaultPixelDataLock);
            tile = new TileType(col, row, m_defaultTileData, 0);
        }

        TileTypeSP::ref(&tile, tile.data());
        TileType *discardedTile = 0;

        m_iteratorLock.lockForRead();

        m_map.getGC().lockRawPointerAccess();

        LockFreeTileMapMutator mutator = m_map.insertOrFind(idx);
        if (!mutator.getValue()) {
            discardedTile = mutator.exchangeValue(tile.data());
        } else {
            discardedTile = tile.data();
        }

        m_iteratorLock.unlock();

        if (discardedTile) {
            tile = 0;

            discardedTile->notifyDeadWithoutDetaching();
            m_map.getGC().enqueue(&MemoryReclaimer::destroy, new MemoryReclaimer(discardedTile));

            tile = m_map.get(idx);
            continue;

        } else {
            newTile = true;
            m_numTiles.fetchAndAddRelaxed(1);

            tile->notifyAttachedToDataManager(m_mementoManager);
        }
    }
    m_map.getGC().unlockRawPointerAccess();

    m_map.getGC().update();
    return tile;
}

template <class T>
typename KisTileHashTableTraits2<T>::TileTypeSP KisTileHashTableTraits2<T>::getReadOnlyTileLazy(qint32 col, qint32 row, bool &existingTile)
{
    const quint32 idx = calculateHashSafe(col, row);
    if (!idx) {

        existingTile = false;

        QReadLocker locker(&m_defaultPixelDataLock);
        return new TileType(col, row, m_defaultTileData, 0);
    }

    m_map.getGC().lockRawPointerAccess();
    TileTypeSP tile = m_map.get(idx);
    m_map.getGC().unlockRawPointerAccess();

    existingTile = tile;

    if (!existingTile) {
        QReadLocker locker(&m_defaultPixelDataLock);
        tile = new TileType(col, row, m_defaultTileData, 0);
    }

    m_map.getGC().update();
    return tile;
}

template <class T>
void KisTileHashTableTraits2<T>::addTile(TileTypeSP tile)
{
    quint32 idx = calculateHash(tile->col(), tile->row());
    insert(idx, tile);
}

template <class T>
bool KisTileHashTableTraits2<T>::deleteTile(TileTypeSP tile)
{
    return deleteTile(tile->col(), tile->row());
}

template <class T>
bool KisTileHashTableTraits2<T>::deleteTile(qint32 col, qint32 row)
{
    const quint32 idx = calculateHashSafe(col, row);
    if (!idx) {
        return false;
    }

    return erase(idx);
}

template<class T>
void KisTileHashTableTraits2<T>::clear()
{
    {
        QWriteLocker locker(&m_iteratorLock);

        typename ConcurrentMap<quint32, TileType*>::Iterator iter(m_map);
        TileType *tile = 0;

        while (iter.isValid()) {
            m_map.getGC().lockRawPointerAccess();
            tile = m_map.erase(iter.getKey());

            if (tile) {
                tile->notifyDetachedFromDataManager();
                m_map.getGC().enqueue(&MemoryReclaimer::destroy, new MemoryReclaimer(tile));
            }
            m_map.getGC().unlockRawPointerAccess();

            iter.next();
        }

        m_numTiles.storeRelaxed(0);
    }

    m_map.getGC().update();
}

template <class T>
inline void KisTileHashTableTraits2<T>::setDefaultTileData(KisTileData *defaultTileData)
{
    QWriteLocker locker(&m_defaultPixelDataLock);

    if (m_defaultTileData) {
        m_defaultTileData->release();
        m_defaultTileData = 0;
    }

    if (defaultTileData) {
        defaultTileData->acquire();
        m_defaultTileData = defaultTileData;
    }
}

template <class T>
inline KisTileData* KisTileHashTableTraits2<T>::defaultTileData()
{
    QReadLocker locker(&m_defaultPixelDataLock);
    return m_defaultTileData;
}

template <class T>
inline KisTileData* KisTileHashTableTraits2<T>::refAndFetchDefaultTileData()
{
    QReadLocker locker(&m_defaultPixelDataLock);
    m_defaultTileData->ref();
    return m_defaultTileData;
}

template <class T>
void KisTileHashTableTraits2<T>::debugPrintInfo()
{
}

template <class T>
void KisTileHashTableTraits2<T>::debugMaxListLength(qint32 & , qint32 & )
{
}

typedef KisTileHashTableTraits2<KisTile> KisTileHashTable;
typedef KisTileHashTableIteratorTraits2<KisTile> KisTileHashTableIterator;
typedef KisTileHashTableIteratorTraits2<KisTile> KisTileHashTableConstIterator;

#endif
