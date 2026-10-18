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

bool inpaint(uint8_t *pixels, const uint8_t *holeIn, int w, int h, int channels, const Options &opt, uint8_t *filledHole)
{






    if (!pixels || !holeIn || w < 8 || h < 8 || channels < 1 || channels > 4) return false;
    const int r = std::max(1, opt.patchRadius);
    const int T = opt.threads > 0 ? opt.threads : int(std::max(1u, std::min(4u, std::thread::hardware_concurrency())));
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

} // namespace KrimbleInpaint
