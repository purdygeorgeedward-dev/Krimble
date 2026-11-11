// Krimble 2026-11-11: moved from plugins/dockers/advancedcolorselector/krimble_cmyk_engine.h to libs/ui/KrimbleCmykEngine.h so that the program (the Custom CMYK dialog, KrimbleCustomCmykDialog.h) and the Color Lab plugin can both include it.
// Krimble 2026-11-08: George: "Please build this and include any available open source formulas for professional conversion. We should have some equivalent tools to [the industry standard] in the long run."
// (the industry standard Custom CMYK, Ink Colors, Dot Gain Curves and Convert to Profile dialogs are the model).
//
// A CMYK conversion engine for the Color Lab: RGB (sRGB) to and from CMYK plates (percent of ink) for a "profile" the user sets in the Custom CMYK dialog:
//   - ink colors: the colors of the paper, the 3 inks, their overprints and black (CIE Lab D50), with presets (SWOP Coated from the industry standard Ink Colors dialog; ISO 12647-2 coated and matte coated
//     from the ISO / ICC printing conditions),
//   - dot gain: the "Standard" curve from one number (the gain at 50%; a smooth curve fitted to the industry standard one, see KrimbleDotGain::standard), or a table of 13 points per ink (monotone cubic),
//   - separation: UCR or GCR black generation (None, Light, Medium, Heavy, Maximum), black ink limit, total ink limit and undercolor addition (UCA),
//   - black point compensation.
// The formulas are the open ones: the Neugebauer equations with the Demichel dot overlap weights for the colors of the 16 overprints (the overprints with black are the black ink's color times
// the other overprint's reflectance), the PostScript / PDF black generation and undercolor removal (k = BG(min(c, m, y)), c' = c - UCR(k)), the "total area coverage" guard (when the sum of the four
// inks is over the limit, black is raised and C, M, Y are reduced equally until it fits) and a least squares (Levenberg-Marquardt) solution of C, M, Y for a chosen K so that the printed color
// matches the wanted color.
#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include <QColor>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QVector>

struct KrimbleVec3
{
    double x = 0, y = 0, z = 0;
    KrimbleVec3() = default;
    KrimbleVec3(double a, double b, double c) : x(a), y(b), z(c) {}
    KrimbleVec3 operator+(const KrimbleVec3 &o) const { return {x + o.x, y + o.y, z + o.z}; }
    KrimbleVec3 operator-(const KrimbleVec3 &o) const { return {x - o.x, y - o.y, z - o.z}; }
    KrimbleVec3 operator*(double s) const { return {x * s, y * s, z * s}; }
    double dot(const KrimbleVec3 &o) const { return x * o.x + y * o.y + z * o.z; }
};

typedef std::array<double, 4> KrimblePlates;   // C, M, Y, K as 0 to 1

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// color math (D50, sRGB)
namespace KrimbleColorMath {
static const KrimbleVec3 kD50(0.96422, 1.0, 0.82521);

inline KrimbleVec3 labToXyz(double L, double a, double b)
{
    const double fy = (L + 16.0) / 116.0, fx = fy + a / 500.0, fz = fy - b / 200.0;
    auto finv = [](double t) { const double t3 = t * t * t; return t3 > 216.0 / 24389.0 ? t3 : (116.0 * t - 16.0) / (24389.0 / 27.0); };
    return {kD50.x * finv(fx), kD50.y * finv(fy), kD50.z * finv(fz)};
}
inline void xyzToLab(const KrimbleVec3 &v, double &L, double &a, double &b)
{
    auto f = [](double t) { return t > 216.0 / 24389.0 ? std::cbrt(t) : ((24389.0 / 27.0) * t + 16.0) / 116.0; };
    const double fx = f(v.x / kD50.x), fy = f(v.y / kD50.y), fz = f(v.z / kD50.z);
    L = 116.0 * fy - 16.0;
    a = 500.0 * (fx - fy);
    b = 200.0 * (fy - fz);
}
inline KrimbleVec3 xyYToXyz(double Y, double x, double y)   // Y in percent (100 = a perfect white)
{
    const double Yn = Y / 100.0;
    if (y <= 0.0) return {0, Yn, 0};
    return {x / y * Yn, Yn, (1.0 - x - y) / y * Yn};
}
inline double srgbDecode(double v) { return v <= 0.04045 ? v / 12.92 : std::pow((v + 0.055) / 1.055, 2.4); }
inline double srgbEncode(double v) { v = std::clamp(v, 0.0, 1.0); return v <= 0.0031308 ? v * 12.92 : 1.055 * std::pow(v, 1.0 / 2.4) - 0.055; }
inline KrimbleVec3 srgbToXyz(const QColor &c)
{
    const double r = srgbDecode(c.redF()), g = srgbDecode(c.greenF()), b = srgbDecode(c.blueF());
    return {0.4360747 * r + 0.3850649 * g + 0.1430804 * b, 0.2225045 * r + 0.7168786 * g + 0.0606169 * b, 0.0139322 * r + 0.0971045 * g + 0.7141733 * b};
}
inline QColor xyzToSrgb(const KrimbleVec3 &v)
{
    const double r = 3.1338561 * v.x - 1.6168667 * v.y - 0.4906146 * v.z;
    const double g = -0.9787684 * v.x + 1.9161415 * v.y + 0.0334540 * v.z;
    const double b = 0.0719453 * v.x - 0.2289914 * v.y + 1.4052427 * v.z;
    return QColor::fromRgbF(float(srgbEncode(r)), float(srgbEncode(g)), float(srgbEncode(b)));
}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ink colors
struct KrimbleInkSet
{
    // the colors (XYZ, D50, 1.0 = a perfect white) of: 0 paper, 1 C, 2 M, 3 Y, 4 MY (red), 5 CY (green), 6 CM (blue), 7 CMY, 8 K (solid black ink on the paper)
    std::array<KrimbleVec3, 9> xyz;
    bool estimateOverprints = false;

