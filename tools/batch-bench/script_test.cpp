// Tests Krimble Script: every command, the errors, and that every built-in recipe written as a script means the same as the recipe.
// Build and run: tools/batch-bench/build_script_test.sh
#include "KrimbleBatch.h"
#include <QCoreApplication>
#include <QPainter>
#include <QTextStream>

using namespace KrimbleBatch;

static int failures = 0;
static QTextStream out(stdout);
static void check(bool ok, const QString &what) { out << (ok ? "  ok   " : "  FAIL ") << what << "\n"; if (!ok) ++failures; }

static Program parse(const QString &text, QString *error = nullptr)
{
    Program p; QString e;
    const bool ok = parseScript(text, &p, &e);
    if (error) *error = ok ? QString() : e;
    return p;
}
static QString errorOf(const QString &text) { Program p; QString e; return parseScript(text, &p, &e) ? QString() : e; }

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    // a 400 x 200 test picture: left half red, right half blue, with a transparent top-left corner
    QImage pic(400, 200, QImage::Format_ARGB32);
    pic.fill(QColor(255, 0, 0, 255));
    { QPainter p(&pic); p.fillRect(200, 0, 200, 200, QColor(0, 0, 255)); p.setCompositionMode(QPainter::CompositionMode_Clear); p.fillRect(0, 0, 40, 40, Qt::transparent); }
    out << "-- resize, fit, crop\n";
    check(runProgram(pic, parse("resize longest 100")).size() == QSize(100, 50), "resize longest 100 on 400x200 -> 100x50");
    check(runProgram(pic, parse("resize width 100")).size() == QSize(100, 50), "resize width 100 -> 100x50");
    check(runProgram(pic, parse("resize height 100")).size() == QSize(200, 100), "resize height 100 -> 200x100");
    check(runProgram(pic, parse("resize longest 800")).size() == QSize(400, 200), "resize longest 800: not made bigger");
    check(runProgram(pic, parse("resize longest 800 grow")).size() == QSize(800, 400), "resize longest 800 grow -> 800x400");
    check(runProgram(pic, parse("fit 100 100")).size() == QSize(100, 50), "fit 100 100 -> 100x50");
    check(runProgram(pic, parse("fit 1000 1000")).size() == QSize(400, 200), "fit 1000 1000: not made bigger");
    check(runProgram(pic, parse("fit 1000 1000 grow")).size() == QSize(1000, 500), "fit 1000 1000 grow -> 1000x500");
    check(runProgram(pic, parse("crop square")).size() == QSize(200, 200), "crop square -> 200x200");
    check(runProgram(pic, parse("crop 50 30")).size() == QSize(50, 30), "crop 50 30 -> 50x30");
    check(runProgram(pic, parse("crop 900 900")).size() == QSize(400, 200), "crop 900 900: not bigger than the picture");
    out << "-- rotate, flip\n";
    check(runProgram(pic, parse("rotate 90")).size() == QSize(200, 400), "rotate 90 -> 200x400");
    check(runProgram(pic, parse("rotate -90")).size() == QSize(200, 400), "rotate -90 -> 200x400");
    check(runProgram(pic, parse("rotate 180")).size() == QSize(400, 200), "rotate 180 -> 400x200");
    { QImage f = runProgram(pic, parse("flip h")); check(qBlue(f.pixel(10, 100)) == 255 && qRed(f.pixel(390, 100)) == 255, "flip h: red and blue swap sides"); }
    { QImage f = runProgram(pic, parse("flip v")); check(qAlpha(f.pixel(10, 190)) == 0 && qAlpha(f.pixel(10, 10)) == 255, "flip v: the transparent corner moves to the bottom"); }
    out << "-- color\n";
    { QImage g = runProgram(pic, parse("gray")); QRgb c = g.pixel(300, 100); check(qRed(c) == qGreen(c) && qGreen(c) == qBlue(c), "gray: r = g = b"); check(qAlpha(g.pixel(10, 10)) == 0, "gray keeps the transparency"); }
    { QImage b = runProgram(pic, parse("brightness 50")); check(qGreen(b.pixel(300, 100)) > 100 && qAlpha(b.pixel(10, 10)) == 0, "brightness 50 lifts the dark channels, keeps the transparency"); }
    { QImage b = runProgram(pic, parse("brightness -50")); check(qRed(b.pixel(100, 100)) < 200, "brightness -50 darkens"); }
    { QImage c = runProgram(pic, parse("contrast 50")); check(qAlpha(c.pixel(10, 10)) == 0, "contrast 50 keeps the transparency"); }
    { QImage f = runProgram(pic, parse("flatten red")); check(!f.hasAlphaChannel() && qRed(f.pixel(10, 10)) == 255 && qBlue(f.pixel(10, 10)) == 0, "flatten red: the transparent corner is red"); }
    { QImage f = runProgram(pic, parse("flatten #00ff00")); check(qGreen(f.pixel(10, 10)) == 255 && qRed(f.pixel(10, 10)) == 0, "flatten #00ff00: the corner is green"); }
    { QImage j = runProgram(pic, parse("format jpg 90")); check(!j.hasAlphaChannel() && qRed(j.pixel(10, 10)) == 255 && qBlue(j.pixel(10, 10)) == 255, "format jpg: what is transparent becomes white"); }
    { QImage j = runProgram(pic, parse("format png")); check(j.hasAlphaChannel() && qAlpha(j.pixel(10, 10)) == 0, "format png keeps the transparency"); }
    out << "-- format, name, folder\n";
    { Program p = parse("format webp 70\nname add web\nname prefix my\nfolder Web Site"); check(p.format == FormatWebPLossy && p.quality == 70 && p.nameAdd == "web" && p.namePrefix == "my" && p.folder == "Web Site", "format webp 70, name add, name prefix, folder"); check(outputNameFor("/x/photo.png", p) == "myphoto-web.webp", "name: myphoto-web.webp"); }
    { Program p = parse("format webp lossless"); check(p.format == FormatWebPLossless, "format webp lossless"); }
    { Program p = parse("gray"); check(p.format == FormatSame, "no format line: the picture keeps its kind");
      check(resolveFormat(p, "a.jpg") == FormatJpg && resolveFormat(p, "a.JPEG") == FormatJpg && resolveFormat(p, "a.png") == FormatPng && resolveFormat(p, "a.webp") == FormatWebPLossless && resolveFormat(p, "a.bmp") == FormatPng, "same: jpg->jpg, png->png, webp->webp lossless, bmp->png");
      check(outputNameFor("/x/a.webp", p) == "a.webp" && outputNameFor("/x/a.jpg", p) == "a.jpg", "same: the names keep their ending"); }
    { Program p = parse("format webp"); check(p.format == FormatWebPLossy && p.quality == 85, "format webp = lossy 85"); }
    { Program p = parse("format JPG"); check(p.format == FormatJpg && p.quality == 90, "FORMAT JPG: upper case is fine, quality 90"); }
    { Program p = parse("# a note\n\n   resize   longest   500   # not a note, a number then a note\n"); check(p.steps.size() == 1 && p.steps.first().a == 500, "notes, empty lines, extra spaces"); }
    { Program p = parse("flatten #ff0000 # a note"); check(p.steps.size() == 1 && p.steps.first().color == QColor("#ff0000"), "#ff0000 is a color, \"# a note\" is a note"); }
    out << "-- errors\n";
    check(errorOf("resize") .startsWith("Line 1: resize needs"), "resize alone: " + errorOf("resize"));
    check(errorOf("\n\nrezise longest 5").startsWith("Line 3: \"rezise\" is not a Krimble Script command"), "unknown command, line 3: " + errorOf("\n\nrezise longest 5"));
    check(errorOf("resize longest abc").contains("number from 1 to 16000"), "resize longest abc: " + errorOf("resize longest abc"));
    check(errorOf("rotate 45").contains("90, 180 or 270"), "rotate 45: " + errorOf("rotate 45"));
    check(errorOf("flip x").contains("not h or v"), "flip x: " + errorOf("flip x"));
    check(errorOf("brightness 500").contains("-100 to 100"), "brightness 500: " + errorOf("brightness 500"));
    check(errorOf("flatten chartreuse").contains("needs a color"), "flatten chartreuse: " + errorOf("flatten chartreuse"));
    check(errorOf("format gif").contains("not jpg, png or webp"), "format gif: " + errorOf("format gif"));
    check(errorOf("format jpg 101").contains("1 to 100"), "format jpg 101: " + errorOf("format jpg 101"));
    check(errorOf("name add ../x").contains("letters, numbers"), "name add ../x: " + errorOf("name add ../x"));
    check(errorOf("folder a/b").contains("letters, numbers"), "folder a/b: " + errorOf("folder a/b"));
    check(errorOf("resize longest 5 shrink").contains("only grow"), "resize longest 5 shrink: " + errorOf("resize longest 5 shrink"));
    { QString many; for (int i = 0; i < 41; ++i) many += "gray\n"; check(errorOf(many).contains("at most 40"), "41 steps: " + errorOf(many)); }
    check(errorOf("").isEmpty() && errorOf("# only a note").isEmpty(), "an empty script, and a script of notes, are not errors");
    out << "-- every built-in recipe, written as a script, means the same\n";
    for (const Recipe &r : builtInRecipes()) {
        QString e; Program a = programFromRecipe(r), b = parse(scriptFromRecipe(r), &e);
        const bool same = e.isEmpty() && a.format == b.format && (a.format == FormatPng || a.format == FormatWebPLossless || a.quality == b.quality) && a.nameAdd == b.nameAdd && a.steps.size() == b.steps.size()
                          && (a.steps.isEmpty() || (a.steps[0].mode == b.steps[0].mode && a.steps[0].a == b.steps[0].a && a.steps[0].grow == b.steps[0].grow));
        check(same, r.name);
    }
    out << (failures ? QString("\n%1 FAILED\n").arg(failures) : QString("\nALL PASSED\n"));
    return failures ? 1 : 0;
}
