// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <vector>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace KrimbleSmart {

inline std::vector<uint8_t> detailMask(const uint8_t *lum, int width, int height, int sensitivity)
{
    const int bs = 8;
    const int bw = (width + bs - 1) / bs, bh = (height + bs - 1) / bs;
    std::vector<float> score(size_t(bw) * bh, 0.0f);
    for (int by = 0; by < bh; ++by) {
        for (int bx = 0; bx < bw; ++bx) {
            double sum = 0; int n = 0;
            for (int y = by * bs; y < std::min(height, (by + 1) * bs); ++y) {
                for (int x = bx * bs; x < std::min(width, (bx + 1) * bs); ++x) {
                    const int c = lum[size_t(y) * width + x];
                    const int r = x + 1 < width ? lum[size_t(y) * width + x + 1] : c;
                    const int d = y + 1 < height ? lum[size_t(y + 1) * width + x] : c;
                    sum += std::abs(r - c) + std::abs(d - c);
                    ++n;
                }
            }
            score[size_t(by) * bw + bx] = n ? float(sum / n) : 0.0f;
        }
    }
    const float s = std::max(0, std::min(100, sensitivity)) / 100.0f;
    const float threshold = 24.0f * std::pow(1.5f / 24.0f, s);
    std::vector<uint8_t> protect(size_t(bw) * bh, 0);
    for (size_t i = 0; i < protect.size(); ++i) protect[i] = score[i] > threshold ? 1 : 0;
    std::vector<uint8_t> grown(protect);
    for (int by = 0; by < bh; ++by)
        for (int bx = 0; bx < bw; ++bx)
            if (protect[size_t(by) * bw + bx])
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) {
                        const int x = bx + dx, y = by + dy;
                        if (x >= 0 && y >= 0 && x < bw && y < bh) grown[size_t(y) * bw + x] = 1;
                    }
    std::vector<uint8_t> mask(size_t(width) * height, 0);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            mask[size_t(y) * width + x] = grown[size_t(y / bs) * bw + x / bs] ? 255 : 0;
    return mask;
}

}
