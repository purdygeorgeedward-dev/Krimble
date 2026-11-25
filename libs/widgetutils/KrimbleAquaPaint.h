// SPDX-License-Identifier: GPL-2.0-or-later
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

    QLinearGradient g(r.topLeft(), r.bottomLeft());
    g.setColorAt(0.00, krimbleAquaMix(base, black, 0.30));
    g.setColorAt(0.12, krimbleAquaMix(base, black, 0.12));
    g.setColorAt(0.50, krimbleAquaMix(base, black, 0.05));
    g.setColorAt(0.85, krimbleAquaMix(base, white, 0.14));
    g.setColorAt(1.00, krimbleAquaMix(base, white, 0.24));
    p.fillPath(body, g);

    p.setClipPath(body);

    {
        QRadialGradient glow(QPointF(0, 0), 1.0);
        const QColor c = krimbleAquaMix(base, white, 0.75);
        glow.setColorAt(0.0, QColor(c.red(), c.green(), c.blue(), 85));
        glow.setColorAt(0.5, QColor(c.red(), c.green(), c.blue(), 28));
        glow.setColorAt(1.0, QColor(c.red(), c.green(), c.blue(), 0));
        QTransform t;
        t.translate(r.center().x(), r.top() + r.height() * 1.02);
        t.scale(r.width() * 0.55, r.height() * 0.55);
        QBrush brush(glow);
        brush.setTransform(t);
        p.fillRect(r, brush);
    }

    {
        const qreal rim = qMax<qreal>(1.0, qMin(r.width(), r.height()) * 0.012);
        QPen pen(krimbleAquaMix(base, black, 0.40), rim * 2);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawPath(body);
        p.setPen(Qt::NoPen);
    }

    {
        const qreal h = r.height();
        const qreal c = qMin(0.30 * h, r.width() / 2.0);
        const QColor d = krimbleAquaMix(base, black, 0.50);
        for (int side = 0; side < 2; ++side) {
            const qreal x = side == 0 ? r.left() : r.right() - c;
            const QRectF sq(x, r.bottom() - c, c, c);
            const QPointF centre(side == 0 ? r.left() + c : r.right() - c, r.bottom() - c);
            QPainterPath wedge;
            wedge.addRect(sq);
            QPainterPath round;
            round.addEllipse(centre, c, c);
            wedge = wedge.subtracted(round).intersected(body);
            QRadialGradient rg(side == 0 ? QPointF(r.left(), r.bottom()) : QPointF(r.right(), r.bottom()), c);
            rg.setColorAt(0.0, QColor(d.red(), d.green(), d.blue(), 230));
            rg.setColorAt(1.0, QColor(d.red(), d.green(), d.blue(), 230 / 3));
            p.fillPath(wedge, rg);
        }
    }

    {
        const qreal h = r.height();
        const qreal inset = qMin(r.width(), h) * 0.08;
        const qreal gt = r.top() + h * 0.05, gb = r.top() + h * 0.50;
        const qreal L = r.left() + inset, R = r.right() - inset;
        const qreal sh = qMin(0.16 * h, (R - L) / 2.0);
        const qreal narrow = qMin(0.38 * h, (R - L) / 4.0);
        QPainterPath gloss;
        gloss.moveTo(L + sh, gt);
        gloss.lineTo(R - sh, gt);
        gloss.cubicTo(R - sh * 0.35, gt, R, gt + sh * 0.25, R, gt + sh * 0.65);
        gloss.cubicTo(R, gt + (gb - gt) * 0.85, R - narrow * 0.6, gb, R - narrow - sh * 0.4, gb);
        gloss.lineTo(L + narrow + sh * 0.4, gb);
        gloss.cubicTo(L + narrow * 0.6, gb, L, gt + (gb - gt) * 0.85, L, gt + sh * 0.65);
        gloss.cubicTo(L, gt + sh * 0.25, L + sh * 0.35, gt, L + sh, gt);
        gloss.closeSubpath();
        QLinearGradient gg(QPointF(0, gt), QPointF(0, gb));
        gg.setColorAt(0.0, QColor(255, 255, 255, 150));
        gg.setColorAt(1.0, QColor(255, 255, 255, 60));
        p.fillPath(gloss, gg);
    }
    p.restore();
}

inline void krimblePaintSelectedGlow(QPainter &p, const QRectF &outer, qreal dpr)
{
    const qreal real = 1.0 / qMax<qreal>(1.0, dpr);
    const QRectF r = outer.adjusted(4.0 * real, 4.0 * real, -4.0 * real, -4.0 * real);
    if (r.width() < 2 || r.height() < 2) return;
    const qreal radius = qMin<qreal>(12.0 * real, qMin(r.width(), r.height()) / 2.0);
    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);
    for (int i = 4; i >= 1; --i) {
        QColor c(70, 175, 225);
        c.setAlpha(18 + (4 - i) * 22);
        p.setBrush(c);
        p.drawRoundedRect(r.adjusted(-i * real, -i * real, i * real, i * real), radius + i * real, radius + i * real);
    }
    QLinearGradient face(r.topLeft(), r.bottomLeft());
    face.setColorAt(0.0, QColor(13, 135, 198));
    face.setColorAt(0.49, QColor(9, 99, 157));
    face.setColorAt(0.5, QColor(7, 65, 115));
    face.setColorAt(1.0, QColor(0, 77, 129));
    p.setBrush(face);
    p.setPen(QPen(QColor(113, 226, 255), 2.0 * real));
    p.drawRoundedRect(r, radius, radius);
    p.restore();
}

inline void krimblePaintBevelButton(QPainter &p, const QRectF &outer, qreal dpr, const QColor &window, bool down)
{
    const qreal real = 1.0 / qMax<qreal>(1.0, dpr);
    const QRectF r = outer.adjusted(4.0 * real, 4.0 * real, -4.0 * real, -4.0 * real);
    if (r.width() < 2 || r.height() < 2) return;
    const qreal radius = qMin<qreal>(12.0 * real, qMin(r.width(), r.height()) / 2.0);
    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);
    QLinearGradient face(r.topLeft(), r.bottomLeft());
    face.setColorAt(0.0, down ? window.darker(190) : window.darker(127));
    face.setColorAt(1.0, down ? window.darker(127) : window.darker(190));
    p.setBrush(face);
    p.setPen(QPen(window.darker(400), 2.0 * real));
    p.drawRoundedRect(r, radius, radius);
    p.setBrush(Qt::NoBrush);
    const QRectF in = r.adjusted(2.5 * real, 2.5 * real, -2.5 * real, -2.5 * real);
    QPainterPath top;
    top.moveTo(in.left(), in.top() + radius * 0.6);
    top.quadTo(in.left(), in.top(), in.left() + radius * 0.6, in.top());
    top.lineTo(in.right() - radius * 0.6, in.top());
    p.setPen(QPen(down ? window.darker(250) : window.lighter(140), 2.0 * real));
    p.drawPath(top);
    p.restore();
}
