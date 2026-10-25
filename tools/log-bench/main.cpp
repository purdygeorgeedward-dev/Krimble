// Draws the Krimble Log window offscreen. Usage: log_dialog OUT.png WIDTH HEIGHT krita.log ["extra text"]
#include "KrimbleLogDialog.h"
#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    QPalette pal;
    pal.setColor(QPalette::Window, QColor("#363954")); pal.setColor(QPalette::WindowText, QColor("#e6e6ee"));
    pal.setColor(QPalette::Base, QColor("#2a2c44")); pal.setColor(QPalette::Text, QColor("#e6e6ee"));
    pal.setColor(QPalette::Button, QColor("#434766")); pal.setColor(QPalette::ButtonText, QColor("#e6e6ee"));
    app.setPalette(pal);
    QFont f = app.font(); f.setPointSizeF(12); app.setFont(f);
    KrimbleLogDialog dialog(nullptr, QString::fromUtf8(argv[4]), argc > 5 ? QString::fromUtf8(argv[5]) : QString());
    dialog.resize(atoi(argv[2]), atoi(argv[3]));
    dialog.show();
    for (int i = 0; i < 5; ++i) app.processEvents();
    dialog.grab().save(QString::fromUtf8(argv[1]));
    return 0;
}
