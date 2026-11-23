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
 * - a window background of soft colored haze over dark blue
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
    // QLinearGradient fill(0, 0, 0, S);
    // fill.setColorAt(0, QColor(70, 90, 150, 175));
    // fill.setColorAt(1, QColor(30, 40, 80, 165));
    // George: "It doesn't look much like the mockups at all." Lighter frost, like the reference: pale blue at the top, deeper blue below
    QLinearGradient fill(0, 0, 0, S);
    fill.setColorAt(0, QColor(120, 140, 210, 150));
    fill.setColorAt(0.5, QColor(70, 86, 160, 135));
    fill.setColorAt(1, QColor(50, 60, 125, 140));
    p.fillPath(shape, fill);
    if (!openTop) {
        // a soft glossy band just inside the top rim
        QLinearGradient gloss(0, 0, 0, S * 0.35);
        gloss.setColorAt(0, QColor(255, 255, 255, 55));
        gloss.setColorAt(1, QColor(255, 255, 255, 0));
        p.save(); p.setClipPath(shape); p.fillRect(QRectF(0, 0, S, S * 0.35), gloss); p.restore();
    }
    // the rim: bright at the top, fading to faint down the sides and along the bottom
    QLinearGradient rim(0, 0, 0, S);
    rim.setColorAt(0, QColor(255, 255, 255, openTop ? 90 : 240));
    rim.setColorAt(0.4, QColor(210, 225, 255, 110));
    rim.setColorAt(1, QColor(170, 195, 255, 70));
    QPainterPath edge = shape;
    if (openTop) {
        edge = QPainterPath();
        edge.moveTo(0.75, 0); edge.lineTo(0.75, S - R); edge.quadTo(0.75, S - 0.75, R, S - 0.75); edge.lineTo(S - R, S - 0.75); edge.quadTo(S - 0.75, S - 0.75, S - 0.75, S - R); edge.lineTo(S - 0.75, 0);
    }
    p.setBrush(Qt::NoBrush);
    // p.setPen(QPen(QBrush(rim), 1.5));
    p.setPen(QPen(QBrush(rim), 4.0));   // the picture is shrunk about 4 times on the screen: this is about one unit wide there
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

// soft colored haze over dark blue, like the reference picture ("Those balls have to go.")
inline QImage krimbleGlass2Background(QSize s)
{
    QImage im(s, QImage::Format_ARGB32_Premultiplied);
    QLinearGradient base(0, 0, 0, s.height());
    base.setColorAt(0, QColor(20, 24, 58));
    base.setColorAt(1, QColor(12, 14, 36));
    QPainter p(&im);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(im.rect(), base);
    p.setPen(Qt::NoPen);
    struct Haze { qreal x, y, rx, ry; QColor c; };
    const Haze hazes[] = {Haze{0.08, 0.85, 0.35, 0.5, QColor(120, 70, 220, 150)}, Haze{0.35, 0.25, 0.3, 0.45, QColor(60, 110, 230, 120)},
                          Haze{0.62, 0.9, 0.35, 0.4, QColor(220, 90, 170, 110)}, Haze{0.88, 0.35, 0.3, 0.5, QColor(90, 150, 255, 120)},
                          Haze{0.5, 0.55, 0.25, 0.3, QColor(255, 150, 90, 70)}};
    for (const Haze &h : hazes) {
        const QPointF c(h.x * s.width(), h.y * s.height());
        QRadialGradient g(c, h.rx * s.width());
        QColor col = h.c;
        g.setColorAt(0, col);
        col.setAlpha(0);
        g.setColorAt(1, col);
        p.save();
        p.translate(c); p.scale(1.0, h.ry * s.height() / (h.rx * s.width())); p.translate(-c);
        p.setBrush(g);
        p.drawEllipse(c, h.rx * s.width(), h.rx * s.width());
        p.restore();
    }
    p.end();
    return krimbleGlass2Blur(im, 3);
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
QMenuBar { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(120, 140, 210, 150), stop:1 rgba(60, 74, 140, 140)); border: none; }
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
