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
