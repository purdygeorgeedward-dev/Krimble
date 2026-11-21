// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble: the Aqua-style selection look (reference: docs/krimble-refs/2026-11-20_2311_macos-aqua-gradient-reference.png,
// approved demo: docs/krimble-refs/2026-11-20_2312_APPROVED-aqua-gradient-demo.png), in the selection blue #0b87dd.
// Four layers: a blue body darkest at the top edge and deep through the middle, a bright glow rising from the bottom,
// a thin darker rim, and a white gloss over the top half that fades as it goes down.
#pragma once
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QColor>
#include <QRectF>
#include <QtMath>

inline QColor krimbleAquaMix(const QColor &c, const QColor &to, qreal a, int alpha = 255)
{
    return QColor(qRound(c.red() + (to.red() - c.red()) * a),
                  qRound(c.green() + (to.green() - c.green()) * a),
                  qRound(c.blue() + (to.blue() - c.blue()) * a), alpha);
}

// A glossy Aqua rim for outlines: light at the top, the selection blue in the middle, deep at the bottom.
inline QBrush krimbleAquaRimBrush(const QRectF &r)
{
    QLinearGradient g(r.topLeft(), r.bottomLeft());
    g.setColorAt(0.0, QColor(0x9b, 0xd6, 0xff));
    g.setColorAt(0.5, QColor(0x0b, 0x87, 0xdd));
    g.setColorAt(1.0, QColor(0x07, 0x5a, 0x96));
    return QBrush(g);
}

inline void krimblePaintAqua(QPainter &p, const QRectF &r, qreal radius, const QColor &base = QColor(11, 135, 221))
{
    if (r.width() < 2 || r.height() < 2) return;
    const QColor black(0, 0, 0), white(255, 255, 255);
    radius = qMin(radius, qMin(r.width(), r.height()) / 2.0);
    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);

    QPainterPath body;
    body.addRoundedRect(r, radius, radius);

    // 1. the body
    QLinearGradient g(r.topLeft(), r.bottomLeft());
    g.setColorAt(0.00, krimbleAquaMix(base, black, 0.30));
    g.setColorAt(0.12, krimbleAquaMix(base, black, 0.12));
    g.setColorAt(0.50, krimbleAquaMix(base, black, 0.05));
    g.setColorAt(0.85, krimbleAquaMix(base, white, 0.35));
    g.setColorAt(1.00, krimbleAquaMix(base, white, 0.50));
    p.fillPath(body, g);

    p.setClipPath(body);

    // 2. the glow rising from the bottom (an ellipse 1.1 x the width and the height, centered just under the bottom edge)
    {
        QRadialGradient glow(QPointF(0, 0), 1.0);
        const QColor c = krimbleAquaMix(base, white, 0.75);
        glow.setColorAt(0.0, QColor(c.red(), c.green(), c.blue(), 140));
        glow.setColorAt(0.5, QColor(c.red(), c.green(), c.blue(), 50));
        glow.setColorAt(1.0, QColor(c.red(), c.green(), c.blue(), 0));
        QTransform t;
        t.translate(r.center().x(), r.top() + r.height() * 1.02);
        t.scale(r.width() * 0.55, r.height() * 0.55);
        QBrush brush(glow);
        brush.setTransform(t);
        p.fillRect(r, brush);
    }

    // 3. the rim
    {
        const qreal rim = qMax<qreal>(1.0, qMin(r.width(), r.height()) * 0.012);
        QPen pen(krimbleAquaMix(base, black, 0.40), rim * 2);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawPath(body);   // half of the pen is clipped away: a rim of 'rim' inside the edge
        p.setPen(Qt::NoPen);
    }

    // 4. the gloss over the top half
    {
        const QRectF gr(r.left() + r.width() * 0.07, r.top() + r.height() * 0.05, r.width() * 0.86, r.height() * 0.45);
        const qreal gRadius = qMin(radius * 0.85, gr.height() / 2.0);
        QPainterPath gloss;
        gloss.addRoundedRect(gr, gRadius, gRadius);
        QLinearGradient gg(gr.topLeft(), gr.bottomLeft());
        gg.setColorAt(0.0, QColor(255, 255, 255, 217));
        gg.setColorAt(1.0, QColor(255, 255, 255, 31));
        p.fillPath(gloss, gg);
    }
    p.restore();
}
