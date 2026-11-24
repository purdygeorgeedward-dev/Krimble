/*
 *  SPDX-FileCopyrightText: 2004 C. Boemann <cbo@boemann.dk>
 *            (c) 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_TILE_H_
#define KIS_TILE_H_

#include <QReadWriteLock>

#include <QMutex>
#include <QAtomicPointer>

#include <QRect>
#include <QStack>

#include <kis_shared.h>
#include <kis_shared_ptr.h>

#include "kis_tile_data.h"
#include "kis_tile_data_store.h"

class KisTile;
typedef KisSharedPtr<KisTile> KisTileSP;

class KisMementoManager;

class KRITAIMAGE_EXPORT KisTile : public KisShared
{
public:
    KisTile(qint32 col, qint32 row,
            KisTileData *defaultTileData, KisMementoManager* mm);
    KisTile(const KisTile& rhs, qint32 col, qint32 row, KisMementoManager* mm);
    KisTile(const KisTile& rhs, KisMementoManager* mm);
    KisTile(const KisTile& rhs);
    ~KisTile();

    void notifyDetachedFromDataManager();

    void notifyDeadWithoutDetaching();

    void notifyAttachedToDataManager(KisMementoManager *mm);

public:

    void debugPrintInfo();
    void debugDumpTile();

    void lockForRead() const;
    void lockForWrite();
    void unlockForWrite();
    void unlockForRead() const;

    inline quint8 *data() const {
        return m_tileData->data();
    }
    inline void setData(const quint8 *data) {
        m_tileData->setData(data);
    }

    inline qint32 row() const {
        return m_row;
    }
    inline qint32 col() const {
        return m_col;
    }

    inline QRect extent() const {
        return m_extent;
    }

    inline KisTileSP next() const {
        return m_nextTile;
    }

    void setNext(KisTileSP next) {
        m_nextTile = next;
    }

    inline qint32 pixelSize() const {
        return m_tileData->pixelSize();
    }

    inline KisTileData*  tileData() const {
        return m_tileData;
    }

private:
    void init(qint32 col, qint32 row,
              KisTileData *defaultTileData, KisMementoManager* mm);

    inline void blockSwapping() const;
    inline void unblockSwapping() const;

    inline void safeReleaseOldTileData(KisTileData *td);

private:
    KisTileData *m_tileData;
    mutable QStack<KisTileData*> m_oldTileData;
    mutable volatile int m_lockCounter;

    qint32 m_col;
    qint32 m_row;

    QRect m_extent;

    KisTileSP m_nextTile;

    QAtomicPointer<KisMementoManager> m_mementoManager;

    QMutex m_COWMutex;

    mutable QMutex m_swapBarrierLock;

#ifdef DEAD_TILES_SANITY_CHECK
    QAtomicInt m_sanityHasBeenDetached;
    QAtomicInt m_sanityIsDead;
    QAtomicInt m_sanityMMHasBeenInitializedManually;
    QAtomicInt m_sanityNumCOWHappened;
    QAtomicInt m_sanityLockedForWrite;
    mutable QAtomicInt m_sanityLockedForRead;

    void sanityCheckIsNotDestroyedYet();
    void sanityCheckIsNotLockedForWrite();
#endif

};

#endif

