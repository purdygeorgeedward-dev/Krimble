// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-10-19: moved here from content_aware_fill_plugin.cpp (krimbleContentAwareFill, 2026-10-18 and 19) so that the healing brush can use it too,
// with a choice of method (George: "Make the different types options in the tool options panel.").
#include "krimble_fill.h"
#include "krimble_inpaint.h"

#include <KSharedConfig>
#include <KConfigGroup>
#include <kis_paint_device.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <kis_iterator_ng.h>
#include <QImage>
#include <vector>
#include <cstring>

namespace KrimbleFill {

int savedMethod()
{
    const int m = KSharedConfig::openConfig()->group("KrimbleContentAwareFill").readEntry("method", int(Sharp));
    return (m >= Sharp && m <= Classic) ? m : int(Sharp);
}

void saveMethod(int method)
{
    KConfigGroup group = KSharedConfig::openConfig()->group("KrimbleContentAwareFill");
    group.writeEntry("method", method);
    group.sync();
}

bool fill(KisPaintDeviceSP imageDev, KisPaintDeviceSP maskDev, const QRect &bounds, int method)
{
    if (method != Sharp && method != Smooth) return false;
    const QRect maskRect = maskDev->exactBounds();
    if (maskRect.isEmpty()) return false;
    const int holeShort = qMin(maskRect.width(), maskRect.height());
    // const int margin = qMax(64, 2 * holeShort + 24);
    // const QRect roi = maskRect.adjusted(-margin, -margin, margin, margin) & bounds;
    // if (roi.width() < 24 || roi.height() < 24) return false;
    // if (qint64(roi.width()) * roi.height() > 9000000) return false;      // the memory of the engine is about 70 bytes per pixel of this area
    // Krimble 2026-10-19: George (1.0.39): Content-Aware Fill still "a blurry mess". On a phone photo (12 MP and more) the area above was often over the 9 MP limit, and then
    // Krita's old Smart Patch fill ran instead (the blur). Now the area around the hole is made smaller until it fits a budget (4 MP: the new engine needs about 50 bytes
    // per pixel of it, about 200 MB), so the new engine runs for any hole whose own box fits; only a hole bigger than that still falls back.
    // const qint64 budget = 4000000;
    // Krimble 2026-10-19 (2nd): 3 MP: the Photoshop-style engine keeps a little more per pixel (the transform, the colour shift, the window sums: about 75 bytes)
    const qint64 budget = 3000000;
    int margin = qMax(64, 2 * holeShort + 24);
    QRect roi = maskRect.adjusted(-margin, -margin, margin, margin) & bounds;
    while (margin > 16 && qint64(roi.width()) * roi.height() > budget) {
        margin = margin * 3 / 4;
        roi = maskRect.adjusted(-margin, -margin, margin, margin) & bounds;
    }
    if (roi.width() < 24 || roi.height() < 24) return false;
    if (qint64(roi.width()) * roi.height() > budget) return false;

    const int w = roi.width(), h = roi.height();
    const KoColorSpace *rgb8 = KoColorSpaceRegistry::instance()->rgb8();
    QImage img = imageDev->convertToQImage(rgb8->profile(), roi.x(), roi.y(), w, h).convertToFormat(QImage::Format_ARGB32);
    if (img.isNull()) return false;

    std::vector<uint8_t> pixels(size_t(w) * h * 4);
    for (int y = 0; y < h; ++y) std::memcpy(&pixels[size_t(y) * w * 4], img.constScanLine(y), size_t(w) * 4);

    std::vector<uint8_t> maskBytes(size_t(w) * h);
    maskDev->readBytes(maskBytes.data(), roi);                            // the selection is an 8-bit alpha device: one byte per pixel
    std::vector<uint8_t> hole(size_t(w) * h);
    size_t holeCount = 0;
    for (size_t i = 0; i < hole.size(); ++i) { hole[i] = maskBytes[i] > 127 ? 1 : 0; holeCount += hole[i]; }
    if (holeCount == 0) return false;

    std::vector<uint8_t> filled(size_t(w) * h, 0);
    KrimbleInpaint::Options options;
    const bool ok = (method == Smooth)
        ? KrimbleInpaint::inpaintLegacy(pixels.data(), hole.data(), w, h, 4, options, filled.data())
        : KrimbleInpaint::inpaint(pixels.data(), hole.data(), w, h, 4, options, filled.data());
    if (!ok) return false;

    // the result, in the colour space of the layer, and only the replaced pixels copied
    QImage out(w, h, QImage::Format_ARGB32);
    for (int y = 0; y < h; ++y) std::memcpy(out.scanLine(y), &pixels[size_t(y) * w * 4], size_t(w) * 4);
    KisPaintDeviceSP result = new KisPaintDevice(imageDev->colorSpace());
    result->convertFromQImage(out, rgb8->profile(), roi.x(), roi.y());
    const int pixelSize = imageDev->pixelSize();
    for (int y = 0; y < h; ++y) {
        KisHLineIteratorSP dst = imageDev->createHLineIteratorNG(roi.x(), roi.y() + y, w);
        KisHLineConstIteratorSP src = result->createHLineConstIteratorNG(roi.x(), roi.y() + y, w);
        int x = 0;
        do {
            if (filled[size_t(y) * w + x]) std::memcpy(dst->rawData(), src->rawDataConst(), size_t(pixelSize));
            ++x;
        } while (dst->nextPixel() && src->nextPixel());
    }
    return true;
}

}
