/*
 * Krimble: Auto Tone, Auto Contrast and Auto Color, written new from published descriptions
 * (research: KRIMBLE_RESEARCH_auto_tone_color.md).
 *
 * - Auto Tone: each of R, G, B stretched on its own between its 0.1% darkest and 0.1% lightest values
 *   (Simplest Color Balance, Limare et al., IPOL 2011). Can remove a color cast.
 * - Auto Contrast: one stretch for R, G and B together (the 0.1% tails of all three channels taken as one):
 *   more contrast, no color shift.
 * - Auto Color: the average of the darkest 0.5% of the pixels (by brightness) becomes black and the average
 *   of the lightest 0.5% becomes white, channel by channel (the dark and light colors); then the near-neutral
 *   midtones are made neutral with a gamma for each channel, aimed at their own average brightness so the
 *   overall tone does not change; then a gentle saturation lift that keeps brightness and hue and lifts dull
 *   colors more than bright ones.
 *
 * Every curve is a curve per channel (a lookup table). Fully transparent pixels are not counted.
 * RGB pictures only; other color models are not changed.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KRIMBLE_AUTO_ADJUST_H
#define KRIMBLE_AUTO_ADJUST_H

#include <QVector>
#include <QtMath>
#include <cmath>

#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoColorModelStandardIds.h>
#include <KoColorConversionTransformation.h>
#include <KisSequentialIteratorProgress.h>
#include <kis_sequential_iterator.h>
#include <kis_paint_device.h>
#include <KoUpdater.h>

namespace KrimbleAutoAdjust {

enum Mode { Tone, Contrast, Color };

static const int Bins = 1024;   // 16-bit values / 64

struct Pixel16 { quint16 b, g, r, a; };   // the 16-bit RGBA layout of Krita (blue first)

inline qreal lum(qreal r, qreal g, qreal b) { return 0.2126 * r + 0.7152 * g + 0.0722 * b; }

// the value (0..1) under which 'fraction' of the counted values lie
inline qreal lowCut(const QVector<quint64> &h, quint64 total, qreal fraction)
{
    const quint64 limit = quint64(total * fraction);
    quint64 sum = 0;
    for (int i = 0; i < Bins; ++i) { sum += h[i]; if (sum > limit) return qreal(i) / (Bins - 1); }
    return 1.0;
}
inline qreal highCut(const QVector<quint64> &h, quint64 total, qreal fraction)
{
    const quint64 limit = quint64(total * fraction);
    quint64 sum = 0;
    for (int i = Bins - 1; i >= 0; --i) { sum += h[i]; if (sum > limit) return qreal(i) / (Bins - 1); }
    return 0.0;
}

template <class F>
inline void forEachPixel16(KisPaintDeviceSP device, const QRect &rect, const KoColorSpace *cs16, F f)
{
    const KoColorSpace *cs = device->colorSpace();
    QVector<Pixel16> buf;
    KisSequentialConstIterator it(device, rect);
    int n = it.nConseqPixels();
    while (it.nextPixels(n)) {
        n = it.nConseqPixels();
        if (buf.size() < n) buf.resize(n);
        cs->convertPixelsTo(it.rawDataConst(), reinterpret_cast<quint8*>(buf.data()), cs16, n,
                            KoColorConversionTransformation::internalRenderingIntent(),
                            KoColorConversionTransformation::internalConversionFlags());
        for (int i = 0; i < n; ++i) f(buf[i]);
    }
}

inline void apply(KisPaintDeviceSP device, const QRect &rect, Mode mode, KoUpdater *progress)
{
    const KoColorSpace *cs = device->colorSpace();
    if (cs->colorModelId() != RGBAColorModelID) return;
    const KoColorSpace *cs16 = KoColorSpaceRegistry::instance()->colorSpace(RGBAColorModelID.id(), Integer16BitsColorDepthID.id(), cs->profile());
    if (!cs16) return;

    // 1. histograms
    QVector<quint64> hist[3] = {QVector<quint64>(Bins, 0), QVector<quint64>(Bins, 0), QVector<quint64>(Bins, 0)};
    QVector<quint64> histAll(Bins, 0), histLum(Bins, 0);
    QVector<double> lumSum[3] = {QVector<double>(Bins, 0), QVector<double>(Bins, 0), QVector<double>(Bins, 0)};
    quint64 total = 0;
    forEachPixel16(device, rect, cs16, [&](const Pixel16 &p) {
        if (p.a == 0) return;
        const int r = p.r >> 6, g = p.g >> 6, b = p.b >> 6;
        hist[0][r]++; hist[1][g]++; hist[2][b]++;
        histAll[r]++; histAll[g]++; histAll[b]++;
        const qreal fr = p.r / 65535.0, fg = p.g / 65535.0, fb = p.b / 65535.0;
        const int l = qBound(0, int(lum(fr, fg, fb) * (Bins - 1) + 0.5), Bins - 1);
        histLum[l]++;
        lumSum[0][l] += fr; lumSum[1][l] += fg; lumSum[2][l] += fb;
        total++;
    });
    if (total == 0) return;

    // 2. black and white point of each channel
    qreal black[3], white[3], power[3] = {1.0, 1.0, 1.0};
    for (int c = 0; c < 3; ++c) {
        if (mode == Contrast) {
            black[c] = lowCut(histAll, total * 3, 0.001);
            white[c] = highCut(histAll, total * 3, 0.001);
        } else {
            black[c] = lowCut(hist[c], total, 0.001);
            white[c] = highCut(hist[c], total, 0.001);
        }
    }
    if (mode == Color) {
        // the dark and light colors: the average color of the darkest and of the lightest 0.5% of the pixels
        const quint64 want = qMax<quint64>(1, quint64(total * 0.005));
        double dark[3] = {0, 0, 0}, light[3] = {0, 0, 0};
        quint64 n = 0;
        for (int i = 0; i < Bins && n < want; ++i) { for (int c = 0; c < 3; ++c) dark[c] += lumSum[c][i]; n += histLum[i]; }
        const quint64 nDark = n;
        n = 0;
        for (int i = Bins - 1; i >= 0 && n < want; --i) { for (int c = 0; c < 3; ++c) light[c] += lumSum[c][i]; n += histLum[i]; }
        const quint64 nLight = n;
        for (int c = 0; c < 3; ++c) {
            const qreal d = nDark ? dark[c] / nDark : black[c];
            const qreal l = nLight ? light[c] / nLight : white[c];
            if (l - d > 0.05) { black[c] = d; white[c] = l; }   // otherwise the 0.1% points of the channel stay
        }
    }
    for (int c = 0; c < 3; ++c) {
        if (white[c] - black[c] < 1.0 / 64) { black[c] = 0.0; white[c] = 1.0; }   // a flat channel is left alone
    }
    auto mapped = [&](int c, qreal v) { return qBound(0.0, (v - black[c]) / (white[c] - black[c]), 1.0); };

    // 3. Auto Color: the near-neutral midtones made neutral, at their own average brightness
    if (mode == Color) {
        double sum[3] = {0, 0, 0}, sumAll[3] = {0, 0, 0};
        quint64 n = 0, nAll = 0;
        forEachPixel16(device, rect, cs16, [&](const Pixel16 &p) {
            if (p.a == 0) return;
            const qreal v[3] = {mapped(0, p.r / 65535.0), mapped(1, p.g / 65535.0), mapped(2, p.b / 65535.0)};
            const qreal l = lum(v[0], v[1], v[2]);
            if (l < 0.25 || l > 0.75) return;
            for (int c = 0; c < 3; ++c) sumAll[c] += v[c];
            nAll++;
            const qreal chroma = qMax(v[0], qMax(v[1], v[2])) - qMin(v[0], qMin(v[1], v[2]));
            if (chroma > 0.2) return;
            for (int c = 0; c < 3; ++c) sum[c] += v[c];
            n++;
        });
        const bool enough = n >= qMax<quint64>(16, total / 200);
        const double *s = enough ? sum : sumAll;
        const quint64 cnt = enough ? n : nAll;
        if (cnt > 0) {
            const qreal m[3] = {qreal(s[0] / cnt), qreal(s[1] / cnt), qreal(s[2] / cnt)};
            const qreal target = (m[0] + m[1] + m[2]) / 3.0;
            for (int c = 0; c < 3; ++c) {
                if (m[c] > 0.01 && m[c] < 0.99 && target > 0.01 && target < 0.99) {
                    power[c] = qBound(0.5, std::log(target) / std::log(m[c]), 2.0);
                }
            }
        }
    }

    // 4. the curves
    QVector<quint16> lut[3];
    for (int c = 0; c < 3; ++c) {
        lut[c].resize(65536);
        for (int v = 0; v < 65536; ++v) {
            const qreal y = std::pow(mapped(c, v / 65535.0), power[c]);
            lut[c][v] = quint16(qBound(0.0, y * 65535.0 + 0.5, 65535.0));
        }
    }

    // 5. apply (with the saturation lift for Auto Color)
    QVector<Pixel16> buf;
    KisSequentialIteratorProgress it(device, rect, progress);
    int n = it.nConseqPixels();
    while (it.nextPixels(n)) {
        n = it.nConseqPixels();
        if (buf.size() < n) buf.resize(n);
        cs->convertPixelsTo(it.oldRawData(), reinterpret_cast<quint8*>(buf.data()), cs16, n,
                            KoColorConversionTransformation::internalRenderingIntent(),
                            KoColorConversionTransformation::internalConversionFlags());
        for (int i = 0; i < n; ++i) {
            Pixel16 &p = buf[i];
            p.r = lut[0][p.r]; p.g = lut[1][p.g]; p.b = lut[2][p.b];
            if (mode == Color) {
                const qreal r = p.r / 65535.0, g = p.g / 65535.0, b = p.b / 65535.0;
                const qreal L = lum(r, g, b);
                const qreal sat = qMax(r, qMax(g, b)) - qMin(r, qMin(g, b));
                const qreal f = 1.0 + 0.25 * (1.0 - sat);   // dull colors get more, bright colors less
                p.r = quint16(qBound(0.0, (L + (r - L) * f) * 65535.0 + 0.5, 65535.0));
                p.g = quint16(qBound(0.0, (L + (g - L) * f) * 65535.0 + 0.5, 65535.0));
                p.b = quint16(qBound(0.0, (L + (b - L) * f) * 65535.0 + 0.5, 65535.0));
            }
        }
        cs16->convertPixelsTo(reinterpret_cast<const quint8*>(buf.data()), it.rawData(), cs, n,
                              KoColorConversionTransformation::internalRenderingIntent(),
                              KoColorConversionTransformation::internalConversionFlags());
    }
}

} // namespace KrimbleAutoAdjust

#endif
