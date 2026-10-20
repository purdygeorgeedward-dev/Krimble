// SPDX-License-Identifier: GPL-2.0-or-later
#include "krimble_inpaint.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <random>
#include <thread>
#include <functional>
#include <vector>

namespace KrimbleInpaint {
namespace {

struct Level {
    int w = 0, h = 0, c = 3;
    std::vector<float> img;       // the picture: known pixels are the truth, hole pixels are the current estimate
    std::vector<uint8_t> hole;    // 1 = hole
    std::vector<int> sat;         // summed-area table of the hole, (w+1) * (h+1)
    std::vector<int> nx, ny;      // the nearest-neighbour field: for each pixel, the centre of the source patch
    std::vector<float> dist;      // the mean squared difference of the match
    std::vector<uint8_t> target;  // 1 = this pixel's patch overlaps the hole: it has a match to find
    std::vector<int> sources;     // centres of the valid source patches (fully inside the picture and fully known), as y * w + x
    int sx0 = 0, sy0 = 0, sx1 = 0, sy1 = 0;   // the window the sources may be taken from (around the hole): inclusive centre coordinates
    std::vector<float> lp, hp;    // the picture split in two: the smooth colour (a box blur) and the detail (the picture minus the smooth colour)
    std::vector<float> guide;     // the smooth colour of the hole: what the surroundings of the hole say it should be (re-solved at every level)
};

// runs f(a, b) on `threads` pieces of [0, n)
template <class F> void parallelFor(int n, int threads, F f)
{
    if (threads <= 1 || n < 2 * threads) { f(0, n); return; }
    std::vector<std::thread> pool;
    const int chunk = (n + threads - 1) / threads;
    for (int t = 0; t < threads; ++t) {
        const int a = t * chunk, b = std::min(n, a + chunk);
        if (a >= b) break;
        pool.emplace_back([&f, a, b]() { f(a, b); });
    }
    for (auto &th : pool) th.join();
}

inline int clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

void buildSat(Level &L)
{
    L.sat.assign(size_t(L.w + 1) * (L.h + 1), 0);
    for (int y = 0; y < L.h; ++y) {
        int row = 0;
        for (int x = 0; x < L.w; ++x) {
            row += L.hole[size_t(y) * L.w + x];
            L.sat[size_t(y + 1) * (L.w + 1) + (x + 1)] = L.sat[size_t(y) * (L.w + 1) + (x + 1)] + row;
        }
    }
}

inline int holeCount(const Level &L, int x0, int y0, int x1, int y1)   // inclusive rectangle, inside the picture
{
    const int W = L.w + 1;
    return L.sat[size_t(y1 + 1) * W + (x1 + 1)] - L.sat[size_t(y0) * W + (x1 + 1)] - L.sat[size_t(y1 + 1) * W + x0] + L.sat[size_t(y0) * W + x0];
}

inline bool validSource(const Level &L, int x, int y, int r)
{
    if (x < L.sx0 || y < L.sy0 || x > L.sx1 || y > L.sy1) return false;
    if (x - r < 0 || y - r < 0 || x + r >= L.w || y + r >= L.h) return false;
    return holeCount(L, x - r, y - r, x + r, y + r) == 0;
}

// squared difference between the patch around (x, y) (clamped at the borders) and the source patch around (sx, sy); stops when it passes `cutoff`
static float g_colorWeight = 0.15f;   // how much the difference of the smooth colours of two patches counts next to the difference of their detail
static float g_patchSigma = 1.6f;     // the weight of a pixel inside a voting patch falls off with its distance from the centre of the patch (in pixels)
static float g_gainMax = 3.0f;        // the detail of the hole may be raised at most this many times to match the detail around it
static float g_windowFactor = 2.0f;   // the source window reaches this many times the shorter side of the hole beyond the hole
static float g_coarseSharp = 2.0f;    // the sharpness of the vote on the coarse levels (the fine levels use Options::sharpness)
static int g_blurRadius = 3;          // the radius of the box blur that separates the smooth colour from the detail
static int g_relaxIters = 60;         // the smooth colour of the hole is relaxed this many times at every level

// the picture split in the smooth colour (a box blur of radius g_blurRadius) and the detail
void splitLowHigh(Level &L)
{
    const int w = L.w, h = L.h, c = L.c, R = g_blurRadius;
    L.lp.resize(L.img.size());
    L.hp.resize(L.img.size());
    std::vector<float> tmp(L.img.size());
    for (int y = 0; y < h; ++y) {
        for (int k = 0; k < c; ++k) {
            // horizontal running sum with the border repeated
            float sum = 0.0f;
            for (int dx = -R; dx <= R; ++dx) sum += L.img[(size_t(y) * w + clampi(dx, 0, w - 1)) * c + k];
            for (int x = 0; x < w; ++x) {
                tmp[(size_t(y) * w + x) * c + k] = sum / float(2 * R + 1);
                sum += L.img[(size_t(y) * w + clampi(x + R + 1, 0, w - 1)) * c + k] - L.img[(size_t(y) * w + clampi(x - R, 0, w - 1)) * c + k];
            }
        }
    }
    for (int x = 0; x < w; ++x) {
        for (int k = 0; k < c; ++k) {
            float sum = 0.0f;
            for (int dy = -R; dy <= R; ++dy) sum += tmp[(size_t(clampi(dy, 0, h - 1)) * w + x) * c + k];
            for (int y = 0; y < h; ++y) {
                const size_t i = (size_t(y) * w + x) * c + k;
                L.lp[i] = sum / float(2 * R + 1);
                L.hp[i] = L.img[i] - L.lp[i];
                sum += tmp[(size_t(clampi(y + R + 1, 0, h - 1)) * w + x) * c + k] - tmp[(size_t(clampi(y - R, 0, h - 1)) * w + x) * c + k];
            }
        }
    }
}

// Seamless correction (membrane interpolation, as in seamless cloning): the estimate that came from the level above is kept, with all its detail, but its mismatch with the
// known pixels at the edge of the hole is measured and spread smoothly into the hole (a Laplace solution), and subtracted. The colours at the edge are then right, and the
// middle of a big hole keeps what the coarser level made of it.
void correctBoundary(Level &L, int iterations)
{
    const int w = L.w, h = L.h, c = L.c;
    std::vector<float> corr(L.img.size(), 0.0f);
    std::vector<uint8_t> ring(L.hole.size(), 0);
    int minX = w, minY = h, maxX = -1, maxY = -1;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const size_t i = size_t(y) * w + x;
            if (!L.hole[i]) continue;
            minX = std::min(minX, x); maxX = std::max(maxX, x); minY = std::min(minY, y); maxY = std::max(maxY, y);
            float acc[4] = {0, 0, 0, 0}; int n = 0;
            const int nxs[4] = {x - 1, x + 1, x, x};
            const int nys[4] = {y, y, y - 1, y + 1};
            for (int q = 0; q < 4; ++q) {
                if (nxs[q] < 0 || nys[q] < 0 || nxs[q] >= w || nys[q] >= h) continue;
                const size_t j = size_t(nys[q]) * w + nxs[q];
                if (L.hole[j]) continue;
                for (int k = 0; k < c; ++k) acc[k] += L.img[j * c + k];
                ++n;
            }
            if (n) {
                ring[i] = 1;
                for (int k = 0; k < c; ++k) corr[i * c + k] = acc[k] / float(n) - L.img[i * c + k];
            }
        }
    }
    if (maxX < 0) return;
    for (int it = 0; it < iterations; ++it) {
        const bool fwd = (it & 1) == 0;
        for (int yy = minY; yy <= maxY; ++yy) {
            const int y = fwd ? yy : (maxY - (yy - minY));
            for (int xx = minX; xx <= maxX; ++xx) {
                const int x = fwd ? xx : (maxX - (xx - minX));
                const size_t i = size_t(y) * w + x;
                if (!L.hole[i] || ring[i]) continue;
                for (int k = 0; k < c; ++k) {
                    float acc = 0.0f; int n = 0;
                    if (x > 0) { acc += corr[(i - 1) * c + k]; ++n; }
                    if (x < w - 1) { acc += corr[(i + 1) * c + k]; ++n; }
                    if (y > 0) { acc += corr[(i - w) * c + k]; ++n; }
                    if (y < h - 1) { acc += corr[(i + w) * c + k]; ++n; }
                    const float old = corr[i * c + k];
                    corr[i * c + k] = old + 1.6f * (acc / float(n) - old);
                }
            }
        }
    }
    for (size_t i = 0; i < L.hole.size(); ++i)
        if (L.hole[i]) for (int k = 0; k < c; ++k) L.img[i * c + k] += corr[i * c + k];
}

#if 0   // replaced by correctBoundary(); kept for reference
// the hole is relaxed toward the smooth solution (Laplace) with the known pixels of THIS level as the edge (not used any more: correctBoundary keeps the detail)
void relaxHole(Level &L, int iterations)
{
    const int w = L.w, h = L.h, c = L.c;
    int minX = w, minY = h, maxX = -1, maxY = -1;
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) if (L.hole[size_t(y) * w + x]) { minX = std::min(minX, x); maxX = std::max(maxX, x); minY = std::min(minY, y); maxY = std::max(maxY, y); }
    if (maxX < 0) return;
    for (int it = 0; it < iterations; ++it) {
        const bool fwd = (it & 1) == 0;
        for (int yy = minY; yy <= maxY; ++yy) {
            const int y = fwd ? yy : (maxY - (yy - minY));
            for (int xx = minX; xx <= maxX; ++xx) {
                const int x = fwd ? xx : (maxX - (xx - minX));
                const size_t i = size_t(y) * w + x;
                if (!L.hole[i]) continue;
                for (int k = 0; k < c; ++k) {
                    float acc = 0.0f; int n = 0;
                    if (x > 0) { acc += L.img[(i - 1) * c + k]; ++n; }
                    if (x < w - 1) { acc += L.img[(i + 1) * c + k]; ++n; }
                    if (y > 0) { acc += L.img[(i - w) * c + k]; ++n; }
                    if (y < h - 1) { acc += L.img[(i + w) * c + k]; ++n; }
                    const float old = L.img[i * c + k];
                    L.img[i * c + k] = old + 1.6f * (acc / float(n) - old);
                }
            }
        }
    }
}
#endif

inline float patchDist(const Level &L, int x, int y, int sx, int sy, int r, float cutoff)
{
    (void)cutoff;
    // The difference of the DETAIL of the two patches (the picture minus its smooth colour), plus, with a small weight, the difference of their smooth colour at the centre. The detail
    // does not depend on the colour, so a patch of dark texture matches a patch of bright texture; the colour of the fill comes from the surroundings of the hole, not from the source.
    float ssd = 0.0f;
    const int c = L.c;
    for (int dy = -r; dy <= r; ++dy) {
        const int ty = clampi(y + dy, 0, L.h - 1);
        const float *a = &L.hp[(size_t(ty) * L.w) * c];
        const float *b = &L.hp[(size_t(sy + dy) * L.w) * c];
        for (int dx = -r; dx <= r; ++dx) {
            const int tx = clampi(x + dx, 0, L.w - 1);
            const float *pa = a + size_t(tx) * c;
            const float *pb = b + size_t(sx + dx) * c;
            for (int k = 0; k < c; ++k) {
                const float d = pa[k] - pb[k];
                ssd += d * d;
            }
        }
    }
    const float N = float((2 * r + 1) * (2 * r + 1));
    const float *la = &L.lp[(size_t(y) * L.w + x) * c];
    const float *lb = &L.lp[(size_t(sy) * L.w + sx) * c];
    for (int k = 0; k < c; ++k) { const float d = la[k] - lb[k]; ssd += g_colorWeight * N * d * d; }
    return ssd;
}


// the first fill of the smallest level: the hole is filled from its edge inward with the mean of the neighbours already filled
void diffuseFill(Level &L)
{
    std::vector<uint8_t> filled(L.hole.size());
    for (size_t i = 0; i < filled.size(); ++i) filled[i] = L.hole[i] ? 0 : 1;
    bool any = true;
    int guard = 0;
    while (any && guard++ < 10000) {
        any = false;
        std::vector<size_t> now;
        for (int y = 0; y < L.h; ++y) {
            for (int x = 0; x < L.w; ++x) {
                const size_t i = size_t(y) * L.w + x;
                if (filled[i]) continue;
                float acc[4] = {0, 0, 0, 0};
                int n = 0;
                const int nxs[4] = {x - 1, x + 1, x, x};
                const int nys[4] = {y, y, y - 1, y + 1};
                for (int q = 0; q < 4; ++q) {
                    if (nxs[q] < 0 || nys[q] < 0 || nxs[q] >= L.w || nys[q] >= L.h) continue;
                    const size_t j = size_t(nys[q]) * L.w + nxs[q];
                    if (!filled[j]) continue;
                    for (int k = 0; k < L.c; ++k) acc[k] += L.img[j * L.c + k];
                    ++n;
                }
                if (n) {
                    for (int k = 0; k < L.c; ++k) L.img[i * L.c + k] = acc[k] / n;
                    now.push_back(i);
                }
            }
        }
        for (size_t i : now) { filled[i] = 1; any = true; }
    }
}

#if 0   // exemplar-based first stage (Criminisi et al. 2004): tried, copies wrong chunks on big holes; kept for reference
// Exemplar-based filling (Criminisi, Perez, Toyama 2004): the hole is filled from its edge inward, one patch at a time. At each step the pixel of the edge with the highest priority
// (many known pixels around it, and an image edge that runs into the hole) is chosen, its patch is matched against the known picture using only the pixels that are known, and the
// unknown pixels are copied from the best match. Real pixels are copied, never averaged, so the result is sharp and the edges continue.
void exemplarFill(Level &L, int r)
{
    const int w = L.w, h = L.h, c = L.c;
    const size_t n = L.hole.size();
    std::vector<uint8_t> known(n);
    std::vector<float> conf(n);
    size_t remaining = 0;
    for (size_t i = 0; i < n; ++i) { known[i] = L.hole[i] ? 0 : 1; conf[i] = known[i] ? 1.0f : 0.0f; remaining += L.hole[i] ? 1 : 0; }
    auto lum = [&](size_t i) { return c >= 3 ? 0.299f * L.img[i * c] + 0.587f * L.img[i * c + 1] + 0.114f * L.img[i * c + 2] : L.img[i * c]; };
    auto knownAt = [&](int x, int y) { return (x < 0 || y < 0 || x >= w || y >= h) ? 1 : int(known[size_t(y) * w + x]); };
    int guard = 0;
    while (remaining > 0 && guard++ < int(n) + 10) {
        // the pixel of the edge with the highest priority
        float bestP = -1.0f; int bx = -1, by = -1;
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                const size_t i = size_t(y) * w + x;
                if (known[i]) continue;
                if (!(knownAt(x - 1, y) || knownAt(x + 1, y) || knownAt(x, y - 1) || knownAt(x, y + 1))) continue;   // not on the edge
                // confidence: the mean confidence of the patch
                float cs = 0.0f; int cn = 0;
                float gxBest = 0, gyBest = 0, gMag = 0;
                for (int dy = -r; dy <= r; ++dy) {
                    for (int dx = -r; dx <= r; ++dx) {
                        const int qx = x + dx, qy = y + dy;
                        if (qx < 1 || qy < 1 || qx >= w - 1 || qy >= h - 1) continue;
                        const size_t q = size_t(qy) * w + qx;
                        ++cn; cs += conf[q];
                        // the gradient where the pixel and its 8 neighbours are all known
                        if (!known[q]) continue;
                        bool all = true;
                        for (int ey = -1; ey <= 1 && all; ++ey) for (int ex = -1; ex <= 1; ++ex) if (!known[size_t(qy + ey) * w + (qx + ex)]) { all = false; break; }
                        if (!all) continue;
                        const float gx = lum(q + 1) - lum(q - 1), gy = lum(q + w) - lum(q - w);
                        const float m = gx * gx + gy * gy;
                        if (m > gMag) { gMag = m; gxBest = gx; gyBest = gy; }
                    }
                }
                const float C = cn ? cs / cn : 0.0f;
                // the direction of the edge of the hole (the normal): from the known mask
                float nxm = 0, nym = 0;
                for (int ey = -1; ey <= 1; ++ey) for (int ex = -1; ex <= 1; ++ex) { const float k = float(knownAt(x + ex, y + ey)); nxm += k * ex; nym += k * ey; }
                const float nl = std::sqrt(nxm * nxm + nym * nym);
                float D = 0.001f;
                if (nl > 0 && gMag > 0) {
                    // the isophote is perpendicular to the gradient; D = |isophote . normal|
                    const float ix = -gyBest, iy = gxBest, il = std::sqrt(ix * ix + iy * iy);
                    D = std::fabs((ix * nxm + iy * nym) / (il * nl)) * std::sqrt(gMag) / 255.0f + 0.001f;
                }
                static const float structureBase = getenv("KI_BASE") ? float(atof(getenv("KI_BASE"))) : 0.15f;   // temporary: for tests
                const float P = C * (structureBase + D);
                if (P > bestP) { bestP = P; bx = x; by = y; }
            }
        }
        if (bx < 0) break;
        // the known pixels of the patch (these decide the match)
        struct KP { int dx, dy; };
        std::vector<KP> kp;
        for (int dy = -r; dy <= r; ++dy)
            for (int dx = -r; dx <= r; ++dx) {
                const int qx = bx + dx, qy = by + dy;
                if (qx < 0 || qy < 0 || qx >= w || qy >= h) continue;
                if (known[size_t(qy) * w + qx]) kp.push_back({dx, dy});
            }
        // the best match among the sources
        float bestD = 1e30f; int bsx = -1, bsy = -1;
        for (int s : L.sources) {
            const int sx = s % w, sy = s / w;
            float sum = 0.0f;
            for (const KP &k : kp) {
                const float *a = &L.img[(size_t(by + k.dy) * w + (bx + k.dx)) * c];
                const float *b = &L.img[(size_t(sy + k.dy) * w + (sx + k.dx)) * c];
                for (int q = 0; q < c; ++q) { const float d = a[q] - b[q]; sum += d * d; }
                if (sum >= bestD) break;
            }
            if (sum < bestD) { bestD = sum; bsx = sx; bsy = sy; }
        }
        if (bsx < 0) break;
        const float Cp = [&]() { float cs = 0; int cn = 0; for (const KP &k : kp) { cs += conf[size_t(by + k.dy) * w + (bx + k.dx)]; ++cn; } return cn ? cs / float(cn) : 0.0f; }();
        for (int dy = -r; dy <= r; ++dy)
            for (int dx = -r; dx <= r; ++dx) {
                const int qx = bx + dx, qy = by + dy;
                if (qx < 0 || qy < 0 || qx >= w || qy >= h) continue;
                const size_t q = size_t(qy) * w + qx;
                if (known[q]) continue;
                for (int k = 0; k < c; ++k) L.img[q * c + k] = L.img[(size_t(bsy + dy) * w + (bsx + dx)) * c + k];
                known[q] = 1; conf[q] = Cp; --remaining;
            }
    }
}

