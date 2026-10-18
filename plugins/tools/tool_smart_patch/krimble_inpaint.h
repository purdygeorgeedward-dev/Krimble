// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble content-aware fill: a multi-scale PatchMatch inpainting engine (no Qt, no Krita types: plain pixel buffers, so it can be tested on its own).
#pragma once
#include <cstdint>
#include <functional>

namespace KrimbleInpaint {

struct Options {
    int patchRadius = 3;       // patches are (2r+1) x (2r+1): 7 x 7
    int expand = 2;            // the hole is grown by this many pixels (halos of the selection edge), at full size
    int maxLevels = 8;         // at most this many pyramid levels
    unsigned seed = 1234;      // random seed (the same input gives the same result)
    int threads = 0;           // 0 = as many as the device has (at most 4)
    std::function<bool(float)> progress;   // called now and then with 0..1; return false to stop (inpaint() then returns false and changes nothing)
    float sharpness = 8.0f;    // higher = sharper and more "copied" (a few sources win), lower = smoother (many sources are averaged)
};

// pixels: w * h * channels, 8 bit, interleaved (RGB or RGBA); hole: w * h, non-zero = to be filled.
// The hole pixels of `pixels` are replaced. Returns false when there is nothing to copy from (the whole picture is hole) or on bad arguments.
//
// filledHole (optional, w * h): set to 1 for every pixel that was replaced (the hole, grown by Options::expand), so that the caller can write back only those.
bool inpaint(uint8_t *pixels, const uint8_t *hole, int w, int h, int channels, const Options &options = Options(), uint8_t *filledHole = nullptr);

}
