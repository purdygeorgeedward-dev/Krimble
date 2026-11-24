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
enum Format { FormatJpg = 0, FormatPng, FormatWebPLossless, FormatWebPLossy, FormatSame = 4 };

struct Recipe {
    QString name;
    int resize = ResizeNone;
    int size = 1920;
    bool noEnlarge = true;
    int format = FormatJpg;
    int quality = 90;
    QString suffix;
    QString script;
    bool builtIn = false;

    QString extension() const;
    QString mime() const;
    bool keepsTransparency() const;
    bool usesQuality() const;
    QJsonObject toJson() const;
    static Recipe fromJson(const QJsonObject &object);
};

struct ScriptStep {
    enum Kind { Resize, Fit, Crop, Rotate, Flip, Gray, Brightness, Contrast, Flatten };
    Kind kind = Resize;
    int mode = 0;
    int a = 0;
    int b = 0;
    bool grow = false;
    QColor color;
};

struct Program {
    QList<ScriptStep> steps;
    int format = FormatSame;
    int quality = 90;
    QString nameAdd;
    QString namePrefix;
    QString folder = QStringLiteral("Batch");
    bool framesOut = false;
    int fps = 0;
    int loop = -1;
};

struct Frame {
    QImage image;
    int delayMs = 100;
};

struct Animation {
    QList<Frame> frames;
    int loopCount = 0;
};

bool gifReadable();
bool loadAnimation(const QString &path, Animation *animation);
bool encodeAnimation(const Animation &animation, int format, int quality, QByteArray *bytes);

bool parseScript(const QString &text, Program *program, QString *error);
Program programFromRecipe(const Recipe &recipe);
QString scriptFromRecipe(const Recipe &recipe);
int resolveFormat(const Program &program, const QString &sourcePath);
QImage runProgram(const QImage &source, const Program &program, int outputFormat = -1);
bool encodeAs(const QImage &image, int format, int quality, QByteArray *bytes);
QString outputNameFor(const QString &sourcePath, const Program &program, int frameNumber = 0);
QString extensionOf(int format);
QString mimeOf(int format);
QString scriptHelp();

QList<Recipe> builtInRecipes();
QList<Recipe> userRecipes();
void saveUserRecipes(const QList<Recipe> &recipes);

bool webpAvailable();
QImage loadPicture(const QString &path);
QImage applyRecipe(const QImage &source, const Recipe &recipe);
bool encodePicture(const QImage &image, const Recipe &recipe, QByteArray *bytes);
QString outputName(const QString &sourcePath, const Recipe &recipe);

}

#endif
