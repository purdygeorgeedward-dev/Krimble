/*
 * Krimble 2026-10-25: see KrimbleBatch.h.
 */
#include "KrimbleBatch.h"

#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <QImageWriter>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPainter>
#include <QSettings>

#ifdef KRIMBLE_HAVE_WEBP
#include <webp/decode.h>
#include <webp/encode.h>
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

QImage applyRecipe(const QImage &source, const Recipe &recipe)
{
    QImage image = source;
    if (recipe.resize != ResizeNone && !image.isNull()) {
        const int w = image.width(), h = image.height();
        int newW = w, newH = h;
        if (recipe.resize == ResizeLongestSide) {
            const int longest = qMax(w, h);
            if (longest != recipe.size && !(recipe.noEnlarge && longest <= recipe.size)) {
                newW = qMax(1, qRound(double(w) * recipe.size / longest));
                newH = qMax(1, qRound(double(h) * recipe.size / longest));
            }
        } else if (recipe.resize == ResizeWidth) {
            if (w != recipe.size && !(recipe.noEnlarge && w <= recipe.size)) {
                newW = recipe.size;
                newH = qMax(1, qRound(double(h) * recipe.size / w));
            }
        } else {
            if (h != recipe.size && !(recipe.noEnlarge && h <= recipe.size)) {
                newH = recipe.size;
                newW = qMax(1, qRound(double(w) * recipe.size / h));
            }
        }
        if (newW != w || newH != h) {
            // scaled in premultiplied form, so that the edges of a transparent picture do not get dark or light fringes
            if (image.hasAlphaChannel()) image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
            image = image.scaled(newW, newH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
    }
    if (recipe.format == FormatJpg && !image.isNull()) {
        QImage onWhite(image.size(), QImage::Format_RGB32);
        onWhite.fill(Qt::white);
        QPainter painter(&onWhite);
        painter.drawImage(0, 0, image);
        painter.end();
        image = onWhite;
    }
    return image;
}

bool encodePicture(const QImage &image, const Recipe &recipe, QByteArray *bytes)
{
    if (image.isNull() || !bytes) return false;
    bytes->clear();
    if (recipe.format == FormatJpg || recipe.format == FormatPng) {
        QBuffer buffer(bytes);
        buffer.open(QIODevice::WriteOnly);
        return recipe.format == FormatJpg ? image.save(&buffer, "JPG", recipe.quality) : image.save(&buffer, "PNG");
    }
    // WebP: at most 16383 px on a side
    if (image.width() > 16383 || image.height() > 16383) return false;
#ifdef KRIMBLE_HAVE_WEBP
    const QImage rgba = image.convertToFormat(QImage::Format_RGBA8888);   // not premultiplied: what libwebp wants
    uint8_t *output = nullptr;
    size_t size = 0;
    if (recipe.format == FormatWebPLossless) {
        size = WebPEncodeLosslessRGBA(rgba.constBits(), rgba.width(), rgba.height(), int(rgba.bytesPerLine()), &output);
    } else {
        size = WebPEncodeRGBA(rgba.constBits(), rgba.width(), rgba.height(), int(rgba.bytesPerLine()), float(recipe.quality), &output);
    }
    if (size == 0 || !output) return false;
    bytes->append(reinterpret_cast<const char*>(output), int(size));
    WebPFree(output);
    return true;
#else
    QBuffer buffer(bytes);
    buffer.open(QIODevice::WriteOnly);
    return image.save(&buffer, "WEBP", recipe.format == FormatWebPLossless ? 100 : recipe.quality);   // Qt: 100 is lossless
#endif
}

QString outputName(const QString &sourcePath, const Recipe &recipe)
{
    QString base = QFileInfo(sourcePath).completeBaseName();
    if (base.isEmpty()) base = QStringLiteral("picture");
    const QString suffix = recipe.suffix.trimmed();
    return (suffix.isEmpty() ? base : base + QLatin1Char('-') + suffix) + QLatin1Char('.') + recipe.extension();
}

}
