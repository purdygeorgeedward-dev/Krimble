/*
 *  SPDX-FileCopyrightText: 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *  SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_vline_iterator.h"

#include <iostream>

KisVLineIterator2::KisVLineIterator2(KisDataManager *dataManager, qint32 x, qint32 y, qint32 h, qint32 offsetX, qint32 offsetY, bool writable, KisIteratorCompleteListener *completeListener)
    : KisBaseIterator(dataManager, writable, completeListener),
      m_offsetX(offsetX),
      m_offsetY(offsetY)
{
    x -= m_offsetX;
    y -= m_offsetY;
    Q_ASSERT(dataManager != 0);

    Q_ASSERT(h > 0);
    if (h < 1) h = 1;

    m_lineStride = m_pixelSize * KisTileData::WIDTH;

    m_x = x;
    m_y = y;

    m_top = y;
    m_bottom = y + h - 1;

    m_left = m_x;

    m_havePixels = (h == 0) ? false : true;
    if (m_top > m_bottom) {
        m_havePixels = false;
        return;
    }

    m_topRow = yToRow(m_top);
    m_bottomRow = yToRow(m_bottom);

    m_column = xToCol(m_x);
    m_xInTile = calcXInTile(m_x, m_column);

    m_topInTopmostTile = m_top - m_topRow * KisTileData::WIDTH;

    m_tilesCacheSize = m_bottomRow - m_topRow + 1;
    m_tilesCache.resize(m_tilesCacheSize);

    m_tileSize = m_lineStride * KisTileData::HEIGHT;

    for (int i = 0; i < m_tilesCacheSize; i++){
        fetchTileDataForCache(m_tilesCache[i], m_column, m_topRow + i);
    }
    m_index = 0;
    switchToTile(m_topInTopmostTile);
}

void KisVLineIterator2::resetPixelPos()
{
    m_y = m_top;

    m_index = 0;
    switchToTile(m_topInTopmostTile);

    m_havePixels = true;
}

void KisVLineIterator2::resetColumnPos()
{
    m_x = m_left;

    m_column = xToCol(m_x);
    m_xInTile = calcXInTile(m_x, m_column);
    preallocateTiles();

    resetPixelPos();
}

bool KisVLineIterator2::nextPixel()
{
    if (m_y >= m_bottom) {
        return m_havePixels = false;
    } else {
        ++m_y;
        m_data += m_lineStride;
        if (m_data < m_dataBottom)
            m_oldData += m_lineStride;
        else {
            ++m_index;
            switchToTile(0);
        }
    }

    return m_havePixels;
}

void KisVLineIterator2::nextColumn()
{
    m_y = m_top;
    ++m_x;

    if (++m_xInTile < KisTileData::HEIGHT) {
    } else {
        ++m_column;
        m_xInTile = 0;
        preallocateTiles();
    }
    m_index = 0;
    switchToTile(m_topInTopmostTile);

    m_havePixels = true;
}

qint32 KisVLineIterator2::nConseqPixels() const
{
    return 1;
}

bool KisVLineIterator2::nextPixels(qint32 n)
{
    Q_ASSERT_X(!(m_y > 0 && (m_y + n) < 0), "vlineIt+=", "Integer overflow");

    qint32 previousRow = yToRow(m_y);
    if (m_y >= m_bottom || (m_y += n) > m_bottom) {
        m_havePixels = false;
    } else {
        qint32 row = yToRow(m_y);
        if (row == previousRow) {
            m_data += n * m_pixelSize;
        } else {
            qint32 yInTile = calcYInTile(m_y, row);
            m_index += row - previousRow;
            switchToTile(yInTile);
        }
    }
    return m_havePixels;
}

KisVLineIterator2::~KisVLineIterator2()
{
    for (int i = 0; i < m_tilesCacheSize; i++) {
        unlockTile(m_tilesCache[i].tile);
        unlockOldTile(m_tilesCache[i].oldtile);
    }
}

quint8* KisVLineIterator2::rawData()
{
    return m_data;
}

const quint8* KisVLineIterator2::oldRawData() const
{
    return m_oldData;
}

const quint8* KisVLineIterator2::rawDataConst() const
{
    return m_data;
}

void KisVLineIterator2::switchToTile(qint32 yInTile)
{
    Q_ASSERT(m_index < m_tilesCacheSize);
    Q_ASSERT(m_index >= 0);

    int offset_row = m_pixelSize * m_xInTile;
    m_data = m_tilesCache[m_index].data;
    m_oldData = m_tilesCache[m_index].oldData;
    m_data += offset_row;
    m_dataBottom = m_data + m_tileSize;
    int offset_col = m_pixelSize * yInTile * KisTileData::WIDTH;
    m_data  += offset_col;
    m_oldData += offset_row + offset_col;
}

void KisVLineIterator2::fetchTileDataForCache(KisTileInfo& kti, qint32 col, qint32 row)
{
    m_dataManager->getTilesPair(col, row, m_writable, &kti.tile, &kti.oldtile);

    lockTile(kti.tile);
    kti.data = kti.tile->data();

    lockOldTile(kti.oldtile);
    kti.oldData = kti.oldtile->data();
}

void KisVLineIterator2::preallocateTiles()
{
    for (int i = 0; i < m_tilesCacheSize; ++i){
        unlockTile(m_tilesCache[i].tile);
        unlockOldTile(m_tilesCache[i].oldtile);
        fetchTileDataForCache(m_tilesCache[i], m_column, m_topRow + i );
    }
}

qint32 KisVLineIterator2::x() const
{
    return m_x + m_offsetX;
}

qint32 KisVLineIterator2::y() const
{
    return m_y + m_offsetY;
}
