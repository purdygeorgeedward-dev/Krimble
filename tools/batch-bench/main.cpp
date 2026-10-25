// Draws the Batch Process window offscreen (upright and sideways) and runs it on the pictures given, saving the results into a folder.
// Usage: batch_dialog OUTDIR WIDTH HEIGHT picture...   (pictures are optional)
#include "KrimbleBatchDialog.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QPalette>
#include <QStyleFactory>
#include <QTextStream>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    QPalette pal;
    pal.setColor(QPalette::Window, QColor("#363954")); pal.setColor(QPalette::WindowText, QColor("#e6e6ee"));
    pal.setColor(QPalette::Base, QColor("#2a2c44")); pal.setColor(QPalette::Text, QColor("#e6e6ee"));
    pal.setColor(QPalette::Button, QColor("#434766")); pal.setColor(QPalette::ButtonText, QColor("#e6e6ee"));
    pal.setColor(QPalette::Highlight, QColor("#4f7396")); pal.setColor(QPalette::HighlightedText, Qt::white);
    app.setPalette(pal);
    QFont f = app.font(); f.setPointSizeF(13); app.setFont(f);
    QTextStream out(stdout);
    const QString outDir = QString::fromUtf8(argv[1]);
    QDir().mkpath(outDir);
    const int w = atoi(argv[2]), h = atoi(argv[3]);
    QStringList pictures;
    for (int i = 4; i < argc; ++i) pictures << QString::fromUtf8(argv[i]);
    KrimbleBatchDialog dialog(nullptr, []() { return QStringList(); },
                              [&](const QString &temp, const QString &mime, const QString &name) { Q_UNUSED(mime); return QFile::copy(temp, QDir(outDir).filePath(name)); },
                              QStringLiteral("Pictures/Krimble/Batch"));
    dialog.resize(w, h);
    dialog.show();
    app.processEvents();
    dialog.grab().save(outDir + "/dialog.png");
    if (!pictures.isEmpty()) {
        dialog.runOn(pictures);
        app.processEvents();
        dialog.grab().save(outDir + "/dialog_after.png");
    }
    out << "recipe now: " << dialog.currentRecipe().name << " ext=" << dialog.currentRecipe().extension() << "\n";
    return 0;
}
