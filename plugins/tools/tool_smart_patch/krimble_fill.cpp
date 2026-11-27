// SPDX-License-Identifier: GPL-2.0-or-later
#include "krimble_fill.h"
#include "krimble_inpaint.h"

#include <KSharedConfig>
#include <KConfigGroup>
#include <kis_paint_device.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <kis_iterator_ng.h>
#include <QImage>
#include <QMutex>
#include <vector>
#include <cstring>
#include <new>

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
static bool say(const QString &status)
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
    maskDev->readBytes(maskBytes.data(), roi);
    std::vector<uint8_t> hole(size_t(w) * h);
    size_t holeCount = 0;
    for (size_t i = 0; i < hole.size(); ++i) { hole[i] = maskBytes[i] > 127 ? 1 : 0; holeCount += hole[i]; }
    if (holeCount == 0) return say(QStringLiteral("nothing selected"));

    std::vector<uint8_t> filled(size_t(w) * h, 0);
    KrimbleInpaint::Options options;
    bool ok = false;
    try {
        ok = (method == Smooth)
            ? KrimbleInpaint::inpaintLegacy(pixels.data(), hole.data(), w, h, 4, options, filled.data())
            : KrimbleInpaint::inpaint(pixels.data(), hole.data(), w, h, 4, options, filled.data());
    } catch (const std::bad_alloc &) {
        ok = false;
        return say(QStringLiteral("out of memory"));
    }
    if (!ok) return say(QStringLiteral("engine could not fill"));
    size_t filledCount = 0;
    for (uint8_t v : filled) filledCount += v ? 1 : 0;
    if (filledCount == 0) return say(QStringLiteral("nothing filled"));

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
