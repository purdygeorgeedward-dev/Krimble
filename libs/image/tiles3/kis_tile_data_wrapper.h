/*
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_TILE_DATA_WRAPPER_H
#define __KIS_TILE_DATA_WRAPPER_H

class KisTileDataWrapper
{
public:
    enum accessType {
        READ,
        WRITE
    };

    inline KisTileDataWrapper(KisTiledDataManager *dm,
                              qint32 x, qint32 y,
                              enum KisTileDataWrapper::accessType type)
    {
        const qint32 col = dm->xToCol(x);
        const qint32 row = dm->yToRow(y);

        const qint32 xInTile = x - col * KisTileData::WIDTH;
        const qint32 yInTile = y - row * KisTileData::HEIGHT;

        const qint32 pixelIndex = xInTile + yInTile * KisTileData::WIDTH;

        KisTileSP tile = dm->getTile(col, row, type == WRITE);

        m_tile = tile;
        m_offset = pixelIndex * dm->pixelSize();

        if (type == READ) {
            m_tile->lockForRead();
        }
        else {
            m_tile->lockForWrite();
        }

        m_type = type;
    }

    virtual ~KisTileDataWrapper()
    {
        if (m_type == READ) {
            m_tile->unlockForRead();
        } else {
            m_tile->unlockForWrite();
        }
    }

    inline qint32 offset() const
    {
        return m_offset;
    }

    inline KisTileSP& tile()
    {
        return m_tile;
    }

    inline quint8* data() const
    {
        return m_tile->data() + m_offset;
    }

private:
    Q_DISABLE_COPY(KisTileDataWrapper)

    KisTileSP m_tile;
    qint32 m_offset;
    KisTileDataWrapper::accessType m_type;
};
#endif
