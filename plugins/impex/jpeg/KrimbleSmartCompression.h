// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <csetjmp>
#include <cstdlib>
#include <algorithm>
#include "KrimbleSmartDetail.h"

extern "C" {
#include <jpeglib.h>
}

namespace KrimbleSmart {

struct ErrorMgr { jpeg_error_mgr pub; jmp_buf jump; };
inline void errorExit(j_common_ptr cinfo) { longjmp(reinterpret_cast<ErrorMgr *>(cinfo->err)->jump, 1); }

inline bool requantize(const uint8_t *jpeg, size_t length, const uint8_t *mask, int width, int height, int level, bool progressive, std::vector<uint8_t> &out)
{
    out.clear();
    if (level <= 0) { out.assign(jpeg, jpeg + length); return true; }
    jpeg_decompress_struct dinfo;
    jpeg_compress_struct cinfo;
    ErrorMgr djerr, cjerr;
    unsigned char *outBuffer = nullptr;
    unsigned long outSize = 0;
    bool compressing = false;
    dinfo.err = jpeg_std_error(&djerr.pub); djerr.pub.error_exit = errorExit;
    cinfo.err = jpeg_std_error(&cjerr.pub); cjerr.pub.error_exit = errorExit;
    if (setjmp(djerr.jump) || setjmp(cjerr.jump)) {
        if (compressing) jpeg_destroy_compress(&cinfo);
        jpeg_destroy_decompress(&dinfo);
        free(outBuffer);
        return false;
    }
    jpeg_create_decompress(&dinfo);
    jpeg_mem_src(&dinfo, const_cast<unsigned char *>(jpeg), length);
    jpeg_save_markers(&dinfo, JPEG_COM, 0xFFFF);
    for (int m = 1; m <= 15; ++m) jpeg_save_markers(&dinfo, JPEG_APP0 + m, 0xFFFF);
    jpeg_read_header(&dinfo, TRUE);
    jvirt_barray_ptr *coefs = jpeg_read_coefficients(&dinfo);

    std::vector<uint32_t> integral(size_t(width + 1) * (height + 1), 0);
    for (int y = 0; y < height; ++y) {
        uint32_t row = 0;
        for (int x = 0; x < width; ++x) {
            row += mask[size_t(y) * width + x] > 127 ? 1 : 0;
            integral[size_t(y + 1) * (width + 1) + x + 1] = integral[size_t(y) * (width + 1) + x + 1] + row;
        }
    }
    auto protectedPixels = [&](int x0, int y0, int x1, int y1) -> uint32_t {
        x0 = std::max(0, std::min(width, x0)); x1 = std::max(0, std::min(width, x1));
        y0 = std::max(0, std::min(height, y0)); y1 = std::max(0, std::min(height, y1));
        if (x1 <= x0 || y1 <= y0) return 0;
        const size_t w1 = width + 1;
        return integral[y1 * w1 + x1] - integral[y0 * w1 + x1] - integral[y1 * w1 + x0] + integral[y0 * w1 + x0];
    };

    const float L = std::min(100, level) / 100.0f;
    for (int ci = 0; ci < dinfo.num_components; ++ci) {
        const jpeg_component_info *comp = &dinfo.comp_info[ci];
        const int hs = dinfo.max_h_samp_factor / comp->h_samp_factor;
        const int vs = dinfo.max_v_samp_factor / comp->v_samp_factor;
        const float M = ci == 0 ? 1.0f + 5.0f * L : 1.0f + 8.0f * L;
        int step[64];
        for (int k = 0; k < 64; ++k) {
            const int u = k % 8, v = k / 8;
            step[k] = std::max(1, int(std::lround(M * (0.6f + 0.8f * (u + v) / 14.0f))));
        }
        for (JDIMENSION by = 0; by < comp->height_in_blocks; ++by) {
            JBLOCKARRAY rows = (dinfo.mem->access_virt_barray)(reinterpret_cast<j_common_ptr>(&dinfo), coefs[ci], by, 1, TRUE);
            for (JDIMENSION bx = 0; bx < comp->width_in_blocks; ++bx) {
                const int x0 = int(bx) * 8 * hs, y0 = int(by) * 8 * vs;
                if (protectedPixels(x0, y0, x0 + 8 * hs, y0 + 8 * vs) > 0) continue;
                JCOEF *block = rows[0][bx];
                for (int k = 1; k < 64; ++k) {
                    const int c = block[k];
                    if (c == 0 || step[k] <= 1) continue;
                    const int a = std::abs(c);
                    const int q = int((a + 0.4f * step[k]) / step[k]) * step[k];
                    block[k] = JCOEF(c < 0 ? -q : q);
                }
            }
        }
    }

    jpeg_create_compress(&cinfo);
    compressing = true;
    jpeg_mem_dest(&cinfo, &outBuffer, &outSize);
    jpeg_copy_critical_parameters(&dinfo, &cinfo);
    cinfo.optimize_coding = TRUE;
    if (progressive) jpeg_simple_progression(&cinfo);
    jpeg_write_coefficients(&cinfo, coefs);
    for (jpeg_saved_marker_ptr m = dinfo.marker_list; m; m = m->next) {
        if (m->marker == JPEG_APP0 + 0) continue;
        if (m->marker == JPEG_APP0 + 14 && cinfo.write_Adobe_marker && m->data_length >= 5 && std::memcmp(m->data, "Adobe", 5) == 0) continue;
        jpeg_write_marker(&cinfo, m->marker, m->data, m->data_length);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    compressing = false;
    jpeg_finish_decompress(&dinfo);
    jpeg_destroy_decompress(&dinfo);
    out.assign(outBuffer, outBuffer + outSize);
    free(outBuffer);
    return true;
}

}