#endif

// The smooth first guess: the hole is the solution of the Laplace equation with the known pixels around it as the boundary (every hole pixel is the mean of its four neighbours).
// That is what a stretched rubber sheet does: the colours at the nearest edges count the most, so a long thin hole is not averaged over its whole length.
void harmonicFill(Level &L)
{
    diffuseFill(L);   // a first guess to start from
    const int w = L.w, h = L.h, c = L.c;
    int minX = w, minY = h, maxX = -1, maxY = -1;
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) if (L.hole[size_t(y) * w + x]) { minX = std::min(minX, x); maxX = std::max(maxX, x); minY = std::min(minY, y); maxY = std::max(maxY, y); }
    if (maxX < 0) return;
    const int dim = std::max(maxX - minX + 1, maxY - minY + 1);
    const int iterations = std::min(6000, 20 + 3 * dim * dim);
    for (int it = 0; it < iterations; ++it) {
        float change = 0.0f;
        // Gauss-Seidel with over-relaxation, in both directions of the sweep
        const bool fwd = (it & 1) == 0;
        for (int yy = minY; yy <= maxY; ++yy) {
            const int y = fwd ? yy : (maxY - (yy - minY));
            for (int xx = minX; xx <= maxX; ++xx) {
                const int x = fwd ? xx : (maxX - (xx - minX));
                const size_t i = size_t(y) * w + x;
                if (!L.hole[i]) continue;
                for (int k = 0; k < c; ++k) {
                    float acc = 0.0f; int n = 0;
                    if (x > 0) { acc += L.img[(i - 1) * c + k]; ++n; }
                    if (x < w - 1) { acc += L.img[(i + 1) * c + k]; ++n; }
                    if (y > 0) { acc += L.img[(i - w) * c + k]; ++n; }
                    if (y < h - 1) { acc += L.img[(i + w) * c + k]; ++n; }
                    const float target = acc / float(n);
                    const float old = L.img[i * c + k];
                    const float v = old + 1.6f * (target - old);   // successive over-relaxation
                    L.img[i * c + k] = v;
                    change = std::max(change, std::fabs(v - old));
                }
            }
        }
        if (change < 0.02f) break;
    }
}

void prepareLevel(Level &L, int r, int margin)
{
    buildSat(L);
    // the window the sources are taken from: the box of the hole, grown by `margin`. Far-away patches that happen to have a similar colour are what makes a bad fill.
    int minX = L.w, minY = L.h, maxX = -1, maxY = -1;
    for (int y = 0; y < L.h; ++y)
        for (int x = 0; x < L.w; ++x)
            if (L.hole[size_t(y) * L.w + x]) { minX = std::min(minX, x); maxX = std::max(maxX, x); minY = std::min(minY, y); maxY = std::max(maxY, y); }
    L.sx0 = std::max(0, minX - margin); L.sy0 = std::max(0, minY - margin);
    L.sx1 = std::min(L.w - 1, maxX + margin); L.sy1 = std::min(L.h - 1, maxY + margin);
    L.target.assign(L.hole.size(), 0);
    // a pixel is a target when a patch centred on it touches the hole
    for (int y = 0; y < L.h; ++y) {
        for (int x = 0; x < L.w; ++x) {
            const int x0 = std::max(0, x - r), y0 = std::max(0, y - r), x1 = std::min(L.w - 1, x + r), y1 = std::min(L.h - 1, y + r);
            if (holeCount(L, x0, y0, x1, y1) > 0) L.target[size_t(y) * L.w + x] = 1;
        }
    }
    L.sources.clear();
    for (int y = std::max(r, L.sy0); y <= std::min(L.h - r - 1, L.sy1); ++y)
        for (int x = std::max(r, L.sx0); x <= std::min(L.w - r - 1, L.sx1); ++x)
            if (validSource(L, x, y, r)) L.sources.push_back(y * L.w + x);
    L.nx.assign(L.hole.size(), 0);
    L.ny.assign(L.hole.size(), 0);
    L.dist.assign(L.hole.size(), 1e30f);
}

void randomInit(Level &L, int r, std::mt19937 &rng)
{
    for (size_t i = 0; i < L.target.size(); ++i) {
        if (!L.target[i]) continue;
        const int s = L.sources[rng() % L.sources.size()];
        L.nx[i] = s % L.w;
        L.ny[i] = s / L.w;
        const int x = int(i % L.w), y = int(i / L.w);
        L.dist[i] = patchDist(L, x, y, L.nx[i], L.ny[i], r, 1e30f);
    }
}

inline void tryCandidate(Level &L, size_t i, int x, int y, int sx, int sy, int r)
{
    if (!validSource(L, sx, sy, r)) return;
    if (sx == L.nx[i] && sy == L.ny[i]) return;
    const float d = patchDist(L, x, y, sx, sy, r, L.dist[i]);
    if (d < L.dist[i]) { L.dist[i] = d; L.nx[i] = sx; L.ny[i] = sy; }
}

// one PatchMatch pass over the targets: propagation from the two neighbours already visited, then a random search of shrinking size
// one PatchMatch pass over the rows [rowA, rowB) of the level; several bands run at the same time (a band reads the matches of the neighbouring band while they are being
// changed: that is how PatchMatch is usually parallelised, a stale value only costs a missed chance)
void patchMatchPass(Level &L, int r, bool forward, int rowA, int rowB, std::mt19937 &rng)
{
    const int y0 = forward ? rowA : rowB - 1, y1 = forward ? rowB : rowA - 1, ys = forward ? 1 : -1;
    const int x0 = forward ? 0 : L.w - 1, x1 = forward ? L.w : -1, xs = forward ? 1 : -1;
    const int maxR = std::max(L.sx1 - L.sx0, L.sy1 - L.sy0) + 1;
    for (int y = y0; y != y1; y += ys) {
        for (int x = x0; x != x1; x += xs) {
            const size_t i = size_t(y) * L.w + x;
            if (!L.target[i]) continue;
            // propagation
            const int px = x - xs, py = y - ys;
            if (px >= 0 && px < L.w && L.target[size_t(y) * L.w + px]) {
                const size_t j = size_t(y) * L.w + px;
                tryCandidate(L, i, x, y, L.nx[j] + xs, L.ny[j], r);
            }
            if (py >= 0 && py < L.h && L.target[size_t(py) * L.w + x]) {
                const size_t j = size_t(py) * L.w + x;
                tryCandidate(L, i, x, y, L.nx[j], L.ny[j] + ys, r);
            }
            // random search around the best so far
            for (int rad = maxR; rad >= 1; rad >>= 1) {
                const int sx = L.nx[i] + int(rng() % (2 * rad + 1)) - rad;
                const int sy = L.ny[i] + int(rng() % (2 * rad + 1)) - rad;
                tryCandidate(L, i, x, y, sx, sy, r);
            }
        }
    }
}

void patchMatchPassParallel(Level &L, int r, bool forward, int threads, std::mt19937 &rng)
{
    std::vector<unsigned> seeds(size_t(std::max(1, threads)));
    for (auto &sd : seeds) sd = unsigned(rng());
    const int n = L.h;
    if (threads <= 1 || n < 8 * threads) { std::mt19937 g(seeds[0]); patchMatchPass(L, r, forward, 0, n, g); return; }
    std::vector<std::thread> pool;
    const int chunk = (n + threads - 1) / threads;
    for (int t = 0; t < threads; ++t) {
        const int a = t * chunk, b = std::min(n, a + chunk);
        if (a >= b) break;
        pool.emplace_back([&L, r, forward, a, b, &seeds, t]() { std::mt19937 g(seeds[size_t(t)]); patchMatchPass(L, r, forward, a, b, g); });
    }
    for (auto &th : pool) th.join();
}

// the vote: every pixel of the hole takes the weighted mean of what the matches of all the patches that cover it say it should be
void vote(Level &L, int r, float sharpness, int threads)
{
    const int c = L.c, w = L.w, h = L.h;
    const float norm = 1.0f / float((2 * r + 1) * (2 * r + 1) * c);
    // the spread of the weights: from the distribution of the match distances
    std::vector<float> ds;
    for (size_t i = 0; i < L.target.size(); ++i) if (L.target[i]) ds.push_back(L.dist[i] * norm);
    if (ds.empty()) return;
    std::nth_element(ds.begin(), ds.begin() + ds.size() / 2, ds.end());
    const float sigma2 = std::max(ds[ds.size() / 2], 1.0f) / std::max(0.05f, sharpness);

    // the weight of every match, and the weight of a pixel inside a patch (the centre counts more)
    std::vector<float> wgt(L.hole.size(), 0.0f);
    for (size_t i = 0; i < L.target.size(); ++i) if (L.target[i]) wgt[i] = std::exp(-(L.dist[i] * norm) / (2.0f * sigma2));
    const int P = 2 * r + 1;
    std::vector<float> spatial(size_t(P) * P);
    for (int dy = -r; dy <= r; ++dy)
        for (int dx = -r; dx <= r; ++dx)
            spatial[size_t(dy + r) * P + (dx + r)] = std::exp(-float(dx * dx + dy * dy) / (2.0f * g_patchSigma * g_patchSigma));

    // Gathering: the pixel t asks every patch that covers it. The pixels are independent of each other, so the rows can be done at the same time.
    std::vector<float> hpNew(L.img.size(), 0.0f);      // the voted detail
    std::vector<float> before(L.hole.size(), 0.0f);    // the energy of the detail of the copied pixels, before the averaging
    std::vector<float> wsum(L.hole.size(), 0.0f);
    parallelFor(h, threads, [&](int ya, int yb) {
        for (int y = ya; y < yb; ++y) {
            for (int x = 0; x < w; ++x) {
                const size_t t = size_t(y) * w + x;
                if (!L.hole[t]) continue;
                float a[4] = {0, 0, 0, 0}, ws = 0.0f, en = 0.0f;
                for (int dy = -r; dy <= r; ++dy) {
                    const int qy = y - dy;
                    if (qy < 0 || qy >= h) continue;
                    for (int dx = -r; dx <= r; ++dx) {
                        const int qx = x - dx;
                        if (qx < 0 || qx >= w) continue;
                        const size_t q = size_t(qy) * w + qx;
                        if (!L.target[q]) continue;
                        const float sw = wgt[q] * spatial[size_t(dy + r) * P + (dx + r)];
                        const size_t src = size_t(L.ny[q] + dy) * w + (L.nx[q] + dx);
                        float e = 0.0f;
                        for (int k = 0; k < c; ++k) { const float v = L.hp[src * c + k]; a[k] += sw * v; e += v * v; }
                        en += sw * e;
                        ws += sw;
                    }
                }
                if (ws <= 0.0f) continue;
                wsum[t] = ws;
                before[t] = en / ws;
                for (int k = 0; k < c; ++k) hpNew[t * c + k] = a[k] / ws;
            }
        }
    });

    // Averaging many patches lowers the contrast of the detail (opposite strokes cancel). The energy of the detail of the copied pixels BEFORE they are averaged is compared with the
    // energy of the average, and the detail of the hole is raised by the typical ratio (the median, so that no single pixel decides), up to g_gainMax. It can only give back what the copied
    // patches had: a smooth source stays smooth, and nothing is invented from the strong edges around the hole.
    std::vector<float> ratios;
    for (size_t t = 0; t < L.hole.size(); ++t) {
        if (!L.hole[t] || wsum[t] <= 0.0f) continue;
        float after = 0.0f;
        for (int k = 0; k < c; ++k) { const float v = hpNew[t * c + k]; after += v * v; }
        if (before[t] > 1.0f) ratios.push_back(std::sqrt(before[t] / std::max(after, 1e-3f)));
    }
    float gain = 1.0f;
    if (!ratios.empty()) {
        std::nth_element(ratios.begin(), ratios.begin() + ratios.size() / 2, ratios.end());
        gain = std::max(1.0f, std::min(g_gainMax, ratios[ratios.size() / 2]));
    }
    for (size_t t = 0; t < L.hole.size(); ++t) {
        if (!L.hole[t] || wsum[t] <= 0.0f) continue;
        for (int k = 0; k < c; ++k) L.img[t * c + k] = L.guide[t * c + k] + gain * hpNew[t * c + k];   // the smooth colour of the guide plus the voted detail
    }
}

Level downsample(const Level &F)
{
    Level L;
    L.c = F.c;
    L.w = (F.w + 1) / 2;
    L.h = (F.h + 1) / 2;
    L.img.assign(size_t(L.w) * L.h * L.c, 0.0f);
    L.hole.assign(size_t(L.w) * L.h, 0);
    for (int y = 0; y < L.h; ++y) {
        for (int x = 0; x < L.w; ++x) {
            float acc[4] = {0, 0, 0, 0};
            int n = 0;
            bool anyHole = false;
            for (int dy = 0; dy < 2; ++dy) {
                for (int dx = 0; dx < 2; ++dx) {
                    const int sx = std::min(F.w - 1, 2 * x + dx), sy = std::min(F.h - 1, 2 * y + dy);
                    const size_t s = size_t(sy) * F.w + sx;
                    if (F.hole[s]) { anyHole = true; continue; }
                    for (int k = 0; k < F.c; ++k) acc[k] += F.img[s * F.c + k];
                    ++n;
                }
            }
            const size_t i = size_t(y) * L.w + x;
            if (anyHole || n == 0) {
                L.hole[i] = 1;
            } else {
                for (int k = 0; k < L.c; ++k) L.img[i * L.c + k] = acc[k] / n;
            }
        }
    }
    return L;
}

