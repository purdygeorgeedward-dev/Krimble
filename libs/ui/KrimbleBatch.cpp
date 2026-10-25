/*
 * Krimble 2026-10-25: see KrimbleBatch.h.
 */
#include "KrimbleBatch.h"

#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPainter>
#include <QSettings>

#ifdef KRIMBLE_HAVE_WEBP
#include <webp/decode.h>
#include <webp/demux.h>
#include <webp/encode.h>
#include <webp/mux.h>
#endif

namespace KrimbleBatch {

QString Recipe::extension() const
{
    return format == FormatJpg ? QStringLiteral("jpg") : (format == FormatPng ? QStringLiteral("png") : QStringLiteral("webp"));
}

QString Recipe::mime() const
{
    return format == FormatJpg ? QStringLiteral("image/jpeg") : (format == FormatPng ? QStringLiteral("image/png") : QStringLiteral("image/webp"));
}

bool Recipe::keepsTransparency() const
{
    return format != FormatJpg;
}

bool Recipe::usesQuality() const
{
    return format == FormatJpg || format == FormatWebPLossy;
}

QJsonObject Recipe::toJson() const
{
    QJsonObject o;
    o.insert(QStringLiteral("name"), name);
    o.insert(QStringLiteral("resize"), resize);
    o.insert(QStringLiteral("size"), size);
    o.insert(QStringLiteral("noEnlarge"), noEnlarge);
    o.insert(QStringLiteral("format"), format);
    o.insert(QStringLiteral("quality"), quality);
    o.insert(QStringLiteral("suffix"), suffix);
    if (!script.isEmpty()) o.insert(QStringLiteral("script"), script);
    return o;
}

Recipe Recipe::fromJson(const QJsonObject &o)
{
    Recipe r;
    r.name = o.value(QStringLiteral("name")).toString();
    r.resize = qBound(int(ResizeNone), o.value(QStringLiteral("resize")).toInt(ResizeNone), int(ResizeHeight));
    r.size = qBound(16, o.value(QStringLiteral("size")).toInt(1920), 16000);
    r.noEnlarge = o.value(QStringLiteral("noEnlarge")).toBool(true);
    r.format = qBound(int(FormatJpg), o.value(QStringLiteral("format")).toInt(FormatJpg), int(FormatWebPLossy));
    r.quality = qBound(1, o.value(QStringLiteral("quality")).toInt(90), 100);
    r.suffix = o.value(QStringLiteral("suffix")).toString();
    r.script = o.value(QStringLiteral("script")).toString();
    return r;
}

static Recipe make(const QString &name, int resize, int size, int format, int quality, const QString &suffix)
{
    Recipe r;
    r.name = name;
    r.resize = resize;
    r.size = size;
    r.format = format;
    r.quality = quality;
    r.suffix = suffix;
    r.builtIn = true;
    return r;
}

QList<Recipe> builtInRecipes()
{
    // the WebP ones first (George works with WebP a lot), then the ones that were there before (Batch Process, 2026-10-12)
    QList<Recipe> list;
    list << make(QStringLiteral("WebP, lossless (keeps transparency)"), ResizeNone, 0, FormatWebPLossless, 100, QStringLiteral("webp"))
         << make(QStringLiteral("WebP, quality 90 (keeps transparency)"), ResizeNone, 0, FormatWebPLossy, 90, QStringLiteral("webp"))
         << make(QStringLiteral("WebP for the web: longest side 1920 px, quality 85"), ResizeLongestSide, 1920, FormatWebPLossy, 85, QStringLiteral("web"))
         << make(QStringLiteral("WebP thumbnail: longest side 256 px"), ResizeLongestSide, 256, FormatWebPLossy, 85, QStringLiteral("thumb"))
         << make(QStringLiteral("HD 1080p: longest side 1920 px (JPG)"), ResizeLongestSide, 1920, FormatJpg, 90, QStringLiteral("hd"))
         << make(QStringLiteral("Social: longest side 1080 px (JPG)"), ResizeLongestSide, 1080, FormatJpg, 90, QStringLiteral("social"))
         << make(QStringLiteral("Messages: longest side 1280 px (JPG 80%)"), ResizeLongestSide, 1280, FormatJpg, 80, QStringLiteral("msg"))
         << make(QStringLiteral("Thumbnail: longest side 256 px (PNG)"), ResizeLongestSide, 256, FormatPng, 100, QStringLiteral("thumb"))
         << make(QStringLiteral("Convert to JPG (same size)"), ResizeNone, 0, FormatJpg, 92, QStringLiteral("jpg"))
         << make(QStringLiteral("Convert to PNG (same size)"), ResizeNone, 0, FormatPng, 100, QStringLiteral("png"));
    return list;
}

QList<Recipe> userRecipes()
{
    QSettings settings(QStringLiteral("Krimble"), QStringLiteral("Batch"));
    const QJsonDocument doc = QJsonDocument::fromJson(settings.value(QStringLiteral("recipes")).toString().toUtf8());
    QList<Recipe> list;
    const QJsonArray array = doc.array();
    for (const QJsonValue &v : array) {
        Recipe r = Recipe::fromJson(v.toObject());
        if (!r.name.trimmed().isEmpty()) list << r;
    }
    return list;
}

void saveUserRecipes(const QList<Recipe> &recipes)
{
    QJsonArray array;
    for (const Recipe &r : recipes) {
        if (!r.builtIn) array.append(r.toJson());
    }
    QSettings settings(QStringLiteral("Krimble"), QStringLiteral("Batch"));
    settings.setValue(QStringLiteral("recipes"), QString::fromUtf8(QJsonDocument(array).toJson(QJsonDocument::Compact)));
    settings.sync();
}

bool webpAvailable()
{
#ifdef KRIMBLE_HAVE_WEBP
    return true;
#else
    return QImageWriter::supportedImageFormats().contains("webp");
#endif
}

QImage loadPicture(const QString &path)
{
#ifdef KRIMBLE_HAVE_WEBP
    if (path.endsWith(QLatin1String(".webp"), Qt::CaseInsensitive)) {
        // libwebp reads the picture with its transparency; Qt may not have a WebP reader on the phone
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            const QByteArray data = file.readAll();
            int w = 0, h = 0;
            if (WebPGetInfo(reinterpret_cast<const uint8_t*>(data.constData()), size_t(data.size()), &w, &h) && w > 0 && h > 0) {
                uint8_t *rgba = WebPDecodeRGBA(reinterpret_cast<const uint8_t*>(data.constData()), size_t(data.size()), &w, &h);
                if (rgba) {
                    QImage image(w, h, QImage::Format_RGBA8888);
                    for (int y = 0; y < h; ++y) {
                        memcpy(image.scanLine(y), rgba + size_t(y) * size_t(w) * 4, size_t(w) * 4);
                    }
                    WebPFree(rgba);
                    return image;
                }
            }
        }
    }
#endif
    return QImage(path);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// the programs: what a recipe or a script does to a picture
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

QString extensionOf(int format)
{
    return format == FormatJpg ? QStringLiteral("jpg") : (format == FormatPng ? QStringLiteral("png") : QStringLiteral("webp"));
}

QString mimeOf(int format)
{
    return format == FormatJpg ? QStringLiteral("image/jpeg") : (format == FormatPng ? QStringLiteral("image/png") : QStringLiteral("image/webp"));
}

Program programFromRecipe(const Recipe &recipe)
{
    Program p;
    if (recipe.resize != ResizeNone) {
        ScriptStep step;
        step.kind = ScriptStep::Resize;
        step.mode = recipe.resize;
        step.a = recipe.size;
        step.grow = !recipe.noEnlarge;
        p.steps << step;
    }
    p.format = recipe.format;
    p.quality = recipe.quality;
    p.nameAdd = recipe.suffix.trimmed();
    return p;
}

QString scriptFromRecipe(const Recipe &recipe)
{
    QStringList lines;
    if (!recipe.name.isEmpty()) lines << QStringLiteral("# ") + recipe.name;
    if (recipe.resize != ResizeNone) {
        const char *what = recipe.resize == ResizeLongestSide ? "longest" : (recipe.resize == ResizeWidth ? "width" : "height");
        lines << QStringLiteral("resize %1 %2%3").arg(QLatin1String(what)).arg(recipe.size).arg(recipe.noEnlarge ? QString() : QStringLiteral(" grow"));
    }
    if (recipe.format == FormatJpg) lines << QStringLiteral("format jpg %1").arg(recipe.quality);
    else if (recipe.format == FormatPng) lines << QStringLiteral("format png");
    else if (recipe.format == FormatWebPLossless) lines << QStringLiteral("format webp lossless");
    else lines << QStringLiteral("format webp %1").arg(recipe.quality);
    if (!recipe.suffix.trimmed().isEmpty()) lines << QStringLiteral("name add ") + recipe.suffix.trimmed();
    return lines.join(QLatin1Char('\n')) + QLatin1Char('\n');
}

static QImage scaledKeepingEdges(const QImage &image, int w, int h)
{
    // scaled in premultiplied form, so that the edges of a transparent picture do not get dark or light fringes
    QImage source = image.hasAlphaChannel() ? image.convertToFormat(QImage::Format_ARGB32_Premultiplied) : image;
    return source.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

static QImage flattened(const QImage &image, const QColor &color)
{
    QImage out(image.size(), QImage::Format_RGB32);
    out.fill(color);
    QPainter painter(&out);
    painter.drawImage(0, 0, image);
    painter.end();
    return out;
}

static QImage mapChannels(const QImage &image, const uchar *lut)
{
    QImage out = image.convertToFormat(QImage::Format_ARGB32);   // not premultiplied: the colors are changed, the transparency is not
    for (int y = 0; y < out.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(out.scanLine(y));
        for (int x = 0; x < out.width(); ++x) {
            line[x] = qRgba(lut[qRed(line[x])], lut[qGreen(line[x])], lut[qBlue(line[x])], qAlpha(line[x]));
        }
    }
    return out;
}

int resolveFormat(const Program &program, const QString &sourcePath)
{
    if (program.format != FormatSame) return program.format;
    const QString ext = QFileInfo(sourcePath).suffix().toLower();
    if (ext == QLatin1String("jpg") || ext == QLatin1String("jpeg")) return FormatJpg;
    if (ext == QLatin1String("webp")) return FormatWebPLossless;
    if (ext == QLatin1String("gif")) return FormatWebPLossless;   // a GIF cannot be written yet: a lossless WebP (animated, if the GIF is) is the closest
    return FormatPng;
}

QImage runProgram(const QImage &source, const Program &program, int outputFormat)
{
    const int format = outputFormat >= 0 ? outputFormat : (program.format == FormatSame ? int(FormatPng) : program.format);
    QImage image = source;
    if (image.isNull()) return image;
    for (const ScriptStep &step : program.steps) {
        const int w = image.width(), h = image.height();
        switch (step.kind) {
        case ScriptStep::Resize: {
            int newW = w, newH = h;
            if (step.mode == ResizeLongestSide) {
                const int longest = qMax(w, h);
                if (longest != step.a && (step.grow || longest > step.a)) {
                    newW = qMax(1, qRound(double(w) * step.a / longest));
                    newH = qMax(1, qRound(double(h) * step.a / longest));
                }
            } else if (step.mode == ResizeWidth) {
                if (w != step.a && (step.grow || w > step.a)) {
                    newW = step.a;
                    newH = qMax(1, qRound(double(h) * step.a / w));
                }
            } else if (step.mode == ResizeHeight) {
                if (h != step.a && (step.grow || h > step.a)) {
                    newH = step.a;
                    newW = qMax(1, qRound(double(w) * step.a / h));
                }
            }
            if (newW != w || newH != h) image = scaledKeepingEdges(image, newW, newH);
            break;
        }
        case ScriptStep::Fit: {
            const double scale = qMin(double(step.a) / w, double(step.b) / h);
            if (scale != 1.0 && (step.grow || scale < 1.0)) {
                image = scaledKeepingEdges(image, qMax(1, qRound(w * scale)), qMax(1, qRound(h * scale)));
            }
            break;
        }
        case ScriptStep::Crop: {
            const int cw = step.mode == 0 ? qMin(w, h) : qMin(step.a, w);
            const int ch = step.mode == 0 ? qMin(w, h) : qMin(step.b, h);
            image = image.copy((w - cw) / 2, (h - ch) / 2, cw, ch);
            break;
        }
        case ScriptStep::Rotate: {
            QTransform t;
            t.rotate(step.a);
            image = image.transformed(t, Qt::SmoothTransformation);
            break;
        }
        case ScriptStep::Flip:
            image = image.mirrored(step.mode == 0, step.mode == 1);
            break;
        case ScriptStep::Gray: {
            QImage out = image.convertToFormat(QImage::Format_ARGB32);
            for (int y = 0; y < out.height(); ++y) {
                QRgb *line = reinterpret_cast<QRgb*>(out.scanLine(y));
                for (int x = 0; x < out.width(); ++x) {
                    const int g = qGray(line[x]);
                    line[x] = qRgba(g, g, g, qAlpha(line[x]));
                }
            }
            image = out;
            break;
        }
        case ScriptStep::Brightness: {
            uchar lut[256];
            for (int v = 0; v < 256; ++v) lut[v] = uchar(qBound(0, v + qRound(step.a * 2.55), 255));
            image = mapChannels(image, lut);
            break;
        }
        case ScriptStep::Contrast: {
            const double c = step.a * 2.55;   // -255 to 255
            const double factor = (259.0 * (c + 255.0)) / (255.0 * (259.0 - c));
            uchar lut[256];
            for (int v = 0; v < 256; ++v) lut[v] = uchar(qBound(0, qRound(factor * (v - 128) + 128), 255));
            image = mapChannels(image, lut);
            break;
        }
        case ScriptStep::Flatten:
            image = flattened(image, step.color);
            break;
        }
    }
    // a JPG has no transparency: what is still transparent becomes white
    if (format == FormatJpg && image.hasAlphaChannel()) image = flattened(image, Qt::white);
    return image;
}

bool encodeAs(const QImage &image, int format, int quality, QByteArray *bytes)
{
    if (image.isNull() || !bytes) return false;
    bytes->clear();
    if (format == FormatJpg || format == FormatPng) {
        QBuffer buffer(bytes);
        buffer.open(QIODevice::WriteOnly);
        return format == FormatJpg ? image.save(&buffer, "JPG", quality) : image.save(&buffer, "PNG");
    }
    // WebP: at most 16383 px on a side
    if (image.width() > 16383 || image.height() > 16383) return false;
#ifdef KRIMBLE_HAVE_WEBP
    const QImage rgba = image.convertToFormat(QImage::Format_RGBA8888);   // not premultiplied: what libwebp wants
    uint8_t *output = nullptr;
    size_t size = 0;
    if (format == FormatWebPLossless) {
        size = WebPEncodeLosslessRGBA(rgba.constBits(), rgba.width(), rgba.height(), int(rgba.bytesPerLine()), &output);
    } else {
        size = WebPEncodeRGBA(rgba.constBits(), rgba.width(), rgba.height(), int(rgba.bytesPerLine()), float(quality), &output);
    }
    if (size == 0 || !output) return false;
    bytes->append(reinterpret_cast<const char*>(output), int(size));
    WebPFree(output);
    return true;
#else
    QBuffer buffer(bytes);
    buffer.open(QIODevice::WriteOnly);
    return image.save(&buffer, "WEBP", format == FormatWebPLossless ? 100 : quality);   // Qt: 100 is lossless
#endif
}

QString outputNameFor(const QString &sourcePath, const Program &program, int frameNumber)
{
    QString base = QFileInfo(sourcePath).completeBaseName();
    if (base.isEmpty()) base = QStringLiteral("picture");
    const QString add = program.nameAdd.trimmed();
    const QString frame = frameNumber > 0 ? QStringLiteral("-%1").arg(frameNumber, 3, 10, QLatin1Char('0')) : QString();
    return program.namePrefix.trimmed() + base + (add.isEmpty() ? QString() : QLatin1Char('-') + add) + frame + QLatin1Char('.') + extensionOf(resolveFormat(program, sourcePath));
}

// the older functions of the recipes, now written with the programs
QImage applyRecipe(const QImage &source, const Recipe &recipe) { return runProgram(source, programFromRecipe(recipe)); }
bool encodePicture(const QImage &image, const Recipe &recipe, QByteArray *bytes) { return encodeAs(image, recipe.format, recipe.quality, bytes); }
QString outputName(const QString &sourcePath, const Recipe &recipe) { return outputNameFor(sourcePath, programFromRecipe(recipe)); }

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// animated pictures
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const qint64 kMaxAnimationPixels = 60LL * 1000 * 1000;   // frames x width x height: about 240 MB as pictures; more is refused (a phone has not the memory)

bool gifReadable()
{
    return QImageReader::supportedImageFormats().contains("gif");
}

static bool loadAnimatedGif(const QString &path, Animation *animation)
{
    if (!gifReadable()) return false;
    QImageReader reader(path, "gif");
    if (!reader.canRead()) return false;
    qint64 pixels = 0;
    while (reader.canRead()) {
        QImage image = reader.read();
        if (image.isNull()) break;
        pixels += qint64(image.width()) * image.height();
        if (pixels > kMaxAnimationPixels || animation->frames.size() >= 2000) { animation->frames.clear(); return false; }
        Frame frame;
        frame.image = image.convertToFormat(QImage::Format_ARGB32);
        int delay = reader.nextImageDelay();      // how long the frame just read is shown
        if (delay <= 10) delay = 100;             // 0 or 1 hundredth of a second: browsers show it for a tenth of a second
        frame.delayMs = delay;
        animation->frames << frame;
    }
    return !animation->frames.isEmpty();
}

static bool loadAnimatedWebP(const QString &path, Animation *animation)
{
#ifdef KRIMBLE_HAVE_WEBP
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return false;
    const QByteArray data = file.readAll();
    WebPData webpData;
    webpData.bytes = reinterpret_cast<const uint8_t*>(data.constData());
    webpData.size = size_t(data.size());
    WebPAnimDecoderOptions options;
    if (!WebPAnimDecoderOptionsInit(&options)) return false;
    options.color_mode = MODE_RGBA;
    options.use_threads = 0;
    WebPAnimDecoder *decoder = WebPAnimDecoderNew(&webpData, &options);
    if (!decoder) return false;
    WebPAnimInfo info;
    bool ok = WebPAnimDecoderGetInfo(decoder, &info) != 0;
    if (ok && qint64(info.canvas_width) * info.canvas_height * info.frame_count > kMaxAnimationPixels) ok = false;
    if (ok) {
        animation->loopCount = int(info.loop_count);
        int previous = 0;
        while (WebPAnimDecoderHasMoreFrames(decoder)) {
            uint8_t *buffer = nullptr;
            int timestamp = 0;
            if (!WebPAnimDecoderGetNext(decoder, &buffer, &timestamp)) { ok = false; break; }
            QImage image(int(info.canvas_width), int(info.canvas_height), QImage::Format_RGBA8888);
            for (int y = 0; y < image.height(); ++y) memcpy(image.scanLine(y), buffer + size_t(y) * size_t(info.canvas_width) * 4, size_t(info.canvas_width) * 4);
            Frame frame;
            frame.image = image.convertToFormat(QImage::Format_ARGB32);
            frame.delayMs = qMax(1, timestamp - previous);
            previous = timestamp;
            animation->frames << frame;
        }
    }
    WebPAnimDecoderDelete(decoder);
    if (!ok) animation->frames.clear();
    return ok && !animation->frames.isEmpty();
#else
    Q_UNUSED(path);
    Q_UNUSED(animation);
    return false;
#endif
}

bool loadAnimation(const QString &path, Animation *animation)
{
    if (!animation) return false;
    animation->frames.clear();
    animation->loopCount = 0;
    const QString ext = QFileInfo(path).suffix().toLower();
    if (ext == QLatin1String("gif")) return loadAnimatedGif(path, animation);
    if (ext == QLatin1String("webp")) return loadAnimatedWebP(path, animation);
    return false;
}

bool encodeAnimation(const Animation &animation, int format, int quality, QByteArray *bytes)
{
    if (!bytes || animation.frames.isEmpty()) return false;
    bytes->clear();
    const QSize size = animation.frames.first().image.size();
    if (size.isEmpty() || size.width() > 16383 || size.height() > 16383) return false;
#ifdef KRIMBLE_HAVE_WEBP
    WebPAnimEncoderOptions options;
    if (!WebPAnimEncoderOptionsInit(&options)) return false;
    options.anim_params.loop_count = qBound(0, animation.loopCount, 65535);
    options.anim_params.bgcolor = 0x00000000;           // transparent
    options.minimize_size = (qint64(size.width()) * size.height() * animation.frames.size() <= 2000000) ? 1 : 0;   // smaller files, slower: only for small animations
    WebPAnimEncoder *encoder = WebPAnimEncoderNew(size.width(), size.height(), &options);
    if (!encoder) return false;
    WebPConfig config;
    bool ok = WebPConfigInit(&config) != 0;
    if (ok) {
        config.lossless = (format == FormatWebPLossless) ? 1 : 0;
        config.quality = (format == FormatWebPLossless) ? 70.0f : float(quality);
        ok = WebPValidateConfig(&config) != 0;
    }
    int timestamp = 0;
    for (int i = 0; ok && i < animation.frames.size(); ++i) {
        QImage image = animation.frames.at(i).image;
        if (image.size() != size) image = image.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);   // every frame must be as big as the first
        const QImage rgba = image.convertToFormat(QImage::Format_RGBA8888);
        WebPPicture picture;
        if (!WebPPictureInit(&picture)) { ok = false; break; }
        picture.use_argb = 1;
        picture.width = size.width();
        picture.height = size.height();
        ok = WebPPictureImportRGBA(&picture, rgba.constBits(), int(rgba.bytesPerLine())) != 0 && WebPAnimEncoderAdd(encoder, &picture, timestamp, &config) != 0;
        WebPPictureFree(&picture);
        timestamp += qMax(1, animation.frames.at(i).delayMs);
    }
    if (ok) ok = WebPAnimEncoderAdd(encoder, nullptr, timestamp, nullptr) != 0;
    WebPData assembled;
    WebPDataInit(&assembled);
    if (ok) ok = WebPAnimEncoderAssemble(encoder, &assembled) != 0;
    if (ok && assembled.size > 0) bytes->append(reinterpret_cast<const char*>(assembled.bytes), int(assembled.size));
    WebPDataClear(&assembled);
    WebPAnimEncoderDelete(encoder);
    return ok && !bytes->isEmpty();
#else
    Q_UNUSED(format);
    Q_UNUSED(quality);
    return false;   // no libwebp in this build: Qt cannot write an animated WebP
#endif
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Krimble Script: the parser
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

QString scriptHelp()
{
    return QStringLiteral(
        "Krimble Script: one command on each line. A line that starts with # and a space is a note.\n"
        "\n"
        "resize longest|width|height N [grow]\n"
        "fit W H [grow]\n"
        "crop square   or   crop W H\n"
        "rotate 90|180|270\n"
        "flip h|v\n"
        "gray\n"
        "brightness N    (-100 to 100)\n"
        "contrast N    (-100 to 100)\n"
        "flatten COLOR    (white, black, #rrggbb ...)\n"
        "format jpg [Q]  |  png  |  webp [Q]  |  webp lossless\n"
        "name add TEXT  |  name prefix TEXT\n"
        "folder NAME\n"
        "frames    (an animation is written as separate pictures)\n"
        "fps N    (an animation's speed)\n"
        "loop N    (how many times it plays; 0 = for ever)\n"
        "\n"
        "A picture is never made bigger, unless the command ends with grow.\n"
        "Without a format line a picture keeps its kind: a JPG stays a JPG, a PNG a PNG, a WebP a WebP.\n"
        "Example:\n"
        "  resize longest 1920\n"
        "  format webp 85\n"
        "  name add web");
}

static bool parseColorName(const QString &word, QColor *color)
{
    static const char *names[][2] = {{"white", "#ffffff"}, {"black", "#000000"}, {"gray", "#808080"}, {"grey", "#808080"}, {"red", "#ff0000"}, {"green", "#00a000"},
                                     {"blue", "#0000ff"}, {"yellow", "#ffff00"}, {"orange", "#ff8000"}, {"purple", "#800080"}, {"pink", "#ff80c0"}, {"brown", "#804000"}};
    for (const auto &n : names) {
        if (word.compare(QLatin1String(n[0]), Qt::CaseInsensitive) == 0) { *color = QColor(QLatin1String(n[1])); return true; }
    }
    if (word.startsWith(QLatin1Char('#')) && (word.size() == 7 || word.size() == 4)) {
        const QColor c(word);
        if (c.isValid()) { *color = c; return true; }
    }
    return false;
}

static bool safeWord(const QString &w, bool allowSpace)
{
    if (w.isEmpty() || w.size() > 40) return false;
    for (const QChar ch : w) {
        if (!(ch.isLetterOrNumber() || ch == QLatin1Char('-') || ch == QLatin1Char('_') || (allowSpace && ch == QLatin1Char(' ')))) return false;
    }
    return true;
}

// a note starts with # at the start of a line or after a space, and is followed by a space or the end of the line ("#ff0000" is a color, "# a note" is a note)
static QString withoutNote(const QString &line)
{
    for (int i = 0; i < line.size(); ++i) {
        if (line.at(i) == QLatin1Char('#') && (i == 0 || line.at(i - 1).isSpace()) && (i + 1 >= line.size() || line.at(i + 1).isSpace() || line.at(i + 1) == QLatin1Char('#'))) {
            return line.left(i);
        }
    }
    return line;
}

bool parseScript(const QString &text, Program *program, QString *error)
{
    Program p;
    const QStringList lines = text.split(QLatin1Char('\n'));
    int steps = 0;
    for (int n = 0; n < lines.size(); ++n) {
        const QString line = withoutNote(lines.at(n)).simplified();
        if (line.isEmpty()) continue;
        const QStringList w = line.split(QLatin1Char(' '));
        const QString cmd = w.at(0).toLower();
        const auto fail = [&](const QString &why) {
            if (error) *error = QStringLiteral("Line %1: %2").arg(n + 1).arg(why);
            return false;
        };
        const auto number = [&](int index, int lo, int hi, int *out) -> bool {
            if (index >= w.size()) return false;
            bool ok = false;
            const int v = w.at(index).toInt(&ok);
            if (!ok || v < lo || v > hi) return false;
            *out = v;
            return true;
        };
        const auto hasGrow = [&](int index) { return w.size() > index && w.at(index).compare(QLatin1String("grow"), Qt::CaseInsensitive) == 0; };
        ScriptStep step;
        if (cmd == QLatin1String("resize")) {
            if (w.size() < 3) return fail(QStringLiteral("resize needs longest, width or height and a number, like: resize longest 1920"));
            const QString what = w.at(1).toLower();
            if (what == QLatin1String("longest")) step.mode = ResizeLongestSide;
            else if (what == QLatin1String("width")) step.mode = ResizeWidth;
            else if (what == QLatin1String("height")) step.mode = ResizeHeight;
            else return fail(QStringLiteral("resize: \"%1\" is not longest, width or height").arg(w.at(1)));
            if (!number(2, 1, 16000, &step.a)) return fail(QStringLiteral("resize: the size must be a number from 1 to 16000"));
            if (w.size() > 3 && !hasGrow(3)) return fail(QStringLiteral("resize: \"%1\" is not understood (only grow can be added)").arg(w.at(3)));
            step.kind = ScriptStep::Resize;
            step.grow = hasGrow(3);
        } else if (cmd == QLatin1String("fit")) {
            if (!number(1, 1, 16000, &step.a) || !number(2, 1, 16000, &step.b)) return fail(QStringLiteral("fit needs a width and a height, like: fit 1920 1080"));
            if (w.size() > 3 && !hasGrow(3)) return fail(QStringLiteral("fit: \"%1\" is not understood (only grow can be added)").arg(w.at(3)));
            step.kind = ScriptStep::Fit;
            step.grow = hasGrow(3);
        } else if (cmd == QLatin1String("crop")) {
            step.kind = ScriptStep::Crop;
            if (w.size() == 2 && w.at(1).compare(QLatin1String("square"), Qt::CaseInsensitive) == 0) step.mode = 0;
            else if (w.size() == 3 && number(1, 1, 16000, &step.a) && number(2, 1, 16000, &step.b)) step.mode = 1;
            else return fail(QStringLiteral("crop needs square, or a width and a height: crop square   or   crop 800 600"));
        } else if (cmd == QLatin1String("rotate")) {
            int deg = 0;
            if (!number(1, -270, 270, &deg) || !(deg == 90 || deg == 180 || deg == 270 || deg == -90 || deg == -180 || deg == -270)) return fail(QStringLiteral("rotate must be 90, 180 or 270"));
            step.kind = ScriptStep::Rotate;
            step.a = ((deg % 360) + 360) % 360;
        } else if (cmd == QLatin1String("flip")) {
            if (w.size() != 2) return fail(QStringLiteral("flip needs h or v"));
            const QString what = w.at(1).toLower();
            if (what == QLatin1String("h") || what == QLatin1String("horizontal")) step.mode = 0;
            else if (what == QLatin1String("v") || what == QLatin1String("vertical")) step.mode = 1;
            else return fail(QStringLiteral("flip: \"%1\" is not h or v").arg(w.at(1)));
            step.kind = ScriptStep::Flip;
        } else if (cmd == QLatin1String("gray") || cmd == QLatin1String("grey")) {
            step.kind = ScriptStep::Gray;
        } else if (cmd == QLatin1String("brightness") || cmd == QLatin1String("contrast")) {
            if (!number(1, -100, 100, &step.a)) return fail(QStringLiteral("%1 needs a number from -100 to 100").arg(cmd));
            step.kind = cmd == QLatin1String("brightness") ? ScriptStep::Brightness : ScriptStep::Contrast;
        } else if (cmd == QLatin1String("flatten")) {
            if (w.size() != 2 || !parseColorName(w.at(1), &step.color)) return fail(QStringLiteral("flatten needs a color: a name (white, black, gray, red, green, blue, yellow, orange, purple, pink, brown) or #rrggbb"));
            step.kind = ScriptStep::Flatten;
        } else if (cmd == QLatin1String("format")) {
            if (w.size() < 2) return fail(QStringLiteral("format needs jpg, png or webp"));
            const QString what = w.at(1).toLower();
            if (what == QLatin1String("jpg") || what == QLatin1String("jpeg")) {
                p.format = FormatJpg;
                p.quality = 90;
                if (w.size() > 2 && !number(2, 1, 100, &p.quality)) return fail(QStringLiteral("format jpg: the quality must be a number from 1 to 100"));
            } else if (what == QLatin1String("png")) {
                p.format = FormatPng;
                if (w.size() > 2) return fail(QStringLiteral("format png has nothing after png"));
            } else if (what == QLatin1String("webp")) {
                p.quality = 85;
                if (w.size() > 2 && w.at(2).compare(QLatin1String("lossless"), Qt::CaseInsensitive) == 0) p.format = FormatWebPLossless;
                else {
                    p.format = FormatWebPLossy;
                    if (w.size() > 2 && !number(2, 1, 100, &p.quality)) return fail(QStringLiteral("format webp: after webp comes lossless or a quality from 1 to 100"));
                }
            } else {
                return fail(QStringLiteral("format: \"%1\" is not jpg, png or webp").arg(w.at(1)));
            }
            continue;
        } else if (cmd == QLatin1String("name")) {
            if (w.size() != 3 || !safeWord(w.at(2), false)) return fail(QStringLiteral("name needs add or prefix and a word of letters, numbers, - and _, like: name add web"));
            const QString what = w.at(1).toLower();
            if (what == QLatin1String("add")) p.nameAdd = w.at(2);
            else if (what == QLatin1String("prefix")) p.namePrefix = w.at(2);
            else return fail(QStringLiteral("name: \"%1\" is not add or prefix").arg(w.at(1)));
            continue;
        } else if (cmd == QLatin1String("frames")) {
            if (w.size() != 1) return fail(QStringLiteral("frames has nothing after it"));
            p.framesOut = true;
            continue;
        } else if (cmd == QLatin1String("fps")) {
            if (!number(1, 1, 100, &p.fps) || w.size() != 2) return fail(QStringLiteral("fps needs a number from 1 to 100, like: fps 12"));
            continue;
        } else if (cmd == QLatin1String("loop")) {
            if (!number(1, 0, 65535, &p.loop) || w.size() != 2) return fail(QStringLiteral("loop needs a number from 0 to 65535 (0 plays for ever), like: loop 0"));
            continue;
        } else if (cmd == QLatin1String("folder")) {
            const QString name = w.mid(1).join(QLatin1Char(' '));
            if (!safeWord(name, true)) return fail(QStringLiteral("folder needs a name of letters, numbers, spaces, - and _, like: folder Web"));
            p.folder = name;
            continue;
        } else {
            return fail(QStringLiteral("\"%1\" is not a Krimble Script command (resize, fit, crop, rotate, flip, gray, brightness, contrast, flatten, format, name, folder, frames, fps, loop)").arg(w.at(0)));
        }
        if (++steps > 40) return fail(QStringLiteral("a script can have at most 40 steps"));
        p.steps << step;
    }
    if (program) *program = p;
    if (error) error->clear();
    return true;
}

}