    void setLab(int i, double L, double a, double b) { xyz[i] = KrimbleColorMath::labToXyz(L, a, b); }
    void labOf(int i, double &L, double &a, double &b) const { KrimbleColorMath::xyzToLab(xyz[i], L, a, b); }
    // the overprints made from the single inks: the reflectances multiply (the paper's color divided out)
    void estimate()
    {
        auto mul = [&](const KrimbleVec3 &p, const KrimbleVec3 &q) { return KrimbleVec3(p.x * q.x / xyz[0].x, p.y * q.y / xyz[0].y, p.z * q.z / xyz[0].z); };
        xyz[4] = mul(xyz[2], xyz[3]);
        xyz[5] = mul(xyz[1], xyz[3]);
        xyz[6] = mul(xyz[1], xyz[2]);
        xyz[7] = mul(xyz[6], xyz[3]);
    }
    static QStringList presetNames()
    {
        return {QStringLiteral("SWOP (Coated)"), QStringLiteral("ISO Coated (FOGRA39)"), QStringLiteral("ISO Matte Coated (PC4)")};
    }
    static bool preset(const QString &name, KrimbleInkSet &out)
    {
        KrimbleInkSet s;
        if (name == QLatin1String("SWOP (Coated)")) {
            // the values of the industry standard Ink Colors dialog for SWOP (Coated): Y (percent), x, y of C, M, Y, MY, CY, CM, CMY, paper, K
            static const double v[9][3] = {{83.02, .3149, .3321}, {26.25, .1673, .2328}, {14.50, .4845, .2396}, {71.20, .4357, .5013}, {14.09, .6075, .3191},
                                           {19.25, .2271, .5513}, {2.98, .2052, .1245}, {2.79, .3227, .2962}, {0.82, .3202, .3241}};
            for (int i = 0; i < 9; ++i) s.xyz[i] = KrimbleColorMath::xyYToXyz(v[i][0], v[i][1], v[i][2]);
        } else if (name == QLatin1String("ISO Coated (FOGRA39)")) {
            // ISO 12647-2:2004 (paper type 1 and 2), the characterization data FOGRA39: CIE Lab D50
            s.setLab(0, 95, 0, -2);
            s.setLab(1, 55, -37, -50);
            s.setLab(2, 48, 74, -3);
            s.setLab(3, 89, -5, 93);
            s.setLab(4, 47, 68, 48);
            s.setLab(5, 50, -65, 27);
            s.setLab(6, 24, 22, -46);
            s.setLab(8, 16, 0, 0);
            KrimbleInkSet e = s;
            e.estimate();
            s.xyz[7] = e.xyz[7];
        } else if (name == QLatin1String("ISO Matte Coated (PC4)")) {
            // the ISO 12647-2 / ICC printing condition PC4 (standard matte coated): paper and the colour aims of the 4 inks; the overprints are estimated
            s.setLab(0, 91, 0, 1);
            s.setLab(1, 56, -33, -42);
            s.setLab(2, 48, 68, -1);
            s.setLab(3, 85, -2, 83);
            s.setLab(8, 24, 1, 2);
            s.estimate();
            s.estimateOverprints = true;
        } else {
            return false;
        }
        out = s;
        return true;
    }
};

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// dot gain
struct KrimbleDotGain
{
    static const int N = 13;
    static constexpr int kPoints[N] = {2, 4, 6, 8, 10, 20, 30, 40, 50, 60, 70, 80, 90};
    bool curves = false;       // false: the Standard curve from percent; true: the table below
    double percent = 20.0;     // the gain of a 50% dot
    bool allSame = true;
    double table[4][N];        // the printed dot (percent) for a plate dot of kPoints[i], for C, M, Y, K (used when curves is true)