inline float sampleBilinear(const Level &C, float fx, float fy, int k)
{
    fx = std::max(0.0f, std::min(fx, float(C.w - 1)));
    fy = std::max(0.0f, std::min(fy, float(C.h - 1)));
    const int x0 = int(fx), y0 = int(fy), x1 = std::min(C.w - 1, x0 + 1), y1 = std::min(C.h - 1, y0 + 1);
    const float ax = fx - x0, ay = fy - y0;
    auto at = [&](int x, int y) { return C.img[(size_t(y) * C.w + x) * C.c + k]; };
    return (1 - ay) * ((1 - ax) * at(x0, y0) + ax * at(x1, y0)) + ay * ((1 - ax) * at(x0, y1) + ax * at(x1, y1));
}

} // namespace

// Krimble 2026-10-19: the first engine (2026-10-18), kept, no longer called. It fills the hole with a smooth colour (a Laplace solution from the edge) and votes only the
// fine detail (a radius-3 high-pass) on top of it; anything larger than the fine detail came from the smooth colour, which is why George saw "a smear" ("anything with
// detail is going to fail", "Still looked blurry"). The engine that is used now is inpaint() at the end of this file.
bool inpaintLegacy(uint8_t *pixels, const uint8_t *holeIn, int w, int h, int channels, const Options &opt, uint8_t *filledHole)
{






    if (!pixels || !holeIn || w < 8 || h < 8 || channels < 1 || channels > 4) return false;
    const int r = std::max(1, opt.patchRadius);
    // const int T = opt.threads > 0 ? opt.threads : int(std::max(1u, std::min(4u, std::thread::hardware_concurrency())));
    // Krimble 2026-10-20: George: "Use as many threads as it takes. That's what multi-core is for." Every core the device has (was at most 4).
    const int T = opt.threads > 0 ? opt.threads : int(std::max(1u, std::thread::hardware_concurrency()));
    std::mt19937 rng(opt.seed);

    // level 0: the picture and the hole (grown by `expand` pixels)
    std::vector<Level> pyr(1);
    Level &L0 = pyr[0];
    L0.w = w; L0.h = h; L0.c = channels;
    L0.img.resize(size_t(w) * h * channels);
    for (size_t i = 0; i < L0.img.size(); ++i) L0.img[i] = pixels[i];
    L0.hole.assign(size_t(w) * h, 0);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (holeIn[size_t(y) * w + x]) L0.hole[size_t(y) * w + x] = 1;
    for (int pass = 0; pass < opt.expand; ++pass) {
        std::vector<uint8_t> grown = L0.hole;
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                if (L0.hole[size_t(y) * w + x]) {
                    for (int dy = -1; dy <= 1; ++dy)
                        for (int dx = -1; dx <= 1; ++dx) {
                            const int nx = x + dx, ny = y + dy;
                            if (nx >= 0 && ny >= 0 && nx < w && ny < h) grown[size_t(ny) * w + nx] = 1;
                        }
                }
        L0.hole.swap(grown);
    }
    int minX = w, minY = h, maxX = -1, maxY = -1;
    size_t holeCountAll = 0;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (L0.hole[size_t(y) * w + x]) { minX = std::min(minX, x); maxX = std::max(maxX, x); minY = std::min(minY, y); maxY = std::max(maxY, y); ++holeCountAll; }
    if (holeCountAll == 0) return true;
    if (holeCountAll >= size_t(w) * h) return false;

    // the number of levels: the hole must be small (about 12 px across) at the smallest one, and the picture big enough to find patches in
    const int holeDim = std::max(maxX - minX + 1, maxY - minY + 1);
    int levels = 1;
    while (levels < opt.maxLevels && (holeDim >> (levels - 1)) > 56 && std::min(w, h) >> levels >= 6 * (2 * r + 1)) ++levels;
    for (int i = 1; i < levels; ++i) pyr.push_back(downsample(pyr[i - 1]));

    // from the smallest level to the full size
    for (int lv = levels - 1; lv >= 0; --lv) {
        Level &L = pyr[lv];
        const bool coarsest = (lv == levels - 1);
        if (coarsest) {
            // filled below, by exemplarFill, once the sources are known
        } else {
            // the estimate of the level above, enlarged, fills the hole here; the known pixels stay the truth
            const Level &C = pyr[lv + 1];
            for (int y = 0; y < L.h; ++y)
                for (int x = 0; x < L.w; ++x) {
                    const size_t i = size_t(y) * L.w + x;
                    if (!L.hole[i]) continue;
                    for (int k = 0; k < L.c; ++k) L.img[i * L.c + k] = sampleBilinear(C, (x - 0.5f) / 2.0f, (y - 0.5f) / 2.0f, k);
                }
        }
        // the margin of the source window, in pixels of this level: 1.5 times the size of the hole, at least 40 px at full size
        // (the shorter side of the hole decides, so that a long thin hole, a line of text for instance, does not search half the picture)
        const int holeShort = std::min(maxX - minX + 1, maxY - minY + 1);
        const int margin = std::max(6 * (2 * r + 1), int(std::max(g_windowFactor * holeShort, 48.0f)) >> lv);
        prepareLevel(L, r, margin);
        if (coarsest) {
            // the first estimate: the smooth (Laplace) solution, or the exemplar fill
            harmonicFill(L);
        } else {
            correctBoundary(L, g_relaxIters);   // what the level above made of the hole, with its detail, made to meet the edge of this level
        }
        splitLowHigh(L);
        L.guide = L.lp;                          // the smooth colour of the estimate: the detail is voted on top of it
        if (L.sources.empty()) continue;
        if (coarsest) {
            randomInit(L, r, rng);
        } else {
            const Level &C = pyr[lv + 1];
            for (int y = 0; y < L.h; ++y)
                for (int x = 0; x < L.w; ++x) {
                    const size_t i = size_t(y) * L.w + x;
                    if (!L.target[i]) continue;
                    const size_t ci = size_t(std::min(C.h - 1, y / 2)) * C.w + std::min(C.w - 1, x / 2);
                    int sx = 0, sy = 0;
                    bool ok = false;
                    if (C.target[ci] && !C.sources.empty()) {
                        sx = C.nx[ci] * 2 + (x & 1);
                        sy = C.ny[ci] * 2 + (y & 1);
                        ok = validSource(L, sx, sy, r);
                    }
                    if (!ok) { const int s = L.sources[rng() % L.sources.size()]; sx = s % L.w; sy = s / L.w; }
                    L.nx[i] = sx; L.ny[i] = sy;
                    L.dist[i] = patchDist(L, x, y, sx, sy, r, 1e30f);
                }
        }
        // Seed the detail: one vote with equal weights, from the matches as they came (random on the smallest level, the matches of the level above on the others). Without it the estimate
        // has no detail, the matching finds only smooth patches for it, and the fill stays a smooth blur.
        for (size_t i = 0; i < L.target.size(); ++i) if (L.target[i]) L.dist[i] = 1.0f;
        vote(L, r, 1.0f, T);
        splitLowHigh(L);
        parallelFor(int(L.target.size()), T, [&](int a, int b) {
            for (int i = a; i < b; ++i)
                if (L.target[size_t(i)]) L.dist[size_t(i)] = patchDist(L, i % L.w, i / L.w, L.nx[size_t(i)], L.ny[size_t(i)], r, 1e30f);
        });
        int iterations = coarsest ? 12 : (lv == 0 ? 4 : 6);
        for (int it = 0; it < iterations; ++it) {
            patchMatchPassParallel(L, r, (it & 1) == 0, T, rng);
            patchMatchPassParallel(L, r, (it & 1) != 0, T, rng);
            if (opt.progress && !opt.progress(float(levels - 1 - lv) / float(levels) + (float(it) / float(iterations)) / float(levels))) return false;
            // the matches were measured against the old estimate: measure again against the one the vote makes
            const float sharp = (lv <= 1) ? opt.sharpness * ((lv == 0 && it >= iterations - 2) ? 2.0f : 1.0f) : std::min(opt.sharpness, g_coarseSharp);
            vote(L, r, sharp, T);
            splitLowHigh(L);
            parallelFor(int(L.target.size()), T, [&](int a, int b) {
                for (int i = a; i < b; ++i)
                    if (L.target[size_t(i)]) L.dist[size_t(i)] = patchDist(L, i % L.w, i / L.w, L.nx[size_t(i)], L.ny[size_t(i)], r, 1e30f);
            });
        }
    }

    // write the hole back (only the pixels the caller asked for, not the pixels of the grown edge unless they were in the hole)
    const Level &F = pyr[0];
    if (filledHole) for (size_t i = 0; i < F.hole.size(); ++i) filledHole[i] = F.hole[i] ? 1 : 0;
    for (size_t i = 0; i < F.hole.size(); ++i) {
        if (!F.hole[i]) continue;
        for (int k = 0; k < channels; ++k)
            pixels[i * channels + k] = uint8_t(std::max(0.0f, std::min(255.0f, F.img[i * channels + k] + 0.5f)));
    }
    return true;
}


// ====================================================================================================================================================================
// Krimble 2026-10-19: the engine used now. George (1.0.39): Content-Aware Fill "just leaves a blurry mess", "anything with detail is going to fail", "We need it to look like
// magic. Not a smear.", and, of the plan: "Using real pixels is better." This is the classic way (Wexler, Shechtman and Irani 2007; Barnes et al. 2009, PatchMatch), with
// the whole colour of the patches, at every size:
//  - a pyramid of the picture, down to a size where the hole is small;
//  - at every size, every patch that touches the hole gets the most similar patch of the known picture (PatchMatch: neighbours' matches spread, plus a random search);
//  - the hole is rebuilt from those matches (each pixel from the patches that cover it), and the matching is done again on the rebuilt hole, a few times;
//  - going up a size, the matches are carried up and the hole is rebuilt from the bigger picture's OWN pixels (not an enlarged smaller picture), so no blur comes up;
//  - at full size, the last rebuild takes every pixel from the ONE best patch that covers it: real pixels, copied, never averaged.
// ====================================================================================================================================================================
namespace {

struct PLevel {
    int w = 0, h = 0, c = 0;
    std::vector<float> img;       // w * h * c
    std::vector<uint8_t> hole;    // w * h, 1 = to be filled
    std::vector<uint8_t> src;     // w * h, 1 = a patch centred here lies wholly in the known picture (a valid source)
    std::vector<uint8_t> tgt;     // w * h, 1 = the patch centred here touches the hole (a target)
    std::vector<int> nx, ny;      // the match of every target
    std::vector<float> nd;        // its distance
    std::vector<int> srcList;     // the valid sources, as indices
    std::vector<float> conf;      // w * h: how much a pixel counts when patches are compared (known pixels 1; the guessed hole less)
    // Krimble 2026-10-19 (2nd), the Photoshop-style engine (inpaint(), below): per target, which transform its source is copied with (turned, resized,
    // mirrored) and the colour shift that adapts the copied patch to where it lands; per pixel, how far it is from the hole and the picture's edge.
    std::vector<uint8_t> xf;      // w * h: index into the transform table
    std::vector<float> bias;      // w * h * c
    std::vector<int> freeR;       // w * h: Chebyshev distance to the nearest hole pixel or the edge of the picture
    std::vector<int32_t> sat;     // (w + 1) * (h + 1) * c: sums of the picture (whole levels), for the mean colour of any window (the colour shift)
};

void pBuild(PLevel &L, int r)
{
    const int w = L.w, h = L.h;
    // integral image of the hole, to test whole windows at once
    std::vector<int> sat(size_t(w + 1) * (h + 1), 0);
    for (int y = 0; y < h; ++y) {
        int row = 0;
        for (int x = 0; x < w; ++x) {
            row += L.hole[size_t(y) * w + x] ? 1 : 0;
            sat[size_t(y + 1) * (w + 1) + (x + 1)] = sat[size_t(y) * (w + 1) + (x + 1)] + row;
        }
    }
    auto holesIn = [&](int x0, int y0, int x1, int y1) {   // inclusive, clamped
        x0 = std::max(0, x0); y0 = std::max(0, y0); x1 = std::min(w - 1, x1); y1 = std::min(h - 1, y1);
        if (x1 < x0 || y1 < y0) return 0;
        return sat[size_t(y1 + 1) * (w + 1) + (x1 + 1)] - sat[size_t(y0) * (w + 1) + (x1 + 1)] - sat[size_t(y1 + 1) * (w + 1) + x0] + sat[size_t(y0) * (w + 1) + x0];
    };
    L.src.assign(size_t(w) * h, 0);
    L.tgt.assign(size_t(w) * h, 0);
    L.srcList.clear();
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const size_t i = size_t(y) * w + x;
            if (x >= r && y >= r && x < w - r && y < h - r && holesIn(x - r, y - r, x + r, y + r) == 0) { L.src[i] = 1; L.srcList.push_back(int(i)); }
            if (holesIn(x - r, y - r, x + r, y + r) > 0) L.tgt[i] = 1;
        }
    L.nx.assign(size_t(w) * h, 0);
    L.ny.assign(size_t(w) * h, 0);
    L.nd.assign(size_t(w) * h, 1e30f);
    L.conf.assign(size_t(w) * h, 1.0f);
}

// how much the guessed hole counts in the comparisons: little while the guess is rough (the known picture around the hole decides what is copied), more as it improves
void pSetConfidence(PLevel &L, float holeWeight)
{
    for (size_t i = 0; i < L.hole.size(); ++i) L.conf[i] = L.hole[i] ? holeWeight : 1.0f;
}

// the distance between the patch around target (tx, ty) and the patch around source (sx, sy); pixels of the target patch outside the picture are skipped
inline float pDist(const PLevel &L, int tx, int ty, int sx, int sy, int r, float cutoff)
{
    const int c = L.c, w = L.w, h = L.h;
    float d = 0.0f;
    for (int dy = -r; dy <= r; ++dy) {
        const int y = ty + dy;
        if (y < 0 || y >= h) continue;
        const float *a = &L.img[(size_t(y) * w) * c];
        const float *b = &L.img[(size_t(sy + dy) * w) * c];
        for (int dx = -r; dx <= r; ++dx) {
            const int x = tx + dx;
            if (x < 0 || x >= w) continue;
            const float *pa = a + size_t(x) * c, *pb = b + size_t(sx + dx) * c;
            float e2 = 0.0f;
            for (int k = 0; k < c; ++k) { const float e = pa[k] - pb[k]; e2 += e * e; }
            d += e2 * L.conf[size_t(y) * w + x];
        }
        if (d > cutoff) return d;
    }
    return d;
}

template <typename F>
void pParallel(int n, int threads, F f)
{
    if (threads <= 1 || n < 64) { f(0, n); return; }
    std::vector<std::thread> pool;
    const int step = (n + threads - 1) / threads;
    for (int t = 0; t < threads; ++t) {
        const int a = t * step, b = std::min(n, a + step);
        if (a < b) pool.emplace_back([=]() { f(a, b); });
    }
    for (auto &th : pool) th.join();
}

inline void pTry(PLevel &L, size_t i, int x, int y, int sx, int sy, int r)
{
    if (sx < r || sy < r || sx >= L.w - r || sy >= L.h - r) return;
    if (!L.src[size_t(sy) * L.w + sx]) return;
    if (sx == L.nx[i] && sy == L.ny[i]) return;
    const float d = pDist(L, x, y, sx, sy, r, L.nd[i]);
    if (d < L.nd[i]) { L.nd[i] = d; L.nx[i] = sx; L.ny[i] = sy; }
}

// one PatchMatch pass over rows [ya, yb): propagation from the two neighbours already visited, then a random search of shrinking size around the current match
void pPass(PLevel &L, int r, bool forward, int ya, int yb, unsigned seed)
{
    std::mt19937 rng(seed);
    const int w = L.w;
    const int maxR = std::max(L.w, L.h);
    const int sY = forward ? ya : yb - 1, eY = forward ? yb : ya - 1, st = forward ? 1 : -1;
    for (int y = sY; y != eY; y += st) {
        const int sX = forward ? 0 : w - 1, eX = forward ? w : -1;
        for (int x = sX; x != eX; x += st) {
            const size_t i = size_t(y) * w + x;
            if (!L.tgt[i]) continue;
            const int px = x - st, py = y - st;
            if (px >= 0 && px < w && L.tgt[size_t(y) * w + px]) { const size_t q = size_t(y) * w + px; pTry(L, i, x, y, L.nx[q] + st, L.ny[q], r); }
            if (py >= 0 && py < L.h && L.tgt[size_t(py) * w + x]) { const size_t q = size_t(py) * w + x; pTry(L, i, x, y, L.nx[q], L.ny[q] + st, r); }
            for (int rad = maxR; rad >= 1; rad /= 2) {
                const int sx = L.nx[i] + int(rng() % unsigned(2 * rad + 1)) - rad;
                const int sy = L.ny[i] + int(rng() % unsigned(2 * rad + 1)) - rad;
                pTry(L, i, x, y, sx, sy, r);
            }
        }
    }
}

