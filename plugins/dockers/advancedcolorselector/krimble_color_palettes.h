// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-08: George: "palettes have names. You can save them." The palettes of the Color panel: each is a GPL file (the GIMP palette text format, which Krita, GIMP and many
// other programs read and write) in the program's data folder, with a name, a number of columns and its colors. The Palette page and the palette row of the Picker, Sliders and
// Values pages use them: the drop-down lists the names, the plus adds the foreground color to the active palette, and New, Open, Import, Export, Duplicate, Rename and Delete work on
// the files. The Mixer panel's "add to palette" adds to the active palette too.
#pragma once
#include <array>
#include <QColor>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVector>

// the four plates of a CMYK color, in percent (0 to 100)
typedef std::array<int, 4> KrimbleCmyk;
inline KrimbleCmyk krimbleCmykOf(const QColor &color)
{
    const QColor c = color.toCmyk();
    return {qRound(c.cyanF() * 100.0), qRound(c.magentaF() * 100.0), qRound(c.yellowF() * 100.0), qRound(c.blackF() * 100.0)};
}
inline QColor krimbleColorOfCmyk(const KrimbleCmyk &v)
{
    return QColor::fromCmykF(qBound(0, v[0], 100) / 100.0, qBound(0, v[1], 100) / 100.0, qBound(0, v[2], 100) / 100.0, qBound(0, v[3], 100) / 100.0).toRgb();
}

struct KrimblePalette
{
    QString name;
    QVector<QColor> colors;                // always filled: the RGB value of each swatch (for a CMYK palette: made from its plates)
    int columns = 16;
    // 2026-11-08: George: "I want to be able to edit RGB and CMYK palettes too." and "palettes stored as CMYK color values (4 channels, commonly called plates in printing)". A palette is
    // an RGB palette (each swatch is R, G, B) or a CMYK palette (each swatch is its four plates C, M, Y, K in percent, which is what is stored and edited; the RGB value is made from them).
    QString model = QStringLiteral("RGB");
    QVector<KrimbleCmyk> cmyk;             // for a CMYK palette: the plates of each swatch (the same size as colors)
    bool isCmyk() const { return model == QLatin1String("CMYK"); }
    void append(const QColor &c)
    {
        QColor rgb = c.toRgb();
        rgb.setAlpha(255);
        if (isCmyk()) { const KrimbleCmyk v = krimbleCmykOf(rgb); cmyk.append(v); colors.append(krimbleColorOfCmyk(v)); }
        else colors.append(rgb);
    }
    void appendCmyk(const KrimbleCmyk &v) { cmyk.append(v); colors.append(krimbleColorOfCmyk(v)); }
    void removeAt(int i)
    {
        if (i < 0 || i >= colors.size()) return;
        colors.removeAt(i);
        if (isCmyk() && i < cmyk.size()) cmyk.removeAt(i);
    }
    void replace(int i, const QColor &c)
    {
        if (i < 0 || i >= colors.size()) return;
        QColor rgb = c.toRgb();
        rgb.setAlpha(255);
        if (isCmyk()) { const KrimbleCmyk v = krimbleCmykOf(rgb); if (i < cmyk.size()) cmyk[i] = v; colors[i] = krimbleColorOfCmyk(v); }
        else colors[i] = rgb;
    }
    void replaceCmyk(int i, const KrimbleCmyk &v)
    {
        if (i < 0 || i >= colors.size()) return;
        if (i < cmyk.size()) cmyk[i] = v;
        colors[i] = krimbleColorOfCmyk(v);
    }
    void swap(int i, int j)
    {
        if (i < 0 || j < 0 || i >= colors.size() || j >= colors.size()) return;
        colors.swapItemsAt(i, j);
        if (isCmyk() && i < cmyk.size() && j < cmyk.size()) cmyk.swapItemsAt(i, j);
    }
    // a CMYK palette has its plates for every swatch (made from the RGB value where a file lacked them), and its RGB values come from the plates
    void normalize()
    {
        if (!isCmyk()) { cmyk.clear(); return; }
        while (cmyk.size() < colors.size()) cmyk.append(krimbleCmykOf(colors[cmyk.size()]));
        while (cmyk.size() > colors.size()) cmyk.removeLast();
        for (int i = 0; i < colors.size(); ++i) colors[i] = krimbleColorOfCmyk(cmyk[i]);
    }
};