    KrimbleDotGain() { fillFromStandard(); }
    // The Standard curve: the gain of a dot of plate area a is gain50 * sin(pi * a)^0.8 (most at 50%, none at 0 and 100%). The shape was fitted to the Dot Gain Curves table of the industry standard Custom CMYK
    // dialog (a 23% curve: +10 at 10%, +16 at 20%, +21.5 at 30%, +23 at 50%, +15 at 80%, +8.5 at 90%; this gives +9, +15, +19, +23, +15, +9). The GRL formula of Viggiano,
    // gain = 2 * gain50 * sqrt(a * (1 - a)), is the older open one; it has a flat top near 90% that a solver cannot climb, so it is not used. The printed dot never reaches 100% before the plate does
    // (the curve is kept below a + (1 - a) * 0.98), which keeps it rising all the way.
    static double standard(double a, double gain50)
    {
        a = std::clamp(a, 0.0, 1.0);
        const double v = a + gain50 * std::pow(std::sin(M_PI * a), 0.8);
        return std::clamp(std::min(v, 0.02 * a + 0.98), 0.0, 1.0);
    }
    void fillFromStandard()
    {
        for (int ch = 0; ch < 4; ++ch)
            for (int i = 0; i < N; ++i) table[ch][i] = standard(kPoints[i] / 100.0, percent / 100.0) * 100.0;
    }
    // a monotone cubic through (0, 0), the 13 points and (1, 1) (Fritsch and Carlson)
    double apply(int ch, double a) const
    {
        a = std::clamp(a, 0.0, 1.0);
        if (!curves) return standard(a, percent / 100.0);
        const int n = N + 2;
        double x[n], y[n], d[n - 1], m[n];
        x[0] = 0; y[0] = 0; x[n - 1] = 1; y[n - 1] = 1;
        for (int i = 0; i < N; ++i) { x[i + 1] = kPoints[i] / 100.0; y[i + 1] = std::clamp(table[ch][i] / 100.0, 0.0, 1.0); }
        for (int i = 0; i + 1 < n; ++i) d[i] = (y[i + 1] - y[i]) / (x[i + 1] - x[i]);
        m[0] = d[0]; m[n - 1] = d[n - 2];
        for (int i = 1; i + 1 < n; ++i) m[i] = (d[i - 1] * d[i] <= 0.0) ? 0.0 : (d[i - 1] + d[i]) / 2.0;
        for (int i = 0; i + 1 < n; ++i) {
            if (d[i] == 0.0) { m[i] = 0.0; m[i + 1] = 0.0; continue; }
            const double al = m[i] / d[i], be = m[i + 1] / d[i], h = std::hypot(al, be);
            if (h > 3.0) { const double t = 3.0 / h; m[i] = t * al * d[i]; m[i + 1] = t * be * d[i]; }
        }
        int k = 0;
        while (k + 2 < n && a > x[k + 1]) ++k;
        const double h = x[k + 1] - x[k], t = (a - x[k]) / h, t2 = t * t, t3 = t2 * t;
        const double v = (2 * t3 - 3 * t2 + 1) * y[k] + (t3 - 2 * t2 + t) * h * m[k] + (-2 * t3 + 3 * t2) * y[k + 1] + (t3 - t2) * h * m[k + 1];
        return std::clamp(v, 0.0, 1.0);
    }
};
constexpr int KrimbleDotGain::kPoints[KrimbleDotGain::N];

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// the profile: everything the Custom CMYK dialog sets
struct KrimbleCmykProfile
{
    QString name;                 // empty: made from the settings, like "SWOP (Coated), 20%, GCR, Medium"
    QString inkPreset = QStringLiteral("SWOP (Coated)");   // a preset's name, or "Custom"
    KrimbleInkSet inks;
    KrimbleDotGain dotGain;
    bool gcr = true;              // false: UCR
    QString blackGeneration = QStringLiteral("Medium");   // None, Light, Medium, Heavy, Maximum (GCR only)
    double blackLimit = 100.0;    // percent
    double totalLimit = 300.0;    // percent
    double uca = 0.0;             // undercolor addition, percent (GCR only)
    bool blackPointCompensation = true;
    bool limitInkWhenEditing = true;   // when a plate of a CMYK palette swatch is edited, the ink limits hold

    KrimbleCmykProfile() { KrimbleInkSet::preset(inkPreset, inks); }
    QString autoName() const
    {
        QString g = dotGain.curves ? QStringLiteral("Curves") : QStringLiteral("%1%").arg(qRound(dotGain.percent));
        QString s = inkPreset + QStringLiteral(", ") + g + QStringLiteral(", ") + (gcr ? QStringLiteral("GCR, ") + blackGeneration : QStringLiteral("UCR"));
        return s;
    }
    QString displayName() const { return name.isEmpty() ? autoName() : name; }