void pMatch(PLevel &L, int r, int iterations, int threads, unsigned &seed)
{
    for (int it = 0; it < iterations; ++it) {
        const bool fwd = (it & 1) == 0;
        pParallel(L.h, threads, [&](int a, int b) { pPass(L, r, fwd, a, b, seed + unsigned(a) * 7919u); });
        seed += 104729u;
    }
}

// The hole is rebuilt from the matches. `winner` false: every pixel is the weighted mean of what the patches that cover it say (weights from the match quality); used while the
// matches are still being found. `winner` true: every pixel is taken from the one covering patch with the best match (sharp; the final rebuild).
void pVote(PLevel &L, int r, bool winner, int threads)
{
    const int c = L.c, w = L.w, h = L.h;
    std::vector<float> ds;
    for (size_t i = 0; i < L.tgt.size(); ++i) if (L.tgt[i] && L.nd[i] < 1e29f) ds.push_back(L.nd[i]);
    if (ds.empty()) return;
    std::nth_element(ds.begin(), ds.begin() + ds.size() * 3 / 4, ds.end());
    const float sigma2 = std::max(ds[ds.size() * 3 / 4], 1.0f);
    std::vector<float> out(L.img);
    pParallel(h, threads, [&](int ya, int yb) {
        for (int y = ya; y < yb; ++y)
            for (int x = 0; x < w; ++x) {
                const size_t p = size_t(y) * w + x;
                if (!L.hole[p]) continue;
                float acc[4] = {0, 0, 0, 0}, ws = 0.0f, best = 1e30f;
                for (int dy = -r; dy <= r; ++dy) {
                    const int qy = y - dy;
                    if (qy < 0 || qy >= h) continue;
                    for (int dx = -r; dx <= r; ++dx) {
                        const int qx = x - dx;
                        if (qx < 0 || qx >= w) continue;
                        const size_t q = size_t(qy) * w + qx;
                        if (!L.tgt[q] || L.nd[q] >= 1e29f) continue;
                        const size_t s = (size_t(L.ny[q] + dy) * w + (L.nx[q] + dx)) * c;
                        if (winner) {
                            // the best match wins; a patch centred nearer the pixel wins a tie
                            const float score = L.nd[q] * (1.0f + 0.02f * float(dx * dx + dy * dy));
                            if (score < best) { best = score; for (int k = 0; k < c; ++k) acc[k] = L.img[s + k]; ws = 1.0f; }
                        } else {
                            const float wq = std::exp(-L.nd[q] / (2.0f * sigma2));
                            for (int k = 0; k < c; ++k) acc[k] += wq * L.img[s + k];
                            ws += wq;
                        }
                    }
                }
                if (ws > 0.0f) for (int k = 0; k < c; ++k) out[p * c + k] = acc[k] / ws;
            }
    });
    L.img.swap(out);
}

// The final rebuild with the seam taken away. Every hole pixel is taken from its best covering patch (real pixels), and the same patches also predict the known pixels
// in a thin ring around the hole. Where a prediction differs from the real pixel (a copied piece is a little lighter, darker or of another hue than this spot), that
// difference is spread smoothly into the hole (a membrane, as in seamless cloning) and added, so the copied pixels meet the picture with no visible edge. Only a smooth
// colour shift is added: the copied detail stays as sharp as it was.
void pFinalSeamless(PLevel &L, int r, int threads)
{
    const int c = L.c, w = L.w, h = L.h;
    std::vector<float> pred(L.img.size(), 0.0f);
    std::vector<uint8_t> has(L.hole.size(), 0);
    pParallel(h, threads, [&](int ya, int yb) {
        for (int y = ya; y < yb; ++y)
            for (int x = 0; x < w; ++x) {
                const size_t p = size_t(y) * w + x;
                if (!L.tgt[p]) continue;    // the hole and the ring of known pixels whose patches touch it
                float best = 1e30f;
                for (int dy = -r; dy <= r; ++dy) {
                    const int qy = y - dy;
                    if (qy < 0 || qy >= h) continue;
                    for (int dx = -r; dx <= r; ++dx) {
                        const int qx = x - dx;
                        if (qx < 0 || qx >= w) continue;
                        const size_t q = size_t(qy) * w + qx;
                        if (!L.tgt[q] || L.nd[q] >= 1e29f) continue;
                        // ring pixels (known): only patches CENTRED IN THE HOLE count. A patch centred on a known pixel near the hole is matched mostly by its known part
                        // and predicts the ring almost exactly, which would hide the seam; the patches centred in the hole are the ones whose pixels were copied.
                        if (!L.hole[p] && !L.hole[q]) continue;
                        const float score = L.nd[q] * (1.0f + 0.02f * float(dx * dx + dy * dy));
                        if (score < best) {
                            best = score;
                            const size_t s = (size_t(L.ny[q] + dy) * w + (L.nx[q] + dx)) * c;
                            for (int k = 0; k < c; ++k) pred[p * c + k] = L.img[s + k];
                            has[p] = 1;
                        }
                    }
                }
            }
    });
    // the offset: fixed on the ring (real - predicted), unknown in the hole
    std::vector<float> off(L.img.size(), 0.0f);
    std::vector<uint8_t> fixedRing(L.hole.size(), 0);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const size_t p = size_t(y) * w + x;
            if (L.hole[p] || !has[p]) continue;
            bool nearHole = false;
            for (int dy = -1; dy <= 1 && !nearHole; ++dy)
                for (int dx = -1; dx <= 1; ++dx) {
                    const int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && ny >= 0 && nx < w && ny < h && L.hole[size_t(ny) * w + nx]) { nearHole = true; break; }
                }
            if (!nearHole) continue;
            fixedRing[p] = 1;
            for (int k = 0; k < c; ++k) off[p * c + k] = L.img[p * c + k] - pred[p * c + k];
        }
    // the hole pixels take their best patch
    for (size_t p = 0; p < L.hole.size(); ++p)
        if (L.hole[p] && has[p]) for (int k = 0; k < c; ++k) L.img[p * c + k] = pred[p * c + k];
    // the membrane: first an onion-peel guess, then over-relaxation (each hole value toward the mean of its four neighbours)
    std::vector<uint8_t> known(L.hole.size(), 0);
    for (size_t p = 0; p < known.size(); ++p) known[p] = fixedRing[p];
    std::vector<size_t> order;
    bool changed = true;
    while (changed) {
        changed = false;
        std::vector<size_t> wave;
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                const size_t p = size_t(y) * w + x;
                if (!L.hole[p] || known[p]) continue;
                float acc[4] = {0, 0, 0, 0};
                int n = 0;
                const int nbx[4] = {x - 1, x + 1, x, x}, nby[4] = {y, y, y - 1, y + 1};
                for (int j = 0; j < 4; ++j) {
                    if (nbx[j] < 0 || nby[j] < 0 || nbx[j] >= w || nby[j] >= h) continue;
                    const size_t q = size_t(nby[j]) * w + nbx[j];
                    if (!known[q]) continue;
                    for (int k = 0; k < c; ++k) acc[k] += off[q * c + k];
                    ++n;
                }
                if (n) { for (int k = 0; k < c; ++k) off[p * c + k] = acc[k] / n; wave.push_back(p); }
            }
        for (size_t p : wave) { known[p] = 1; order.push_back(p); changed = true; }
    }
    std::vector<size_t> holeList;
    for (size_t p = 0; p < L.hole.size(); ++p) if (L.hole[p]) holeList.push_back(p);
    const float omega = 1.9f;
    for (int it = 0; it < 400; ++it) {
        for (size_t p : holeList) {
            const int x = int(p % size_t(w)), y = int(p / size_t(w));
            float acc[4] = {0, 0, 0, 0};
            int n = 0;
            const int nbx[4] = {x - 1, x + 1, x, x}, nby[4] = {y, y, y - 1, y + 1};
            for (int j = 0; j < 4; ++j) {
                if (nbx[j] < 0 || nby[j] < 0 || nbx[j] >= w || nby[j] >= h) continue;
                const size_t q = size_t(nby[j]) * w + nbx[j];
                if (!L.hole[q] && !fixedRing[q]) continue;   // the outer edge of the picture: free (no pull)
                for (int k = 0; k < c; ++k) acc[k] += off[q * c + k];
                ++n;
            }
            if (!n) continue;
            for (int k = 0; k < c; ++k) off[p * c + k] += omega * (acc[k] / n - off[p * c + k]);
        }
    }
    for (size_t p : holeList) for (int k = 0; k < c; ++k) L.img[p * c + k] += off[p * c + k];
}

void pRemeasure(PLevel &L, int r, int threads)
{
    pParallel(int(L.tgt.size()), threads, [&](int a, int b) {
        for (int i = a; i < b; ++i)
            if (L.tgt[size_t(i)]) L.nd[size_t(i)] = pDist(L, i % L.w, i / L.w, L.nx[size_t(i)], L.ny[size_t(i)], r, 1e30f);
    });
}

PLevel pDown(const PLevel &F)
{
    PLevel L;
    L.c = F.c; L.w = (F.w + 1) / 2; L.h = (F.h + 1) / 2;
    L.img.assign(size_t(L.w) * L.h * L.c, 0.0f);
    L.hole.assign(size_t(L.w) * L.h, 0);
    for (int y = 0; y < L.h; ++y)
        for (int x = 0; x < L.w; ++x) {
            float acc[4] = {0, 0, 0, 0};
            int n = 0, holes = 0;
            for (int dy = 0; dy < 2; ++dy)
                for (int dx = 0; dx < 2; ++dx) {
                    const int sx = std::min(F.w - 1, 2 * x + dx), sy = std::min(F.h - 1, 2 * y + dy);
                    const size_t s = size_t(sy) * F.w + sx;
                    if (F.hole[s]) { ++holes; continue; }
                    for (int k = 0; k < F.c; ++k) acc[k] += F.img[s * F.c + k];
                    ++n;
                }
            const size_t i = size_t(y) * L.w + x;
            if (holes > 0 || n == 0) L.hole[i] = 1;
            else for (int k = 0; k < L.c; ++k) L.img[i * L.c + k] = acc[k] / n;
        }
    return L;
}

// the first guess at the smallest size: the hole is filled from its edge inward, each pixel the mean of its neighbours that are already known or filled (onion peel)
void pPeel(PLevel &L)
{
    const int w = L.w, h = L.h, c = L.c;
    std::vector<uint8_t> known(L.hole.size());
    for (size_t i = 0; i < known.size(); ++i) known[i] = L.hole[i] ? 0 : 1;
    bool changed = true;
    while (changed) {
        changed = false;
        std::vector<uint8_t> next = known;
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                const size_t i = size_t(y) * w + x;
                if (known[i]) continue;
                float acc[4] = {0, 0, 0, 0};
                int n = 0;
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) {
                        const int nx = x + dx, ny = y + dy;
                        if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
                        const size_t q = size_t(ny) * w + nx;
                        if (!known[q]) continue;
                        for (int k = 0; k < c; ++k) acc[k] += L.img[q * c + k];
                        ++n;
                    }
                if (n) { for (int k = 0; k < c; ++k) L.img[i * c + k] = acc[k] / n; next[i] = 1; changed = true; }
            }
        known.swap(next);
    }
}

} // namespace

// Krimble 2026-10-19 (2nd): the engine of the 206th entry (whole-colour PatchMatch, real pixels, seamless edge), kept; the engine used now is inpaint() at the end
// of this file, the same with Photoshop's additions.
bool inpaintPatchMatchV1(uint8_t *pixels, const uint8_t *holeIn, int w, int h, int channels, const Options &opt, uint8_t *filledHole)
{
    if (!pixels || !holeIn || w < 8 || h < 8 || channels < 1 || channels > 4) return false;
    const int r = std::max(2, opt.patchRadius);
    // const int T = opt.threads > 0 ? opt.threads : int(std::max(1u, std::min(4u, std::thread::hardware_concurrency())));
    // Krimble 2026-10-20: George: "Use as many threads as it takes. That's what multi-core is for." Every core the device has (was at most 4).
    const int T = opt.threads > 0 ? opt.threads : int(std::max(1u, std::thread::hardware_concurrency()));
    unsigned seed = opt.seed;

    std::vector<PLevel> pyr(1);
    PLevel &L0 = pyr[0];
    L0.w = w; L0.h = h; L0.c = channels;
    L0.img.resize(size_t(w) * h * channels);
    for (size_t i = 0; i < L0.img.size(); ++i) L0.img[i] = pixels[i];
    L0.hole.assign(size_t(w) * h, 0);
    for (size_t i = 0; i < L0.hole.size(); ++i) L0.hole[i] = holeIn[i] ? 1 : 0;
    for (int pass = 0; pass < opt.expand; ++pass) {
        std::vector<uint8_t> grown = L0.hole;
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                if (L0.hole[size_t(y) * w + x])
                    for (int dy = -1; dy <= 1; ++dy)
                        for (int dx = -1; dx <= 1; ++dx) {
                            const int nx = x + dx, ny = y + dy;
                            if (nx >= 0 && ny >= 0 && nx < w && ny < h) grown[size_t(ny) * w + nx] = 1;
                        }
        L0.hole.swap(grown);
    }
    int minX = w, minY = h, maxX = -1, maxY = -1;
    size_t holeCount = 0;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (L0.hole[size_t(y) * w + x]) { minX = std::min(minX, x); maxX = std::max(maxX, x); minY = std::min(minY, y); maxY = std::max(maxY, y); ++holeCount; }
    if (holeCount == 0) return true;
    if (holeCount >= size_t(w) * h) return false;

    // the levels: down until the hole is at most about 24 px across (or the picture gets too small to find patches in)
    const int holeDim = std::max(maxX - minX + 1, maxY - minY + 1);
    int levels = 1;
    while (levels < std::max(opt.maxLevels, 10) && (holeDim >> (levels - 1)) > 24 && (std::min(w, h) >> levels) >= 8 * (2 * r + 1)) ++levels;
    for (int i = 1; i < levels; ++i) pyr.push_back(pDown(pyr[size_t(i - 1)]));

    for (int lv = levels - 1; lv >= 0; --lv) {
        PLevel &L = pyr[size_t(lv)];
        pBuild(L, r);
        if (L.srcList.empty()) {
            if (lv == levels - 1) pPeel(L);
            continue;
        }
        std::mt19937 rng(seed + unsigned(lv));
        if (lv == levels - 1) {
            pPeel(L);
            for (size_t i = 0; i < L.tgt.size(); ++i) {
                if (!L.tgt[i]) continue;
                const int s = L.srcList[rng() % L.srcList.size()];
                L.nx[i] = s % L.w; L.ny[i] = s / L.w;
            }
        } else {
            // the matches of the smaller size, carried up (doubled), each checked; the hole is then rebuilt from THIS size's own pixels
            const PLevel &C = pyr[size_t(lv + 1)];
            for (int y = 0; y < L.h; ++y)
                for (int x = 0; x < L.w; ++x) {
                    const size_t i = size_t(y) * L.w + x;
                    if (!L.tgt[i]) continue;
                    const int cx = std::min(C.w - 1, x / 2), cy = std::min(C.h - 1, y / 2);
                    const size_t ci = size_t(cy) * C.w + cx;
                    int sx = -1, sy = -1;
                    if (C.tgt[ci] && C.nd[ci] < 1e29f) { sx = C.nx[ci] * 2 + (x - cx * 2); sy = C.ny[ci] * 2 + (y - cy * 2); }
                    else if (!L.hole[i]) { sx = x; sy = y; }
                    if (sx < r || sy < r || sx >= L.w - r || sy >= L.h - r || !L.src[size_t(sy) * L.w + sx]) {
                        const int s = L.srcList[rng() % L.srcList.size()];
                        sx = s % L.w; sy = s / L.w;
                    }
                    L.nx[i] = sx; L.ny[i] = sy;
                }
            // a first rebuild of the hole from the carried matches (the distances are not known yet: all equal)
            for (size_t i = 0; i < L.tgt.size(); ++i) if (L.tgt[i]) L.nd[i] = 1.0f;
            pVote(L, r, false, T);
        }
        const int emIters = (lv == levels - 1) ? 10 : (lv == 0 ? 3 : 5);
        // the guessed hole counts little at the smallest size (its first guess is only a peeled average), more on the bigger sizes (their guess came from real patches)
        pSetConfidence(L, (lv == levels - 1) ? 0.15f : 0.6f);
        pRemeasure(L, r, T);
        for (int it = 0; it < emIters; ++it) {
            if (lv == levels - 1) { pSetConfidence(L, 0.15f + 0.6f * float(it) / float(emIters)); }
            pMatch(L, r, 2, T, seed);
            if (opt.progress && !opt.progress(float(levels - 1 - lv) / float(levels) + (float(it + 1) / float(emIters)) / float(levels))) return false;
            const bool last = (lv == 0 && it == emIters - 1);
            if (last) pFinalSeamless(L, r, T);
            else { pVote(L, r, false, T); pRemeasure(L, r, T); }
        }
    }

    const PLevel &F = pyr[0];
    if (filledHole) for (size_t i = 0; i < F.hole.size(); ++i) filledHole[i] = F.hole[i] ? 1 : 0;
    for (size_t i = 0; i < F.hole.size(); ++i) {
        if (!F.hole[i]) continue;
        for (int k = 0; k < channels; ++k)
            pixels[i * channels + k] = uint8_t(std::max(0.0f, std::min(255.0f, F.img[i * channels + k] + 0.5f)));
    }
    return true;
}


