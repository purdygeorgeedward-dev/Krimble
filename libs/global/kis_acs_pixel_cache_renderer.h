/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_ACS_PIXEL_CACHE_RENDERER_H
#define __KIS_ACS_PIXEL_CACHE_RENDERER_H

#include "KoColorSpace.h"
#include "kis_paint_device.h"
#include "kis_iterator_ng.h"
#include "kis_display_color_converter.h"

namespace Acs {

    class PixelCacheRenderer {
    public:
        template <class Sampler>
        static void render(Sampler *sampler,
                           const KisDisplayColorConverter *converter,
                           const QRect &sampleRect,
                           KisPaintDeviceSP &realPixelCache,
                           QImage &pixelCache,
                           QPoint &pixelCacheOffset,
                           qreal devicePixelRatioF)
            {
                const KoColorSpace *cacheColorSpace = converter->paintingColorSpace();
                const int pixelSize = cacheColorSpace->pixelSize();

                if (!realPixelCache || realPixelCache->colorSpace() != cacheColorSpace) {
                    realPixelCache = new KisPaintDevice(cacheColorSpace);
                }

                KoColor color;

                QRect sampleRectHighDPI = QRect(sampleRect.topLeft(), sampleRect.size()*devicePixelRatioF);
                KisSequentialIterator it(realPixelCache, sampleRectHighDPI);

                while (it.nextPixel()) {
                    color = sampler->colorAt(it.x()/devicePixelRatioF, it.y()/devicePixelRatioF);
                    memcpy(it.rawData(), color.data(), pixelSize);
                }

                pixelCache = converter->toQImage(realPixelCache);
                pixelCache.setDevicePixelRatio(devicePixelRatioF);
                pixelCacheOffset = realPixelCache->exactBounds().topLeft()/devicePixelRatioF - sampleRect.topLeft();
        }
    };
}

#endif