    QJsonObject toJson() const
    {
        QJsonObject o;
        o["name"] = name;
        o["inkPreset"] = inkPreset;
        QJsonArray inksArr;
        for (int i = 0; i < 9; ++i) { double L, a, b; inks.labOf(i, L, a, b); inksArr.append(QJsonArray{L, a, b}); }
        o["inks"] = inksArr;
        o["estimateOverprints"] = inks.estimateOverprints;
        o["dotGainCurves"] = dotGain.curves;
        o["dotGainPercent"] = dotGain.percent;
        o["dotGainAllSame"] = dotGain.allSame;
        QJsonArray tbl;
        for (int ch = 0; ch < 4; ++ch) { QJsonArray row; for (int i = 0; i < KrimbleDotGain::N; ++i) row.append(dotGain.table[ch][i]); tbl.append(row); }
        o["dotGainTable"] = tbl;
        o["gcr"] = gcr;
        o["blackGeneration"] = blackGeneration;
        o["blackLimit"] = blackLimit;
        o["totalLimit"] = totalLimit;
        o["uca"] = uca;
        o["bpc"] = blackPointCompensation;
        o["limitInkWhenEditing"] = limitInkWhenEditing;
        return o;
    }
    static KrimbleCmykProfile fromJson(const QJsonObject &o)
    {
        KrimbleCmykProfile p;
        p.name = o["name"].toString();
        p.inkPreset = o.value("inkPreset").toString(p.inkPreset);
        const QJsonArray inksArr = o["inks"].toArray();
        if (inksArr.size() == 9)
            for (int i = 0; i < 9; ++i) { const QJsonArray v = inksArr[i].toArray(); if (v.size() == 3) p.inks.setLab(i, v[0].toDouble(), v[1].toDouble(), v[2].toDouble()); }
        p.inks.estimateOverprints = o["estimateOverprints"].toBool(false);
        p.dotGain.curves = o["dotGainCurves"].toBool(false);
        p.dotGain.percent = o.value("dotGainPercent").toDouble(20.0);
        p.dotGain.allSame = o["dotGainAllSame"].toBool(true);
        const QJsonArray tbl = o["dotGainTable"].toArray();
        if (tbl.size() == 4)
            for (int ch = 0; ch < 4; ++ch) { const QJsonArray row = tbl[ch].toArray(); for (int i = 0; i < KrimbleDotGain::N && i < row.size(); ++i) p.dotGain.table[ch][i] = row[i].toDouble(); }
        p.gcr = o.value("gcr").toBool(true);
        p.blackGeneration = o.value("blackGeneration").toString(p.blackGeneration);
        p.blackLimit = o.value("blackLimit").toDouble(100.0);
        p.totalLimit = o.value("totalLimit").toDouble(300.0);
        p.uca = o.value("uca").toDouble(0.0);
        p.blackPointCompensation = o.value("bpc").toBool(true);
        p.limitInkWhenEditing = o.value("limitInkWhenEditing").toBool(true);
        return p;
    }
};

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// the engine
class KrimbleCmykEngine
{
public:
    explicit KrimbleCmykEngine(const KrimbleCmykProfile &profile) : m_p(profile)
    {
        m_inks = m_p.inks;
        if (m_inks.estimateOverprints) m_inks.estimate();
        // the 16 overprints: index bits 1 = C, 2 = M, 4 = Y, 8 = K
        const int map[8] = {0, 1, 2, 6 /*CM*/, 3 /*Y*/, 5 /*CY*/, 4 /*MY*/, 7 /*CMY*/};   // bits (C=1, M=2, Y=4) to the ink set's index
        for (int i = 0; i < 8; ++i) m_prim[i] = m_inks.xyz[map[i]];
        const KrimbleVec3 w = m_inks.xyz[0], k = m_inks.xyz[8];
        for (int i = 0; i < 8; ++i)
            m_prim[8 + i] = KrimbleVec3(k.x * m_prim[i].x / w.x, k.y * m_prim[i].y / w.y, k.z * m_prim[i].z / w.z);
        m_paper = m_inks.xyz[0];
        const double klim = std::min(m_p.blackLimit, m_p.totalLimit) / 100.0;
        const double cmy = std::clamp((m_p.totalLimit / 100.0 - klim) / 3.0, 0.0, 1.0);
        m_black = forward({cmy, cmy, cmy, klim});
    }
    const KrimbleCmykProfile &profile() const { return m_p; }

    // the color (XYZ, D50) that the four plates (0 to 1) print: dot gain, then the Neugebauer equations with the Demichel weights
    KrimbleVec3 forward(const KrimblePlates &plates) const
    {
        double a[4];
        for (int i = 0; i < 4; ++i) a[i] = m_p.dotGain.apply(m_p.dotGain.allSame ? 0 : i, std::clamp(plates[i], 0.0, 1.0));
        KrimbleVec3 out;
        for (int s = 0; s < 16; ++s) {
            double w = 1.0;
            for (int i = 0; i < 4; ++i) w *= (s & (1 << i)) ? a[i] : (1.0 - a[i]);
            if (w <= 0.0) continue;
            out = out + m_prim[s] * w;
        }
        return out;
    }

    double totalInk(const KrimblePlates &p) const { return (p[0] + p[1] + p[2] + p[3]) * 100.0; }