// ====================================================================================================================================================================
// Krimble 2026-10-19 (2nd): the engine used now. George: "For now try to do Photoshop's way. Develop an algorithm." The same whole-colour multi-scale PatchMatch as
// inpaintPatchMatchV1 (real pixels, seamless edge), with what Photoshop's Content-Aware Fill adds on top of PatchMatch (Barnes et al. 2010, "The generalized PatchMatch
// correspondence algorithm"; Darabi et al. 2012, "Image melding"):
//  - ROTATION and SCALE: a source patch may be copied turned (-30 to +30 degrees) and resized (0.8 to 1.25 times), so curves, perspective and repeated things at
//    other sizes continue properly;
//  - MIRRORING: a source patch may be copied flipped left-right;
//  - COLOUR ADAPTATION: each copied patch gets the colour shift (per channel, at most 28 levels of 255) that makes it fit where it lands, so a darker or warmer copy
//    of a texture can still be used; the shift is part of the match (patches are compared after it), and a large shift costs a little;
//  - each transform costs a little more than a plain copy, so a plain copy wins when it is as good.
// ====================================================================================================================================================================
namespace {

struct QXf { float a, b, c, d; float reach; float cost; };    // source offset = [a b; c d] * patch offset; reach: how far the patch reaches; cost: the penalty factor

const std::vector<QXf> &qTable(int r)
{
    static std::vector<QXf> table;
    static int tableR = -1;
    if (tableR == r) return table;
    table.clear();
    const float rots[5] = {0.0f, -15.0f, 15.0f, -30.0f, 30.0f};
    const float scales[5] = {1.0f, 0.9f, 1.12f, 0.8f, 1.25f};
    for (int flip = 0; flip < 2; ++flip)
        for (int ri = 0; ri < 5; ++ri)
            for (int si = 0; si < 5; ++si) {
                const float th = rots[ri] * 3.14159265f / 180.0f, sc = scales[si];
                const float fx = flip ? -1.0f : 1.0f;
                QXf t;
                t.a = sc * std::cos(th) * fx; t.b = -sc * std::sin(th);
                t.c = sc * std::sin(th) * fx; t.d = sc * std::cos(th);
                t.reach = std::ceil(float(r) * sc * 1.4143f) + 1.0f;
                t.cost = 1.0f + 0.10f * float((ri + 1) / 2) + 0.08f * float((si + 1) / 2) + 0.12f * float(flip);
                table.push_back(t);
            }
    tableR = r;
    return table;   // index 0 is the plain copy
}

// the distance (Chebyshev) of every pixel to the nearest hole pixel or the edge of the picture: a transformed source patch must lie wholly in the known picture
void qFreeRadius(PLevel &L)
{
    const int w = L.w, h = L.h;
    const int INF = 1 << 20;
    L.freeR.assign(size_t(w) * h, INF);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const size_t i = size_t(y) * w + x;
            if (L.hole[i]) { L.freeR[i] = 0; continue; }
            int v = std::min(std::min(x + 1, y + 1), std::min(w - x, h - y));   // the edge of the picture
            if (x > 0) v = std::min(v, L.freeR[i - 1] + 1);
            if (y > 0) {
                v = std::min(v, L.freeR[i - w] + 1);
                if (x > 0) v = std::min(v, L.freeR[i - w - 1] + 1);
                if (x < w - 1) v = std::min(v, L.freeR[i - w + 1] + 1);
            }
            L.freeR[i] = v;
        }
    for (int y = h - 1; y >= 0; --y)
        for (int x = w - 1; x >= 0; --x) {
            const size_t i = size_t(y) * w + x;
            int v = L.freeR[i];
            if (x < w - 1) v = std::min(v, L.freeR[i + 1] + 1);
            if (y < h - 1) {
                v = std::min(v, L.freeR[i + w] + 1);
                if (x < w - 1) v = std::min(v, L.freeR[i + w + 1] + 1);
                if (x > 0) v = std::min(v, L.freeR[i + w - 1] + 1);
            }
            L.freeR[i] = v;
        }
}

inline bool qValid(const PLevel &L, int sx, int sy, const QXf &t)
{
    if (sx < 0 || sy < 0 || sx >= L.w || sy >= L.h) return false;
    return float(L.freeR[size_t(sy) * L.w + sx]) > t.reach;
}

inline void qSample(const PLevel &L, float fx, float fy, float *out)
{
    const int c = L.c, w = L.w;
    const int x0 = int(fx), y0 = int(fy);
    const float ax = fx - float(x0), ay = fy - float(y0);
    const float *p00 = &L.img[(size_t(y0) * w + x0) * c];
    const float *p10 = p00 + c, *p01 = p00 + size_t(w) * c, *p11 = p01 + c;
    for (int k = 0; k < c; ++k)
        out[k] = (1 - ay) * ((1 - ax) * p00[k] + ax * p10[k]) + ay * ((1 - ax) * p01[k] + ax * p11[k]);
}

// the value the match of target q gives to the pixel at offset (dx, dy) from q (the transformed source, plus the colour shift). `nearest`: the source pixel nearest
// to the transformed spot, not a blend of four (the final rebuild: a blend of four neighbours softens every turned or resized copy)
inline void qValue(const PLevel &L, const std::vector<QXf> &tab, size_t q, int dx, int dy, float *out, bool nearest = false)
{
    const QXf &t = tab[L.xf[q]];
    if (nearest && L.xf[q] != 0) {
        const int sx = int(std::lround(float(L.nx[q]) + t.a * dx + t.b * dy)), sy = int(std::lround(float(L.ny[q]) + t.c * dx + t.d * dy));
        const float *p = &L.img[(size_t(sy) * L.w + sx) * L.c];
        for (int k = 0; k < L.c; ++k) out[k] = p[k] + L.bias[q * L.c + k];
        return;
    }
    if (L.xf[q] == 0) {
        const float *p = &L.img[(size_t(L.ny[q] + dy) * L.w + (L.nx[q] + dx)) * L.c];
        for (int k = 0; k < L.c; ++k) out[k] = p[k] + L.bias[q * L.c + k];
        return;
    }
    qSample(L, float(L.nx[q]) + t.a * dx + t.b * dy, float(L.ny[q]) + t.c * dx + t.d * dy, out);
    for (int k = 0; k < L.c; ++k) out[k] += L.bias[q * L.c + k];
}

const float kMaxBias = 28.0f;      // the largest colour shift, per channel (of 255)
const float kBiasCost = 0.35f;     // what a shift costs, next to the difference it removes

// the sums of the picture, so that the mean colour of any window takes four reads (rebuilt whenever the hole is rebuilt)
void qBuildSat(PLevel &L)
{
    const int w = L.w, h = L.h, c = L.c;
    // 32-bit integers (4 bytes a channel): at most 255 * 4 M pixels, below 2^31
    L.sat.assign(size_t(w + 1) * (h + 1) * c, 0);
    for (int y = 0; y < h; ++y) {
        int32_t row[4] = {0, 0, 0, 0};
        for (int x = 0; x < w; ++x) {
            const size_t o = (size_t(y + 1) * (w + 1) + (x + 1)) * c, u = (size_t(y) * (w + 1) + (x + 1)) * c;
            for (int k = 0; k < c; ++k) { row[k] += int32_t(L.img[(size_t(y) * w + x) * c + k] + 0.5f); L.sat[o + k] = L.sat[u + k] + row[k]; }
        }
    }
}

inline void qMean(const PLevel &L, int x, int y, int rad, float *out)
{
    const int w = L.w, c = L.c;
    const int x0 = std::max(0, x - rad), y0 = std::max(0, y - rad), x1 = std::min(L.w - 1, x + rad), y1 = std::min(L.h - 1, y + rad);
    const double n = double(x1 - x0 + 1) * double(y1 - y0 + 1);
    const size_t a = (size_t(y1 + 1) * (w + 1) + (x1 + 1)) * c, b = (size_t(y0) * (w + 1) + (x1 + 1)) * c,
                 d = (size_t(y1 + 1) * (w + 1) + x0) * c, e = (size_t(y0) * (w + 1) + x0) * c;
    for (int k = 0; k < c; ++k) out[k] = float(double(L.sat[a + k] - L.sat[b + k] - L.sat[d + k] + L.sat[e + k]) / n);
}

// the distance between the target patch around (tx, ty) and the source patch at (sx, sy) copied with transform `ti`, after the colour shift (returned in `bias`):
// the shift is the difference of the two patches' mean colours (the source's over its window at its own size), limited to kMaxBias; the patches are then
// compared pixel by pixel (stopping early once past `cutoff`), and a large shift costs a little. Each transform costs its factor.
float qDist(const PLevel &L, const std::vector<QXf> &tab, int tx, int ty, int sx, int sy, int ti, int r, float *bias, float cutoff = 1e30f)
{
    const int c = L.c, w = L.w, h = L.h;
    const QXf &t = tab[size_t(ti)];
    float mt[4], ms[4];
    qMean(L, tx, ty, r, mt);
    qMean(L, sx, sy, int(std::lround(float(r) * std::sqrt(t.a * t.a + t.c * t.c))), ms);
    float bcost = 0.0f;
    for (int k = 0; k < c; ++k) {
        bias[k] = std::max(-kMaxBias, std::min(kMaxBias, mt[k] - ms[k]));
        bcost += bias[k] * bias[k];
    }
    const float limit = cutoff / t.cost;
    float d = kBiasCost * bcost * float((2 * r + 1) * (2 * r + 1));
    if (d > limit) return d * t.cost;
    float sv[4];
    for (int dy = -r; dy <= r; ++dy) {
        const int y = ty + dy;
        if (y < 0 || y >= h) continue;
        for (int dx = -r; dx <= r; ++dx) {
            const int x = tx + dx;
            if (x < 0 || x >= w) continue;
            const float *pt = &L.img[(size_t(y) * w + x) * c];
            const float *ps;
            if (ti == 0) ps = &L.img[(size_t(sy + dy) * w + (sx + dx)) * c];
            else { qSample(L, float(sx) + t.a * dx + t.b * dy, float(sy) + t.c * dx + t.d * dy, sv); ps = sv; }
            float e2 = 0.0f;
            for (int k = 0; k < c; ++k) { const float e = pt[k] - ps[k] - bias[k]; e2 += e * e; }
            d += L.conf[size_t(y) * w + x] * e2;
        }
        if (d > limit) return d * t.cost;
    }
    return d * t.cost;
}

inline void qTry(PLevel &L, const std::vector<QXf> &tab, size_t i, int x, int y, int sx, int sy, int ti, int r)
{
    if (!qValid(L, sx, sy, tab[size_t(ti)])) return;
    if (sx == L.nx[i] && sy == L.ny[i] && ti == L.xf[i]) return;
    float b[4];
    const float d = qDist(L, tab, x, y, sx, sy, ti, r, b, L.nd[i]);
    if (d < L.nd[i]) {
        L.nd[i] = d; L.nx[i] = sx; L.ny[i] = sy; L.xf[i] = uint8_t(ti);
        for (int k = 0; k < L.c; ++k) L.bias[i * L.c + k] = b[k];
    }
}

void qPass(PLevel &L, const std::vector<QXf> &tab, int r, bool forward, int ya, int yb, unsigned seed, bool tryTransforms = true)
{
    std::mt19937 rng(seed);
    const int w = L.w;
    const int maxR = std::max(L.w, L.h);
    const int nT = int(tab.size());
    const int sY = forward ? ya : yb - 1, eY = forward ? yb : ya - 1, st = forward ? 1 : -1;
    for (int y = sY; y != eY; y += st) {
        const int sX = forward ? 0 : w - 1, eX = forward ? w : -1;
        for (int x = sX; x != eX; x += st) {
            const size_t i = size_t(y) * w + x;
            if (!L.tgt[i]) continue;
            // propagation: the neighbour's match, moved by one step in the neighbour's transform
            const int px = x - st, py = y - st;
            if (px >= 0 && px < w && L.tgt[size_t(y) * w + px]) {
                const size_t q = size_t(y) * w + px;
                const QXf &t = tab[L.xf[q]];
                qTry(L, tab, i, x, y, int(std::lround(float(L.nx[q]) + t.a * st)), int(std::lround(float(L.ny[q]) + t.c * st)), L.xf[q], r);
            }
            if (py >= 0 && py < L.h && L.tgt[size_t(py) * w + x]) {
                const size_t q = size_t(py) * w + x;
                const QXf &t = tab[L.xf[q]];
                qTry(L, tab, i, x, y, int(std::lround(float(L.nx[q]) + t.b * st)), int(std::lround(float(L.ny[q]) + t.d * st)), L.xf[q], r);
            }
            // random search: around the current match, shrinking, with the same transform; and a few other transforms at the current spot
            for (int rad = maxR; rad >= 1; rad /= 2) {
                const int sx = L.nx[i] + int(rng() % unsigned(2 * rad + 1)) - rad;
                const int sy = L.ny[i] + int(rng() % unsigned(2 * rad + 1)) - rad;
                qTry(L, tab, i, x, y, sx, sy, L.xf[i], r);
            }
            // Krimble 2026-10-20: at full size the transforms found on the smaller sizes are kept and refined, not searched again (speed)
            if (tryTransforms) {
                qTry(L, tab, i, x, y, L.nx[i], L.ny[i], int(rng() % unsigned(nT)), r);
                if (L.xf[i] != 0) qTry(L, tab, i, x, y, L.nx[i], L.ny[i], 0, r);
            }
        }
    }
}

void qMatch(PLevel &L, const std::vector<QXf> &tab, int r, int iterations, int threads, unsigned &seed, bool tryTransforms = true)
{
    for (int it = 0; it < iterations; ++it) {
        const bool fwd = (it & 1) == 0;
        pParallel(L.h, threads, [&](int a, int b) { qPass(L, tab, r, fwd, a, b, seed + unsigned(a) * 7919u, tryTransforms); });
        seed += 104729u;
    }
}

void qRemeasure(PLevel &L, const std::vector<QXf> &tab, int r, int threads)
{
    qBuildSat(L);   // the picture (the hole) has changed since the last sums
    pParallel(int(L.tgt.size()), threads, [&](int a, int b) {
        for (int i = a; i < b; ++i) {
            const size_t u = size_t(i);
            if (!L.tgt[u]) continue;
            if (!qValid(L, L.nx[u], L.ny[u], tab[L.xf[u]])) { L.nd[u] = 1e30f; continue; }
            float bb[4];
            L.nd[u] = qDist(L, tab, i % L.w, i / L.w, L.nx[u], L.ny[u], L.xf[u], r, bb);
            for (int k = 0; k < L.c; ++k) L.bias[u * L.c + k] = bb[k];
        }
    });
}

