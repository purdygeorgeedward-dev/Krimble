// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-13 (task 55): the SmartCompression panel's preview. Makes a JPEG of a picture in memory (the same settings as the save: quality, 1x1 color, gray, progressive) and reads a JPEG back, so the
// panel can show how large the result will be and what it looks like, without writing a file. Only libjpeg.
#pragma once

#include <vector>
#include <cstdint>
#include <cstdio>
#include <csetjmp>
#include <cstdlib>
#include <cstring>
#include "KrimbleSmartCompression.h"

namespace KrimbleSmart {

// rgb: width * height * 3 bytes (R, G, B). Returns false on an error.
inline bool encodeJpeg(const uint8_t *rgb, int width, int height, int quality, bool fullColor, bool gray, bool progressive, std::vector<uint8_t> &out)
{
    out.clear();
    jpeg_compress_struct cinfo;
    ErrorMgr jerr;
    unsigned char *buffer = nullptr;
    unsigned long size = 0;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = errorExit;
    if (setjmp(jerr.jump)) {
        jpeg_destroy_compress(&cinfo);
        free(buffer);
        return false;
    }
    jpeg_create_compress(&cinfo);
    jpeg_mem_dest(&cinfo, &buffer, &size);
    cinfo.image_width = width;
    cinfo.image_height = height;
    if (gray) {
        cinfo.input_components = 1;
        cinfo.in_color_space = JCS_GRAYSCALE;
    } else {
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;
    }
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, std::max(0, std::min(100, quality)), TRUE);
    if (!gray) {
        cinfo.comp_info[0].h_samp_factor = fullColor ? 1 : 2;   // 2x2 (smallest) or 1x1 (best), like the Advanced tab
        cinfo.comp_info[0].v_samp_factor = fullColor ? 1 : 2;
    }
    cinfo.optimize_coding = TRUE;
    if (progressive) jpeg_simple_progression(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);
    std::vector<uint8_t> line(static_cast<size_t>(width));
    while (cinfo.next_scanline < cinfo.image_height) {
        const uint8_t *src = rgb + size_t(cinfo.next_scanline) * width * 3;
        JSAMPROW row;
        if (gray) {
            for (int x = 0; x < width; ++x) line[x] = uint8_t((299 * src[x * 3] + 587 * src[x * 3 + 1] + 114 * src[x * 3 + 2]) / 1000);
            row = line.data();
        } else {
            row = const_cast<uint8_t *>(src);
        }
        jpeg_write_scanlines(&cinfo, &row, 1);
    }
    jpeg_finish_compress(&cinfo);
    out.assign(buffer, buffer + size);
    jpeg_destroy_compress(&cinfo);
    free(buffer);
    return true;
}

// Reads a JPEG into 3 bytes per pixel (R, G, B; a gray JPEG becomes gray RGB).
inline bool decodeJpeg(const std::vector<uint8_t> &jpeg, std::vector<uint8_t> &rgb, int &width, int &height)
{
    rgb.clear();
    jpeg_decompress_struct dinfo;
    ErrorMgr jerr;
    dinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = errorExit;
    if (setjmp(jerr.jump)) {
        jpeg_destroy_decompress(&dinfo);
        return false;
    }
    jpeg_create_decompress(&dinfo);
    jpeg_mem_src(&dinfo, const_cast<unsigned char *>(jpeg.data()), jpeg.size());
    jpeg_read_header(&dinfo, TRUE);
    dinfo.out_color_space = JCS_RGB;
    jpeg_start_decompress(&dinfo);
    width = int(dinfo.output_width);
    height = int(dinfo.output_height);
    rgb.resize(size_t(width) * height * 3);
    while (dinfo.output_scanline < dinfo.output_height) {
        JSAMPROW row = rgb.data() + size_t(dinfo.output_scanline) * width * 3;
        jpeg_read_scanlines(&dinfo, &row, 1);
    }
    jpeg_finish_decompress(&dinfo);
    jpeg_destroy_decompress(&dinfo);
    return true;
}

} // namespace KrimbleSmart