class KrimblePaletteStore
{
public:
    // the folder of the palette files (tests can set another)
    static QString &directoryOverride() { static QString d; return d; }
    static QString directory()
    {
        QString dir = directoryOverride();
        if (dir.isEmpty()) dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/krimble-palettes");
        QDir().mkpath(dir);
        return dir;
    }
    static QString fileFor(const QString &name)
    {
        QString safe = name;
        for (QChar &c : safe) if (!c.isLetterOrNumber() && c != QLatin1Char(' ') && c != QLatin1Char('-') && c != QLatin1Char('_')) c = QLatin1Char('_');
        return directory() + QLatin1Char('/') + safe.trimmed() + QStringLiteral(".gpl");
    }
    static QStringList names()
    {
        QStringList out;
        const QDir dir(directory());
        for (const QFileInfo &info : dir.entryInfoList(QStringList() << QStringLiteral("*.gpl"), QDir::Files, QDir::Name | QDir::IgnoreCase)) {
            KrimblePalette pal;
            if (readFile(info.absoluteFilePath(), pal) && !pal.name.isEmpty()) out << pal.name;
        }
        return out;
    }
    static bool readFile(const QString &path, KrimblePalette &out)
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
        QTextStream in(&file);
        if (in.readLine().trimmed() != QLatin1String("GIMP Palette")) return false;
        out = KrimblePalette();
        out.name = QFileInfo(path).completeBaseName();
        while (!in.atEnd()) {
            const QString line = in.readLine().trimmed();
            if (line.startsWith(QLatin1String("#KrimbleModel:"))) { out.model = line.mid(14).trimmed().toUpper() == QLatin1String("CMYK") ? QStringLiteral("CMYK") : QStringLiteral("RGB"); continue; }
            if (line.startsWith(QLatin1String("#KCMYK"))) {
                const QStringList v = line.mid(6).split(QLatin1Char(' '), Qt::SkipEmptyParts);
                if (v.size() >= 4 && !out.colors.isEmpty()) {
                    while (out.cmyk.size() < out.colors.size() - 1) out.cmyk.append(krimbleCmykOf(out.colors[out.cmyk.size()]));
                    out.cmyk.append({v[0].toInt(), v[1].toInt(), v[2].toInt(), v[3].toInt()});
                }
                continue;
            }
            if (line.isEmpty() || line.startsWith(QLatin1Char('#'))) continue;
            if (line.startsWith(QLatin1String("Name:"))) { out.name = line.mid(5).trimmed(); continue; }
            if (line.startsWith(QLatin1String("Columns:"))) { out.columns = qBound(1, line.mid(8).trimmed().toInt(), 64); continue; }
            const QStringList parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                bool a = false, b = false, c = false;
                const int r = parts[0].toInt(&a), g = parts[1].toInt(&b), bl = parts[2].toInt(&c);
                if (a && b && c) out.colors.append(QColor(qBound(0, r, 255), qBound(0, g, 255), qBound(0, bl, 255)));
            }
        }
        out.normalize();
        return true;
    }
    static bool load(const QString &name, KrimblePalette &out)
    {
        if (readFile(fileFor(name), out)) { if (out.name.isEmpty()) out.name = name; return true; }
        // the file name may differ from the name inside (a palette copied in): look through the files
        const QDir dir(directory());
        for (const QFileInfo &info : dir.entryInfoList(QStringList() << QStringLiteral("*.gpl"), QDir::Files)) {
            KrimblePalette pal;
            if (readFile(info.absoluteFilePath(), pal) && pal.name == name) { out = pal; return true; }
        }
        return false;
    }
    static bool writeFile(const KrimblePalette &palette, const QString &path)
    {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return false;
        QTextStream out(&file);
        out << "GIMP Palette\nName: " << palette.name << "\nColumns: " << palette.columns << "\n#\n";
        // a CMYK palette is a GIMP palette too: every swatch has its RGB value (the line other programs read) and, as a comment, its four plates (which this program reads and edits)
        if (palette.isCmyk()) out << "#KrimbleModel: CMYK\n";
        for (int i = 0; i < palette.colors.size(); ++i) {
            const QColor &c = palette.colors[i];
            out << c.red() << ' ' << c.green() << ' ' << c.blue() << ' ' << c.name(QColor::HexRgb).toUpper() << '\n';
            if (palette.isCmyk() && i < palette.cmyk.size())
                out << "#KCMYK " << palette.cmyk[i][0] << ' ' << palette.cmyk[i][1] << ' ' << palette.cmyk[i][2] << ' ' << palette.cmyk[i][3] << '\n';
        }
        return true;
    }
    static bool save(const KrimblePalette &palette) { return !palette.name.isEmpty() && writeFile(palette, fileFor(palette.name)); }
    static bool remove(const QString &name) { return QFile::remove(fileFor(name)); }
    static bool exists(const QString &name) { return names().contains(name); }
    static QString uniqueName(const QString &base)
    {
        const QStringList existing = names();
        if (!existing.contains(base)) return base;
        for (int i = 2; i < 1000; ++i) {
            const QString candidate = QStringLiteral("%1 %2").arg(base).arg(i);
            if (!existing.contains(candidate)) return candidate;
        }
        return base + QStringLiteral(" copy");
    }
    static bool rename(const QString &from, const QString &to)
    {
        if (to.trimmed().isEmpty() || (to != from && exists(to))) return false;
        KrimblePalette pal;
        if (!load(from, pal)) return false;
        pal.name = to.trimmed();
        if (!save(pal)) return false;
        if (fileFor(from) != fileFor(pal.name)) QFile::remove(fileFor(from));
        if (active() == from) setActive(pal.name);
        return true;
    }
    static bool duplicate(const QString &name, QString *newName = nullptr)
    {
        KrimblePalette pal;
        if (!load(name, pal)) return false;
        pal.name = uniqueName(name + QStringLiteral(" copy"));
        if (newName) *newName = pal.name;
        return save(pal);
    }
    static bool importFile(const QString &path, QString *name = nullptr)
    {
        KrimblePalette pal;
        if (!readFile(path, pal) || pal.colors.isEmpty()) return false;
        pal.name = uniqueName(pal.name);
        if (name) *name = pal.name;
        return save(pal);
    }
    static bool exportTo(const QString &name, const QString &path)
    {
        KrimblePalette pal;
        return load(name, pal) && writeFile(pal, path);
    }
    static QString active()
    {
        const QString stored = QSettings().value(QStringLiteral("Krimble/ColorStudioActivePalette"), QStringLiteral("Krimble 256")).toString();   // 2026-11-08: was "My Palette"
        return exists(stored) ? stored : (names().isEmpty() ? stored : names().first());
    }
    static void setActive(const QString &name) { QSettings().setValue(QStringLiteral("Krimble/ColorStudioActivePalette"), name); }
    // add a color to the active palette (a color already in it is not added twice)
    static bool addColor(const QColor &c)
    {
        ensureDefaults();
        KrimblePalette pal;
        if (!load(active(), pal) || !c.isValid()) return false;
        QColor opaque = c.toRgb();
        opaque.setAlpha(255);
        QColor shown = pal.isCmyk() ? krimbleColorOfCmyk(krimbleCmykOf(opaque)) : opaque;
        if (!pal.colors.contains(shown)) pal.append(opaque);
        return save(pal);
    }
    // a text that changes when the active palette's name or colors change (the panel reloads when it does)
    static QString signature()
    {
        KrimblePalette pal;
        if (!load(active(), pal)) return QString();
        QString text = pal.name + QLatin1Char('|') + QString::number(pal.columns) + QLatin1Char('|') + pal.model;
        for (const QColor &c : pal.colors) text += QLatin1Char(',') + c.name(QColor::HexRgb);
        for (const KrimbleCmyk &v : pal.cmyk) text += QStringLiteral(";%1,%2,%3,%4").arg(v[0]).arg(v[1]).arg(v[2]).arg(v[3]);
        return text;
    }
    // the first run: "My Palette" (the user's own swatches: the saved list of earlier versions, or a starting set) and "Krimble 256" (16 x 16: grays, then hues from tints to shades)
    static void ensureDefaults()
    {
        if (!names().isEmpty()) return;
        QSettings settings;
        KrimblePalette mine;
        mine.name = QStringLiteral("My Palette");
        mine.columns = 19;
        const QStringList legacy = settings.value(QStringLiteral("Krimble/ColorStudioSwatches")).toStringList();
        for (const QString &item : legacy) { const QColor c(item); if (c.isValid()) mine.colors.append(c); }
        if (mine.colors.isEmpty()) {
            for (int i = 0; i < 10; ++i) mine.colors.append(QColor::fromHsv(0, 0, 255 - i * 255 / 9));
            const int saturation[3] = {255, 120, 255};
            const int brightness[3] = {255, 255, 140};
            for (int row = 0; row < 3; ++row)
                for (int col = 0; col < 10; ++col) mine.colors.append(QColor::fromHsv(col * 36, saturation[row], brightness[row]));
        }
        save(mine);
        KrimblePalette big;
        big.name = QStringLiteral("Krimble 256");
        big.columns = 16;
        for (int c = 0; c < 16; ++c) big.colors.append(QColor::fromHsv(0, 0, 255 - c * 255 / 15));
        for (int row = 1; row < 16; ++row) {
            const int hue = (row - 1) * 24;
            for (int c = 0; c < 16; ++c) {
                if (c < 8) big.colors.append(QColor::fromHsv(hue, 64 + c * 191 / 7, 255));
                else big.colors.append(QColor::fromHsv(hue, 255, 255 - (c - 7) * 190 / 8));
            }
        }
        save(big);
        // setActive(mine.name);
        setActive(big.name);   // 2026-11-08: George: "palette should default to 256 colors." The active palette on the first run is the 256 color one.
    }
};