// the hole rebuilt from the matches: the weighted mean of what the covering patches say (`winner` false), or the value of the one best covering patch (`winner` true)
void qVote(PLevel &L, const std::vector<QXf> &tab, int r, bool winner, int threads)
{
    const int c = L.c, w = L.w, h = L.h;
    std::vector<float> ds;
    for (size_t i = 0; i < L.tgt.size(); ++i) if (L.tgt[i] && L.nd[i] < 1e29f) ds.push_back(L.nd[i]);
    if (ds.empty()) return;
    std::nth_element(ds.begin(), ds.begin() + ds.size() * 3 / 4, ds.end());
    const float sigma2 = std::max(ds[ds.size() * 3 / 4], 1.0f);
    std::vector<float> out(L.img);
    pParallel(h, threads, [&](int ya, int yb) {
        float v[4];
        for (int y = ya; y < yb; ++y)
            for (int x = 0; x < w; ++x) {
                const size_t p = size_t(y) * w + x;
                if (!L.hole[p]) continue;
                float acc[4] = {0, 0, 0, 0}, ws = 0.0f, best = 1e30f;
                for (int dy = -r; dy <= r; ++dy) {
                    const int qy = y - dy;
                    if (qy < 0 || qy >= h) continue;
                    for (int dx = -r; dx <= r; ++dx) {
                        const int qx = x - dx;
                        if (qx < 0 || qx >= w) continue;
                        const size_t q = size_t(qy) * w + qx;
                        if (!L.tgt[q] || L.nd[q] >= 1e29f) continue;
                        qValue(L, tab, q, dx, dy, v);
                        if (winner) {
                            const float score = L.nd[q] * (1.0f + 0.02f * float(dx * dx + dy * dy));
                            if (score < best) { best = score; for (int k = 0; k < c; ++k) acc[k] = v[k]; ws = 1.0f; }
                        } else {
                            const float wq = std::exp(-L.nd[q] / (2.0f * sigma2));
                            for (int k = 0; k < c; ++k) acc[k] += wq * v[k];
                            ws += wq;
                        }
                    }
                }
                if (ws > 0.0f) for (int k = 0; k < c; ++k) out[p * c + k] = acc[k] / ws;
            }
    });
    L.img.swap(out);
}

// the final rebuild (each hole pixel from its best covering patch) with the seam taken away (see pFinalSeamless: the same, with the transformed, colour-shifted patches)
void qFinalSeamless(PLevel &L, const std::vector<QXf> &tab, int r, int threads)
{
    const int c = L.c, w = L.w, h = L.h;
    std::vector<float> pred(L.img.size(), 0.0f);
    std::vector<uint8_t> has(L.hole.size(), 0);
    pParallel(h, threads, [&](int ya, int yb) {
        float v[4];
        for (int y = ya; y < yb; ++y)
            for (int x = 0; x < w; ++x) {
                const size_t p = size_t(y) * w + x;
                if (!L.tgt[p]) continue;
                float best = 1e30f;
                for (int dy = -r; dy <= r; ++dy) {
                    const int qy = y - dy;
                    if (qy < 0 || qy >= h) continue;
                    for (int dx = -r; dx <= r; ++dx) {
                        const int qx = x - dx;
                        if (qx < 0 || qx >= w) continue;
                        const size_t q = size_t(qy) * w + qx;
                        if (!L.tgt[q] || L.nd[q] >= 1e29f) continue;
                        if (!L.hole[p] && !L.hole[q]) continue;
                        const float score = L.nd[q] * (1.0f + 0.02f * float(dx * dx + dy * dy));
                        if (score < best) {
                            best = score;
                            qValue(L, tab, q, dx, dy, v, true);
                            for (int k = 0; k < c; ++k) pred[p * c + k] = v[k];
                            has[p] = 1;
                        }
                    }
                }
            }
    });
    std::vector<float> off(L.img.size(), 0.0f);
    std::vector<uint8_t> fixedRing(L.hole.size(), 0);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const size_t p = size_t(y) * w + x;
            if (L.hole[p] || !has[p]) continue;
            bool nearHole = false;
            for (int dy = -1; dy <= 1 && !nearHole; ++dy)
                for (int dx = -1; dx <= 1; ++dx) {
                    const int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && ny >= 0 && nx < w && ny < h && L.hole[size_t(ny) * w + nx]) { nearHole = true; break; }
                }
            if (!nearHole) continue;
            fixedRing[p] = 1;
            for (int k = 0; k < c; ++k) off[p * c + k] = L.img[p * c + k] - pred[p * c + k];
        }
    for (size_t p = 0; p < L.hole.size(); ++p)
        if (L.hole[p] && has[p]) for (int k = 0; k < c; ++k) L.img[p * c + k] = pred[p * c + k];
    std::vector<uint8_t> known(fixedRing);
    // Krimble 2026-10-20: the peel only looks inside the hole's box (it scanned the whole picture once a ring: about 6 s for a big hole)
    int bx0 = w, by0 = h, bx1 = -1, by1 = -1;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (L.hole[size_t(y) * w + x]) { bx0 = std::min(bx0, x); bx1 = std::max(bx1, x); by0 = std::min(by0, y); by1 = std::max(by1, y); }
    bool changed = bx1 >= 0;
    while (changed) {
        changed = false;
        std::vector<size_t> wave;
        for (int y = by0; y <= by1; ++y)
            for (int x = bx0; x <= bx1; ++x) {
                const size_t p = size_t(y) * w + x;
                if (!L.hole[p] || known[p]) continue;
                float acc[4] = {0, 0, 0, 0};
                int n = 0;
                const int nbx[4] = {x - 1, x + 1, x, x}, nby[4] = {y, y, y - 1, y + 1};
                for (int j = 0; j < 4; ++j) {
                    if (nbx[j] < 0 || nby[j] < 0 || nbx[j] >= w || nby[j] >= h) continue;
                    const size_t q = size_t(nby[j]) * w + nbx[j];
                    if (!known[q]) continue;
                    for (int k = 0; k < c; ++k) acc[k] += off[q * c + k];
                    ++n;
                }
                if (n) { for (int k = 0; k < c; ++k) off[p * c + k] = acc[k] / n; wave.push_back(p); }
            }
        for (size_t p : wave) { known[p] = 1; changed = true; }
    }
    std::vector<size_t> holeList;
    for (size_t p = 0; p < L.hole.size(); ++p) if (L.hole[p]) holeList.push_back(p);
    // Krimble 2026-10-20: red-black order, each colour in parallel (the same result as the one-by-one sweep converges to, about 4 times faster on
    // the phone's 4 cores), and fewer rounds for a very big hole (the peeled guess above is already close).
    std::vector<size_t> redBlack[2];
    for (size_t p : holeList) redBlack[((p % size_t(w)) + (p / size_t(w))) & 1].push_back(p);
    const int rounds = holeList.size() > 60000 ? 200 : 400;
    for (int it = 0; it < rounds; ++it)
        for (int colour = 0; colour < 2; ++colour) {
            const std::vector<size_t> &list = redBlack[colour];
            pParallel(int(list.size()), threads, [&](int a, int b) {
                for (int idx = a; idx < b; ++idx) {
                    const size_t p = list[size_t(idx)];
                    const int x = int(p % size_t(w)), y = int(p / size_t(w));
                    float acc[4] = {0, 0, 0, 0};
                    int n = 0;
                    const int nbx[4] = {x - 1, x + 1, x, x}, nby[4] = {y, y, y - 1, y + 1};
                    for (int jn = 0; jn < 4; ++jn) {
                        if (nbx[jn] < 0 || nby[jn] < 0 || nbx[jn] >= w || nby[jn] >= h) continue;
                        const size_t q = size_t(nby[jn]) * w + nbx[jn];
                        if (!L.hole[q] && !fixedRing[q]) continue;
                        for (int k = 0; k < c; ++k) acc[k] += off[q * c + k];
                        ++n;
                    }
                    if (!n) continue;
                    for (int k = 0; k < c; ++k) off[p * c + k] += 1.9f * (acc[k] / n - off[p * c + k]);
                }
            });
        }
    if (false)   // Krimble 2026-10-20: the one-by-one sweep (kept, not run)
    for (int it = 0; it < 400; ++it)
        for (size_t p : holeList) {
            const int x = int(p % size_t(w)), y = int(p / size_t(w));
            float acc[4] = {0, 0, 0, 0};
            int n = 0;
            const int nbx[4] = {x - 1, x + 1, x, x}, nby[4] = {y, y, y - 1, y + 1};
            for (int j = 0; j < 4; ++j) {
                if (nbx[j] < 0 || nby[j] < 0 || nbx[j] >= w || nby[j] >= h) continue;
                const size_t q = size_t(nby[j]) * w + nbx[j];
                if (!L.hole[q] && !fixedRing[q]) continue;
                for (int k = 0; k < c; ++k) acc[k] += off[q * c + k];
                ++n;
            }
            if (!n) continue;
            for (int k = 0; k < c; ++k) off[p * c + k] += 1.9f * (acc[k] / n - off[p * c + k]);
        }
    for (size_t p : holeList) for (int k = 0; k < c; ++k) L.img[p * c + k] += off[p * c + k];
}

} // namespace

// Krimble 2026-10-20: the Photoshop-style engine of the 208th to 225th entries, kept; the engine used now is inpaint() at the end of this file (the same,
// in less memory).
bool inpaintPhotoshopV2(uint8_t *pixels, const uint8_t *holeIn, int w, int h, int channels, const Options &opt, uint8_t *filledHole)
{
    if (!pixels || !holeIn || w < 8 || h < 8 || channels < 1 || channels > 4) return false;
    const int r = std::max(2, std::min(5, opt.patchRadius));
    // const int T = opt.threads > 0 ? opt.threads : int(std::max(1u, std::min(4u, std::thread::hardware_concurrency())));
    // Krimble 2026-10-20: George: "Use as many threads as it takes. That's what multi-core is for." Every core the device has (was at most 4).
    const int T = opt.threads > 0 ? opt.threads : int(std::max(1u, std::thread::hardware_concurrency()));
    const std::vector<QXf> &tab = qTable(r);
    unsigned seed = opt.seed;

    std::vector<PLevel> pyr(1);
    PLevel &L0 = pyr[0];
    L0.w = w; L0.h = h; L0.c = channels;
    L0.img.resize(size_t(w) * h * channels);
    for (size_t i = 0; i < L0.img.size(); ++i) L0.img[i] = pixels[i];
    L0.hole.assign(size_t(w) * h, 0);
    for (size_t i = 0; i < L0.hole.size(); ++i) L0.hole[i] = holeIn[i] ? 1 : 0;
    for (int pass = 0; pass < opt.expand; ++pass) {
        std::vector<uint8_t> grown = L0.hole;
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                if (L0.hole[size_t(y) * w + x])
                    for (int dy = -1; dy <= 1; ++dy)
                        for (int dx = -1; dx <= 1; ++dx) {
                            const int nx = x + dx, ny = y + dy;
                            if (nx >= 0 && ny >= 0 && nx < w && ny < h) grown[size_t(ny) * w + nx] = 1;
                        }
        L0.hole.swap(grown);
    }
    int minX = w, minY = h, maxX = -1, maxY = -1;
    size_t holeCount = 0;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (L0.hole[size_t(y) * w + x]) { minX = std::min(minX, x); maxX = std::max(maxX, x); minY = std::min(minY, y); maxY = std::max(maxY, y); ++holeCount; }
    if (holeCount == 0) return true;
    if (holeCount >= size_t(w) * h) return false;

    const int holeDim = std::max(maxX - minX + 1, maxY - minY + 1);
    int levels = 1;
    while (levels < std::max(opt.maxLevels, 10) && (holeDim >> (levels - 1)) > 24 && (std::min(w, h) >> levels) >= 8 * (2 * r + 1)) ++levels;
    for (int i = 1; i < levels; ++i) pyr.push_back(pDown(pyr[size_t(i - 1)]));

    for (int lv = levels - 1; lv >= 0; --lv) {
        PLevel &L = pyr[size_t(lv)];
        pBuild(L, r);
        qFreeRadius(L);
        L.xf.assign(size_t(L.w) * L.h, 0);
        L.bias.assign(size_t(L.w) * L.h * L.c, 0.0f);
        if (L.srcList.empty()) {
            if (lv == levels - 1) pPeel(L);
            continue;
        }
        std::mt19937 rng(seed + unsigned(lv));
        auto randomSource = [&](int &sx, int &sy) {
            for (int tries = 0; tries < 64; ++tries) {
                const int s = L.srcList[rng() % L.srcList.size()];
                sx = s % L.w; sy = s / L.w;
                if (qValid(L, sx, sy, tab[0])) return;
            }
        };
        if (lv == levels - 1) {
            pPeel(L);
            for (size_t i = 0; i < L.tgt.size(); ++i) {
                if (!L.tgt[i]) continue;
                int sx = 0, sy = 0;
                randomSource(sx, sy);
                L.nx[i] = sx; L.ny[i] = sy; L.xf[i] = 0;
            }
        } else {
            const PLevel &C = pyr[size_t(lv + 1)];
            for (int y = 0; y < L.h; ++y)
                for (int x = 0; x < L.w; ++x) {
                    const size_t i = size_t(y) * L.w + x;
                    if (!L.tgt[i]) continue;
                    const int cx = std::min(C.w - 1, x / 2), cy = std::min(C.h - 1, y / 2);
                    const size_t ci = size_t(cy) * C.w + cx;
                    int sx = -1, sy = -1, ti = 0;
                    if (C.tgt[ci] && C.nd[ci] < 1e29f) {
                        const QXf &t = tab[C.xf[ci]];
                        const int ox = x - cx * 2, oy = y - cy * 2;
                        sx = int(std::lround(float(C.nx[ci] * 2) + t.a * ox + t.b * oy));
                        sy = int(std::lround(float(C.ny[ci] * 2) + t.c * ox + t.d * oy));
                        ti = C.xf[ci];
                        for (int k = 0; k < L.c; ++k) L.bias[i * L.c + k] = C.bias[ci * C.c + k];
                    } else if (!L.hole[i]) { sx = x; sy = y; }
                    if (!qValid(L, sx, sy, tab[size_t(ti)])) { ti = 0; if (!qValid(L, sx, sy, tab[0])) randomSource(sx, sy); }
                    L.nx[i] = sx; L.ny[i] = sy; L.xf[i] = uint8_t(ti);
                }
            for (size_t i = 0; i < L.tgt.size(); ++i) if (L.tgt[i]) L.nd[i] = 1.0f;
            qVote(L, tab, r, false, T);
        }
        const int emIters = (lv == levels - 1) ? 10 : (lv == 0 ? 3 : 5);
        pSetConfidence(L, (lv == levels - 1) ? 0.15f : 0.6f);
        qRemeasure(L, tab, r, T);
        for (int it = 0; it < emIters; ++it) {
            if (lv == levels - 1) pSetConfidence(L, 0.15f + 0.6f * float(it) / float(emIters));
            // qMatch(L, tab, r, 2, T, seed);
            // Krimble 2026-10-20: George (1.0.40): "Content-Aware fill does nothing now." A big hole took about 20 s or more (one core, a PC) with no sign of
            // work. At full size: one PatchMatch pass per round instead of two, and no new transforms tried (those of the smaller sizes are kept).
            if (lv == 0) qMatch(L, tab, r, 1, T, seed, false);
            else qMatch(L, tab, r, 2, T, seed);
            if (opt.progress && !opt.progress(float(levels - 1 - lv) / float(levels) + (float(it + 1) / float(emIters)) / float(levels))) return false;
            const bool last = (lv == 0 && it == emIters - 1);
            if (last) qFinalSeamless(L, tab, r, T);
            else { qVote(L, tab, r, false, T); qRemeasure(L, tab, r, T); }
        }
    }

    const PLevel &F = pyr[0];
    if (filledHole) for (size_t i = 0; i < F.hole.size(); ++i) filledHole[i] = F.hole[i] ? 1 : 0;
    for (size_t i = 0; i < F.hole.size(); ++i) {
        if (!F.hole[i]) continue;
        for (int k = 0; k < channels; ++k)
            pixels[i * channels + k] = uint8_t(std::max(0.0f, std::min(255.0f, F.img[i * channels + k] + 0.5f)));
    }
    return true;
}


