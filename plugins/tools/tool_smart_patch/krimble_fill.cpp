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
#include <QMutex>   // Krimble 2026-10-20 (2nd): the status of the last fill
#include <vector>
#include <cstring>
#include <new>   // Krimble 2026-10-20: std::bad_alloc

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

static QString s_lastStatus;
static QMutex s_statusMutex;
static bool say(const QString &status)   // Krimble 2026-10-20 (2nd): remember what happened, and return false (every use of it is a "could not")
{
    QMutexLocker lock(&s_statusMutex);
    s_lastStatus = status;
    return false;
}

QString lastStatus()
{
    QMutexLocker lock(&s_statusMutex);
    return s_lastStatus;
}

bool fill(KisPaintDeviceSP imageDev, KisPaintDeviceSP maskDev, const QRect &bounds, int method)
{
    if (method != Sharp && method != Smooth) return say(QStringLiteral("Classic method chosen"));
    const QRect maskRect = maskDev->exactBounds();
    if (maskRect.isEmpty()) return say(QStringLiteral("nothing selected"));
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
    // const qint64 budget = 3000000;
    // Krimble 2026-10-20: George (1.0.40): "Content-Aware fill does nothing now." (1.0.39 at least blurred.) The engine, on the bench, fills exactly as the
    // app calls it, with 1 or 4 threads; on the phone, an area of 3 MP needs about 450 MB at the peak (the pyramid, the matches, the colour shifts, the
    // window sums, the seam correction), and a failed allocation inside the fill's job may end the job without a word. The area is now at most 1.5 MP
    // (about 230 MB), and an allocation failure falls back to Krita's fill (see the try below) instead of doing nothing.
    // const qint64 budget = 1500000;
    // Krimble 2026-10-20 (2nd): George asked for the leaner storage ("Yes."): the engine now peaks at about 170 MB for a 2.9 MP area (tested, was 425 MB),
    // so the area goes back to 3 MP.
    const qint64 budget = 3000000;
    int margin = qMax(64, 2 * holeShort + 24);
    QRect roi = maskRect.adjusted(-margin, -margin, margin, margin) & bounds;
    while (margin > 16 && qint64(roi.width()) * roi.height() > budget) {
        margin = margin * 3 / 4;
        roi = maskRect.adjusted(-margin, -margin, margin, margin) & bounds;
    }
    if (roi.width() < 24 || roi.height() < 24) return say(QStringLiteral("area too small"));
    if (qint64(roi.width()) * roi.height() > budget) return say(QStringLiteral("area too big"));

    const int w = roi.width(), h = roi.height();
    const KoColorSpace *rgb8 = KoColorSpaceRegistry::instance()->rgb8();
    QImage img = imageDev->convertToQImage(rgb8->profile(), roi.x(), roi.y(), w, h).convertToFormat(QImage::Format_ARGB32);
    if (img.isNull()) return say(QStringLiteral("picture could not be read"));

    std::vector<uint8_t> pixels(size_t(w) * h * 4);
    for (int y = 0; y < h; ++y) std::memcpy(&pixels[size_t(y) * w * 4], img.constScanLine(y), size_t(w) * 4);

    std::vector<uint8_t> maskBytes(size_t(w) * h);
    maskDev->readBytes(maskBytes.data(), roi);                            // the selection is an 8-bit alpha device: one byte per pixel
    std::vector<uint8_t> hole(size_t(w) * h);
    size_t holeCount = 0;
    for (size_t i = 0; i < hole.size(); ++i) { hole[i] = maskBytes[i] > 127 ? 1 : 0; holeCount += hole[i]; }
    if (holeCount == 0) return say(QStringLiteral("nothing selected"));

    std::vector<uint8_t> filled(size_t(w) * h, 0);
    KrimbleInpaint::Options options;
    // const bool ok = (method == Smooth)
    //     ? KrimbleInpaint::inpaintLegacy(pixels.data(), hole.data(), w, h, 4, options, filled.data())
    //     : KrimbleInpaint::inpaint(pixels.data(), hole.data(), w, h, 4, options, filled.data());
    bool ok = false;
    try {
        ok = (method == Smooth)
            ? KrimbleInpaint::inpaintLegacy(pixels.data(), hole.data(), w, h, 4, options, filled.data())
            : KrimbleInpaint::inpaint(pixels.data(), hole.data(), w, h, 4, options, filled.data());
    } catch (const std::bad_alloc &) {
        ok = false;   // Krimble 2026-10-20: out of memory: Krita's fill instead
        return say(QStringLiteral("out of memory"));
    }
    if (!ok) return say(QStringLiteral("engine could not fill"));
    // Krimble 2026-10-20: nothing filled (should not happen): Krita's fill instead, rather than doing nothing
    size_t filledCount = 0;
    for (uint8_t v : filled) filledCount += v ? 1 : 0;
    if (filledCount == 0) return say(QStringLiteral("nothing filled"));

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
    {
        QMutexLocker lock(&s_statusMutex);
        s_lastStatus = (method == Smooth) ? QStringLiteral("Smooth engine") : QStringLiteral("Sharp engine");
    }
    return true;
}

}