    // sRGB to plates
    KrimblePlates fromRgb(const QColor &color) const
    {
        return separate(sourceToPaper(KrimbleColorMath::srgbToXyz(color.toRgb())));
    }
    // plates to sRGB
    QColor toRgb(const KrimblePlates &plates) const
    {
        return KrimbleColorMath::xyzToSrgb(paperToSource(forward(plates)));
    }

    // the ink limits as a guard for plates that are typed in: the black is capped at the black ink limit and the total at the total ink limit (the plate that was just changed gives way)
    KrimblePlates limitInk(KrimblePlates p, int changed = -1) const
    {
        for (double &v : p) v = std::clamp(v, 0.0, 1.0);
        p[3] = std::min(p[3], m_p.blackLimit / 100.0);
        const double limit = m_p.totalLimit / 100.0;
        double sum = p[0] + p[1] + p[2] + p[3];
        if (sum > limit + 1e-9) {
            if (changed >= 0 && changed < 4) {
                p[changed] = std::max(0.0, p[changed] - (sum - limit));
                sum = p[0] + p[1] + p[2] + p[3];
            }
            if (sum > limit + 1e-9) {   // still over: C, M, Y give the same amount each (black stays)
                const double room = std::max(0.0, limit - p[3]);
                const double cmy = p[0] + p[1] + p[2];
                if (cmy > 0.0) { const double s = room / cmy; for (int i = 0; i < 3; ++i) p[i] *= std::min(1.0, s); }
                p[3] = std::min(p[3], limit);
            }
        }
        return p;
    }

    // the plates of a neutral ramp, from white (0) to black (1), for the Gray Ramp picture
    QVector<KrimblePlates> grayRamp(int steps) const
    {
        QVector<KrimblePlates> out;
        for (int i = 0; i <= steps; ++i) {
            const double v = 1.0 - double(i) / steps;
            out.append(fromRgb(QColor::fromRgbF(float(v), float(v), float(v))));
        }
        return out;
    }

    // the black generation: the FRACTION (0 to 1) of the gray component (the smallest of C, M, Y of the K = 0 separation) that black takes over. GCR starts at a gray level that the preset sets
    // (Light 40%, Medium 20%, Heavy 10%, Maximum from the start, as in the industry standard Black Generation presets) and rises to the full replacement at the darkest.
    double blackFraction(double g, double neutrality) const
    {
        double g0 = 0.2, pw = 1.2;
        const QString bg = m_p.blackGeneration;
        if (m_p.gcr) {
            if (bg == QLatin1String("None")) return 0.0;
            if (bg == QLatin1String("Light")) { g0 = 0.40; pw = 1.6; }
            else if (bg == QLatin1String("Heavy")) { g0 = 0.10; pw = 0.9; }
            else if (bg == QLatin1String("Maximum")) { g0 = 0.0; pw = 0.5; }
            const double u = std::clamp((g - g0) / (1.0 - g0), 0.0, 1.0);
            return std::pow(u, pw);
        }
        // UCR: only dark, neutral colors get black ("skeleton" black); chromatic colors keep their C, M, Y
        g0 = 0.55; pw = 1.5;
        const double u = std::clamp((g - g0) / (1.0 - g0), 0.0, 1.0);
        return std::pow(u, pw) * neutrality;
    }
    // the K plate that is as dark as C, M, Y all at g (the "gray component" in K plate terms)
    double equivalentBlack(double g) const
    {
        const double target = forward({g, g, g, 0.0}).y;
        if (forward({0.0, 0.0, 0.0, 1.0}).y >= target) return 1.0;
        double lo = 0.0, hi = 1.0;
        for (int i = 0; i < 30; ++i) {
            const double mid = (lo + hi) / 2.0;
            if (forward({0.0, 0.0, 0.0, mid}).y > target) lo = mid; else hi = mid;
        }
        return (lo + hi) / 2.0;
    }

private:
    KrimbleVec3 sourceToPaper(const KrimbleVec3 &in) const
    {
        const KrimbleVec3 w = KrimbleColorMath::kD50;
        const KrimbleVec3 bd = m_p.blackPointCompensation ? m_black : KrimbleVec3();
        return {bd.x + (m_paper.x - bd.x) * in.x / w.x, bd.y + (m_paper.y - bd.y) * in.y / w.y, bd.z + (m_paper.z - bd.z) * in.z / w.z};
    }
    KrimbleVec3 paperToSource(const KrimbleVec3 &p) const
    {
        const KrimbleVec3 w = KrimbleColorMath::kD50;
        const KrimbleVec3 bd = m_p.blackPointCompensation ? m_black : KrimbleVec3();
        auto back = [](double v, double b, double wp, double wd) { const double den = wp - b; return den > 1e-9 ? (v - b) / den * wd : 0.0; };
        return {back(p.x, bd.x, m_paper.x, w.x), back(p.y, bd.y, m_paper.y, w.y), back(p.z, bd.z, m_paper.z, w.z)};
    }