// ====================================================================================================================================================================
// Krimble 2026-10-20: the engine used now: the Photoshop-style engine above (now inpaintPhotoshopV2, kept), the same steps and results, kept in less memory.
// George, asked whether the fill could use less memory (it needed about 450 MB at its peak for a 3 MP area): "Yes." Per pixel of each level:
//  - the picture in bytes, not floats (4 bytes a pixel instead of 16; the rebuilt hole is rounded to whole levels of 0 to 255, as it is saved anyway);
//  - the match positions as 16-bit numbers (pictures up to 32 767 px a side), the colour shifts as signed bytes (they are at most 28), the distance map
//    as 16-bit numbers;
//  - no per-pixel weight array (a pixel's weight is 1, or the hole's weight for the whole level), no list of every valid source (a random source is found
//    by trying random spots);
//  - the seam correction works only inside the hole's box;
//  - a smaller size's arrays are freed as soon as the next size has taken its matches.
// About 55 bytes a pixel of the full size at the peak (was about 150), so the area around the hole can be bigger again (krimble_fill.cpp).
// ====================================================================================================================================================================
namespace {

struct RLevel {
    int w = 0, h = 0, c = 0;
    std::vector<uint8_t> img;      // w * h * c
    std::vector<uint8_t> hole;     // 1 = to be filled
    std::vector<uint8_t> tgt;      // 1 = the patch centred here touches the hole
    std::vector<int16_t> nx, ny;   // the match of every target
    std::vector<float> nd;         // its distance
    std::vector<uint8_t> xf;       // its transform (index into the table)
    std::vector<int8_t> bias;      // its colour shift, per channel (w * h * c)
    std::vector<uint16_t> freeR;   // Chebyshev distance to the nearest hole pixel or the picture's edge (capped at 65535)
    std::vector<int32_t> sat;      // window sums of the picture ((w + 1) * (h + 1) * c)
    float holeWeight = 1.0f;       // how much a hole pixel counts in the comparisons (known pixels count 1)
    size_t sourceCount = 0;        // how many valid sources there are (plain copies)
    inline float conf(size_t i) const { return hole[i] ? holeWeight : 1.0f; }
};

inline uint8_t rClamp(float v) { return uint8_t(std::max(0.0f, std::min(255.0f, v + 0.5f))); }

void rBuild(RLevel &L, int r)
{
    const int w = L.w, h = L.h;
    std::vector<int> sat(size_t(w + 1) * (h + 1), 0);
    for (int y = 0; y < h; ++y) {
        int row = 0;
        for (int x = 0; x < w; ++x) {
            row += L.hole[size_t(y) * w + x] ? 1 : 0;
            sat[size_t(y + 1) * (w + 1) + (x + 1)] = sat[size_t(y) * (w + 1) + (x + 1)] + row;
        }
    }
    auto holesIn = [&](int x0, int y0, int x1, int y1) {
        x0 = std::max(0, x0); y0 = std::max(0, y0); x1 = std::min(w - 1, x1); y1 = std::min(h - 1, y1);
        if (x1 < x0 || y1 < y0) return 0;
        return sat[size_t(y1 + 1) * (w + 1) + (x1 + 1)] - sat[size_t(y0) * (w + 1) + (x1 + 1)] - sat[size_t(y1 + 1) * (w + 1) + x0] + sat[size_t(y0) * (w + 1) + x0];
    };
    L.tgt.assign(size_t(w) * h, 0);
    L.sourceCount = 0;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const size_t i = size_t(y) * w + x;
            const int holes = holesIn(x - r, y - r, x + r, y + r);
            if (holes > 0) L.tgt[i] = 1;
            else if (x >= r && y >= r && x < w - r && y < h - r) ++L.sourceCount;
        }
    L.nx.assign(size_t(w) * h, 0);
    L.ny.assign(size_t(w) * h, 0);
    L.nd.assign(size_t(w) * h, 1e30f);
    L.xf.assign(size_t(w) * h, 0);
    L.bias.assign(size_t(w) * h * L.c, 0);
}

void rFreeRadius(RLevel &L)
{
    const int w = L.w, h = L.h;
    std::vector<int> fr(size_t(w) * h, 1 << 20);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const size_t i = size_t(y) * w + x;
            if (L.hole[i]) { fr[i] = 0; continue; }
            int v = std::min(std::min(x + 1, y + 1), std::min(w - x, h - y));
            if (x > 0) v = std::min(v, fr[i - 1] + 1);
            if (y > 0) {
                v = std::min(v, fr[i - w] + 1);
                if (x > 0) v = std::min(v, fr[i - w - 1] + 1);
                if (x < w - 1) v = std::min(v, fr[i - w + 1] + 1);
            }
            fr[i] = v;
        }
    for (int y = h - 1; y >= 0; --y)
        for (int x = w - 1; x >= 0; --x) {
            const size_t i = size_t(y) * w + x;
            int v = fr[i];
            if (x < w - 1) v = std::min(v, fr[i + 1] + 1);
            if (y < h - 1) {
                v = std::min(v, fr[i + w] + 1);
                if (x < w - 1) v = std::min(v, fr[i + w + 1] + 1);
                if (x > 0) v = std::min(v, fr[i + w - 1] + 1);
            }
            fr[i] = v;
        }
    L.freeR.resize(fr.size());
    for (size_t i = 0; i < fr.size(); ++i) L.freeR[i] = uint16_t(std::min(fr[i], 65535));
}

inline bool rValid(const RLevel &L, int sx, int sy, const QXf &t)
{
    if (sx < 0 || sy < 0 || sx >= L.w || sy >= L.h) return false;
    return float(L.freeR[size_t(sy) * L.w + sx]) > t.reach;
}

void rBuildSat(RLevel &L)
{
    const int w = L.w, h = L.h, c = L.c;
    L.sat.assign(size_t(w + 1) * (h + 1) * c, 0);
    for (int y = 0; y < h; ++y) {
        int32_t row[4] = {0, 0, 0, 0};
        for (int x = 0; x < w; ++x) {
            const size_t o = (size_t(y + 1) * (w + 1) + (x + 1)) * c, u = (size_t(y) * (w + 1) + (x + 1)) * c;
            for (int k = 0; k < c; ++k) { row[k] += L.img[(size_t(y) * w + x) * c + k]; L.sat[o + k] = L.sat[u + k] + row[k]; }
        }
    }
}

inline void rMean(const RLevel &L, int x, int y, int rad, float *out)
{
    const int w = L.w, c = L.c;
    const int x0 = std::max(0, x - rad), y0 = std::max(0, y - rad), x1 = std::min(L.w - 1, x + rad), y1 = std::min(L.h - 1, y + rad);
    const double n = double(x1 - x0 + 1) * double(y1 - y0 + 1);
    const size_t a = (size_t(y1 + 1) * (w + 1) + (x1 + 1)) * c, b = (size_t(y0) * (w + 1) + (x1 + 1)) * c,
                 d = (size_t(y1 + 1) * (w + 1) + x0) * c, e = (size_t(y0) * (w + 1) + x0) * c;
    for (int k = 0; k < c; ++k) out[k] = float(double(L.sat[a + k] - L.sat[b + k] - L.sat[d + k] + L.sat[e + k]) / n);
}

inline void rSample(const RLevel &L, float fx, float fy, float *out)
{
    const int c = L.c, w = L.w;
    const int x0 = int(fx), y0 = int(fy);
    const float ax = fx - float(x0), ay = fy - float(y0);
    const uint8_t *p00 = &L.img[(size_t(y0) * w + x0) * c];
    const uint8_t *p10 = p00 + c, *p01 = p00 + size_t(w) * c, *p11 = p01 + c;
    for (int k = 0; k < c; ++k)
        out[k] = (1 - ay) * ((1 - ax) * p00[k] + ax * p10[k]) + ay * ((1 - ax) * p01[k] + ax * p11[k]);
}

inline void rValue(const RLevel &L, const std::vector<QXf> &tab, size_t q, int dx, int dy, float *out, bool nearest = false)
{
    const QXf &t = tab[L.xf[q]];
    if (L.xf[q] == 0 || nearest) {
        int sx = L.nx[q] + dx, sy = L.ny[q] + dy;
        if (L.xf[q] != 0) { sx = int(std::lround(float(L.nx[q]) + t.a * dx + t.b * dy)); sy = int(std::lround(float(L.ny[q]) + t.c * dx + t.d * dy)); }
        const uint8_t *p = &L.img[(size_t(sy) * L.w + sx) * L.c];
        for (int k = 0; k < L.c; ++k) out[k] = float(p[k]) + float(L.bias[q * L.c + k]);
        return;
    }
    rSample(L, float(L.nx[q]) + t.a * dx + t.b * dy, float(L.ny[q]) + t.c * dx + t.d * dy, out);
    for (int k = 0; k < L.c; ++k) out[k] += float(L.bias[q * L.c + k]);
}

float rDist(const RLevel &L, const std::vector<QXf> &tab, int tx, int ty, int sx, int sy, int ti, int r, int8_t *bias, float cutoff = 1e30f)
{
    const int c = L.c, w = L.w, h = L.h;
    const QXf &t = tab[size_t(ti)];
    float mt[4], ms[4], b[4];
    rMean(L, tx, ty, r, mt);
    rMean(L, sx, sy, int(std::lround(float(r) * std::sqrt(t.a * t.a + t.c * t.c))), ms);
    float bcost = 0.0f;
    for (int k = 0; k < c; ++k) {
        b[k] = std::round(std::max(-kMaxBias, std::min(kMaxBias, mt[k] - ms[k])));
        bias[k] = int8_t(b[k]);
        bcost += b[k] * b[k];
    }
    const float limit = cutoff / t.cost;
    float d = kBiasCost * bcost * float((2 * r + 1) * (2 * r + 1));
    if (d > limit) return d * t.cost;
    float sv[4];
    for (int dy = -r; dy <= r; ++dy) {
        const int y = ty + dy;
        if (y < 0 || y >= h) continue;
        for (int dx = -r; dx <= r; ++dx) {
            const int x = tx + dx;
            if (x < 0 || x >= w) continue;
            const uint8_t *pt = &L.img[(size_t(y) * w + x) * c];
            if (ti == 0) {
                const uint8_t *ps = &L.img[(size_t(sy + dy) * w + (sx + dx)) * c];
                for (int k = 0; k < c; ++k) sv[k] = ps[k];
            } else {
                rSample(L, float(sx) + t.a * dx + t.b * dy, float(sy) + t.c * dx + t.d * dy, sv);
            }
            float e2 = 0.0f;
            for (int k = 0; k < c; ++k) { const float e = float(pt[k]) - sv[k] - b[k]; e2 += e * e; }
            d += L.conf(size_t(y) * w + x) * e2;
        }
        if (d > limit) return d * t.cost;
    }
    return d * t.cost;
}

inline void rTry(RLevel &L, const std::vector<QXf> &tab, size_t i, int x, int y, int sx, int sy, int ti, int r)
{
    if (!rValid(L, sx, sy, tab[size_t(ti)])) return;
    if (sx == L.nx[i] && sy == L.ny[i] && ti == L.xf[i]) return;
    int8_t b[4];
    const float d = rDist(L, tab, x, y, sx, sy, ti, r, b, L.nd[i]);
    if (d < L.nd[i]) {
        L.nd[i] = d; L.nx[i] = int16_t(sx); L.ny[i] = int16_t(sy); L.xf[i] = uint8_t(ti);
        for (int k = 0; k < L.c; ++k) L.bias[i * L.c + k] = b[k];
    }
}

void rPass(RLevel &L, const std::vector<QXf> &tab, int r, bool forward, int ya, int yb, unsigned seed, bool tryTransforms)
{
    std::mt19937 rng(seed);
    const int w = L.w;
    const int maxR = std::max(L.w, L.h);
    const int nT = int(tab.size());
    const int sY = forward ? ya : yb - 1, eY = forward ? yb : ya - 1, st = forward ? 1 : -1;
    for (int y = sY; y != eY; y += st) {
        const int sX = forward ? 0 : w - 1, eX = forward ? w : -1;
        for (int x = sX; x != eX; x += st) {
            const size_t i = size_t(y) * w + x;
            if (!L.tgt[i]) continue;
            const int px = x - st, py = y - st;
            if (px >= 0 && px < w && L.tgt[size_t(y) * w + px]) {
                const size_t q = size_t(y) * w + px;
                const QXf &t = tab[L.xf[q]];
                rTry(L, tab, i, x, y, int(std::lround(float(L.nx[q]) + t.a * st)), int(std::lround(float(L.ny[q]) + t.c * st)), L.xf[q], r);
            }
            if (py >= 0 && py < L.h && L.tgt[size_t(py) * w + x]) {
                const size_t q = size_t(py) * w + x;
                const QXf &t = tab[L.xf[q]];
                rTry(L, tab, i, x, y, int(std::lround(float(L.nx[q]) + t.b * st)), int(std::lround(float(L.ny[q]) + t.d * st)), L.xf[q], r);
            }
            for (int rad = maxR; rad >= 1; rad /= 2) {
                const int sx = L.nx[i] + int(rng() % unsigned(2 * rad + 1)) - rad;
                const int sy = L.ny[i] + int(rng() % unsigned(2 * rad + 1)) - rad;
                rTry(L, tab, i, x, y, sx, sy, L.xf[i], r);
            }
            if (tryTransforms) {
                rTry(L, tab, i, x, y, L.nx[i], L.ny[i], int(rng() % unsigned(nT)), r);
                if (L.xf[i] != 0) rTry(L, tab, i, x, y, L.nx[i], L.ny[i], 0, r);
            }
        }
    }
}

void rMatch(RLevel &L, const std::vector<QXf> &tab, int r, int iterations, int threads, unsigned &seed, bool tryTransforms)
{
    for (int it = 0; it < iterations; ++it) {
        const bool fwd = (it & 1) == 0;
        pParallel(L.h, threads, [&](int a, int b) { rPass(L, tab, r, fwd, a, b, seed + unsigned(a) * 7919u, tryTransforms); });
        seed += 104729u;
    }
}

void rRemeasure(RLevel &L, const std::vector<QXf> &tab, int r, int threads)
{
    rBuildSat(L);
    pParallel(int(L.tgt.size()), threads, [&](int a, int b) {
        for (int i = a; i < b; ++i) {
            const size_t u = size_t(i);
            if (!L.tgt[u]) continue;
            if (!rValid(L, L.nx[u], L.ny[u], tab[L.xf[u]])) { L.nd[u] = 1e30f; continue; }
            int8_t bb[4];
            L.nd[u] = rDist(L, tab, i % L.w, i / L.w, L.nx[u], L.ny[u], L.xf[u], r, bb);
            for (int k = 0; k < L.c; ++k) L.bias[u * L.c + k] = bb[k];
        }
    });
}

// the hole rebuilt as the weighted mean of what the covering patches say; only the hole's pixels are kept aside (not a copy of the whole picture)
void rVote(RLevel &L, const std::vector<QXf> &tab, int r, int threads)
{
    const int c = L.c, w = L.w, h = L.h;
    std::vector<float> ds;
    for (size_t i = 0; i < L.tgt.size(); ++i) if (L.tgt[i] && L.nd[i] < 1e29f) ds.push_back(L.nd[i]);
    if (ds.empty()) return;
    std::nth_element(ds.begin(), ds.begin() + ds.size() * 3 / 4, ds.end());
    const float sigma2 = std::max(ds[ds.size() * 3 / 4], 1.0f);
    std::vector<size_t> holeList;
    for (size_t p = 0; p < L.hole.size(); ++p) if (L.hole[p]) holeList.push_back(p);
    std::vector<uint8_t> out(holeList.size() * c);
    pParallel(int(holeList.size()), threads, [&](int a, int b) {
        float v[4];
        for (int hi = a; hi < b; ++hi) {
            const size_t p = holeList[size_t(hi)];
            const int x = int(p % size_t(w)), y = int(p / size_t(w));
            float acc[4] = {0, 0, 0, 0}, ws = 0.0f;
            for (int dy = -r; dy <= r; ++dy) {
                const int qy = y - dy;
                if (qy < 0 || qy >= h) continue;
                for (int dx = -r; dx <= r; ++dx) {
                    const int qx = x - dx;
                    if (qx < 0 || qx >= w) continue;
                    const size_t q = size_t(qy) * w + qx;
                    if (!L.tgt[q] || L.nd[q] >= 1e29f) continue;
                    rValue(L, tab, q, dx, dy, v);
                    const float wq = std::exp(-L.nd[q] / (2.0f * sigma2));
                    for (int k = 0; k < c; ++k) acc[k] += wq * v[k];
                    ws += wq;
                }
            }
            for (int k = 0; k < c; ++k) out[size_t(hi) * c + k] = ws > 0.0f ? rClamp(acc[k] / ws) : L.img[p * c + k];
        }
    });
    for (size_t hi = 0; hi < holeList.size(); ++hi)
        for (int k = 0; k < c; ++k) L.img[holeList[hi] * c + k] = out[hi * c + k];
}

