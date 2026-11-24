/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_TILEDDATAMANAGER_H_
#define KIS_TILEDDATAMANAGER_H_

#include <QtGlobal>
#include <QVector>
#include <KisRegion.h>

#include <kis_shared.h>
#include <kis_shared_ptr.h>
#include "config-hash-table-implementation.h"

#include "kritaimage_export.h"

#ifdef USE_LOCK_FREE_HASH_TABLE
#include "kis_tile_hash_table2.h"
#else
#include "kis_tile_hash_table.h"
#endif

#include "kis_memento_manager.h"
#include "kis_memento.h"
#include "KisTiledExtentManager.h"

class KisTiledDataManager;
typedef KisSharedPtr<KisTiledDataManager> KisTiledDataManagerSP;

class KisTiledIterator;
class KisTiledRandomAccessor;
class KisPaintDeviceWriter;
class QIODevice;

class KRITAIMAGE_EXPORT KisTiledDataManager : public KisShared
{
private:
    static const qint32 LEGACY_VERSION = 1;
    static const qint32 CURRENT_VERSION = 2;

protected:
public:
    KisTiledDataManager(quint32 pixelSize, const quint8 *defPixel);
    virtual ~KisTiledDataManager();
    KisTiledDataManager(const KisTiledDataManager &dm);
    KisTiledDataManager & operator=(const KisTiledDataManager &dm);

protected:
    friend class KisTiledIterator;
    friend class KisBaseIterator;
    friend class KisTiledRandomAccessor;
    friend class KisRandomAccessor2;
    friend class KisStressJob;

public:
    void setDefaultPixel(const quint8 *defPixel);
    const quint8 *defaultPixel() const {
        return m_defaultPixel;
    }

    inline void getTilesPair(qint32 col, qint32 row, bool writable, KisTileSP *tile, KisTileSP *oldTile) {
        *tile = getTile(col, row, writable);

        bool unused;
        *oldTile = m_mementoManager->getCommittedTile(col, row, unused);

        if (!*oldTile) {
            *oldTile = *tile;
        }
    }

    inline KisTileSP getTile(qint32 col, qint32 row, bool writable) {
        if (writable) {
            bool newTile;
            KisTileSP tile = m_hashTable->getTileLazy(col, row, newTile);
            if (newTile) {
                m_extentManager.notifyTileAdded(col, row);
            }
            return tile;

        } else {
            bool unused;
            return m_hashTable->getReadOnlyTileLazy(col, row, unused);
        }
    }

    inline KisTileSP getReadOnlyTileLazy(qint32 col, qint32 row, bool &existingTile) {
        return m_hashTable->getReadOnlyTileLazy(col, row, existingTile);
    }

    inline KisTileSP getOldTile(qint32 col, qint32 row, bool &existingTile) {
        KisTileSP tile = m_mementoManager->getCommittedTile(col, row, existingTile);
        return tile ? tile : getReadOnlyTileLazy(col, row, existingTile);
    }

    inline KisTileSP getOldTile(qint32 col, qint32 row) {
        bool unused;
        return getOldTile(col, row, unused);
    }

    KisMementoSP getMemento() {
        QWriteLocker locker(&m_lock);
        KisMementoSP memento = m_mementoManager->getMemento();
        memento->saveOldDefaultPixel(m_defaultPixel, m_pixelSize);
        return memento;
    }

    void commit() {
        QWriteLocker locker(&m_lock);

        KisMementoSP memento = m_mementoManager->currentMemento();
        if(memento) {
            memento->saveNewDefaultPixel(m_defaultPixel, m_pixelSize);
        }

        m_mementoManager->commit();
    }

    void rollback(KisMementoSP memento) {
        commit();

        QWriteLocker locker(&m_lock);
        m_mementoManager->rollback(m_hashTable, memento);
        const quint8 *defaultPixel = memento->oldDefaultPixel();
        if(memcmp(m_defaultPixel, defaultPixel, m_pixelSize)) {
            setDefaultPixelImpl(defaultPixel);
        }
        recalculateExtent();
    }
    void rollforward(KisMementoSP memento) {
        commit();

        QWriteLocker locker(&m_lock);
        m_mementoManager->rollforward(m_hashTable, memento);
        const quint8 *defaultPixel = memento->newDefaultPixel();
        if(memcmp(m_defaultPixel, defaultPixel, m_pixelSize)) {
            setDefaultPixelImpl(defaultPixel);
        }
        recalculateExtent();
    }
    bool hasCurrentMemento() const {
        return m_mementoManager->hasCurrentMemento();
    }

