/*
 * Krimble 2026-11-23 (cyber-theme branch): the "Krimble Cyber" theme.
 * George: "So you could build a cyber futuristic theme with transparent blue menus with angular cut corners and futuristic fonts and a red
 * menu bar and holographic panels?" / "Try it." / "How about distant blurred holographic blue layers over black?" / "Much better. If you can
 * save that as a cyber theme that would be cool." / "The outlines are a little heavy handed." / "We don't need the pink underline below the
 * red bar, do we?" / "Push Cyber."
 * Look approved in the Preview Tool: cyber8.png (light outlines, no line under the red bar).
 *
 * It is the Krimble Painted theme with this sheet added last:
 * - a red menu bar in Orbitron, no line under it; an open title takes the menu's color and joins it with no line
 * - see-through dark blue menus with a cut bottom-right corner and a faint cyan edge (a picture made here, used as a border image)
 * - menu text in Rajdhani; the highlighted row is a cyan bar with a cut corner
 * - panels: see-through dark blue with faint scanlines, over a window background of distant blurred blue holographic layers
 * - Color Lab and the toolbox are left out of the panel look (Color Lab is protected; the toolbox keeps its own look)
 * The pictures are drawn once into the cache folder, like the Aqua pictures.
 */
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
#include <QStringList>