// the final rebuild with the seam taken away (see qFinalSeamless), worked only inside the hole's box (and the ring around it)
void rFinalSeamless(RLevel &L, const std::vector<QXf> &tab, int r, int threads)
{
    const int c = L.c, w = L.w, h = L.h;
    int bx0 = w, by0 = h, bx1 = -1, by1 = -1;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (L.hole[size_t(y) * w + x]) { bx0 = std::min(bx0, x); bx1 = std::max(bx1, x); by0 = std::min(by0, y); by1 = std::max(by1, y); }
    if (bx1 < 0) return;
    bx0 = std::max(0, bx0 - 1); by0 = std::max(0, by0 - 1); bx1 = std::min(w - 1, bx1 + 1); by1 = std::min(h - 1, by1 + 1);
    const int bw = bx1 - bx0 + 1, bh = by1 - by0 + 1;
    auto B = [&](int x, int y) { return size_t(y - by0) * bw + size_t(x - bx0); };   // index in the box
    std::vector<float> pred(size_t(bw) * bh * c, 0.0f), off(size_t(bw) * bh * c, 0.0f);
    std::vector<uint8_t> has(size_t(bw) * bh, 0), fixedRing(size_t(bw) * bh, 0);
    pParallel(bh, threads, [&](int ya, int yb) {
        float v[4];
        for (int yy = ya; yy < yb; ++yy) {
            const int y = by0 + yy;
            for (int x = bx0; x <= bx1; ++x) {
                const size_t p = size_t(y) * w + x;
                if (!L.tgt[p]) continue;
                float best = 1e30f;
                for (int dy = -r; dy <= r; ++dy) {
                    const int qy = y - dy;
                    if (qy < 0 || qy >= h) continue;
                    for (int dx = -r; dx <= r; ++dx) {
                        const int qx = x - dx;
                        if (qx < 0 || qx >= w) continue;
                        const size_t q = size_t(qy) * w + qx;
                        if (!L.tgt[q] || L.nd[q] >= 1e29f) continue;
                        if (!L.hole[p] && !L.hole[q]) continue;
                        const float score = L.nd[q] * (1.0f + 0.02f * float(dx * dx + dy * dy));
                        if (score < best) {
                            best = score;
                            rValue(L, tab, q, dx, dy, v, true);
                            for (int k = 0; k < c; ++k) pred[B(x, y) * c + k] = v[k];
                            has[B(x, y)] = 1;
                        }
                    }
                }
            }
        }
    });
    for (int y = by0; y <= by1; ++y)
        for (int x = bx0; x <= bx1; ++x) {
            const size_t p = size_t(y) * w + x, b = B(x, y);
            if (L.hole[p] || !has[b]) continue;
            bool nearHole = false;
            for (int dy = -1; dy <= 1 && !nearHole; ++dy)
                for (int dx = -1; dx <= 1; ++dx) {
                    const int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && ny >= 0 && nx < w && ny < h && L.hole[size_t(ny) * w + nx]) { nearHole = true; break; }
                }
            if (!nearHole) continue;
            fixedRing[b] = 1;
            for (int k = 0; k < c; ++k) off[b * c + k] = float(L.img[p * c + k]) - pred[b * c + k];
        }
    std::vector<uint8_t> known(fixedRing);
    bool changed = true;
    while (changed) {
        changed = false;
        std::vector<size_t> wave;
        for (int y = by0; y <= by1; ++y)
            for (int x = bx0; x <= bx1; ++x) {
                const size_t b = B(x, y);
                if (!L.hole[size_t(y) * w + x] || known[b]) continue;
                float acc[4] = {0, 0, 0, 0};
                int n = 0;
                const int nbx[4] = {x - 1, x + 1, x, x}, nby[4] = {y, y, y - 1, y + 1};
                for (int j = 0; j < 4; ++j) {
                    if (nbx[j] < bx0 || nby[j] < by0 || nbx[j] > bx1 || nby[j] > by1) continue;
                    const size_t q = B(nbx[j], nby[j]);
                    if (!known[q]) continue;
                    for (int k = 0; k < c; ++k) acc[k] += off[q * c + k];
                    ++n;
                }
                if (n) { for (int k = 0; k < c; ++k) off[b * c + k] = acc[k] / n; wave.push_back(b); }
            }
        for (size_t b : wave) { known[b] = 1; changed = true; }
    }
    std::vector<int> redBlack[2];   // box indices of the hole's pixels, by colour
    size_t holeCount = 0;
    for (int y = by0; y <= by1; ++y)
        for (int x = bx0; x <= bx1; ++x)
            if (L.hole[size_t(y) * w + x]) { redBlack[(x + y) & 1].push_back(int(B(x, y))); ++holeCount; }
    const int rounds = holeCount > 60000 ? 200 : 400;
    for (int it = 0; it < rounds; ++it)
        for (int colour = 0; colour < 2; ++colour) {
            const std::vector<int> &list = redBlack[colour];
            pParallel(int(list.size()), threads, [&](int a, int bEnd) {
                for (int idx = a; idx < bEnd; ++idx) {
                    const int b = list[size_t(idx)];
                    const int x = bx0 + b % bw, y = by0 + b / bw;
                    float acc[4] = {0, 0, 0, 0};
                    int n = 0;
                    const int nbx[4] = {x - 1, x + 1, x, x}, nby[4] = {y, y, y - 1, y + 1};
                    for (int jn = 0; jn < 4; ++jn) {
                        if (nbx[jn] < bx0 || nby[jn] < by0 || nbx[jn] > bx1 || nby[jn] > by1) continue;
                        const size_t q = B(nbx[jn], nby[jn]);
                        if (!L.hole[size_t(nby[jn]) * w + nbx[jn]] && !fixedRing[q]) continue;
                        for (int k = 0; k < c; ++k) acc[k] += off[q * c + k];
                        ++n;
                    }
                    if (!n) continue;
                    for (int k = 0; k < c; ++k) off[size_t(b) * c + k] += 1.9f * (acc[k] / n - off[size_t(b) * c + k]);
                }
            });
        }
    for (int y = by0; y <= by1; ++y)
        for (int x = bx0; x <= bx1; ++x) {
            const size_t p = size_t(y) * w + x, b = B(x, y);
            if (!L.hole[p] || !has[b]) continue;
            for (int k = 0; k < c; ++k) L.img[p * c + k] = rClamp(pred[b * c + k] + off[b * c + k]);
        }
}

RLevel rDown(const RLevel &F)
{
    RLevel L;
    L.c = F.c; L.w = (F.w + 1) / 2; L.h = (F.h + 1) / 2;
    L.img.assign(size_t(L.w) * L.h * L.c, 0);
    L.hole.assign(size_t(L.w) * L.h, 0);
    for (int y = 0; y < L.h; ++y)
        for (int x = 0; x < L.w; ++x) {
            int acc[4] = {0, 0, 0, 0};
            int n = 0, holes = 0;
            for (int dy = 0; dy < 2; ++dy)
                for (int dx = 0; dx < 2; ++dx) {
                    const int sx = std::min(F.w - 1, 2 * x + dx), sy = std::min(F.h - 1, 2 * y + dy);
                    const size_t s = size_t(sy) * F.w + sx;
                    if (F.hole[s]) { ++holes; continue; }
                    for (int k = 0; k < F.c; ++k) acc[k] += F.img[s * F.c + k];
                    ++n;
                }
            const size_t i = size_t(y) * L.w + x;
            if (holes > 0 || n == 0) L.hole[i] = 1;
            else for (int k = 0; k < L.c; ++k) L.img[i * L.c + k] = uint8_t((acc[k] + n / 2) / n);
        }
    return L;
}

void rPeel(RLevel &L)
{
    const int w = L.w, h = L.h, c = L.c;
    std::vector<uint8_t> known(L.hole.size());
    for (size_t i = 0; i < known.size(); ++i) known[i] = L.hole[i] ? 0 : 1;
    bool changed = true;
    while (changed) {
        changed = false;
        std::vector<uint8_t> next = known;
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                const size_t i = size_t(y) * w + x;
                if (known[i]) continue;
                int acc[4] = {0, 0, 0, 0};
                int n = 0;
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) {
                        const int nx = x + dx, ny = y + dy;
                        if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
                        const size_t q = size_t(ny) * w + nx;
                        if (!known[q]) continue;
                        for (int k = 0; k < c; ++k) acc[k] += L.img[q * c + k];
                        ++n;
                    }
                if (n) { for (int k = 0; k < c; ++k) L.img[i * c + k] = uint8_t((acc[k] + n / 2) / n); next[i] = 1; changed = true; }
            }
        known.swap(next);
    }
}

// a random valid source (a plain copy), found by trying random spots
bool rRandomSource(const RLevel &L, const std::vector<QXf> &tab, std::mt19937 &rng, int &sx, int &sy)
{
    for (int tries = 0; tries < 2000; ++tries) {
        sx = int(rng() % unsigned(L.w));
        sy = int(rng() % unsigned(L.h));
        if (rValid(L, sx, sy, tab[0])) return true;
    }
    return false;
}

} // namespace

bool inpaint(uint8_t *pixels, const uint8_t *holeIn, int w, int h, int channels, const Options &opt, uint8_t *filledHole)
{
    if (!pixels || !holeIn || w < 8 || h < 8 || channels < 1 || channels > 4) return false;
    if (w > 32767 || h > 32767) return inpaintPhotoshopV2(pixels, holeIn, w, h, channels, opt, filledHole);   // the 16-bit match positions
    const int r = std::max(2, std::min(5, opt.patchRadius));
    const int T = opt.threads > 0 ? opt.threads : int(std::max(1u, std::thread::hardware_concurrency()));
    const std::vector<QXf> &tab = qTable(r);
    unsigned seed = opt.seed;

    std::vector<RLevel> pyr(1);
    {
        RLevel &L0 = pyr[0];
        L0.w = w; L0.h = h; L0.c = channels;
        L0.img.assign(pixels, pixels + size_t(w) * h * channels);
        L0.hole.assign(size_t(w) * h, 0);
        for (size_t i = 0; i < L0.hole.size(); ++i) L0.hole[i] = holeIn[i] ? 1 : 0;
        for (int pass = 0; pass < opt.expand; ++pass) {
            std::vector<uint8_t> grown = L0.hole;
            for (int y = 0; y < h; ++y)
                for (int x = 0; x < w; ++x)
                    if (L0.hole[size_t(y) * w + x])
                        for (int dy = -1; dy <= 1; ++dy)
                            for (int dx = -1; dx <= 1; ++dx) {
                                const int nx = x + dx, ny = y + dy;
                                if (nx >= 0 && ny >= 0 && nx < w && ny < h) grown[size_t(ny) * w + nx] = 1;
                            }
            L0.hole.swap(grown);
        }
    }
    int minX = w, minY = h, maxX = -1, maxY = -1;
    size_t holeCount = 0;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (pyr[0].hole[size_t(y) * w + x]) { minX = std::min(minX, x); maxX = std::max(maxX, x); minY = std::min(minY, y); maxY = std::max(maxY, y); ++holeCount; }
    if (holeCount == 0) return true;
    if (holeCount >= size_t(w) * h) return false;

    const int holeDim = std::max(maxX - minX + 1, maxY - minY + 1);
    int levels = 1;
    while (levels < std::max(opt.maxLevels, 10) && (holeDim >> (levels - 1)) > 24 && (std::min(w, h) >> levels) >= 8 * (2 * r + 1)) ++levels;
    for (int i = 1; i < levels; ++i) pyr.push_back(rDown(pyr[size_t(i - 1)]));

    for (int lv = levels - 1; lv >= 0; --lv) {
        RLevel &L = pyr[size_t(lv)];
        rBuild(L, r);
        rFreeRadius(L);
        if (L.sourceCount == 0) {
            if (lv == levels - 1) rPeel(L);
            continue;
        }
        std::mt19937 rng(seed + unsigned(lv));
        if (lv == levels - 1) {
            rPeel(L);
            for (size_t i = 0; i < L.tgt.size(); ++i) {
                if (!L.tgt[i]) continue;
                int sx = 0, sy = 0;
                if (!rRandomSource(L, tab, rng, sx, sy)) continue;
                L.nx[i] = int16_t(sx); L.ny[i] = int16_t(sy); L.xf[i] = 0;
            }
        } else {
            RLevel &C = pyr[size_t(lv + 1)];
            for (int y = 0; y < L.h; ++y)
                for (int x = 0; x < L.w; ++x) {
                    const size_t i = size_t(y) * L.w + x;
                    if (!L.tgt[i]) continue;
                    const int cx = std::min(C.w - 1, x / 2), cy = std::min(C.h - 1, y / 2);
                    const size_t ci = size_t(cy) * C.w + cx;
                    int sx = -1, sy = -1, ti = 0;
                    if (!C.tgt.empty() && C.tgt[ci] && C.nd[ci] < 1e29f) {
                        const QXf &t = tab[C.xf[ci]];
                        const int ox = x - cx * 2, oy = y - cy * 2;
                        sx = int(std::lround(float(C.nx[ci] * 2) + t.a * ox + t.b * oy));
                        sy = int(std::lround(float(C.ny[ci] * 2) + t.c * ox + t.d * oy));
                        ti = C.xf[ci];
                        for (int k = 0; k < L.c; ++k) L.bias[i * L.c + k] = C.bias[ci * C.c + k];
                    } else if (!L.hole[i]) { sx = x; sy = y; }
                    if (!rValid(L, sx, sy, tab[size_t(ti)])) { ti = 0; if (!rValid(L, sx, sy, tab[0])) rRandomSource(L, tab, rng, sx, sy); }
                    L.nx[i] = int16_t(sx); L.ny[i] = int16_t(sy); L.xf[i] = uint8_t(ti);
                }
            // the smaller size is no longer needed: its memory goes back
            RLevel freed;
            std::swap(C, freed);
            for (size_t i = 0; i < L.tgt.size(); ++i) if (L.tgt[i]) L.nd[i] = 1.0f;
            rVote(L, tab, r, T);
        }
        const int emIters = (lv == levels - 1) ? 10 : (lv == 0 ? 3 : 5);
        L.holeWeight = (lv == levels - 1) ? 0.15f : 0.6f;
        rRemeasure(L, tab, r, T);
        for (int it = 0; it < emIters; ++it) {
            if (lv == levels - 1) L.holeWeight = 0.15f + 0.6f * float(it) / float(emIters);
            if (lv == 0) rMatch(L, tab, r, 1, T, seed, false);
            else rMatch(L, tab, r, 2, T, seed, true);
            if (opt.progress && !opt.progress(float(levels - 1 - lv) / float(levels) + (float(it + 1) / float(emIters)) / float(levels))) return false;
            const bool last = (lv == 0 && it == emIters - 1);
            if (last) rFinalSeamless(L, tab, r, T);
            else { rVote(L, tab, r, T); rRemeasure(L, tab, r, T); }
        }
        if (lv > 0) { std::vector<int32_t>().swap(L.sat); std::vector<uint16_t>().swap(L.freeR); }   // not needed by the bigger size
    }

    const RLevel &F = pyr[0];
    if (filledHole) for (size_t i = 0; i < F.hole.size(); ++i) filledHole[i] = F.hole[i] ? 1 : 0;
    for (size_t i = 0; i < F.hole.size(); ++i) {
        if (!F.hole[i]) continue;
        for (int k = 0; k < channels; ++k) pixels[i * channels + k] = F.img[i * channels + k];
    }
    return true;
}

} // namespace KrimbleInpaint
