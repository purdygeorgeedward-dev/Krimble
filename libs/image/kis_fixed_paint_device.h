/*
 *  SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_FIXED_PAINT_DEVICE_H
#define KIS_FIXED_PAINT_DEVICE_H

#include <kritaimage_export.h>
#include <KoColorSpace.h>
#include "kis_shared.h"
#include <kis_shared_ptr.h>

#include <QRect>
#include <QImage>
#include "KisOptimizedByteArray.h"

class KoColor;

class KRITAIMAGE_EXPORT KisFixedPaintDevice : public KisShared
{

public:

    KisFixedPaintDevice(const KoColorSpace* colorSpace,
                        KisOptimizedByteArray::MemoryAllocatorSP allocator = KisOptimizedByteArray::MemoryAllocatorSP());
    virtual ~KisFixedPaintDevice();

    KisFixedPaintDevice(const KisFixedPaintDevice& rhs);

    KisFixedPaintDevice& operator=(const KisFixedPaintDevice& rhs);

    void setRect(const QRect& rc);

    void setColorSpace(const KoColorSpace *cs);

    QRect bounds() const;

    int allocatedPixels() const;

    quint32 pixelSize() const;

    const KoColorSpace* colorSpace() const {
        return m_colorSpace;
    }

    bool initialize(quint8 defaultValue = 0);

    void reallocateBufferWithoutInitialization();

    void lazyGrowBufferWithoutInitialization();

    quint8* data();

    const quint8* constData() const;

    quint8* data() const;

    void readBytes(quint8 * dstData, qint32 x, qint32 y, qint32 w, qint32 h) const;

    void convertTo(const KoColorSpace * dstColorSpace = 0,
                   KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::internalRenderingIntent(),
                   KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::internalConversionFlags());

    void setProfile(const KoColorProfile *profile);

    virtual void convertFromQImage(const QImage& image, const QString &srcProfileName);

    virtual QImage convertToQImage(const KoColorProfile *dstProfile, qint32 x, qint32 y, qint32 w, qint32 h,
                                   KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::internalRenderingIntent(),
                                   KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::internalConversionFlags()) const;

    virtual QImage convertToQImage(const KoColorProfile *dstProfile,
                                   KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::internalRenderingIntent(),
                                   KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::internalConversionFlags()) const;

    void clear(const QRect & rc);

    void fill(qint32 x, qint32 y, qint32 w, qint32 h, const quint8 *fillPixel);

    void fill(const QRect &rc, const KoColor &color);

    void mirror(bool horizontal, bool vertical);

private:

    const KoColorSpace* m_colorSpace;
    QRect m_bounds;
    KisOptimizedByteArray m_data;
};

#endif
