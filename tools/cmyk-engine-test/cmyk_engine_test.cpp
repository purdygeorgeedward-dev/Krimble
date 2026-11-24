#include <QColor>
#include <cstdio>
#include <cstdlib>
#include "../../libs/ui/KrimbleCmykEngine.h"

static void run(const char *label, const KrimbleCmykProfile &p)
{
    KrimbleCmykEngine e(p);
    srand(11);
    const int n = 400;
    double sum = 0, mx = 0;
    int bad = 0;
    for (int i = 0; i < n; ++i) {
        KrimblePlates pl = {rand() % 101 / 100.0, rand() % 101 / 100.0, rand() % 101 / 100.0, rand() % 101 / 100.0};
        pl = e.limitInk(pl);
        const QColor rgb = e.toRgb(pl);
        const QColor back = e.toRgb(e.fromRgb(rgb));
        const double err = std::abs(back.red() - rgb.red()) + std::abs(back.green() - rgb.green()) + std::abs(back.blue() - rgb.blue());
        sum += err; mx = std::max(mx, err); if (err > 12) ++bad;
    }
    printf("%-34s round trip: mean %.2f, max %.0f, over 12: %d of %d\n", label, sum / n, mx, bad, n);
}

int main()
{
    KrimbleCmykProfile p;
    KrimbleCmykEngine e(p);
    printf("gray ramp (default profile):\n");
    double prev[4] = {0, 0, 0, 0}, prev2[4] = {0, 0, 0, 0}, maxd2 = 0;
    for (int v = 255, i = 0; v >= 0; v -= 15, ++i) {
        const KrimblePlates pl = e.fromRgb(QColor(v, v, v));
        printf("gray %3d  C %5.1f M %5.1f Y %5.1f K %5.1f  total %5.1f  back %3d\n", v, pl[0] * 100, pl[1] * 100, pl[2] * 100, pl[3] * 100, e.totalInk(pl), e.toRgb(pl).red());
        if (i >= 2) for (int c = 0; c < 4; ++c) maxd2 = std::max(maxd2, std::abs(pl[c] * 100 - 2 * prev[c] + prev2[c]));
        for (int c = 0; c < 4; ++c) { prev2[c] = prev[c]; prev[c] = pl[c] * 100; }
    }
    printf("roughness (largest second difference): %.2f\n\n", maxd2);
    run("SWOP, 20%, GCR Medium, 300", p);
    p.blackGeneration = "Heavy"; run("GCR Heavy", p);
    p.blackGeneration = "Light"; run("GCR Light", p);
    p.blackGeneration = "None"; run("GCR None", p);
    p.blackGeneration = "Maximum"; run("GCR Maximum", p);
    p.blackGeneration = "Medium"; p.gcr = false; run("UCR", p);
    p.gcr = true; p.totalLimit = 240; run("GCR Medium, total 240", p);
    p.uca = 20; run("GCR Medium, total 240, UCA 20", p);
    return 0;
}
