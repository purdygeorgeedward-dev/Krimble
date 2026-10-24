// Builds the Type tool window (KrimbleTextDialog) on its own, with a dark palette like Krimble's, and saves pictures of its tabs.
// Build and run: see build.sh. QT_QPA_PLATFORM=offscreen
#include "KrimbleTextDialog.h"
#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include <QPixmap>
#include <QTimer>
#include <QLayout>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    QPalette pal;
    pal.setColor(QPalette::Window, QColor("#363954"));
    pal.setColor(QPalette::WindowText, QColor("#e6e6ee"));
    pal.setColor(QPalette::Base, QColor("#2a2c44"));
    pal.setColor(QPalette::AlternateBase, QColor("#363954"));
    pal.setColor(QPalette::Text, QColor("#e6e6ee"));
    pal.setColor(QPalette::Button, QColor("#434766"));
    pal.setColor(QPalette::ButtonText, QColor("#e6e6ee"));
    pal.setColor(QPalette::Highlight, QColor("#4f7396"));
    pal.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    app.setPalette(pal);
    QFont f = app.font();
    f.setPointSizeF(13);
    app.setFont(f);

    KrimbleTextStyle style;
    style.family = QStringLiteral("DejaVu Serif");
    style.size = 250.0;
    style.align = 1;
    style.color = QColor("#000000");
    QString text = QStringLiteral("Jezzy English");
    KrimbleTextDialog dialog(nullptr, text, style, false, QColor("#c00020"));
    if (getenv("BENCH_DEBUG")) {
        for (QLayout *l : dialog.findChildren<QLayout*>()) {
            if (l->minimumSize().width() > 300) qInfo("layout %s min %d", l->metaObject()->className(), l->minimumSize().width());
        }
        for (QGridLayout *g : dialog.findChildren<QGridLayout*>()) {
            for (int i = 0; i < g->count(); ++i) {
                int r, c, rs, cs; g->getItemPosition(i, &r, &c, &rs, &cs);
                QWidget *w = g->itemAt(i)->widget();
                qInfo("grid item r%d c%d span %dx%d min %d %s %s", r, c, rs, cs, g->itemAt(i)->minimumSize().width(), w ? w->metaObject()->className() : "?", w && qobject_cast<QLabel*>(w) ? qPrintable(qobject_cast<QLabel*>(w)->text()) : "");
            }
            qInfo("---");
        }
        for (QWidget *w : dialog.findChildren<QWidget*>()) {
            if (w->minimumSizeHint().width() > 200) qInfo("widget %s '%s' min %d", w->metaObject()->className(), qPrintable(w->objectName()), w->minimumSizeHint().width());
        }
    }
    qInfo("minimum width of the window: %d px (a phone in portrait is about 340)", dialog.minimumSizeHint().width());
    const int width = argc > 1 ? QString(argv[1]).toInt() : 400;
    const int height = argc > 2 ? QString(argv[2]).toInt() : dialog.sizeHint().height();
    dialog.resize(width, height);
    dialog.show();
    app.processEvents();
    for (int tab = 0; tab < 4; ++tab) {
        dialog.showTab(tab);
        app.processEvents();
        dialog.grab().save(QStringLiteral("/tmp/textdlg_%1_tab%2.png").arg(argc > 2 ? "land" : "port").arg(tab));
    }
    return 0;
}