    void purgeHistory(KisMementoSP oldestMemento) {
        QWriteLocker locker(&m_lock);
        m_mementoManager->purgeHistory(oldestMemento);
    }

    static void releaseInternalPools();

protected:
    bool write(KisPaintDeviceWriter &store);
    bool read(QIODevice *stream);

    void purge(const QRect& area);

    inline quint32 pixelSize() const {
        return m_pixelSize;
    }

public:

    void  extent(qint32 &x, qint32 &y, qint32 &w, qint32 &h) const;
    void  setExtent(qint32 x, qint32 y, qint32 w, qint32 h);
    QRect extent() const;
    void  setExtent(QRect newRect);

    KisRegion region() const;

    void clear(QRect clearRect, quint8 clearValue);
    void clear(QRect clearRect, const quint8 *clearPixel);
    void clear(qint32 x, qint32 y, qint32 w, qint32 h, quint8 clearValue);
    void clear(qint32 x, qint32 y,  qint32 w, qint32 h, const quint8 *clearPixel);
    void clear();

    void bitBlt(KisTiledDataManager *srcDM, const QRect &rect);

    void bitBltOldData(KisTiledDataManager *srcDM, const QRect &rect);

    void bitBltRough(KisTiledDataManager *srcDM, const QRect &rect);

    void bitBltRoughOldData(KisTiledDataManager *srcDM, const QRect &rect);

    void setPixel(qint32 x, qint32 y, const quint8 * data);

    void readBytes(quint8 * bytes,
                   qint32 x, qint32 y,
                   qint32 w, qint32 h,
                   qint32 dataRowStride = -1) const;
    void writeBytes(const quint8 * bytes,
                    qint32 x, qint32 y,
                    qint32 w, qint32 h,
                    qint32 dataRowStride = -1);

    QVector<quint8*> readPlanarBytes(QVector<qint32> channelsizes, qint32 x, qint32 y, qint32 w, qint32 h) const;

    void writePlanarBytes(QVector<quint8*> planes, QVector<qint32> channelsizes, qint32 x, qint32 y, qint32 w, qint32 h);

    qint32 numContiguousColumns(qint32 x, qint32 minY, qint32 maxY) const;

    qint32 numContiguousRows(qint32 y, qint32 minX, qint32 maxX) const;

    qint32 rowStride(qint32 x, qint32 y) const;

private:
    KisTileHashTable *m_hashTable;
    KisMementoManager *m_mementoManager;
    quint8* m_defaultPixel;
    qint32 m_pixelSize;
    KisTiledExtentManager m_extentManager;

    mutable QReadWriteLock m_lock;

private:
    friend class KisAbstractTileCompressor;
    friend class KisTileDataWrapper;
    inline qint32 xToCol(qint32 x) const
    {
        return divideRoundDown(x, KisTileData::WIDTH);
    }
    inline qint32 yToRow(qint32 y) const
    {
        return divideRoundDown(y, KisTileData::HEIGHT);
    }

private:
    void setDefaultPixelImpl(const quint8 *defPixel);

    bool writeTilesHeader(KisPaintDeviceWriter &store, quint32 numTiles);
    bool processTilesHeader(QIODevice *stream, quint32 &numTiles);

    inline qint32 divideRoundDown(qint32 x, const qint32 y) const
    {

        return x >= 0 ? x / y : -(((-x - 1) / y) + 1);
    }

    void recalculateExtent();

    quint8* duplicatePixel(qint32 num, const quint8 *pixel);

    template<bool useOldSrcData>
        void bitBltImpl(KisTiledDataManager *srcDM, const QRect &rect);
    template<bool useOldSrcData>
        void bitBltRoughImpl(KisTiledDataManager *srcDM, const QRect &rect);

    void writeBytesBody(const quint8 *data,
                        qint32 x, qint32 y,
                        qint32 width, qint32 height,
                        qint32 dataRowStride = -1);
    void readBytesBody(quint8 *data,
                       qint32 x, qint32 y,
                       qint32 width, qint32 height,
                       qint32 dataRowStride = -1) const;

    template <bool allChannelsPresent>
    void writePlanarBytesBody(QVector<quint8*> planes,
                              QVector<qint32> channelsizes,
                              qint32 x, qint32 y, qint32 w, qint32 h);
    QVector<quint8*> readPlanarBytesBody(QVector<qint32> channelsizes,
                                         qint32 x, qint32 y,
                                         qint32 w, qint32 h) const;
public:
    void debugPrintInfo() {
        m_mementoManager->debugPrintInfo();
    }

};

#endif

