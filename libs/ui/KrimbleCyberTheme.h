#pragma once

#include <QColor>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QString>
#include <QtMath>
#include <QStringList>

inline void krimbleLoadCyberFonts()
{
    static bool done = false;
    if (done) return;
    done = true;
    const QStringList names{QStringLiteral("Orbitron-VF.ttf"), QStringLiteral("Orbitron-Medium.ttf"), QStringLiteral("Orbitron-Bold.ttf"), QStringLiteral("Rajdhani-Medium.ttf"), QStringLiteral("Rajdhani-SemiBold.ttf")};
    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("krita/fonts"), QStandardPaths::LocateDirectory);
#ifdef Q_OS_ANDROID
    dirs << QStringLiteral("assets:/krita/fonts");
#endif
    Q_FOREACH (const QString &name, names) {
        Q_FOREACH (const QString &dir, dirs) {
            const QString path = dir + QLatin1Char('/') + name;
            if (!QFile::exists(path)) continue;
            QFile f(path);
            if (f.open(QIODevice::ReadOnly) && QFontDatabase::addApplicationFontFromData(f.readAll()) >= 0) break;
        }
    }
}

inline QImage krimbleCyberBlur(const QImage &src, int factor)
{
    QImage small = src.scaled(src.size() / factor, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    small = small.scaled(small.size() / 2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).scaled(small.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    return small.scaled(src.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

inline void krimbleCyberLeftGlow(QPainter &p, int S, qreal in, int cut, bool bottomCut, qreal thick = 8.0)
{
    QPainterPath side;
    if (bottomCut) {
        side.moveTo(in, 0);
        side.lineTo(in, S - in - cut * 0.7);
        side.lineTo(in + cut * 0.7, S - in);
        side.lineTo(S, S - in);
    } else {
        side.moveTo(S, in);
        side.lineTo(in + cut * 0.7, in);
        side.lineTo(in, in + cut * 0.7);
        side.lineTo(in, S);
    }
    p.setBrush(Qt::NoBrush);
    for (int i = 5; i >= 1; --i) {
        p.setPen(QPen(QColor(40, 200, 255, int(85.0 * (1.0 - qreal(i) / 6.0))), thick + thick * 0.3 * i, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
        p.drawPath(side);
    }
    p.setPen(QPen(QColor(120, 235, 255), thick, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
    p.drawPath(side);
}

inline void krimbleCyberPaintMenuStripe(QPainter &p, const QSize &size)
{
    const qreal h = size.height(), w = size.width();
    const qreal left = 6.0, inner = 22.0, bottom = h - 4.0, top = h - 9.0;
    QPolygonF stripe;
    stripe << QPointF(left, 0) << QPointF(left, bottom - 24.0) << QPointF(left + 24.0, bottom) << QPointF(w - 2.0, bottom)
           << QPointF(w - 2.0, top) << QPointF(33.0, top) << QPointF(inner, h - 20.0) << QPointF(inner, 0);
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0x78, 0xeb, 0xff));
    p.drawPolygon(stripe);
    p.restore();
}

inline void krimbleCyberPaintMenuSelectionEnd(QPainter &p, const QRect &item, int menuWidth)
{
    if (item.isEmpty()) return;
    p.fillRect(QRect(item.right() + 1, item.top(), menuWidth - 2 - item.right() - 1, item.height()), QColor(0x78, 0xeb, 0xff));
}

inline QImage krimbleCyberMenuFrame(QColor fill, QColor edge, int cut, int glow)
{
    const int S = 96;
    QImage im(S, S, QImage::Format_ARGB32_Premultiplied);
    im.fill(Qt::transparent);
    QPainter p(&im);
    p.setRenderHint(QPainter::Antialiasing);
    const qreal g = glow;
    QPolygonF poly;
    poly << QPointF(g, 0) << QPointF(S - g, 0) << QPointF(S - g, S - g) << QPointF(g + cut, S - g) << QPointF(g, S - g - cut);
    QPainterPath open;
    open.moveTo(S - g, 0); open.lineTo(S - g, S - g); open.lineTo(g + cut, S - g); open.lineTo(g, S - g - cut); open.lineTo(g, 0);
    for (int i = glow; i >= 1; --i) {
        QColor c = edge; c.setAlpha(int(40.0 * (1.0 - qreal(i) / (glow + 1)) * edge.alphaF()));
        p.setPen(QPen(c, 2 * i, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin)); p.drawPath(open);
    }
    p.setPen(Qt::NoPen); p.setBrush(fill); p.drawPolygon(poly);
    p.setBrush(Qt::NoBrush); p.setPen(QPen(edge, 1.6, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin)); p.drawPath(open);
    return im;
}

inline QImage krimbleCyberCutFrame(QColor fill, QColor edge, int cut, bool topLeft, bool topRight, bool bottomRight, bool bottomLeft, qreal thick = 8.0)
{
    const int S = 48;
    QImage im(S, S, QImage::Format_ARGB32_Premultiplied);
    im.fill(Qt::transparent);
    QPainter p(&im);
    p.setRenderHint(QPainter::Antialiasing);
    const qreal e = 0.8;
    QPolygonF poly;
    poly << QPointF(e + (topLeft ? cut : 0), e) << QPointF(S - e - (topRight ? cut : 0), e);
    if (topRight) poly << QPointF(S - e, e + cut);
    poly << QPointF(S - e, S - e - (bottomRight ? cut : 0));
    if (bottomRight) poly << QPointF(S - e - cut, S - e);
    poly << QPointF(e + (bottomLeft ? cut : 0), S - e);
    if (bottomLeft) poly << QPointF(e, S - e - cut);
    poly << QPointF(e, e + (topLeft ? cut : 0));
    QColor faint = edge;
    faint.setAlpha(edge.alpha() / 3);
    p.setPen(QPen(faint, 1.2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    p.setBrush(fill);
    p.drawPolygon(poly);
    if (thick > 0) krimbleCyberLeftGlow(p, S, 6.0, cut, bottomLeft || topLeft ? bottomLeft : true, thick);
    return im;
}


inline QImage krimbleCyberBackground(QSize s)
{
    QImage out(s, QImage::Format_ARGB32_Premultiplied);
    out.fill(QColor(0, 0, 0));
    QRandomGenerator rg(11);
    struct Depth { int blur; int count; int alpha; qreal scale; };
    const Depth depths[] = {Depth{10, 7, 140, 1.6}, Depth{5, 6, 170, 1.0}, Depth{3, 4, 200, 0.7}};
    const qreal k = s.width() / 780.0;
    for (const Depth &d : depths) {
        QImage layer(s, QImage::Format_ARGB32_Premultiplied);
        layer.fill(Qt::transparent);
        QPainter p(&layer);
        p.setRenderHint(QPainter::Antialiasing);
        for (int i = 0; i < d.count; ++i) {
            const qreal w = (60 + rg.bounded(140)) * d.scale * k, h = (40 + rg.bounded(90)) * d.scale * k;
            const QRectF r(rg.bounded(s.width()) - w / 2, rg.bounded(s.height()) - h / 2, w, h);
            const QColor fill = QColor::fromHsv(195 + rg.bounded(25), 200, 255, d.alpha / 3);
            const QColor edge = QColor::fromHsv(190 + rg.bounded(20), 150, 255, d.alpha);
            p.setBrush(fill); p.setPen(QPen(edge, 2.0 * d.scale * k)); p.drawRect(r);
            p.setPen(QPen(edge, 1.0 * d.scale * k));
            for (int j = 1; j < 4; ++j) p.drawLine(QPointF(r.left() + 8 * k, r.top() + j * h / 4), QPointF(r.left() + 8 * k + w * (0.3 + 0.15 * j), r.top() + j * h / 4));
        }
        p.end();
        QPainter o(&out);
        o.setCompositionMode(QPainter::CompositionMode_Plus);
        o.drawImage(0, 0, krimbleCyberBlur(layer, d.blur));
    }
    return out;
}

inline QImage krimbleCyberBarItem()
{
    const int S = 48;
    QImage im(S, S, QImage::Format_ARGB32_Premultiplied);
    im.fill(QColor(2, 8, 22));
    QPainter p(&im);
    p.setRenderHint(QPainter::Antialiasing);
    const QRectF r(3, 3, S - 6, S - 6);
    for (int i = 5; i >= 1; --i) {
        p.setPen(QPen(QColor(40, 200, 255, int(85.0 * (1.0 - qreal(i) / 6.0))), 4.0 + 1.2 * i));
        p.drawRect(r);
    }
    p.setPen(QPen(QColor(120, 235, 255), 4.0));
    p.drawRect(r);
    return im;
}

inline QImage krimbleCyberLed(int level, int W = 60, int H = 36)
{
    QImage im(W, H, QImage::Format_ARGB32_Premultiplied);
    im.fill(Qt::transparent);
    QPainter p(&im);
    p.setRenderHint(QPainter::Antialiasing);
    QRadialGradient g(QPointF(W / 2.0, H / 2.0), H / 2.0);
    g.setColorAt(0.0, QColor(170, 240, 255, 255));
    g.setColorAt(0.12, QColor(60, 190, 255, 230));
    g.setColorAt(0.45, QColor(20, 120, 255, level ? 150 : 80));
    g.setColorAt(1.0, QColor(20, 120, 255, 0));
    p.setPen(Qt::NoPen);
    p.setBrush(g);
    p.drawEllipse(QPointF(W / 2.0, H / 2.0), level ? H / 2.0 : H / 3.0, level ? H / 2.0 : H / 3.0);
    return im;
}

inline QImage krimbleCyberToolboxBack(QSize s)
{
    QImage glow(s, QImage::Format_ARGB32_Premultiplied);
    glow.fill(QColor(1, 2, 4));
    {
        QPainter p(&glow);
        p.setRenderHint(QPainter::Antialiasing);
        const QPointF c(s.width() / 2.0, s.height() / 2.0);
        QLinearGradient across(0, 0, s.width(), 0);
        across.setColorAt(0.0, QColor(10, 50, 120, 0));
        across.setColorAt(0.5, QColor(14, 70, 160, 110));
        across.setColorAt(1.0, QColor(10, 50, 120, 0));
        p.fillRect(QRect(QPoint(0, 0), s), across);
        QRadialGradient g(c, s.height() * 0.3);
        g.setColorAt(0.0, QColor(60, 150, 255, 230));
        g.setColorAt(0.35, QColor(30, 110, 220, 140));
        g.setColorAt(1.0, QColor(14, 70, 160, 0));
        p.setPen(Qt::NoPen);
        p.setBrush(g);
        p.setTransform(QTransform().translate(c.x(), c.y()).scale(s.width() * 1.6 / s.height(), 1.0).translate(-c.x(), -c.y()));
        p.drawEllipse(c, s.height() * 0.3, s.height() * 0.3);
    }
    glow = krimbleCyberBlur(glow, 4);
    QPainter p(&glow);
    p.setRenderHint(QPainter::Antialiasing);
    QLinearGradient sheen(0, 0, s.width(), s.width());
    sheen.setColorAt(0.0, QColor(255, 255, 255, 26));
    sheen.setColorAt(0.5, QColor(255, 255, 255, 6));
    sheen.setColorAt(0.52, QColor(255, 255, 255, 0));
    sheen.setColorAt(1.0, QColor(255, 255, 255, 0));
    p.fillRect(QRect(QPoint(0, 0), s), sheen);
    QLinearGradient streak(0, s.height() * 0.55, s.width(), s.height() * 0.55 + s.width());
    streak.setColorAt(0.0, QColor(255, 255, 255, 0));
    streak.setColorAt(0.45, QColor(255, 255, 255, 12));
    streak.setColorAt(0.5, QColor(255, 255, 255, 0));
    streak.setColorAt(1.0, QColor(255, 255, 255, 0));
    p.fillRect(QRect(QPoint(0, 0), s), streak);
    return glow;
}

inline QImage krimbleCyberTitleGlow()
{
    const int S = 48;
    QImage im(S, S, QImage::Format_ARGB32_Premultiplied);
    im.fill(QColor(2, 8, 22));
    QPainter p(&im);
    p.setRenderHint(QPainter::Antialiasing);
    for (int i = 10; i >= 1; --i) {
        p.setPen(QPen(QColor(40, 200, 255, int(70.0 * qreal(i) / 10.0)), 2.0));
        const qreal d = 10 - i;
        p.setBrush(Qt::NoBrush);
        p.drawRect(QRectF(d + 1, d + 1, S - 2 * d - 2, S - 2 * d - 2));
    }
    return im;
}

inline void krimbleCyberPaintTab(QPainter &p, const QRectF &r, bool selected, const QString &text, QFont font)
{
    const qreal k = r.height() / 87.0;
    const qreal top = 26.0 * k, corner = 23.0 * k, side = 11.0 * k, cutW = 40.0 * k, cutH = 34.0 * k;
    const qreal x = r.left(), y = r.top(), W = r.width(), H = r.height();
    const QColor cyan(0x78, 0xeb, 0xff);
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    QPolygonF outline;
    outline << QPointF(x, y) << QPointF(x + W - cutW, y) << QPointF(x + W, y + cutH) << QPointF(x + W, y + H) << QPointF(x, y + H);
    p.setBrush(selected ? cyan : QColor(6, 18, 42));
    p.drawPolygon(outline);
    if (!selected) {
        const qreal len = qSqrt(cutW * cutW + cutH * cutH);
        const qreal sx = x + W - cutW - corner * cutH / len, sy = y + corner * cutW / len;
        const qreal ix = sx + (y + top - sy) * cutW / cutH;
        const qreal jy = qMin(y + H, sy + (x + W - side - sx) * cutH / cutW);
        QPolygonF stripe;
        stripe << QPointF(x, y) << QPointF(x + W - cutW, y) << QPointF(x + W, y + cutH) << QPointF(x + W, y + H) << QPointF(x + W - side, y + H)
               << QPointF(x + W - side, jy) << QPointF(ix, y + top) << QPointF(x, y + top);
        p.setBrush(cyan);
        p.drawPolygon(stripe);
    }
    font.setPixelSize(qMax(10, qRound(H * 0.42)));
    p.setFont(font);
    p.setPen(selected ? QColor(2, 18, 38) : cyan);
    p.drawText(QPointF(x + H * 0.2, y + H * 0.8), text);
    p.restore();
}

inline QString krimbleCyberStyleSheet(int menuFontPx, int barFontPx)
{
    krimbleLoadCyberFonts();
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(dir);
    const QString menuFrame = dir + QStringLiteral("/krimble-cyber-menu.png");
    const QString rowFrame = dir + QStringLiteral("/krimble-cyber-row.png");
    const QString back = dir + QStringLiteral("/krimble-cyber-back.png");
    const QString boxFrame = dir + QStringLiteral("/krimble-cyber-box.png");
    const QString plainFrame = dir + QStringLiteral("/krimble-cyber-plain.png");
    const QString buttonFrame = dir + QStringLiteral("/krimble-cyber-button.png");
    const QString barItem = dir + QStringLiteral("/krimble-cyber-bar-item.png");
    const QString ledOff = dir + QStringLiteral("/krimble-cyber-led.png");
    const QString ledOn = dir + QStringLiteral("/krimble-cyber-led-on.png");
    const QString ledTool = dir + QStringLiteral("/krimble-cyber-led-tool.png");
    const QString toolBack = dir + QStringLiteral("/krimble-cyber-toolbox.png");
    krimbleCyberMenuFrame(QColor(6, 30, 70, 153), QColor(56, 225, 255, 110), 34, 3).save(menuFrame);
    krimbleCyberBarItem().save(rowFrame);
    krimbleCyberTitleGlow().save(barItem);
    krimbleCyberCutFrame(QColor(2, 10, 26, 150), QColor(56, 225, 255, 120), 9, false, false, false, true).save(boxFrame);
    krimbleCyberCutFrame(QColor(2, 10, 26, 150), QColor(56, 225, 255, 160), 9, false, false, false, true, 0.0).save(plainFrame);
    {
        const int S = 48;
        QImage im(S, S, QImage::Format_ARGB32_Premultiplied);
        im.fill(Qt::transparent);
        QPainter p(&im);
        p.setRenderHint(QPainter::Antialiasing);
        QPolygonF poly;
        poly << QPointF(20, 0) << QPointF(S, 0) << QPointF(S, S) << QPointF(0, S) << QPointF(0, 20);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(1, 2, 4));
        p.drawPolygon(poly);
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.fillRect(QRectF(21, 21, 6, 6), Qt::transparent);
        p.end();
        im.save(buttonFrame);
    }
    krimbleCyberLed(0).save(ledOff);
    krimbleCyberLed(1).save(ledOn);
    krimbleCyberLed(1, 26, 26).save(ledTool);
    krimbleCyberToolboxBack(QSize(120, 720)).save(toolBack);
    if (!QFile::exists(back)) krimbleCyberBackground(QSize(1560, 720)).save(back);

    QString sheet = QStringLiteral(R"KRIMBLECYBER(
QWidget { font-family: "Rajdhani"; font-weight: 600; }
QMenuBar { background: rgba(14, 37, 80, 140); border: none; font-family: "Orbitron"; font-weight: 600; font-size: BARPX; }
QMenuBar::item { color: #78ebff; background: transparent; border: none; }
QMenuBar::item:selected, QMenuBar::item:pressed { background: rgba(14, 37, 80, 140); color: #78ebff; border-image: none; border: none; }
QMenu { font-family: "Orbitron"; font-weight: 700; font-size: MENUPX; color: #78ebff; background: transparent; border: none;
        border-image: url(MENUFRAME) 0 22 40 40 stretch; border-width: 0px 2px 22px 22px; }
QMenu { padding: 0px; }
QMenu::item { margin: 0px; padding-left: 14px; padding-right: 14px; }
QMenu::item { background: transparent; color: #78ebff; border: 5px solid transparent; }
QMenu::item:selected { color: #021226; background: #78ebff; border-image: none; border: 5px solid #78ebff; } QMenu::item:pressed { border-image: url(ROWFRAME) 12 12 12 12 stretch; }
QMenu::item:disabled { color: rgba(120, 235, 255, 90); }
QMenu::separator { background: rgba(56, 225, 255, 120); height: 1px; border: none; }
KisMainWindow, QMdiArea, KisWelcomePageWidget { background-color: #000000; background-image: url(BACK); background-position: center; }
QDialog, QMessageBox, QInputDialog, QColorDialog, QFileDialog { background-color: rgba(14, 37, 80, 140); background-image: none; color: #e4f9ff; }
QLabel, QCheckBox, QRadioButton, QGroupBox { color: #e4f9ff; }
QDockWidget { background: transparent; }
QDockWidget > QWidget { background-color: rgba(14, 37, 80, 140); background-image: none; color: #e4f9ff; }
QDockWidget#ToolBox, QDockWidget#ToolBox > QWidget { background-color: rgba(14, 37, 80, 140); border-image: none; }
QDockWidget#ToolBox QToolButton { background: transparent; border: none; }
QDockWidget#ToolBox QToolButton:checked { background-color: transparent; background-image: none; border: none; }
QDockWidget#ToolBox QWidget { background-image: none; }
QDockWidget#ColorSelectorNg { background: transparent; }
QDockWidget#ColorSelectorNg > QWidget { background-color: #0e2550; background-image: none; }
QDockWidget#ColorSelectorNg QScrollArea, QDockWidget#ColorSelectorNg QScrollArea > QWidget > QWidget { background: transparent; }
QDockWidget > KoDockWidgetTitleBar { background: transparent; background-image: none; }
KoDockWidgetTitleBar, KoDockWidgetTitleBar QLabel { font-family: "Orbitron"; font-weight: 600; font-size: BARPX; color: #78ebff; }
QStatusBar, QToolBar { background: rgba(14, 37, 80, 140); color: #e4f9ff; }
QAbstractItemView { background: rgba(8, 23, 47, 140); color: #e4f9ff; selection-background-color: #78ebff; selection-color: #021226; }
QAbstractItemView::item:selected { background: #78ebff; color: #021226; }
QLineEdit { background: transparent; border-radius: 0px; border-image: url(BOXFRAME) 9 9 9 9 stretch; border-width: 3px; color: #e4f9ff; }
QAbstractSpinBox, QComboBox { background: transparent; border-radius: 0px; border-image: url(PLAINFRAME) 9 9 9 9 stretch; border-width: 3px; color: #e4f9ff; }
QPushButton, QPushButton:hover, QPushButton:default { background-color: qradialgradient(cx:0.5, cy:0.6, radius:0.8, fx:0.5, fy:0.6, stop:0 rgba(42, 82, 148, 150), stop:0.6 rgba(23, 51, 106, 150), stop:1 rgba(14, 37, 80, 140)); background-clip: padding; background-image: url(LEDOFF); background-position: center; background-repeat: no-repeat; border-radius: 0px; border-image: url(BUTTONFRAME) 21 21 21 21 stretch; border-width: 9px; color: #e4f9ff; padding: 3px 12px; }
QPushButton:pressed, QPushButton:checked { background-color: qradialgradient(cx:0.5, cy:0.6, radius:0.8, fx:0.5, fy:0.6, stop:0 rgba(18, 51, 106, 150), stop:0.6 rgba(11, 31, 68, 150), stop:1 rgba(8, 23, 47, 140)); background-clip: padding; background-image: url(LEDON); background-position: center; background-repeat: no-repeat; border-radius: 0px; border-image: url(BUTTONFRAME) 21 21 21 21 stretch; border-width: 9px; }
QGroupBox { border-radius: 0px; border-image: url(BOXFRAME) 9 9 9 9 stretch; border-width: 3px; }
QMainWindow > QTabBar { background: transparent; }
QMainWindow > QTabBar::tab, QTabBar::tab { background: transparent; border: none; border-image: none; border-radius: 0px; color: transparent; height: 38px; padding: 0px 18px; margin-right: 1px; }
QMainWindow > QTabBar::tab:selected, QTabBar::tab:selected { background: transparent; border: none; border-image: none; color: transparent; }
QScrollBar::handle:vertical, QScrollBar::handle:horizontal { border-radius: 0px; }
)KRIMBLECYBER");
    sheet.replace(QStringLiteral("MENUFRAME"), menuFrame).replace(QStringLiteral("ROWFRAME"), rowFrame).replace(QStringLiteral("BARITEM"), barItem)
         .replace(QStringLiteral("BOXFRAME"), boxFrame).replace(QStringLiteral("PLAINFRAME"), plainFrame).replace(QStringLiteral("BUTTONFRAME"), buttonFrame)
         .replace(QStringLiteral("LEDOFF"), ledOff).replace(QStringLiteral("LEDON"), ledOn).replace(QStringLiteral("LEDTOOL"), ledTool)
         .replace(QStringLiteral("TOOLBACK"), toolBack).replace(QStringLiteral("BACK)"), back + QStringLiteral(")"))
         .replace(QStringLiteral("MENUPX"), QString::number(menuFontPx) + QStringLiteral("px")).replace(QStringLiteral("BARPX"), QString::number(barFontPx) + QStringLiteral("px"));
    return sheet;
}
