/*
 * Krimble 2026-10-25: the engine of Batch Process: "recipes" (resize, format, quality, name), reading pictures (WebP included, with transparency), writing JPG, PNG and WebP
 * (lossless or lossy, with transparency). George: "I'm working a lot with webp. I would like to add this feature."
 * It uses Qt and, when it was built with it, libwebp (KRIMBLE_HAVE_WEBP). It has no Android code, so it can be tried on any machine (tools/batch-bench).
 */
#ifndef KRIMBLE_BATCH_H
#define KRIMBLE_BATCH_H

#include <QByteArray>
#include <QImage>
#include <QJsonObject>
#include <QList>
#include <QString>

namespace KrimbleBatch {

enum ResizeMode { ResizeNone = 0, ResizeLongestSide, ResizeWidth, ResizeHeight };
enum Format { FormatJpg = 0, FormatPng, FormatWebPLossless, FormatWebPLossy };

struct Recipe {
    QString name;
    int resize = ResizeNone;
    int size = 1920;           // px, for the resize modes
    bool noEnlarge = true;     // a picture already smaller than the size is not made bigger
    int format = FormatJpg;
    int quality = 90;          // 1 to 100: JPG and lossy WebP
    QString suffix;            // added to the name: "photo-suffix.webp"
    bool builtIn = false;

    QString extension() const;
    QString mime() const;
    bool keepsTransparency() const;   // PNG and both WebP
    bool usesQuality() const;         // JPG and lossy WebP
    QJsonObject toJson() const;
    static Recipe fromJson(const QJsonObject &object);
};

QList<Recipe> builtInRecipes();
QList<Recipe> userRecipes();
void saveUserRecipes(const QList<Recipe> &recipes);

bool webpAvailable();                                   // WebP can be written
QImage loadPicture(const QString &path);                // null if it cannot be read (animated WebP and other things)
QImage applyRecipe(const QImage &source, const Recipe &recipe);   // resize; a JPG is flattened onto white
bool encodePicture(const QImage &image, const Recipe &recipe, QByteArray *bytes);
QString outputName(const QString &sourcePath, const Recipe &recipe);

}

#endif
