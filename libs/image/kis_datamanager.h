/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_DATAMANAGER_H_
#define KIS_DATAMANAGER_H_

#include <QtGlobal>

class QRect;
class KisPaintDeviceWriter;
class QIODevice;

#include <tiles3/kis_tiled_data_manager.h>
#include <tiles3/kis_memento.h>

#define ACTUAL_DATAMGR KisTiledDataManager

class KisDataManager : public ACTUAL_DATAMGR
{

public:

KisDataManager(quint32 pixelSize, const quint8 *defPixel) : ACTUAL_DATAMGR(pixelSize, defPixel) {}
    KisDataManager(const KisDataManager& dm) : ACTUAL_DATAMGR(dm) { }

    ~KisDataManager() override {
    }

public:
    inline void setDefaultPixel(const quint8 *defPixel) {
        return ACTUAL_DATAMGR::setDefaultPixel(defPixel);
    }

    inline const quint8 *defaultPixel() const {
        return ACTUAL_DATAMGR::defaultPixel();
    }

    inline KisMementoSP getMemento() {
        return ACTUAL_DATAMGR::getMemento();
    }

    inline void rollback(KisMementoSP memento) {
        ACTUAL_DATAMGR::rollback(memento);
    }

    inline void rollforward(KisMementoSP memento) {
        ACTUAL_DATAMGR::rollforward(memento);
    }

    inline bool hasCurrentMemento() const {
        return ACTUAL_DATAMGR::hasCurrentMemento();
    }

public:

    inline bool write(KisPaintDeviceWriter &writer) {
        return ACTUAL_DATAMGR::write(writer);
    }

    inline bool read(QIODevice *io) {
        return ACTUAL_DATAMGR::read(io);
    }

    inline void purge(const QRect& area) {
        ACTUAL_DATAMGR::purge(area);
    }

    static inline void releaseInternalPools() {
        ACTUAL_DATAMGR::releaseInternalPools();
    }

public:

    inline quint32 pixelSize() const {
        return ACTUAL_DATAMGR::pixelSize();
    }

    inline void extent(qint32 &x, qint32 &y, qint32 &w, qint32 &h) const {
        return ACTUAL_DATAMGR::extent(x, y, w, h);
    }

    QRect extent() const {
        return ACTUAL_DATAMGR::extent();
    }

    KisRegion region() const {
        return ACTUAL_DATAMGR::region();
    }

public:

    inline void setExtent(qint32 x, qint32 y, qint32 w, qint32 h) {
        return ACTUAL_DATAMGR::setExtent(x, y, w, h);
    }

    inline void setExtent(const QRect & rect) {
        setExtent(rect.x(), rect.y(), rect.width(), rect.height());
    }

public:

    inline void clear(qint32 x, qint32 y,
                      qint32 w, qint32 h,
                      quint8 def) {
        ACTUAL_DATAMGR::clear(x, y, w, h, def);
    }

    inline void clear(qint32 x, qint32 y,
                      qint32 w, qint32 h,
                      const quint8 * def) {
        ACTUAL_DATAMGR::clear(x, y, w, h, def);
    }

    inline void clear() {
        ACTUAL_DATAMGR::clear();
    }

public:

    inline void bitBlt(KisTiledDataManagerSP srcDM, const QRect &rect) {
        ACTUAL_DATAMGR::bitBlt(const_cast<KisTiledDataManager*>(srcDM.data()), rect);
    }

    inline void bitBltOldData(KisTiledDataManagerSP srcDM, const QRect &rect) {
        ACTUAL_DATAMGR::bitBltOldData(const_cast<KisTiledDataManager*>(srcDM.data()), rect);
    }

    inline void bitBltRough(KisTiledDataManagerSP srcDM, const QRect &rect) {
        ACTUAL_DATAMGR::bitBltRough(const_cast<KisTiledDataManager*>(srcDM.data()), rect);
    }

    inline void bitBltRoughOldData(KisTiledDataManagerSP srcDM, const QRect &rect) {
        ACTUAL_DATAMGR::bitBltRoughOldData(const_cast<KisTiledDataManager*>(srcDM.data()), rect);
    }

public:

    inline void setPixel(qint32 x, qint32 y, const quint8 * data) {
        ACTUAL_DATAMGR::setPixel(x, y, data);
    }

    inline void readBytes(quint8 * data,
                          qint32 x, qint32 y,
                          qint32 w, qint32 h,
                          qint32 dataRowStride = -1) const {
        ACTUAL_DATAMGR::readBytes(data, x, y, w, h, dataRowStride);
    }

    inline void writeBytes(const quint8 * data,
                           qint32 x, qint32 y,
                           qint32 w, qint32 h,
                           qint32 dataRowStride = -1) {
        ACTUAL_DATAMGR::writeBytes(data, x, y, w, h, dataRowStride);
    }

    QVector<quint8*> readPlanarBytes(QVector<qint32> channelsizes, qint32 x, qint32 y, qint32 w, qint32 h) const {
        return ACTUAL_DATAMGR::readPlanarBytes(channelsizes, x, y, w, h);
    }

    void writePlanarBytes(QVector<quint8*> planes, QVector<qint32> channelsizes,  qint32 x, qint32 y, qint32 w, qint32 h) {
        ACTUAL_DATAMGR::writePlanarBytes(planes, channelsizes, x, y, w, h);
    }

    inline qint32 numContiguousColumns(qint32 x, qint32 minY, qint32 maxY) const {
        return ACTUAL_DATAMGR::numContiguousColumns(x, minY, maxY);
    }

    inline qint32 numContiguousRows(qint32 y, qint32 minX, qint32 maxX) const {
        return ACTUAL_DATAMGR::numContiguousRows(y, minX, maxX);
    }

    inline qint32 rowStride(qint32 x, qint32 y) const {
        return ACTUAL_DATAMGR::rowStride(x, y);
    }

protected:
    friend class KisRectIterator;
    friend class KisHLineIterator;
    friend class KisVLineIterator;
};

#endif