// the theme's two fonts (both SIL OFL, bundled in krita/fonts): loaded now, since the other bundled fonts wait for the text tool
inline void krimbleLoadCyberFonts()
{
    static bool done = false;
    if (done) return;
    done = true;
    const QStringList names{QStringLiteral("Orbitron-VF.ttf"), QStringLiteral("Rajdhani-Medium.ttf"), QStringLiteral("Rajdhani-SemiBold.ttf")};
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

// a frame with an open top (it joins the menu bar), a cut bottom-right corner, a faint edge and a small glow
inline QImage krimbleCyberMenuFrame(QColor fill, QColor edge, int cut, int glow)
{
    const int S = 96;
    QImage im(S, S, QImage::Format_ARGB32_Premultiplied);
    im.fill(Qt::transparent);
    QPainter p(&im);
    p.setRenderHint(QPainter::Antialiasing);
    const qreal g = glow;
    QPolygonF poly;
    poly << QPointF(g, 0) << QPointF(S - g, 0) << QPointF(S - g, S - g - cut) << QPointF(S - g - cut, S - g) << QPointF(g, S - g);
    QPainterPath open;
    open.moveTo(g, 0); open.lineTo(g, S - g); open.lineTo(S - g - cut, S - g); open.lineTo(S - g, S - g - cut); open.lineTo(S - g, 0);
    for (int i = glow; i >= 1; --i) {
        QColor c = edge; c.setAlpha(int(40.0 * (1.0 - qreal(i) / (glow + 1)) * edge.alphaF()));
        p.setPen(QPen(c, 2 * i, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin)); p.drawPath(open);
    }
    p.setPen(Qt::NoPen); p.setBrush(fill); p.drawPolygon(poly);
    p.setBrush(Qt::NoBrush); p.setPen(QPen(edge, 1.6, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin)); p.drawPath(open);
    return im;
}

// a closed frame: cut top-left and bottom-right corners (the highlighted menu row)
inline QImage krimbleCyberFrame(QColor fill, QColor edge, int cut, int glow)
{
    const int S = 96;
    QImage im(S, S, QImage::Format_ARGB32_Premultiplied);
    im.fill(Qt::transparent);
    QPainter p(&im);
    p.setRenderHint(QPainter::Antialiasing);
    const qreal g = glow;
    QPolygonF poly;
    poly << QPointF(g + cut, g) << QPointF(S - g, g) << QPointF(S - g, S - g - cut) << QPointF(S - g - cut, S - g) << QPointF(g, S - g) << QPointF(g, g + cut);
    p.setPen(Qt::NoPen); p.setBrush(fill); p.drawPolygon(poly);
    if (edge.alpha() > 0) { p.setBrush(Qt::NoBrush); p.setPen(QPen(edge, 1.6, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)); p.drawPolygon(poly); }
    return im;
}

inline QImage krimbleCyberScanlines()
{
    QImage im(8, 6, QImage::Format_ARGB32_Premultiplied);
    im.fill(Qt::transparent);
    for (int x = 0; x < 8; ++x) im.setPixelColor(x, 0, QColor(120, 230, 255, 22));
    return im;
}

inline QImage krimbleCyberBlur(const QImage &src, int factor)
{
    // a soft blur: shrink smoothly, then grow back smoothly
    QImage small = src.scaled(src.size() / factor, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    small = small.scaled(small.size() / 2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).scaled(small.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    return small.scaled(src.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

// distant, out-of-focus holographic blue layers over black (George: "distant blurred holographic blue layers over black")
inline QImage krimbleCyberBackground(QSize s)
{
    QImage out(s, QImage::Format_ARGB32_Premultiplied);
    out.fill(QColor(0, 0, 0));
    QRandomGenerator rg(11);
    struct Depth { int blur; int count; int alpha; qreal scale; };
    const Depth depths[] = {Depth{10, 7, 140, 1.6}, Depth{5, 6, 170, 1.0}, Depth{3, 4, 200, 0.7}};
    const qreal k = s.width() / 780.0;   // the look was set on a 780-unit-wide window
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

// draws the theme's pictures into the cache folder and returns the sheet that uses them
inline QString krimbleCyberStyleSheet()
{
    krimbleLoadCyberFonts();
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(dir);
    const QString menuFrame = dir + QStringLiteral("/krimble-cyber-menu.png");
    const QString rowFrame = dir + QStringLiteral("/krimble-cyber-row.png");
    const QString scan = dir + QStringLiteral("/krimble-cyber-scan.png");
    const QString back = dir + QStringLiteral("/krimble-cyber-back.png");
    krimbleCyberMenuFrame(QColor(6, 30, 70, 215), QColor(56, 225, 255, 90), 14, 3).save(menuFrame);
    krimbleCyberFrame(QColor(56, 225, 255, 70), QColor(0, 0, 0, 0), 8, 0).save(rowFrame);
    krimbleCyberScanlines().save(scan);
    if (!QFile::exists(back)) krimbleCyberBackground(QSize(1560, 720)).save(back);

    return QStringLiteral(R"KRIMBLECYBER(
/* Krimble Cyber */
QMenuBar { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d01a32, stop:0.55 #9a0f22, stop:1 #5e0714); border: none; font-family: "Orbitron"; font-weight: 600; }
QMenuBar::item { color: #ffe9ec; background: transparent; border: none; }
QMenuBar::item:selected, QMenuBar::item:pressed { background: rgb(6, 30, 70); color: #7ff0ff; border: none; }
QMenu { font-family: "Rajdhani"; font-weight: 600; color: #d8f6ff; background: transparent; border: none;
        border-image: url(%1) 0 22 22 22 stretch; border-width: 0px 10px 10px 10px; }
QMenu::item { background: transparent; color: #d8f6ff; }
QMenu::item:selected { color: #ffffff; background: transparent; border-image: url(%2) 10 10 10 10 stretch; }
QMenu::separator { background: rgba(56, 225, 255, 120); height: 1px; border: none; }
KisMainWindow, QMdiArea, KisWelcomePageWidget { background-color: #000000; background-image: url(%4); background-position: center; }
QDockWidget > QWidget { background-color: rgba(6, 30, 70, 150); background-image: url(%3); }
QDockWidget > KoDockWidgetTitleBar { background-image: none; }
QDockWidget#ColorSelectorNg > QWidget, QDockWidget#ToolBox > QWidget { background-image: none; }
)KRIMBLECYBER").arg(menuFrame, rowFrame, scan, back);
}