    // C, M, Y (least squares, Levenberg-Marquardt) so that the plates with the given K print the wanted color
    void solveLm(const KrimbleVec3 &target, double k, double *cmy, double *errOut = nullptr) const
    {
        // the match is measured in CIE Lab (the error is the square of the color difference, delta E): the same error means the same visible difference in light and dark colors
        auto toLab = [](const KrimbleVec3 &v) { double L, a, b; KrimbleColorMath::xyzToLab(v, L, a, b); return KrimbleVec3(L, a, b); };
        const KrimbleVec3 targetLab = toLab(target);
        auto eval = [&](const double *q) { return toLab(forward({q[0], q[1], q[2], k})) - targetLab; };
        double q[3] = {cmy[0], cmy[1], cmy[2]};
        KrimbleVec3 r = eval(q);
        double err = r.dot(r), lambda = 1e-3;
        for (int iter = 0; iter < 60; ++iter) {
            KrimbleVec3 J[3];
            for (int j = 0; j < 3; ++j) {
                double q2[3] = {q[0], q[1], q[2]};
                const double h = (q2[j] < 0.5) ? 1e-3 : -1e-3;
                q2[j] += h;
                J[j] = (eval(q2) - r) * (1.0 / h);
            }
            double A[3][3], g[3];
            for (int i = 0; i < 3; ++i) {
                g[i] = J[i].dot(r);
                for (int j = 0; j < 3; ++j) A[i][j] = J[i].dot(J[j]) + (i == j ? lambda : 0.0);
            }
            double dp[3];
            if (!solve3(A, g, dp)) break;
            double q3[3];
            for (int i = 0; i < 3; ++i) q3[i] = std::clamp(q[i] - dp[i], 0.0, 1.0);
            const KrimbleVec3 r3 = eval(q3);
            const double err3 = r3.dot(r3);
            if (err3 < err) {
                const double gain = err - err3;
                for (int i = 0; i < 3; ++i) q[i] = q3[i];
                r = r3; err = err3; lambda = std::max(lambda * 0.4, 1e-9);
                if (gain < 1e-9) break;
            } else {
                lambda *= 5.0;
                if (lambda > 1e6) break;
            }
        }
        for (int i = 0; i < 3; ++i) cmy[i] = q[i];
        if (errOut) *errOut = err;
    }
    // C, M, Y for a chosen K: the Levenberg-Marquardt from three starting points (a guess that allows for the black, the hint the caller gives, and the middle), the best is kept
    void solveCmy(const KrimbleVec3 &target, double k, double *cmy, double *errOut = nullptr) const
    {
        double hint[3] = {cmy[0], cmy[1], cmy[2]};
        double a[3], ea = 0.0;
        guessFor(target, k, a);
        solveLm(target, k, a, &ea);
        double best[3] = {a[0], a[1], a[2]}, eb = ea;
        if (eb > 0.01) {
            double b[3] = {hint[0], hint[1], hint[2]}, e2 = 0.0;
            solveLm(target, k, b, &e2);
            if (e2 < eb) { eb = e2; for (int i = 0; i < 3; ++i) best[i] = b[i]; }
        }
        if (eb > 0.1) {
            double c[3] = {0.5, 0.5, 0.5}, e3 = 0.0;
            solveLm(target, k, c, &e3);
            if (e3 < eb) { eb = e3; for (int i = 0; i < 3; ++i) best[i] = c[i]; }
        }
        for (int i = 0; i < 3; ++i) cmy[i] = best[i];
        if (errOut) *errOut = eb;
    }
    // a first guess for C, M, Y when black is K: black dims every channel (the black ink's color over the paper's), so C, M, Y only have to make the wanted color divided by that
    void guessFor(const KrimbleVec3 &t, double k, double *cmy) const
    {
        const double a = m_p.dotGain.apply(m_p.dotGain.allSame ? 0 : 3, std::clamp(k, 0.0, 1.0));
        const KrimbleVec3 kc = m_inks.xyz[8];
        const double fx = (1.0 - a) + a * kc.x / m_paper.x, fy = (1.0 - a) + a * kc.y / m_paper.y, fz = (1.0 - a) + a * kc.z / m_paper.z;
        KrimbleVec3 t2(std::min(t.x / std::max(fx, 1e-6), m_paper.x), std::min(t.y / std::max(fy, 1e-6), m_paper.y), std::min(t.z / std::max(fz, 1e-6), m_paper.z));
        guess(t2, cmy);
    }
    static bool solve3(double A[3][3], const double *b, double *x)
    {
        double M[3][4];
        for (int i = 0; i < 3; ++i) { for (int j = 0; j < 3; ++j) M[i][j] = A[i][j]; M[i][3] = b[i]; }
        for (int c = 0; c < 3; ++c) {
            int piv = c;
            for (int r = c + 1; r < 3; ++r) if (std::fabs(M[r][c]) > std::fabs(M[piv][c])) piv = r;
            if (std::fabs(M[piv][c]) < 1e-18) return false;
            if (piv != c) for (int j = 0; j < 4; ++j) std::swap(M[c][j], M[piv][j]);
            for (int r = c + 1; r < 3; ++r) { const double f = M[r][c] / M[c][c]; for (int j = c; j < 4; ++j) M[r][j] -= f * M[c][j]; }
        }
        for (int i = 2; i >= 0; --i) { double s = M[i][3]; for (int j = i + 1; j < 3; ++j) s -= M[i][j] * x[j]; x[i] = s / M[i][i]; }
        return true;
    }

