// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <cstdint>
#include <functional>

namespace KrimbleInpaint {

struct Options {
    int patchRadius = 3;
    int expand = 2;
    int maxLevels = 8;
    unsigned seed = 1234;
    int threads = 0;
    std::function<bool(float)> progress;
    float sharpness = 8.0f;
};

bool inpaint(uint8_t *pixels, const uint8_t *hole, int w, int h, int channels, const Options &options = Options(), uint8_t *filledHole = nullptr);

bool inpaintLegacy(uint8_t *pixels, const uint8_t *hole, int w, int h, int channels, const Options &options = Options(), uint8_t *filledHole = nullptr);

}
