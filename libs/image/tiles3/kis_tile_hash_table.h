/*
 *  SPDX-FileCopyrightText: 2004 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TILEHASHTABLE_H_
#define KIS_TILEHASHTABLE_H_

#include "kis_tile.h"

template<class T>
class KisTileHashTableTraits
{
public:
    typedef T               TileType;
    typedef KisSharedPtr<T> TileTypeSP;
    typedef KisWeakSharedPtr<T> TileTypeWSP;

    KisTileHashTableTraits(KisMementoManager *mm);
    KisTileHashTableTraits(const KisTileHashTableTraits<T> &ht,
                           KisMementoManager *mm);

    ~KisTileHashTableTraits();

    bool isEmpty() {
        return !m_numTiles;
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
    KisTileData* defaultTileData() const;
    KisTileData* refAndFetchDefaultTileData() const;

    qint32 numTiles() {
        return m_numTiles;
    }

    void debugPrintInfo();
    void debugMaxListLength(qint32 &min, qint32 &max);

private:

    TileTypeSP getTileMinefieldWalk(qint32 col, qint32 row, qint32 idx);
    TileTypeSP getTile(qint32 col, qint32 row, qint32 idx);
    void linkTile(TileTypeSP tile, qint32 idx);
    bool unlinkTile(qint32 col, qint32 row, qint32 idx);

    inline void setDefaultTileDataImp(KisTileData *defaultTileData);
    inline KisTileData* defaultTileDataImp() const;

    static inline quint32 calculateHash(qint32 col, qint32 row);

    inline qint32 debugChainLen(qint32 idx);
    void debugListLengthDistribution();
    void sanityChecksumCheck();
private:
    template<class U, class LockerType> friend class KisTileHashTableIteratorTraits;

    static const qint32 TABLE_SIZE = 1024;
    TileTypeSP *m_hashTable;
    qint32 m_numTiles;

    KisTileData *m_defaultTileData;
    KisMementoManager *m_mementoManager;

    mutable QReadWriteLock m_lock;
};

#include "kis_tile_hash_table_p.h"

template<class T, class LockerType>
class KisTileHashTableIteratorTraits
{
public:
    typedef T               TileType;
    typedef KisSharedPtr<T> TileTypeSP;

    KisTileHashTableIteratorTraits(KisTileHashTableTraits<T> *ht)
        : m_locker(&ht->m_lock)
    {
        m_hashTable = ht;
        m_index = nextNonEmptyList(0);
        if (m_index < KisTileHashTableTraits<T>::TABLE_SIZE)
            m_tile = m_hashTable->m_hashTable[m_index];
    }

    ~KisTileHashTableIteratorTraits() {
    }

    void next() {
        if (m_tile) {
            m_tile = m_tile->next();
            if (!m_tile) {
                qint32 idx = nextNonEmptyList(m_index + 1);
                if (idx < KisTileHashTableTraits<T>::TABLE_SIZE) {
                    m_index = idx;
                    m_tile = m_hashTable->m_hashTable[idx];
                } else {
                    m_index = -1;
                }
            }
        }
    }

    TileTypeSP tile() const {
        return m_tile;
    }
    bool isDone() const {
        return !m_tile;
    }

    template <class Helper = LockerType>
    typename std::enable_if<std::is_same<Helper, QWriteLocker>::value, void>::type
    deleteCurrent() {
        TileTypeSP tile = m_tile;
        next();

        const qint32 idx = m_hashTable->calculateHash(tile->col(), tile->row());
        m_hashTable->unlinkTile(tile->col(), tile->row(), idx);
    }

    template <class Helper = LockerType>
    typename std::enable_if<std::is_same<Helper, QWriteLocker>::value, void>::type
    moveCurrentToHashTable(KisTileHashTableTraits<T> *newHashTable) {
        TileTypeSP tile = m_tile;
        next();

        const qint32 idx = m_hashTable->calculateHash(tile->col(), tile->row());
        m_hashTable->unlinkTile(tile->col(), tile->row(), idx);

        newHashTable->addTile(tile);
    }

protected:
    TileTypeSP m_tile;
    qint32 m_index;
    KisTileHashTableTraits<T> *m_hashTable;
    LockerType m_locker;

protected:
    qint32 nextNonEmptyList(qint32 startIdx) {
        qint32 idx = startIdx;

        while (idx < KisTileHashTableTraits<T>::TABLE_SIZE &&
                !m_hashTable->m_hashTable[idx]) {
            idx++;
        }

        return idx;
    }
private:
    Q_DISABLE_COPY(KisTileHashTableIteratorTraits)
};

typedef KisTileHashTableTraits<KisTile> KisTileHashTable;
typedef KisTileHashTableIteratorTraits<KisTile, QWriteLocker> KisTileHashTableIterator;
typedef KisTileHashTableIteratorTraits<KisTile, QReadLocker> KisTileHashTableConstIterator;

#endif
