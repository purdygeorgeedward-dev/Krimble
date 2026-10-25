// Runs every built-in recipe of KrimbleBatch on the pictures given and writes the results; prints what came out.
// Usage: engine_test OUTDIR picture1 picture2 ...
#include "KrimbleBatch.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QTextStream out(stdout);
    if (argc < 3) { out << "usage: engine_test OUTDIR pictures...\n"; return 2; }
    const QString outDir = QString::fromUtf8(argv[1]);
    QDir().mkpath(outDir);
    out << "webpAvailable: " << (KrimbleBatch::webpAvailable() ? "yes" : "no") << "\n";
    for (int i = 2; i < argc; ++i) {
        const QString path = QString::fromUtf8(argv[i]);
        const QImage source = KrimbleBatch::loadPicture(path);
        out << path << ": " << (source.isNull() ? "CANNOT READ" : QStringLiteral("%1x%2 alpha=%3").arg(source.width()).arg(source.height()).arg(source.hasAlphaChannel())) << "\n";
        if (source.isNull()) continue;
        for (const KrimbleBatch::Recipe &r : KrimbleBatch::builtInRecipes()) {
            QImage image = KrimbleBatch::applyRecipe(source, r);
            QByteArray bytes;
            const bool ok = KrimbleBatch::encodePicture(image, r, &bytes);
            const QString name = KrimbleBatch::outputName(path, r);
            if (ok) { QFile f(outDir + "/" + name); f.open(QIODevice::WriteOnly); f.write(bytes); }
            out << "  " << name << (ok ? QStringLiteral("  %1x%2  %3 bytes").arg(image.width()).arg(image.height()).arg(bytes.size()) : QStringLiteral("  FAILED")) << "\n";
        }
    }
    // a user recipe saved and read back
    KrimbleBatch::Recipe mine;
    mine.name = "My web 1200"; mine.resize = KrimbleBatch::ResizeWidth; mine.size = 1200; mine.format = KrimbleBatch::FormatWebPLossy; mine.quality = 80; mine.suffix = "my";
    KrimbleBatch::saveUserRecipes(QList<KrimbleBatch::Recipe>() << mine);
    const QList<KrimbleBatch::Recipe> back = KrimbleBatch::userRecipes();
    out << "saved/read back: " << back.size() << " recipe(s): " << (back.isEmpty() ? "-" : back.first().name + " " + back.first().extension() + " " + QString::number(back.first().size)) << "\n";
    return 0;
}
