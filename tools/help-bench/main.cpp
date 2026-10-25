// Draws the Krimble Help window offscreen, as a phone would show it, and writes pictures of it.
// Usage: help_dialog OUTDIR WIDTH HEIGHT krimble-help.md [part to go to] [word to find]
#include "KrimbleHelpDialog.h"
#include <QApplication>
#include <QComboBox>
#include <QLineEdit>
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
    pal.setColor(QPalette::Link, QColor("#8fb8ff"));
    app.setPalette(pal);
    QFont f = app.font(); f.setPointSizeF(13); app.setFont(f);
    QTextStream out(stdout);
    const QString outDir = QString::fromUtf8(argv[1]);
    QDir().mkpath(outDir);
    QFile file(QString::fromUtf8(argv[4]));
    file.open(QIODevice::ReadOnly);
    KrimbleHelpDialog dialog(nullptr, QString::fromUtf8(file.readAll()));
    dialog.resize(atoi(argv[2]), atoi(argv[3]));
    dialog.show();
    for (int i = 0; i < 5; ++i) app.processEvents();
    dialog.grab().save(outDir + "/help_top.png");
    if (argc > 5) {
        QComboBox *parts = dialog.findChild<QComboBox*>();
        const int index = parts->findText(QString::fromUtf8(argv[5]));
        out << "part '" << argv[5] << "' at list index " << index << "\n";
        if (index > 0) { parts->setCurrentIndex(index); emit parts->activated(index); }
        for (int i = 0; i < 5; ++i) app.processEvents();
        dialog.grab().save(outDir + "/help_part.png");
    }
    if (argc > 6) {
        QLineEdit *find = dialog.findChild<QLineEdit*>();
        find->setText(QString::fromUtf8(argv[6]));
        for (int i = 0; i < 5; ++i) app.processEvents();
        dialog.grab().save(outDir + "/help_find.png");
    }
    return 0;
}
