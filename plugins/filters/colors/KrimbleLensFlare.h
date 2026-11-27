/*
 * Krimble: Lens Flare, made from scratch (prototype: docs/krimble-refs/lens_flare_prototype_v1.py,
 * look: docs/krimble-refs/2026-11-20_2327_demo-lens-flares-v1.png).
 *
 * Per-pixel math (what a shader would do): for each pixel, how much light each part of the flare adds there.
 * Parts: a white core, two soft glows, rays (fine random rays, or long starburst spikes), a rainbow ring,
 * colored ghosts along the line from the light through the center of the picture (six-sided like an aperture,
 * or oval for the cinematic type), a horizontal streak (cinematic), and a few small specks.
 * The light is added with Screen blending, so the flare only brightens the picture.
 *
 * Sizes are given for a picture whose diagonal is 1255 px and scaled to the real picture.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KRIMBLE_LENS_FLARE_H
#define KRIMBLE_LENS_FLARE_H

#include <QVector>
#include <QtMath>
#include <cmath>
#include <random>

namespace KrimbleLensFlare {

enum Kind { Zoom = 0, Prime = 1, Cinematic = 2, Starburst = 3 };

struct Params {
    Kind kind = Zoom;
    double sx = 0, sy = 0;
    double cx = 0, cy = 0;
    double diagonal = 1255;
    double brightness = 1.0;
    double rays = 1.0;
    double ghosts = 1.0;
    double ring = 1.0;
    double warmth = 0.0;
    unsigned seed = 1;
};

struct RGB { double r, g, b; };

inline RGB hsv(double h)
{
    h = h - std::floor(h);
    const double s = 0.8, v = 1.0;
    const int i = int(h * 6.0) % 6;
    const double f = h * 6.0 - std::floor(h * 6.0);
    const double p = v * (1 - s), q = v * (1 - f * s), t = v * (1 - (1 - f) * s);
    switch (i) {
    case 0: return {v, t, p};
    case 1: return {q, v, p};
    case 2: return {p, v, t};
    case 3: return {p, q, v};
    case 4: return {t, p, v};
    default: return {v, p, q};
    }
}

class Renderer
{
public:
    explicit Renderer(const Params &params) : m(params)
    {
        s = qMax(0.05, m.diagonal / 1255.0);
        RGB warm{1.0, 0.85, 0.6}, cool{0.55, 0.75, 1.0};
        double w = (m.kind == Cinematic ? -0.6 : 0.6) + m.warmth;
        w = qBound(-1.0, w, 1.0);
        const double a = (w + 1.0) / 2.0;
        tint = {cool.r + (warm.r - cool.r) * a, cool.g + (warm.g - cool.g) * a, cool.b + (warm.b - cool.b) * a};

        std::mt19937 rng(m.seed);
        std::uniform_real_distribution<double> uni(0.0, 1.0);
        if (m.kind == Zoom || m.kind == Prime) {
            const int n = 180;
            for (int i = 0; i < n; ++i) rays.append({uni(rng) * 2 * M_PI, 0.15 + uni(rng) * 0.85});
            rayWidth = 0.004; rayAmp = 0.55;
        } else if (m.kind == Starburst) {
            for (int i = 0; i < 6; ++i) rays.append({i * M_PI / 3 + 0.3, 1.0});
            rayWidth = 0.012; rayAmp = 1.1;
        }
        bins.resize(Bins);
        for (int i = 0; i < rays.size(); ++i) {
            const double reach = rayWidth * 3.5;
            const int b0 = int(std::floor((rays[i].angle - reach) / (2 * M_PI) * Bins));
            const int b1 = int(std::floor((rays[i].angle + reach) / (2 * M_PI) * Bins));
            for (int b = b0; b <= b1; ++b) bins[((b % Bins) + Bins) % Bins].append(i);
        }

        struct G { double t, R, a, h; };
        QVector<G> list;
        switch (m.kind) {
        case Zoom: list = {{0.35, 22, 0.30, 0.55}, {0.6, 10, 0.20, 0.15}, {0.9, 55, 0.12, 0.72}, {1.15, 30, 0.18, 0.35}, {1.4, 90, 0.07, 0.6}, {1.7, 16, 0.22, 0.1}, {2.0, 70, 0.09, 0.9}}; break;
        case Prime: list = {{0.5, 40, 0.14, 0.08}, {1.2, 24, 0.18, 0.55}, {1.6, 60, 0.08, 0.75}}; break;
        case Cinematic: list = {{0.6, 30, 0.10, 0.6}, {1.3, 50, 0.08, 0.62}, {1.8, 20, 0.12, 0.58}}; break;
        case Starburst: list = {{0.8, 18, 0.10, 0.12}, {1.5, 35, 0.07, 0.6}}; break;
        }
        const double vx = m.cx - m.sx, vy = m.cy - m.sy;
        for (const G &g : list) ghosts.append({m.sx + vx * g.t, m.sy + vy * g.t, g.R * s, g.a, hsv(g.h)});
        const int speckCount = m.kind == Zoom ? 6 : 2;
        std::normal_distribution<double> jitter(0.0, 15.0);
        for (int k = 0; k < speckCount; ++k) {
            const double t = 0.2 + uni(rng) * 1.9, R = (3 + uni(rng) * 5) * s;
            const double jx = jitter(rng) * s, jy = jitter(rng) * s;
            specks.append({m.sx + vx * t + jx, m.sy + vy * t + jy, R, 0.25, hsv(uni(rng))});
        }
        ringRadius = (m.kind == Zoom ? 230 : 150) * s;
    }

    RGB light(double x, double y) const
    {
        const double dx = x - m.sx, dy = y - m.sy;
        const double r = std::sqrt(dx * dx + dy * dy);
        RGB o{0, 0, 0};
        auto add = [&o](double k, const RGB &c) { o.r += k * c.r; o.g += k * c.g; o.b += k * c.b; };
        auto gauss = [](double v, double sd) { const double q = v / sd; return std::exp(-q * q); };

        add(gauss(r, 10 * s) * 1.6, {1, 1, 1});
        add(gauss(r, 45 * s) * 0.9, tint);
        add(gauss(r, 170 * s) * 0.35, tint);

        if (!rays.isEmpty() && m.rays > 0 && r > 0.5) {
            double th = std::atan2(dy, dx);
            if (th < 0) th += 2 * M_PI;
            const int b = qBound(0, int(th / (2 * M_PI) * Bins), Bins - 1);
            double sum = 0;
            for (int i : bins[b]) {
                double d = th - rays[i].angle;
                d = std::remainder(d, 2 * M_PI);
                const double decay = m.kind == Starburst ? 420 * s : 260 * s * rays[i].length;
                sum += gauss(d, rayWidth) * rays[i].length * std::exp(-r / decay);
            }
            add(qMin(sum, 3.0) * rayAmp * m.rays, tint);
        }

        if (m.kind == Cinematic) {
            const double ax = std::fabs(dx);
            add(gauss(dy, 5 * s) * std::exp(-ax / (520 * s)) * 1.3, {0.45, 0.7, 1.0});
            add(gauss(dy, 18 * s) * std::exp(-ax / (300 * s)) * 0.4, {0.3, 0.5, 1.0});
        }

        if ((m.kind == Zoom || m.kind == Prime) && m.ring > 0) {
            for (int k = 0; k < 6; ++k) add(gauss(r - (ringRadius + k * 6 * s), 6 * s) * 0.06 * m.ring, hsv(0.8 * k / 5.0));
        }

        if (m.ghosts > 0) {
            for (const Ghost &g : ghosts) {
                const double gx = x - g.x, gy = y - g.y;
                double cover;
                if (m.kind == Cinematic) {
                    const double e = std::sqrt((gx / 2.2) * (gx / 2.2) + gy * gy);
                    cover = qBound(0.0, (g.R - e) / (3 * s) + 0.5, 1.0);
                } else {
                    const double rr = std::sqrt(gx * gx + gy * gy);
                    const double seg = M_PI / 3;
                    double a = std::atan2(gy, gx) - 0.4;
                    a = a - seg * std::floor(a / seg) - seg / 2;
                    const double edge = g.R * std::cos(seg / 2) / std::cos(a);
                    cover = qBound(0.0, (edge - rr) / (2.5 * s) + 0.5, 1.0);
                }
                if (cover <= 0) continue;
                const double rr = std::sqrt(gx * gx + gy * gy);
                const double rim = 0.6 + 0.4 * std::pow(qMin(rr / g.R, 1.0), 3);
                add(cover * rim * g.a * m.ghosts, g.c);
            }
            for (const Ghost &g : specks) {
                const double gx = x - g.x, gy = y - g.y;
                add(gauss(std::sqrt(gx * gx + gy * gy), g.R) * g.a * m.ghosts, g.c);
            }
        }

        const double k = m.brightness;
        return {qBound(0.0, o.r * k, 1.0), qBound(0.0, o.g * k, 1.0), qBound(0.0, o.b * k, 1.0)};
    }

private:
    struct Ray { double angle, length; };
    struct Ghost { double x, y, R, a; RGB c; };
    static const int Bins = 4096;
    Params m;
    double s = 1;
    RGB tint{1, 1, 1};
    QVector<Ray> rays;
    double rayWidth = 0.004, rayAmp = 0.55;
    QVector<QVector<int>> bins;
    QVector<Ghost> ghosts, specks;
    double ringRadius = 230;
};

}

#endif
