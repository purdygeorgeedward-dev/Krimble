// Tests animated GIF and WebP: reading, writing animated WebP, the script commands frames / fps / loop.
// Usage: anim_test OUTDIR sprite.gif sprite.webp   (build_anim_test.sh)
#include "KrimbleBatch.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>

using namespace KrimbleBatch;
static int failures = 0;
static QTextStream out(stdout);
static void check(bool ok, const QString &what) { out << (ok ? "  ok   " : "  FAIL ") << what << "\n"; if (!ok) ++failures; }
static void save(const QString &path, const QByteArray &b) { QFile f(path); f.open(QIODevice::WriteOnly); f.write(b); }

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    const QString outDir = QString::fromUtf8(argv[1]);
    QDir().mkpath(outDir);
    out << "gifReadable: " << gifReadable() << "\n";
    Animation gif, webp;
    check(loadAnimation(QString::fromUtf8(argv[2]), &gif), "load the animated GIF");
    check(gif.frames.size() == 10, QString("GIF: 10 frames (%1)").arg(gif.frames.size()));
    check(!gif.frames.isEmpty() && gif.frames.first().image.size() == QSize(72, 72), "GIF: 72 x 72");
    check(!gif.frames.isEmpty() && qAlpha(gif.frames.first().image.pixel(2, 2)) == 0, "GIF: the corner is transparent");
    check(gif.frames.size() == 10 && gif.frames.at(0).delayMs == 100 && gif.frames.at(9).delayMs == 200, QString("GIF: delays 100 ... 200 (%1 ... %2)").arg(gif.frames.first().delayMs).arg(gif.frames.last().delayMs));
    check(loadAnimation(QString::fromUtf8(argv[3]), &webp), "load the animated WebP");
    check(webp.frames.size() == 10 && webp.frames.first().image.size() == QSize(72, 72), QString("WebP: 10 frames, 72 x 72 (%1)").arg(webp.frames.size()));
    check(webp.frames.size() == 10 && webp.frames.at(3).delayMs == 100, "WebP: delay 100");
    check(!webp.frames.isEmpty() && qAlpha(webp.frames.first().image.pixel(2, 2)) == 0, "WebP: the corner is transparent");
    Animation none;
    check(!loadAnimation("/nonexistent.gif", &none) && !loadAnimation("/tmp/test_opaque.jpg", &none), "a missing file and a JPG are not animations");
    QByteArray lossless, lossy;
    check(encodeAnimation(gif, FormatWebPLossless, 100, &lossless), "write an animated WebP, lossless");
    check(encodeAnimation(gif, FormatWebPLossy, 80, &lossy), "write an animated WebP, lossy 80");
    save(outDir + "/sprite_lossless.webp", lossless);
    save(outDir + "/sprite_lossy80.webp", lossy);
    out << "  sizes: GIF source and the two WebP below\n  lossless " << lossless.size() << " bytes, lossy 80 " << lossy.size() << " bytes\n";
    // read my own output back
    Animation back;
    save("/tmp/_back.webp", lossless);
    check(loadAnimation("/tmp/_back.webp", &back) && back.frames.size() == 10 && back.frames.at(9).delayMs == 200, "the lossless animated WebP reads back: 10 frames, last delay 200");
    bool same = back.frames.size() == 10;
    for (int i = 0; same && i < 10; ++i) {
        const QImage a = gif.frames.at(i).image.convertToFormat(QImage::Format_ARGB32), b = back.frames.at(i).image.convertToFormat(QImage::Format_ARGB32);
        for (int y = 0; same && y < 72; ++y) for (int x = 0; same && x < 72; ++x) {
            const QRgb p = a.pixel(x, y), q = b.pixel(x, y);
            if (qAlpha(p) != qAlpha(q) || (qAlpha(p) > 0 && (qRed(p) != qRed(q) || qGreen(p) != qGreen(q) || qBlue(p) != qBlue(q)))) same = false;
        }
    }
    check(same, "lossless: every pixel of every frame is the same after the round trip");
    // the program: resize, fps, loop on the frames
    Program p; QString e;
    check(parseScript("resize width 36\nfps 20\nloop 3\nformat webp 80\nname add small\nframes", &p, &e), "parse: resize, fps, loop, format, name, frames: " + e);
    check(p.fps == 20 && p.loop == 3 && p.framesOut, "fps 20, loop 3, frames are set");
    Animation small = gif;
    for (Frame &f : small.frames) { f.delayMs = 1000 / p.fps; f.image = runProgram(f.image, p, FormatWebPLossy); }
    small.loopCount = p.loop;
    check(small.frames.first().image.size() == QSize(36, 36) && small.frames.first().delayMs == 50, "every frame is 36 x 36 and shown for 50 ms");
    QByteArray smallBytes;
    check(encodeAnimation(small, FormatWebPLossy, 80, &smallBytes), "write the small one");
    save(outDir + "/sprite_small.webp", smallBytes);
    check(outputNameFor("/x/sprite.gif", p, 3) == "sprite-small-003.webp" && outputNameFor("/x/sprite.gif", p) == "sprite-small.webp", "names: sprite-small-003.webp (a frame) and sprite-small.webp");
    Program same2; parseScript("gray", &same2, &e);
    check(resolveFormat(same2, "a.gif") == FormatWebPLossless, "no format line: a GIF is written as a (lossless) WebP");
    check(parseScript("fps 0", &p, &e) == false && e.contains("1 to 100"), "fps 0 is refused: " + e);
    check(parseScript("loop 70000", &p, &e) == false && e.contains("0 to 65535"), "loop 70000 is refused: " + e);
    check(parseScript("frames now", &p, &e) == false, "frames with something after it is refused: " + e);
    out << (failures ? QString("\n%1 FAILED\n").arg(failures) : QString("\nALL PASSED\n"));
    return failures ? 1 : 0;
}
