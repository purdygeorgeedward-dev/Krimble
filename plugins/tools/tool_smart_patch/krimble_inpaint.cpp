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
    std::vector<float> img;
    std::vector<uint8_t> hole;
    std::vector<int> sat;
    std::vector<int> nx, ny;
    std::vector<float> dist;
    std::vector<uint8_t> target;
    std::vector<int> sources;
    int sx0 = 0, sy0 = 0, sx1 = 0, sy1 = 0;
    std::vector<float> lp, hp;
    std::vector<float> guide;
};

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

inline int holeCount(const Level &L, int x0, int y0, int x1, int y1)
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

static float g_colorWeight = 0.15f;
static float g_patchSigma = 1.6f;
static float g_gainMax = 3.0f;
static float g_windowFactor = 2.0f;
static float g_coarseSharp = 2.0f;
static int g_blurRadius = 3;
static int g_relaxIters = 60;

void splitLowHigh(Level &L)
{
    const int w = L.w, h = L.h, c = L.c, R = g_blurRadius;
    L.lp.resize(L.img.size());
    L.hp.resize(L.img.size());
    std::vector<float> tmp(L.img.size());
    for (int y = 0; y < h; ++y) {
        for (int k = 0; k < c; ++k) {
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

#if 0
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

#if 0
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
        float bestP = -1.0f; int bx = -1, by = -1;
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                const size_t i = size_t(y) * w + x;
                if (known[i]) continue;
                if (!(knownAt(x - 1, y) || knownAt(x + 1, y) || knownAt(x, y - 1) || knownAt(x, y + 1))) continue;
                float cs = 0.0f; int cn = 0;
                float gxBest = 0, gyBest = 0, gMag = 0;
                for (int dy = -r; dy <= r; ++dy) {
                    for (int dx = -r; dx <= r; ++dx) {
                        const int qx = x + dx, qy = y + dy;
                        if (qx < 1 || qy < 1 || qx >= w - 1 || qy >= h - 1) continue;
                        const size_t q = size_t(qy) * w + qx;
                        ++cn; cs += conf[q];
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
                float nxm = 0, nym = 0;
                for (int ey = -1; ey <= 1; ++ey) for (int ex = -1; ex <= 1; ++ex) { const float k = float(knownAt(x + ex, y + ey)); nxm += k * ex; nym += k * ey; }
                const float nl = std::sqrt(nxm * nxm + nym * nym);
                float D = 0.001f;
                if (nl > 0 && gMag > 0) {
                    const float ix = -gyBest, iy = gxBest, il = std::sqrt(ix * ix + iy * iy);
                    D = std::fabs((ix * nxm + iy * nym) / (il * nl)) * std::sqrt(gMag) / 255.0f + 0.001f;
                }
                static const float structureBase = getenv("KI_BASE") ? float(atof(getenv("KI_BASE"))) : 0.15f;
                const float P = C * (structureBase + D);
                if (P > bestP) { bestP = P; bx = x; by = y; }
            }
        }
        if (bx < 0) break;
        struct KP { int dx, dy; };
        std::vector<KP> kp;
        for (int dy = -r; dy <= r; ++dy)
            for (int dx = -r; dx <= r; ++dx) {
                const int qx = bx + dx, qy = by + dy;
                if (qx < 0 || qy < 0 || qx >= w || qy >= h) continue;
                if (known[size_t(qy) * w + qx]) kp.push_back({dx, dy});
            }
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

void harmonicFill(Level &L)
{
    diffuseFill(L);
    const int w = L.w, h = L.h, c = L.c;
    int minX = w, minY = h, maxX = -1, maxY = -1;
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) if (L.hole[size_t(y) * w + x]) { minX = std::min(minX, x); maxX = std::max(maxX, x); minY = std::min(minY, y); maxY = std::max(maxY, y); }
    if (maxX < 0) return;
    const int dim = std::max(maxX - minX + 1, maxY - minY + 1);
    const int iterations = std::min(6000, 20 + 3 * dim * dim);
    for (int it = 0; it < iterations; ++it) {
        float change = 0.0f;
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
                    const float v = old + 1.6f * (target - old);
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
    int minX = L.w, minY = L.h, maxX = -1, maxY = -1;
    for (int y = 0; y < L.h; ++y)
        for (int x = 0; x < L.w; ++x)
            if (L.hole[size_t(y) * L.w + x]) { minX = std::min(minX, x); maxX = std::max(maxX, x); minY = std::min(minY, y); maxY = std::max(maxY, y); }
    L.sx0 = std::max(0, minX - margin); L.sy0 = std::max(0, minY - margin);
    L.sx1 = std::min(L.w - 1, maxX + margin); L.sy1 = std::min(L.h - 1, maxY + margin);
    L.target.assign(L.hole.size(), 0);
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

void patchMatchPass(Level &L, int r, bool forward, int rowA, int rowB, std::mt19937 &rng)
{
    const int y0 = forward ? rowA : rowB - 1, y1 = forward ? rowB : rowA - 1, ys = forward ? 1 : -1;
    const int x0 = forward ? 0 : L.w - 1, x1 = forward ? L.w : -1, xs = forward ? 1 : -1;
    const int maxR = std::max(L.sx1 - L.sx0, L.sy1 - L.sy0) + 1;
    for (int y = y0; y != y1; y += ys) {
        for (int x = x0; x != x1; x += xs) {
            const size_t i = size_t(y) * L.w + x;
            if (!L.target[i]) continue;
            const int px = x - xs, py = y - ys;
            if (px >= 0 && px < L.w && L.target[size_t(y) * L.w + px]) {
                const size_t j = size_t(y) * L.w + px;
                tryCandidate(L, i, x, y, L.nx[j] + xs, L.ny[j], r);
            }
            if (py >= 0 && py < L.h && L.target[size_t(py) * L.w + x]) {
                const size_t j = size_t(py) * L.w + x;
                tryCandidate(L, i, x, y, L.nx[j], L.ny[j] + ys, r);
            }
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

void vote(Level &L, int r, float sharpness, int threads)
{
    const int c = L.c, w = L.w, h = L.h;
    const float norm = 1.0f / float((2 * r + 1) * (2 * r + 1) * c);
    std::vector<float> ds;
    for (size_t i = 0; i < L.target.size(); ++i) if (L.target[i]) ds.push_back(L.dist[i] * norm);
    if (ds.empty()) return;
    std::nth_element(ds.begin(), ds.begin() + ds.size() / 2, ds.end());
    const float sigma2 = std::max(ds[ds.size() / 2], 1.0f) / std::max(0.05f, sharpness);

    std::vector<float> wgt(L.hole.size(), 0.0f);
    for (size_t i = 0; i < L.target.size(); ++i) if (L.target[i]) wgt[i] = std::exp(-(L.dist[i] * norm) / (2.0f * sigma2));
    const int P = 2 * r + 1;
    std::vector<float> spatial(size_t(P) * P);
    for (int dy = -r; dy <= r; ++dy)
        for (int dx = -r; dx <= r; ++dx)
            spatial[size_t(dy + r) * P + (dx + r)] = std::exp(-float(dx * dx + dy * dy) / (2.0f * g_patchSigma * g_patchSigma));

    std::vector<float> hpNew(L.img.size(), 0.0f);
    std::vector<float> before(L.hole.size(), 0.0f);
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
        for (int k = 0; k < c; ++k) L.img[t * c + k] = L.guide[t * c + k] + gain * hpNew[t * c + k];
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

}

bool inpaintLegacy(uint8_t *pixels, const uint8_t *holeIn, int w, int h, int channels, const Options &opt, uint8_t *filledHole)
{

    if (!pixels || !holeIn || w < 8 || h < 8 || channels < 1 || channels > 4) return false;
    const int r = std::max(1, opt.patchRadius);
    const int T = opt.threads > 0 ? opt.threads : int(std::max(1u, std::thread::hardware_concurrency()));
    std::mt19937 rng(opt.seed);

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

    const int holeDim = std::max(maxX - minX + 1, maxY - minY + 1);
    int levels = 1;
    while (levels < opt.maxLevels && (holeDim >> (levels - 1)) > 56 && std::min(w, h) >> levels >= 6 * (2 * r + 1)) ++levels;
    for (int i = 1; i < levels; ++i) pyr.push_back(downsample(pyr[i - 1]));

    for (int lv = levels - 1; lv >= 0; --lv) {
        Level &L = pyr[lv];
        const bool coarsest = (lv == levels - 1);
        if (coarsest) {
        } else {
            const Level &C = pyr[lv + 1];
            for (int y = 0; y < L.h; ++y)
                for (int x = 0; x < L.w; ++x) {
                    const size_t i = size_t(y) * L.w + x;
                    if (!L.hole[i]) continue;
                    for (int k = 0; k < L.c; ++k) L.img[i * L.c + k] = sampleBilinear(C, (x - 0.5f) / 2.0f, (y - 0.5f) / 2.0f, k);
                }
        }
        const int holeShort = std::min(maxX - minX + 1, maxY - minY + 1);
        const int margin = std::max(6 * (2 * r + 1), int(std::max(g_windowFactor * holeShort, 48.0f)) >> lv);
        prepareLevel(L, r, margin);
        if (coarsest) {
            harmonicFill(L);
        } else {
            correctBoundary(L, g_relaxIters);
        }
        splitLowHigh(L);
        L.guide = L.lp;
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
            const float sharp = (lv <= 1) ? opt.sharpness * ((lv == 0 && it >= iterations - 2) ? 2.0f : 1.0f) : std::min(opt.sharpness, g_coarseSharp);
            vote(L, r, sharp, T);
            splitLowHigh(L);
            parallelFor(int(L.target.size()), T, [&](int a, int b) {
                for (int i = a; i < b; ++i)
                    if (L.target[size_t(i)]) L.dist[size_t(i)] = patchDist(L, i % L.w, i / L.w, L.nx[size_t(i)], L.ny[size_t(i)], r, 1e30f);
            });
        }
    }

    const Level &F = pyr[0];
    if (filledHole) for (size_t i = 0; i < F.hole.size(); ++i) filledHole[i] = F.hole[i] ? 1 : 0;
    for (size_t i = 0; i < F.hole.size(); ++i) {
        if (!F.hole[i]) continue;
        for (int k = 0; k < channels; ++k)
            pixels[i * channels + k] = uint8_t(std::max(0.0f, std::min(255.0f, F.img[i * channels + k] + 0.5f)));
    }
    return true;
}

namespace {

struct PLevel {
    int w = 0, h = 0, c = 0;
    std::vector<float> img;
    std::vector<uint8_t> hole;
    std::vector<uint8_t> src;
    std::vector<uint8_t> tgt;
    std::vector<int> nx, ny;
    std::vector<float> nd;
    std::vector<int> srcList;
    std::vector<float> conf;
    std::vector<uint8_t> xf;
    std::vector<float> bias;
    std::vector<int> freeR;
    std::vector<int32_t> sat;
};

void pBuild(PLevel &L, int r)
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

void pSetConfidence(PLevel &L, float holeWeight)
{
    for (size_t i = 0; i < L.hole.size(); ++i) L.conf[i] = L.hole[i] ? holeWeight : 1.0f;
}

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

void pFinalSeamless(PLevel &L, int r, int threads)
{
    const int c = L.c, w = L.w, h = L.h;
    std::vector<float> pred(L.img.size(), 0.0f);
    std::vector<uint8_t> has(L.hole.size(), 0);
    pParallel(h, threads, [&](int ya, int yb) {
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
                            const size_t s = (size_t(L.ny[q] + dy) * w + (L.nx[q] + dx)) * c;
                            for (int k = 0; k < c; ++k) pred[p * c + k] = L.img[s + k];
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
                if (!L.hole[q] && !fixedRing[q]) continue;
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

}

bool inpaintPatchMatchV1(uint8_t *pixels, const uint8_t *holeIn, int w, int h, int channels, const Options &opt, uint8_t *filledHole)
{
    if (!pixels || !holeIn || w < 8 || h < 8 || channels < 1 || channels > 4) return false;
    const int r = std::max(2, opt.patchRadius);
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
            for (size_t i = 0; i < L.tgt.size(); ++i) if (L.tgt[i]) L.nd[i] = 1.0f;
            pVote(L, r, false, T);
        }
        const int emIters = (lv == levels - 1) ? 10 : (lv == 0 ? 3 : 5);
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

namespace {

struct QXf { float a, b, c, d; float reach; float cost; };

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
    return table;
}

void qFreeRadius(PLevel &L)
{
    const int w = L.w, h = L.h;
    const int INF = 1 << 20;
    L.freeR.assign(size_t(w) * h, INF);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const size_t i = size_t(y) * w + x;
            if (L.hole[i]) { L.freeR[i] = 0; continue; }
            int v = std::min(std::min(x + 1, y + 1), std::min(w - x, h - y));
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

const float kMaxBias = 28.0f;
const float kBiasCost = 0.35f;

void qBuildSat(PLevel &L)
{
    const int w = L.w, h = L.h, c = L.c;
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
            for (int rad = maxR; rad >= 1; rad /= 2) {
                const int sx = L.nx[i] + int(rng() % unsigned(2 * rad + 1)) - rad;
                const int sy = L.ny[i] + int(rng() % unsigned(2 * rad + 1)) - rad;
                qTry(L, tab, i, x, y, sx, sy, L.xf[i], r);
            }
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
    qBuildSat(L);
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
    if (false)
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

}

bool inpaintStandardV2(uint8_t *pixels, const uint8_t *holeIn, int w, int h, int channels, const Options &opt, uint8_t *filledHole)
{
    if (!pixels || !holeIn || w < 8 || h < 8 || channels < 1 || channels > 4) return false;
    const int r = std::max(2, std::min(5, opt.patchRadius));
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

namespace {

struct RLevel {
    int w = 0, h = 0, c = 0;
    std::vector<uint8_t> img;
    std::vector<uint8_t> hole;
    std::vector<uint8_t> tgt;
    std::vector<int16_t> nx, ny;
    std::vector<float> nd;
    std::vector<uint8_t> xf;
    std::vector<int8_t> bias;
    std::vector<uint16_t> freeR;
    std::vector<int32_t> sat;
    float holeWeight = 1.0f;
    size_t sourceCount = 0;
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
    auto B = [&](int x, int y) { return size_t(y - by0) * bw + size_t(x - bx0); };
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
    std::vector<int> redBlack[2];
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

bool rRandomSource(const RLevel &L, const std::vector<QXf> &tab, std::mt19937 &rng, int &sx, int &sy)
{
    for (int tries = 0; tries < 2000; ++tries) {
        sx = int(rng() % unsigned(L.w));
        sy = int(rng() % unsigned(L.h));
        if (rValid(L, sx, sy, tab[0])) return true;
    }
    return false;
}

void rRingFill(RLevel &L, const std::vector<QXf> &tab, int r, unsigned seed, int threads, std::vector<int16_t> &matchX, std::vector<int16_t> &matchY)
{

    const int w = L.w, h = L.h, c = L.c;
    const size_t n = size_t(w) * h;
    matchX.assign(n, -1);
    matchY.assign(n, -1);
    std::vector<uint8_t> known(n);
    for (size_t i = 0; i < n; ++i) known[i] = L.hole[i] ? 0 : 1;
    std::vector<int> pending;
    for (size_t i = 0; i < n; ++i) if (L.hole[i]) pending.push_back(int(i));
    const int pr = std::min(r, 3);
    const int kRandom = 2500;
    while (!pending.empty()) {
        std::vector<int> layer, rest;
        for (int i : pending) {
            const int x = i % w, y = i / w;
            const bool edge = (x > 0 && known[i - 1]) || (x < w - 1 && known[i + 1]) || (y > 0 && known[i - w]) || (y < h - 1 && known[i + w]);
            (edge ? layer : rest).push_back(i);
        }
        if (layer.empty()) { layer.push_back(rest.back()); rest.pop_back(); }
        pParallel(int(layer.size()), threads, [&](int a, int b) {
            for (int li = a; li < b; ++li) {
                const int p = layer[size_t(li)];
                const int px = p % w, py = p / w;
                std::mt19937 rng(seed ^ (unsigned(p) * 2654435761u));
                auto cost = [&](int sx, int sy, float best) -> float {
                    float sum = 0.0f; int cnt = 0;
                    for (int dy = -pr; dy <= pr; ++dy) {
                        const int y = py + dy; if (y < 0 || y >= h) continue;
                        for (int dx = -pr; dx <= pr; ++dx) {
                            const int x = px + dx; if (x < 0 || x >= w) continue;
                            const size_t q = size_t(y) * w + x;
                            if (!known[q]) continue;
                            const uint8_t *a = &L.img[q * c];
                            const uint8_t *s = &L.img[(size_t(sy + dy) * w + (sx + dx)) * c];
                            for (int k = 0; k < c; ++k) { const float e = float(a[k]) - float(s[k]); sum += e * e; }
                            ++cnt;
                        }
                        if (cnt > 0 && sum / cnt > best * 1.5f && sum > best * 2.0f) return 1e30f;
                    }
                    return cnt > 0 ? sum / float(cnt) : 1e30f;
                };
                float bestCost = 1e30f; int bx = -1, by = -1;
                auto tryCand = [&](int sx, int sy) {
                    if (!rValid(L, sx, sy, tab[0])) return;
                    const float cst = cost(sx, sy, bestCost);
                    if (cst < bestCost) { bestCost = cst; bx = sx; by = sy; }
                };
                for (int dy = -1; dy <= 1; ++dy) for (int dx = -1; dx <= 1; ++dx) {
                    if (!dx && !dy) continue;
                    const int x = px + dx, y = py + dy;
                    if (x < 0 || y < 0 || x >= w || y >= h) continue;
                    const size_t q = size_t(y) * w + x;
                    if (matchX[q] >= 0) tryCand(matchX[q] - dx, matchY[q] - dy);
                }
                for (int t = 0; t < kRandom; ++t) tryCand(int(rng() % unsigned(w)), int(rng() % unsigned(h)));
                if (bx >= 0) {
                    for (int rad = std::max(w, h) / 4; rad >= 1; rad /= 2) {
                        const int cx = bx, cy = by;
                        for (int t = 0; t < 10; ++t) tryCand(cx + int(rng() % unsigned(2 * rad + 1)) - rad, cy + int(rng() % unsigned(2 * rad + 1)) - rad);
                    }
                    matchX[size_t(p)] = int16_t(bx);
                    matchY[size_t(p)] = int16_t(by);
                }
            }
        });
        for (int p : layer) {
            if (matchX[size_t(p)] >= 0) {
                const size_t s = size_t(matchY[size_t(p)]) * w + matchX[size_t(p)];
                for (int k = 0; k < c; ++k) L.img[size_t(p) * c + k] = L.img[s * c + k];
            }
            known[size_t(p)] = 1;
        }
        pending.swap(rest);
    }
}

}

bool inpaint(uint8_t *pixels, const uint8_t *holeIn, int w, int h, int channels, const Options &opt, uint8_t *filledHole)
{
    if (!pixels || !holeIn || w < 8 || h < 8 || channels < 1 || channels > 4) return false;
    if (w > 32767 || h > 32767) return inpaintStandardV2(pixels, holeIn, w, h, channels, opt, filledHole);
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
    int thickness = 0;
    {
        std::vector<uint16_t> dist(size_t(w) * h, 0);
        const std::vector<uint8_t> &hm = pyr[0].hole;
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                const size_t i = size_t(y) * w + x;
                if (!hm[i]) continue;
                int v = 60000;
                if (x == 0 || y == 0) v = 1;
                else v = std::min(std::min(int(dist[i - 1]), int(dist[i - w])), std::min(int(dist[i - w - 1]), x < w - 1 ? int(dist[i - w + 1]) : 60000)) + 1;
                dist[i] = uint16_t(std::min(v, 60000));
            }
        int best = 0;
        for (int y = h - 1; y >= 0; --y)
            for (int x = w - 1; x >= 0; --x) {
                const size_t i = size_t(y) * w + x;
                if (!hm[i]) continue;
                int v = dist[i];
                if (x == w - 1 || y == h - 1) v = std::min(v, 1);
                else v = std::min(v, std::min(std::min(int(dist[i + 1]), int(dist[i + w])), std::min(int(dist[i + w + 1]), x > 0 ? int(dist[i + w - 1]) : 60000)) + 1);
                dist[i] = uint16_t(v);
                best = std::max(best, v);
            }
        thickness = 2 * best - 1;
    }
    int levels = 1;
    while (levels < std::max(opt.maxLevels, 10) && (holeDim >> (levels - 1)) > 24 && (std::min(w, h) >> levels) >= 8 * (2 * r + 1)
           && (thickness >> levels) >= 2 * (2 * r + 1)) ++levels;
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
            std::vector<int16_t> ringX, ringY;
            rPeel(L);
            rRingFill(L, tab, r, seed + unsigned(lv), T, ringX, ringY);
            for (size_t i = 0; i < L.tgt.size(); ++i) {
                if (!L.tgt[i]) continue;
                int sx = 0, sy = 0;
                if (L.hole[i] && ringX[i] >= 0) { sx = ringX[i]; sy = ringY[i]; }
                else if (!rRandomSource(L, tab, rng, sx, sy)) continue;
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
        if (lv > 0) { std::vector<int32_t>().swap(L.sat); std::vector<uint16_t>().swap(L.freeR); }
    }

    const RLevel &F = pyr[0];
    if (filledHole) for (size_t i = 0; i < F.hole.size(); ++i) filledHole[i] = F.hole[i] ? 1 : 0;
    for (size_t i = 0; i < F.hole.size(); ++i) {
        if (!F.hole[i]) continue;
        for (int k = 0; k < channels; ++k) pixels[i * channels + k] = F.img[i * channels + k];
    }
    return true;
}

}