    // a first guess for C, M, Y from the wanted color
    void guess(const KrimbleVec3 &t, double *cmy) const
    {
        const double rx = std::clamp(t.x / m_paper.x, 0.0, 1.0), ry = std::clamp(t.y / m_paper.y, 0.0, 1.0), rz = std::clamp(t.z / m_paper.z, 0.0, 1.0);
        const double r = 3.1338561 * rx * 0.96422 - 1.6168667 * ry - 0.4906146 * rz * 0.82521;
        const double g = -0.9787684 * rx * 0.96422 + 1.9161415 * ry + 0.0334540 * rz * 0.82521;
        const double b = 0.0719453 * rx * 0.96422 - 0.2289914 * ry + 1.4052427 * rz * 0.82521;
        cmy[0] = std::clamp(1.0 - r, 0.0, 1.0);
        cmy[1] = std::clamp(1.0 - g, 0.0, 1.0);
        cmy[2] = std::clamp(1.0 - b, 0.0, 1.0);
    }

public:
    // the wanted color (XYZ on the paper) to plates: black from the gray component, C, M, Y to match, then the ink limits.
    // Black never takes so much that the color can no longer be matched (the biggest K with the same match as K = 0 is used), and the total ink guard raises black (and matches C, M, Y again) before
    // it reduces anything.
    KrimblePlates separate(const KrimbleVec3 &target) const
    {
        double cmy0[3];
        guess(target, cmy0);
        double err0 = 0.0;
        solveCmy(target, 0.0, cmy0, &err0);               // the K = 0 separation
        const double g = std::min({cmy0[0], cmy0[1], cmy0[2]});
        const double hi0 = std::max({cmy0[0], cmy0[1], cmy0[2]});
        const double neutrality = std::clamp(1.0 - (hi0 - g) / 0.35, 0.0, 1.0);
        const double klim = std::min(m_p.blackLimit, m_p.totalLimit) / 100.0;
        const double limit = m_p.totalLimit / 100.0;

        // how well can this color be matched at all (with the best black)? a scan of K from 0 to the black limit; a black generation may not be worse than that match
        const int scan = 8;
        double scanErr[scan + 1], scanCmy[scan + 1][3];
        double errBest = 1e9;
        for (int j2 = 0; j2 <= scan; ++j2) {
            double c[3];
            if (j2 == 0) { for (int a = 0; a < 3; ++a) c[a] = cmy0[a]; scanErr[0] = err0; }
            else { guess(target, c); solveCmy(target, klim * j2 / scan, c, &scanErr[j2]); }
            for (int a = 0; a < 3; ++a) scanCmy[j2][a] = c[a];
            errBest = std::min(errBest, scanErr[j2]);
        }
        const double tol = std::max(0.8, errBest * 1.3);   // delta E squared: a black generation may be at most about one delta E worse than the best match
        int jmin = 0;
        while (jmin < scan && scanErr[jmin] > tol) ++jmin;   // the least black that matches (colors darker than CMY can be need some black)
        const double kmin = klim * jmin / scan;

        double k = std::clamp(blackFraction(g, neutrality) * equivalentBlack(g), kmin, klim);
        double cmy[3] = {scanCmy[jmin][0], scanCmy[jmin][1], scanCmy[jmin][2]};
        double err = scanErr[jmin];
        if (k > kmin + 1e-9) {
            double t[3] = {cmy[0], cmy[1], cmy[2]}, e = 0.0;
            solveCmy(target, k, t, &e);
            if (e <= tol) { for (int a = 0; a < 3; ++a) cmy[a] = t[a]; err = e; }
            else {   // too much black for this color: the biggest K between the least black and the wanted that still matches
                double lo = kmin, up = k;
                double ok[3] = {cmy[0], cmy[1], cmy[2]};
                for (int it = 0; it < 14; ++it) {
                    const double mid = (lo + up) / 2.0;
                    double t2[3] = {ok[0], ok[1], ok[2]}, e2 = 0.0;
                    solveCmy(target, mid, t2, &e2);
                    if (e2 <= tol) { lo = mid; for (int a = 0; a < 3; ++a) ok[a] = t2[a]; } else up = mid;
                }
                k = lo;
                for (int a = 0; a < 3; ++a) cmy[a] = ok[a];
                err = tol;
            }
        } else {
            k = kmin;
        }
        // the total ink guard
        if (cmy[0] + cmy[1] + cmy[2] + k > limit + 1e-9) {
            bool found = false;
            double bestK = k, bestCmy[3] = {cmy[0], cmy[1], cmy[2]}, bestErr = 1e9;
            double t[3] = {cmy[0], cmy[1], cmy[2]};
            double lastK = k, lastCmy[3] = {cmy[0], cmy[1], cmy[2]};
            const int steps = 24;
            for (int j2 = 1; j2 <= steps; ++j2) {
                const double kk = k + (klim - k) * double(j2) / steps;
                double e = 0.0;
                solveCmy(target, kk, t, &e);
                lastK = kk; for (int a = 0; a < 3; ++a) lastCmy[a] = t[a];
                if (t[0] + t[1] + t[2] + kk <= limit + 1e-9) {
                    if (e <= tol) { bestK = kk; for (int a = 0; a < 3; ++a) bestCmy[a] = t[a]; bestErr = e; found = true; break; }
                    if (e < bestErr) { bestK = kk; for (int a = 0; a < 3; ++a) bestCmy[a] = t[a]; bestErr = e; found = true; }
                }
            }
            if (found) { k = bestK; for (int a = 0; a < 3; ++a) cmy[a] = bestCmy[a]; }
            else {   // black is at its limit and the inks are still over: C, M, Y are reduced equally
                k = lastK;
                const double sum = lastCmy[0] + lastCmy[1] + lastCmy[2], room = std::max(0.0, limit - k);
                const double s = sum > 0 ? std::min(1.0, room / sum) : 1.0;
                for (int a = 0; a < 3; ++a) cmy[a] = lastCmy[a] * s;
            }
        }
        // undercolor addition (GCR only): some C, M, Y put back under the black of neutral shadows
        if (m_p.gcr && m_p.uca > 0.0 && k > 0.0) {
            const double add = m_p.uca / 100.0 * (k / std::max(0.01, m_p.blackLimit / 100.0)) * neutrality;
            for (int a = 0; a < 3; ++a) cmy[a] = std::min(1.0, cmy[a] + add);
            const double sum = cmy[0] + cmy[1] + cmy[2];
            if (sum + k > limit) { const double s = std::max(0.0, (limit - k) / sum); for (int a = 0; a < 3; ++a) cmy[a] *= std::min(1.0, s); }
        }
        return {cmy[0], cmy[1], cmy[2], k};
    }

private:
    KrimbleCmykProfile m_p;
    KrimbleInkSet m_inks;
    KrimbleVec3 m_prim[16];
    KrimbleVec3 m_paper;
    KrimbleVec3 m_black;
};

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// the working profile (the one the Color Lab converts with) and the saved profiles (files in the app's data folder)
class KrimbleCmykProfiles
{
public:
    static QString &directoryOverride() { static QString d; return d; }
    static QString directory()
    {
        QString dir = directoryOverride();
        if (dir.isEmpty()) dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/krimble-cmyk");
        QDir().mkpath(dir);
        return dir;
    }
    static int &version() { static int v = 0; return v; }
    static KrimbleCmykProfile current()
    {
        const QByteArray json = QSettings().value(QStringLiteral("Krimble/CmykProfile")).toByteArray();
        const QJsonDocument doc = QJsonDocument::fromJson(json);
        if (doc.isObject()) return KrimbleCmykProfile::fromJson(doc.object());
        return KrimbleCmykProfile();
    }
    static void setCurrent(const KrimbleCmykProfile &p)
    {
        QSettings().setValue(QStringLiteral("Krimble/CmykProfile"), QJsonDocument(p.toJson()).toJson(QJsonDocument::Compact));
        ++version();
    }
    // the engine of the working profile (made again when the profile changes)
    static const KrimbleCmykEngine &engine()
    {
        static int built = -1;
        static KrimbleCmykEngine *eng = nullptr;
        if (!eng || built != version()) {
            delete eng;
            eng = new KrimbleCmykEngine(current());
            built = version();
        }
        return *eng;
    }
    static QString fileFor(const QString &name)
    {
        QString safe = name;
        for (QChar &c : safe) if (!c.isLetterOrNumber() && c != QLatin1Char(' ') && c != QLatin1Char('-') && c != QLatin1Char('(') && c != QLatin1Char(')') && c != QLatin1Char(',') && c != QLatin1Char('%')) c = QLatin1Char('_');
        return directory() + QLatin1Char('/') + safe + QStringLiteral(".krimblecmyk");
    }
    static QStringList list()
    {
        QStringList names;
        const QStringList files = QDir(directory()).entryList(QStringList() << QStringLiteral("*.krimblecmyk"), QDir::Files, QDir::Name);
        for (const QString &f : files) names << f.left(f.size() - 12);
        return names;
    }
    static bool save(const KrimbleCmykProfile &p)
    {
        QFile f(fileFor(p.displayName()));
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
        f.write(QJsonDocument(p.toJson()).toJson(QJsonDocument::Indented));
        return true;
    }
    static bool load(const QString &name, KrimbleCmykProfile &out)
    {
        QFile f(fileFor(name));
        if (!f.open(QIODevice::ReadOnly)) return false;
        const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        if (!doc.isObject()) return false;
        out = KrimbleCmykProfile::fromJson(doc.object());
        return true;
    }
};
