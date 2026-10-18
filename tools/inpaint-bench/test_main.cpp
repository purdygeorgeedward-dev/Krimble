#include "krimble_inpaint.h"
#include <cstdio>
#include <vector>
#include <chrono>
#include <cstdlib>
// usage: test image.ppm mask.pgm out.ppm [expand] [sharpness]
int main(int argc, char **argv)
{
    if (argc < 4) return 1;
    FILE *f = fopen(argv[1], "rb"); int w, h, mx; if (fscanf(f, "P6 %d %d %d", &w, &h, &mx) != 3) return 2; fgetc(f);
    std::vector<uint8_t> img(size_t(w) * h * 3); if (fread(img.data(), 1, img.size(), f) != img.size()) return 3; fclose(f);
    f = fopen(argv[2], "rb"); int w2, h2; if (fscanf(f, "P5 %d %d %d", &w2, &h2, &mx) != 3) return 4; fgetc(f);
    std::vector<uint8_t> mask(size_t(w) * h); if (fread(mask.data(), 1, mask.size(), f) != mask.size()) return 5; fclose(f);
    KrimbleInpaint::Options o;
    if (argc > 4) o.expand = atoi(argv[4]);
    if (argc > 5) o.sharpness = float(atof(argv[5]));
    auto t0 = std::chrono::steady_clock::now();
    bool ok = KrimbleInpaint::inpaint(img.data(), mask.data(), w, h, 3, o);
    auto t1 = std::chrono::steady_clock::now();
    fprintf(stderr, "ok=%d  %.2f s\n", ok, std::chrono::duration<double>(t1 - t0).count());
    f = fopen(argv[3], "wb"); fprintf(f, "P6\n%d %d\n255\n", w, h); fwrite(img.data(), 1, img.size(), f); fclose(f);
    return ok ? 0 : 6;
}
