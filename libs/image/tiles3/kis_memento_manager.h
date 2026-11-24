/*
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_MEMENTO_MANAGER_
#define KIS_MEMENTO_MANAGER_

#include <QList>
#include <QMutex>

#include "kis_memento_item.h"
#include "config-hash-table-implementation.h"

typedef QList<KisMementoItemSP> KisMementoItemList;
typedef QListIterator<KisMementoItemSP> KisMementoItemListIterator;

class KisMemento;
struct KisHistoryItem {
    KisMemento* memento;
    KisMementoItemList itemList;
};

typedef QList<KisHistoryItem> KisHistoryList;

class KisMemento;
typedef KisSharedPtr<KisMemento> KisMementoSP;

#ifdef USE_LOCK_FREE_HASH_TABLE
#include "kis_tile_hash_table2.h"

typedef KisTileHashTableTraits2<KisMementoItem> KisMementoItemHashTable;
typedef KisTileHashTableIteratorTraits2<KisMementoItem> KisMementoItemHashTableIterator;
typedef KisTileHashTableIteratorTraits2<KisMementoItem> KisMementoItemHashTableIteratorConst;
#else
#include "kis_tile_hash_table.h"

typedef KisTileHashTableTraits<KisMementoItem> KisMementoItemHashTable;
typedef KisTileHashTableIteratorTraits<KisMementoItem, QWriteLocker> KisMementoItemHashTableIterator;
typedef KisTileHashTableIteratorTraits<KisMementoItem, QReadLocker> KisMementoItemHashTableIteratorConst;
#endif

class KRITAIMAGE_EXPORT KisMementoManager
{
public:
    KisMementoManager();
    KisMementoManager(const KisMementoManager& rhs);
    ~KisMementoManager();

    void registerTileChange(KisTile *tile);

    void registerTileDeleted(KisTile *tile);

    void commit();

    void rollback(KisTileHashTable *ht, KisMementoSP memento);
    void rollforward(KisTileHashTable *ht, KisMementoSP memento);

    KisTileSP getCommittedTile(qint32 col, qint32 row, bool &existingTile);

    KisMementoSP getMemento();

    bool hasCurrentMemento() {
        return m_currentMemento;
    }

    KisMementoSP currentMemento();

    void setDefaultTileData(KisTileData *defaultTileData);

    void debugPrintInfo();

    void purgeHistory(KisMementoSP oldestMemento);

protected:
    qint32 findRevisionByMemento(KisMementoSP memento) const;
    void resetRevisionHistory(KisMementoItemList list);

protected:
    KisMementoItemHashTable m_index;

    KisHistoryList m_revisions;

    KisHistoryList m_cancelledRevisions;

    KisMementoItemHashTable m_headsHashTable;

    KisMementoSP m_currentMemento;
    QMutex m_currentMementoExtentLock;

    bool m_registrationBlocked;
};

#endif
