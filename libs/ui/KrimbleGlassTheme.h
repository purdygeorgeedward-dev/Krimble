/*
 * Krimble 2026-11-23 (cyber-theme branch): the "Krimble Glass" theme, a mist-glass look.
 * George: "Could you build a theme based on Liquid Glass? Or Glassmorphism?" / "I'm pretty sure you could make a blur like texture and make it
 * transparent." / "Mist" / "Interesting. Not perfectly smooth." / "Can you build it for me?" / "Yes."
 * Look shown in the Preview Tool: glass3.png. Qt cannot blur what is behind a widget, so the frost is a soft, see-through mist texture.
 *
 * It is the Krimble Painted theme with this sheet added last:
 * - see-through panels and menus with the mist texture, a brighter top and left edge; menus keep the small cast shadow
 * - a see-through menu bar; an open title is lighter glass and joins its menu with no line
 * - a window background of soft colored light (seen through the menu bar and panels)
 * - Color Lab and the toolbox are left out of the panel look (Color Lab is protected; the toolbox keeps its own look)
 * The pictures are drawn once into the cache folder, like the Aqua pictures.
 */
#pragma once

#include <QColor>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QRadialGradient>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QString>

inline QImage krimbleGlassBlur(const QImage &src, int factor)
{
    // a soft blur: shrink smoothly, then grow back smoothly
    QImage s = src.scaled(src.size() / factor, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    s = s.scaled(s.size() / 2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).scaled(s.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    return s.scaled(src.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

// soft cloudy white that tiles without seams ("Not perfectly smooth": large shapes only, low contrast, blurred once more)
inline QImage krimbleGlassMist()
{
    const int S = 256;
    QImage acc(S, S, QImage::Format_ARGB32_Premultiplied);
    acc.fill(Qt::transparent);
    QRandomGenerator rg(5);
    for (int scale : {128, 64}) {
        QImage n(S / scale + 2, S / scale + 2, QImage::Format_ARGB32_Premultiplied);
        for (int y = 0; y < n.height(); ++y)
            for (int x = 0; x < n.width(); ++x) n.setPixelColor(x, y, QColor(255, 255, 255, 10 + rg.bounded(22)));
        for (int x = 0; x < n.width(); ++x) n.setPixelColor(x, n.height() - 1, n.pixelColor(x, 0));
        for (int y = 0; y < n.height(); ++y) n.setPixelColor(n.width() - 1, y, n.pixelColor(0, y));
        const QImage big = n.scaled(n.width() * scale, n.height() * scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).copy(scale / 2, scale / 2, S, S);
        QPainter p(&acc);
        p.setOpacity(0.5);
        p.drawImage(0, 0, big);
    }
    QImage tiled(S * 3, S * 3, QImage::Format_ARGB32_Premultiplied);
    tiled.fill(Qt::transparent);
    {
        QPainter p(&tiled);
        for (int y = 0; y < 3; ++y)
            for (int x = 0; x < 3; ++x) p.drawImage(x * S, y * S, acc);
    }
    return krimbleGlassBlur(tiled, 8).copy(S, S, S, S);
}

// soft colored light behind the glass
inline QImage krimbleGlassBackground(QSize s)
{
    QImage im(s, QImage::Format_ARGB32_Premultiplied);
    im.fill(QColor(20, 24, 40));
    QPainter p(&im);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    struct B { qreal x, y, r; QColor c; };
    const B blobs[] = {B{0.15, 0.3, 0.45, QColor(255, 120, 80)}, B{0.55, 0.75, 0.5, QColor(80, 140, 255)},
                       B{0.85, 0.25, 0.4, QColor(190, 90, 255)}, B{0.45, 0.2, 0.3, QColor(255, 200, 90)}};
    for (const B &b : blobs) {
        QRadialGradient g(QPointF(b.x * s.width(), b.y * s.height()), b.r * s.width());
        QColor c = b.c;
        g.setColorAt(0, c);
        c.setAlpha(0);
        g.setColorAt(1, c);
        p.setBrush(g);
        p.drawRect(im.rect());
    }
    p.end();
    return krimbleGlassBlur(im, 4);
}

// draws the theme's pictures into the cache folder and returns the sheet that uses them
inline QString krimbleGlassStyleSheet()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(dir);
    const QString mist = dir + QStringLiteral("/krimble-glass-mist.png");
    const QString back = dir + QStringLiteral("/krimble-glass-back.png");
    krimbleGlassMist().save(mist);
    if (!QFile::exists(back)) krimbleGlassBackground(QSize(1560, 720)).save(back);

    return QStringLiteral(R"KRIMBLEGLASS(
/* Krimble Glass */
KisMainWindow, QMdiArea, KisWelcomePageWidget { background-color: #141828; background-image: url(%2); background-position: center; }
QMenuBar { background: rgba(255, 255, 255, 40); border: none; }
QMenuBar::item { background: transparent; color: #ffffff; border: none; }
QMenuBar::item:selected, QMenuBar::item:pressed { background: rgba(255, 255, 255, 70); color: #ffffff; border: none; }
QMenu { background-color: rgba(60, 64, 90, 170); background-image: url(%1); color: #ffffff;
        border: 1px solid rgba(255, 255, 255, 50); border-top: none; border-left: 1px solid rgba(255, 255, 255, 90); }
QMenu::item { background: transparent; color: #ffffff; }
QMenu::item:selected { background: rgba(255, 255, 255, 60); color: #ffffff; }
QMenu::separator { background: rgba(255, 255, 255, 70); height: 1px; border: none; }
QDockWidget > QWidget { background-color: rgba(255, 255, 255, 30); background-image: url(%1);
                        border: 1px solid rgba(255, 255, 255, 50); border-top: 1px solid rgba(255, 255, 255, 140); border-left: 1px solid rgba(255, 255, 255, 90); }
QDockWidget > KoDockWidgetTitleBar { background-image: none; border: none; }
QDockWidget#ColorSelectorNg > QWidget, QDockWidget#ToolBox > QWidget { background-image: none; border: none; }
QDockWidget#ToolBox QWidget, QDockWidget#ColorSelectorNg QWidget { background-image: none; }
/* build 77: the docked panels kept the Painted gradient behind them, so they did not look see-through; now the window background shows
   through them. The toolbox and Color Lab keep the Painted gradient. */
QDockWidget { background: transparent; }
QDockWidget#ToolBox, QDockWidget#ColorSelectorNg { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #374267, stop:1 #181f34); }
)KRIMBLEGLASS").arg(mist, back);
}
