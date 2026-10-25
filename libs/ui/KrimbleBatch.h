/*
 * Krimble 2026-10-25: the engine of Batch Process: "recipes" (resize, format, quality, name), reading pictures (WebP included, with transparency), writing JPG, PNG and WebP
 * (lossless or lossy, with transparency). George: "I'm working a lot with webp. I would like to add this feature."
 * It uses Qt and, when it was built with it, libwebp (KRIMBLE_HAVE_WEBP). It has no Android code, so it can be tried on any machine (tools/batch-bench).
 */
#ifndef KRIMBLE_BATCH_H
#define KRIMBLE_BATCH_H

#include <QByteArray>
#include <QColor>
#include <QImage>
#include <QJsonObject>
#include <QList>
#include <QString>

namespace KrimbleBatch {

enum ResizeMode { ResizeNone = 0, ResizeLongestSide, ResizeWidth, ResizeHeight };
enum Format { FormatJpg = 0, FormatPng, FormatWebPLossless, FormatWebPLossy, FormatSame = 4 };   // FormatSame (a script without a format line): the picture keeps its kind

struct Recipe {
    QString name;
    int resize = ResizeNone;
    int size = 1920;           // px, for the resize modes
    bool noEnlarge = true;     // a picture already smaller than the size is not made bigger
    int format = FormatJpg;
    int quality = 90;          // 1 to 100: JPG and lossy WebP
    QString suffix;            // added to the name: "photo-suffix.webp"
    QString script;            // Krimble Script (see KRIMBLE_SCRIPT.md): when it is not empty it is what the recipe does, instead of the settings above
    bool builtIn = false;

    QString extension() const;
    QString mime() const;
    bool keepsTransparency() const;   // PNG and both WebP
    bool usesQuality() const;         // JPG and lossy WebP
    QJsonObject toJson() const;
    static Recipe fromJson(const QJsonObject &object);
};

// ---- Krimble Script: a very small language for simple picture operations (KRIMBLE_SCRIPT.md) ----
struct ScriptStep {
    enum Kind { Resize, Fit, Crop, Rotate, Flip, Gray, Brightness, Contrast, Flatten };
    Kind kind = Resize;
    int mode = 0;          // Resize: a ResizeMode; Crop: 0 = square, 1 = W x H; Flip: 0 = horizontal, 1 = vertical
    int a = 0;             // the number: size, width, degrees, amount
    int b = 0;             // the second number: height
    bool grow = false;     // the picture may be made bigger
    QColor color;          // Flatten
};

struct Program {
    QList<ScriptStep> steps;
    int format = FormatSame;
    int quality = 90;
    QString nameAdd;       // "photo-web.webp"
    QString namePrefix;    // "web-photo.webp"
    QString folder = QStringLiteral("Batch");   // the folder in Pictures/Krimble
};

bool parseScript(const QString &text, Program *program, QString *error);   // error: "Line 3: ..." (empty program and no error for an empty script)
Program programFromRecipe(const Recipe &recipe);
QString scriptFromRecipe(const Recipe &recipe);                              // the recipe written as a script
int resolveFormat(const Program &program, const QString &sourcePath);        // FormatSame becomes JPG for a JPG, WebP (lossless) for a WebP, PNG for everything else
QImage runProgram(const QImage &source, const Program &program, int outputFormat = -1);   // outputFormat: what resolveFormat said (if not given: the program's own, PNG when it is "same")
bool encodeAs(const QImage &image, int format, int quality, QByteArray *bytes);
QString outputNameFor(const QString &sourcePath, const Program &program);
QString extensionOf(int format);
QString mimeOf(int format);
QString scriptHelp();                                                        // the short reference shown by the Help button

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
