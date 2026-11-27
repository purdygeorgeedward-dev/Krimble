/*
 *  SPDX-FileCopyrightText: 2024 Maciej Jesionowski <yavnrh@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_GAP_MAP_H
#define __KIS_GAP_MAP_H

#include <kis_types.h>
#include <KoAlwaysInline.h>
#include <kis_shared.h>
#include <QRect>
#include <kis_paint_device.h>
#include <kis_random_accessor_ng.h>

#define KIS_GAP_MAP_MEASURE_ELAPSED_TIME 0

#define KIS_GAP_MAP_DEBUG_LOGGING_AND_ASSERTS 0

class KisTileOptimizedAccessor
{
public:
    KisTileOptimizedAccessor(KisPaintDeviceSP& paintDevice)
        : m_pixelSize(paintDevice->pixelSize())
        , m_accessor(paintDevice->createRandomAccessorNG())
        , m_tile(-1, -1)
        , m_tileRawData(nullptr)
    {
    }

    quint8* tileRawData(int tileX, int tileY)
    {
        if ((m_tile.x() != tileX) || (m_tile.y() != tileY)) {
            m_accessor->moveTo(tileX * TileSize, tileY * TileSize);
            m_tileRawData = m_accessor->rawData();

            m_tile.setX(tileX);
            m_tile.setY(tileY);

#if KIS_GAP_MAP_DEBUG_LOGGING_AND_ASSERTS
            KIS_ASSERT(m_accessor->numContiguousRows(tileY * TileSize) == TileSize);
            KIS_ASSERT(m_accessor->numContiguousColumns(tileX * TileSize) == TileSize);
#endif
        }

        return m_tileRawData;
    }

    ALWAYS_INLINE quint8* rawData(int x, int y)
    {
        const int tileX = x / TileSize;
        const int tileY = y / TileSize;
        const int localX = x & (TileSize - 1);
        const int localY = y & (TileSize - 1);

        m_tileRawData = tileRawData(tileX, tileY);

        return m_tileRawData + m_pixelSize * (localX + TileSize * localY);
    }

private:
    static constexpr int TileSize = 64;

    const quint32 m_pixelSize;

    KisRandomAccessorSP m_accessor;
    QPoint m_tile;
    quint8* m_tileRawData;
};

class KRITAIMAGE_EXPORT KisGapMap : public KisShared
{
public:
    static constexpr quint16 DISTANCE_INFINITE = UINT16_MAX;

    typedef std::function<bool(KisPaintDevice* devicePtr, const QRect& rect)> FillOpacityFunc;

    KisGapMap(int gapSize,
              const QRect& mapBounds,
              const FillOpacityFunc& fillOpacityFunc);

    ALWAYS_INLINE quint16 distance(int x, int y)
    {
        if (isDistanceAvailable(x, y)) {
            return dataPtr(x, y)->distance;
        } else {
            return lazyDistance(x, y);
        }
    }

    ALWAYS_INLINE int gapSize() const
    {
        return m_gapSize;
    }

#if KIS_GAP_MAP_MEASURE_ELAPSED_TIME
public:
    quint64 opacityElapsedMillis() const
    {
        return m_opacityElapsedNanos / 1000000ull;
    }

    quint64 distanceElapsedMillis() const
    {
        return m_distanceElapsedNanos / 1000000ull;
    }

private:
    quint64 m_opacityElapsedNanos = 0;
    quint64 m_distanceElapsedNanos = 0;
#endif

private:
    Q_DISABLE_COPY(KisGapMap);

    static constexpr int TileSize = 64;

    typedef quint8 TileFlags;
    enum TileFlagBits
    {
        TILE_DISTANCE_LOADED     = 0x1,
        TILE_OPACITY_LOADED      = 0x2,
        TILE_HAS_OPAQUE_PIXELS   = 0x4,
    };

    struct Data
    {
        quint16     distance;
        quint8      opacity;
        TileFlags   flags;
    };
    static_assert(sizeof(Data) == sizeof(quint32));

    void loadOpacityTiles(const QRect& tileRect);
    void loadDistanceTile(const QPoint& tile, const QRect& nearbyTilesRect, int guardBand);
    void distanceSearchRowInnerLoop(bool boundsCheck, int y, int x1, int x2);
    quint16 lazyDistance(int x, int y);

    template<bool BoundsCheck, typename CoordinateTransform>
    void gapDistanceSearch(int x, int y, CoordinateTransform op);

    template<bool BoundsCheck> ALWAYS_INLINE bool isOpaque(int x, int y);
    template<bool BoundsCheck> ALWAYS_INLINE bool isOpaque(const QPoint& p);
    void updateDistance(const QPoint& globalPosition, quint16 newDistance);

    ALWAYS_INLINE bool isDistanceAvailable(int x, int y)
    {
        return (*tileFlagsPtr(x / TileSize, y / TileSize) & TILE_DISTANCE_LOADED) != 0;
    }

    ALWAYS_INLINE Data* dataPtr(int x, int y)
    {
        return reinterpret_cast<Data*>(m_accessor->rawData(x, y));
    }

    ALWAYS_INLINE TileFlags* tileFlagsPtr(int tileX, int tileY)
    {
        return reinterpret_cast<TileFlags*>(
            m_accessor->tileRawData(tileX, tileY) + offsetof(Data, flags));
    }

    const int m_gapSize;
    const QSize m_size;
    const QSize m_numTiles;
    const FillOpacityFunc m_fillOpacityFunc;

    QPoint m_tilePosition;
    Data* m_tileDataPtr;

    KisPaintDeviceSP m_deviceSp;
    std::unique_ptr<KisTileOptimizedAccessor> m_accessor;
};

#endif
