/*
 * Krimble 2026-11-23 (cyber-theme branch): the "Krimble Glass 2" theme, after the glassmorphism picture made by Chatty
 * (docs/krimble-refs/2026-11-23_1237_glassmorphism-reference-chatty.png).
 * George: "Chatty made this image of what Glassmorphism would look like." / "The under light effect under the selected menu title is nice." /
 * "I guess you could have a graphic layer below with the light effect under each title?" / "You could try to build it as Glass 2."
 *
 * It is the Krimble Painted theme with this sheet added last:
 * - dark blue frosted glass panels and menus with rounded corners and a bright rim along the top that fades down the sides
 *   (pictures used as border images: gradient borders drawn by Qt break up at rounded corners)
 * - menus are open at the top, so an open menu joins its title with no line
 * - the selected menu title is lit from below: one glow picture behind whichever title is selected
 * - selected menu rows are soft blue (rounded rows would change Qt's spacing of every row)
 * - a window background of colored bokeh lights over dark blue
 * - Color Lab and the toolbox are left out of the panel look (Color Lab is protected; the toolbox keeps its own look)
 * The pictures are drawn once into the cache folder, like the Aqua pictures.
 */
#pragma once

#include <QColor>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QString>

inline QImage krimbleGlass2Blur(const QImage &src, int factor)
{
    QImage s = src.scaled(src.size() / factor, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    s = s.scaled(s.size() / 2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).scaled(s.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    return s.scaled(src.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

// a glass pane: dark blue see-through fill (a little lighter at the top), a bright rim along the top fading down the sides,
// rounded corners. openTop: no rim and square corners at the top (a menu, which joins its title there).
inline QImage krimbleGlass2Pane(bool openTop)
{
    const int S = 96;
    const qreal R = 22;   // corner radius in the picture
    QImage im(S, S, QImage::Format_ARGB32_Premultiplied);
    im.fill(Qt::transparent);
    QPainter p(&im);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath shape;
    if (openTop) {
        shape.moveTo(0, 0); shape.lineTo(S, 0); shape.lineTo(S, S - R); shape.quadTo(S, S, S - R, S); shape.lineTo(R, S); shape.quadTo(0, S, 0, S - R); shape.closeSubpath();
    } else {
        shape.addRoundedRect(QRectF(0.5, 0.5, S - 1, S - 1), R, R);
    }
    QLinearGradient fill(0, 0, 0, S);
    fill.setColorAt(0, QColor(70, 90, 150, 175));
    fill.setColorAt(1, QColor(30, 40, 80, 165));
    p.fillPath(shape, fill);
    // the rim: bright at the top, fading to faint down the sides and along the bottom
    QLinearGradient rim(0, 0, 0, S);
    rim.setColorAt(0, QColor(255, 255, 255, openTop ? 60 : 210));
    rim.setColorAt(0.35, QColor(200, 220, 255, 70));
    rim.setColorAt(1, QColor(160, 190, 255, 40));
    QPainterPath edge = shape;
    if (openTop) {
        edge = QPainterPath();
        edge.moveTo(0.75, 0); edge.lineTo(0.75, S - R); edge.quadTo(0.75, S - 0.75, R, S - 0.75); edge.lineTo(S - R, S - 0.75); edge.quadTo(S - 0.75, S - 0.75, S - 0.75, S - R); edge.lineTo(S - 0.75, 0);
    }
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QBrush(rim), 1.5));
    p.drawPath(edge);
    return im;
}

// the light under a selected menu title: a soft blue glow rising from the bottom, fading toward the top
inline QImage krimbleGlass2TitleGlow()
{
    const int W = 96, H = 64;
    QImage im(W, H, QImage::Format_ARGB32_Premultiplied);
    im.fill(Qt::transparent);
    QPainter p(&im);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath shape;
    shape.addRoundedRect(QRectF(0, 0, W, H + 20), 18, 18);   // rounded at the top only (the bottom runs past the picture)
    QLinearGradient g(0, 0, 0, H);
    g.setColorAt(0, QColor(80, 140, 255, 40));
    g.setColorAt(0.6, QColor(70, 140, 255, 110));
    g.setColorAt(1, QColor(110, 190, 255, 200));
    p.fillPath(shape, g);
    QRadialGradient under(QPointF(W / 2.0, H), W * 0.6);
    under.setColorAt(0, QColor(170, 220, 255, 150));
    under.setColorAt(1, QColor(170, 220, 255, 0));
    p.setClipPath(shape);
    p.fillRect(im.rect(), under);
    p.setClipping(false);
    p.setPen(QPen(QColor(255, 255, 255, 120), 1.2));
    p.drawLine(QPointF(18, 0.6), QPointF(W - 18, 0.6));
    return im;
}

// colored bokeh lights over dark blue
inline QImage krimbleGlass2Background(QSize s)
{
    QImage im(s, QImage::Format_ARGB32_Premultiplied);
    QLinearGradient base(0, 0, 0, s.height());
    base.setColorAt(0, QColor(18, 22, 52));
    base.setColorAt(1, QColor(10, 12, 30));
    {
        QPainter p(&im);
        p.fillRect(im.rect(), base);
    }
    QImage lights(s, QImage::Format_ARGB32_Premultiplied);
    lights.fill(Qt::transparent);
    QPainter p(&lights);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    QRandomGenerator rg(23);
    const QColor tints[] = {QColor(120, 90, 255), QColor(80, 150, 255), QColor(255, 120, 200), QColor(255, 170, 90), QColor(90, 210, 255)};
    const qreal k = s.width() / 1560.0;   // lights sized for a phone screen (the picture is not scaled to the window)
    for (int i = 0; i < 70; ++i) {
        QColor c = tints[rg.bounded(5)];
        c.setAlpha(40 + rg.bounded(90));
        const qreal r = (8 + rg.bounded(40)) * k;
        p.setBrush(c);
        p.drawEllipse(QPointF(rg.bounded(s.width()), rg.bounded(s.height())), r, r);
    }
    p.end();
    QPainter o(&im);
    o.setCompositionMode(QPainter::CompositionMode_Plus);
    o.drawImage(0, 0, krimbleGlass2Blur(lights, 3));
    return im;
}

// draws the theme's pictures into the cache folder and returns the sheet that uses them
inline QString krimbleGlass2StyleSheet()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(dir);
    const QString pane = dir + QStringLiteral("/krimble-glass2-pane.png");
    const QString menuPane = dir + QStringLiteral("/krimble-glass2-menu.png");
    const QString glow = dir + QStringLiteral("/krimble-glass2-title.png");
    const QString back = dir + QStringLiteral("/krimble-glass2-back.png");
    krimbleGlass2Pane(false).save(pane);
    krimbleGlass2Pane(true).save(menuPane);
    krimbleGlass2TitleGlow().save(glow);
    if (!QFile::exists(back)) krimbleGlass2Background(QSize(1560, 720)).save(back);

    return QStringLiteral(R"KRIMBLEGLASS2(
/* Krimble Glass 2 */
QMainWindow, QMdiArea, KisWelcomePageWidget { background-color: #0e1230; background-image: url(%4); background-position: center; }
QMenuBar { background: rgba(40, 52, 100, 150); border: none; }
QMenuBar::item { background: transparent; color: #ffffff; border: none; }
QMenuBar::item:selected, QMenuBar::item:pressed { background: transparent; color: #ffffff; border: none;
        border-image: url(%3) 18 18 0 18 stretch; }
QMenu { background: transparent; color: #ffffff; border: none; border-image: url(%2) 0 24 24 24 stretch; border-width: 0px 6px 6px 6px; }
QMenu::item { background: transparent; color: #ffffff; }
QMenu::item:selected { background: rgba(90, 160, 255, 90); color: #ffffff; }
QMenu::separator { background: rgba(200, 220, 255, 60); height: 1px; border: none; }
QDockWidget > QWidget { background: transparent; border-image: url(%1) 24 24 24 24 stretch; border-width: 6px; }
QDockWidget > KoDockWidgetTitleBar { border-image: none; border-width: 0px; }
QDockWidget#ColorSelectorNg > QWidget, QDockWidget#ToolBox > QWidget { border-image: none; border-width: 0px; }
)KRIMBLEGLASS2").arg(pane, menuPane, glow, back);
}
