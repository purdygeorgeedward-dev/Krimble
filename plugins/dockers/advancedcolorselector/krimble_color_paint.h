// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QPixmap>
#include <QToolButton>
#include <QString>
#include <QSizePolicy>
#include <QFontMetricsF>
#include "krimble_color_symbols.h"
#include "krimble_color_icons.h"
#include "KrimbleAquaPaint.h"

inline void krimbleSetSheet(QWidget *w, const QString &sheet)
{
    if (w && w->styleSheet() != sheet) w->setStyleSheet(sheet);
}

class KrimbleColorBase : public QWidget
{
public:
    static constexpr double DW = 1178.0;
    static constexpr double MinWidth = 330.0;
    explicit KrimbleColorBase(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual double designHeight() const = 0;
    bool hasHeightForWidth() const override { return true; }
    static constexpr double MaxWidth = 384.0;
    int heightForWidth(int w) const override { return qRound(qBound<double>(MinWidth, w, MaxWidth) * designHeight() / DW); }
    qreal scale() const { return qBound<qreal>(MinWidth, width(), MaxWidth) / DW; }
    QSize sizeHint() const override { return QSize(330, heightForWidth(330)); }
    QSize minimumSizeHint() const override { return QSize(240, heightForWidth(240)); }

protected:
    QToolButton *makeButton()
    {
        auto *b = new QToolButton(this);
        b->setAutoRaise(true);
        b->setFocusPolicy(Qt::NoFocus);
        b->setStyleSheet(QStringLiteral("QToolButton { background: transparent; border: none; }"));
        return b;
    }
    QRectF r(double x, double y, double w, double h) const
    {
        const qreal s = scale();
        return QRectF(x * s, y * s, w * s, h * s);
    }
    static QFont fittedFont(const QFont &base, const QString &text, qreal width)
    {
        QFont f = base;
        int px = f.pixelSize() > 0 ? f.pixelSize() : 12;
        while (px > 8) {
            f.setPixelSize(px);
            if (QFontMetricsF(f).horizontalAdvance(text) <= width) break;
            --px;
        }
        f.setPixelSize(qMax(8, px));
        return f;
    }
    void drawSymbol(QPainter &p, const QString &name, const QPointF &center, qreal factor)
    {
        const QPixmap pm = krimbleStudioSymbol(name);
        if (pm.isNull()) return;
        const qreal s = scale() * factor;
        const QSizeF size(pm.width() * s, pm.height() * s);
        p.drawPixmap(QRectF(center.x() - size.width() / 2, center.y() - size.height() / 2, size.width(), size.height()), pm, QRectF(pm.rect()));
    }
    void paintLine(QPainter &p, double x0, double y, double x1, qreal s)
    {
        p.setPen(QPen(QColor(8, 11, 26, 200), 1.2));
        p.drawLine(QPointF(x0 * s, y * s), QPointF(x1 * s, y * s));
        p.setPen(QPen(QColor(110, 125, 175, 95), 1.0));
        p.drawLine(QPointF(x0 * s, y * s + 1.6), QPointF(x1 * s, y * s + 1.6));
    }
    static void paintButton(QPainter &p, const QRectF &rect, qreal radius)
    {
        const QRectF b = rect.adjusted(0.5, 0.5, -0.5, -0.5);
        QLinearGradient g(0, b.top(), 0, b.bottom());
        g.setColorAt(0.0, QColor("#3a4568"));
        g.setColorAt(0.5, QColor("#2c3555"));
        g.setColorAt(1.0, QColor("#212a46"));
        p.setPen(Qt::NoPen);
        p.setBrush(g);
        p.drawRoundedRect(b, radius, radius);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("#0b0f20"), 1.3));
        p.drawRoundedRect(b, radius, radius);
        QLinearGradient hl(0, b.top(), 0, b.bottom());
        hl.setColorAt(0.0, QColor(170, 185, 235, 170));
        hl.setColorAt(0.3, QColor(120, 135, 190, 40));
        hl.setColorAt(1.0, QColor(0, 0, 0, 0));
        p.setPen(QPen(QBrush(hl), 1.2));
        p.drawRoundedRect(b.adjusted(1.6, 1.6, -1.6, -1.6), qMax<qreal>(1.0, radius - 1.6), qMax<qreal>(1.0, radius - 1.6));
    }
    static void paintField(QPainter &p, const QRectF &rect, qreal radius)
    {
        const QRectF b = rect.adjusted(0.5, 0.5, -0.5, -0.5);
        QLinearGradient g(0, b.top(), 0, b.bottom());
        g.setColorAt(0.0, QColor("#141724"));
        g.setColorAt(1.0, QColor("#1f2333"));
        p.setPen(Qt::NoPen);
        p.setBrush(g);
        p.drawRoundedRect(b, radius, radius);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("#5b6ea6"), 1.5));
        p.drawRoundedRect(b, radius, radius);
        p.setPen(QPen(QColor(6, 8, 18, 210), 1.2));
        p.drawLine(QPointF(b.left() + radius, b.top() + 1.6), QPointF(b.right() - radius, b.top() + 1.6));
    }
    static void paintWell(QPainter &p, const QRectF &rect, qreal radius)
    {
        const QRectF b = rect.adjusted(0.5, 0.5, -0.5, -0.5);
        QLinearGradient g(0, b.top(), 0, b.bottom());
        g.setColorAt(0.0, QColor("#04050e"));
        g.setColorAt(1.0, QColor("#0a0e1c"));
        p.setPen(Qt::NoPen);
        p.setBrush(g);
        p.drawRoundedRect(b, radius, radius);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("#394568"), 1.4));
        p.drawRoundedRect(b, radius, radius);
        p.setPen(QPen(QColor(110, 125, 185, 130), 1.2));
        p.drawLine(QPointF(b.left() + radius, b.bottom() + 0.2), QPointF(b.right() - radius, b.bottom() + 0.2));
    }

public:
    static void paintDarkButton(QPainter &p, const QRectF &rect, qreal radius)
    {
        const QRectF b = rect.adjusted(0.5, 0.5, -0.5, -0.5);
        QLinearGradient g(0, b.top(), 0, b.bottom());
        g.setColorAt(0.0, QColor("#2b3550"));
        g.setColorAt(0.5, QColor("#222b43"));
        g.setColorAt(1.0, QColor("#1c2439"));
        p.setPen(Qt::NoPen);
        p.setBrush(g);
        p.drawRoundedRect(b, radius, radius);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("#0d1223"), 1.4));
        p.drawRoundedRect(b, radius, radius);
        QLinearGradient hl(0, b.top(), 0, b.bottom());
        hl.setColorAt(0.0, QColor(114, 131, 183, 220));
        hl.setColorAt(0.2, QColor(114, 131, 183, 40));
        hl.setColorAt(0.8, QColor(80, 96, 150, 0));
        hl.setColorAt(1.0, QColor(80, 96, 150, 150));
        p.setPen(QPen(QBrush(hl), 1.4));
        p.drawRoundedRect(b.adjusted(1.5, 1.5, -1.5, -1.5), qMax<qreal>(1.0, radius - 1.5), qMax<qreal>(1.0, radius - 1.5));
    }
    void paintSpinFrame(QPainter &p, const QRectF &box, double arrowUnits = 38.0)
    {
        const qreal s = scale();
        paintField(p, box, 9 * s);
        const QRectF arrows(box.right() - arrowUnits * s, box.top(), arrowUnits * s, box.height());
        QLinearGradient ag(0, arrows.top(), 0, arrows.bottom());
        ag.setColorAt(0.0, QColor("#343d5e"));
        ag.setColorAt(1.0, QColor("#232b47"));
        p.setPen(Qt::NoPen);
        p.setBrush(ag);
        QPainterPath ap;
        ap.addRoundedRect(arrows.adjusted(0.5, 1.5, -1.5, -1.5), 7 * s, 7 * s);
        p.drawPath(ap);
        p.setPen(QPen(QColor(10, 14, 30, 210), 1.0));
        p.drawLine(QPointF(arrows.left(), arrows.top() + 3 * s), QPointF(arrows.left(), arrows.bottom() - 3 * s));
        p.setPen(QPen(QColor(8, 10, 24, 190), 1.0));
        p.drawLine(QPointF(arrows.left() + 2 * s, arrows.center().y()), QPointF(arrows.right() - 3 * s, arrows.center().y()));
        drawSymbol(p, QStringLiteral("spin_up"), QPointF(arrows.center().x(), arrows.top() + arrows.height() * 0.26), 1.0);
        drawSymbol(p, QStringLiteral("spin_down"), QPointF(arrows.center().x(), arrows.top() + arrows.height() * 0.74), 1.0);
    }
    static void paintBrightLine(QPainter &p, double x0, double y, double x1, qreal s)
    {
        p.setPen(QPen(QColor(120, 138, 192, 200), 1.6));
        p.drawLine(QPointF(x0 * s, y * s), QPointF(x1 * s, y * s));
        p.setPen(QPen(QColor(8, 11, 26, 190), 1.2));
        p.drawLine(QPointF(x0 * s, y * s + 1.8), QPointF(x1 * s, y * s + 1.8));
    }
    static void paintChecker(QPainter &p, const QRectF &rect, qreal cell, qreal radius)
    {
        p.save();
        QPainterPath clip;
        clip.addRoundedRect(rect, radius, radius);
        p.setClipPath(clip);
        p.fillRect(rect, QColor("#f2f2f2"));
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#c4c4c8"));
        const int cols = int(rect.width() / cell) + 2;
        const int rows = int(rect.height() / cell) + 2;
        for (int y = 0; y < rows; ++y)
            for (int x = 0; x < cols; ++x)
                if ((x + y) % 2 == 0) p.drawRect(QRectF(rect.left() + x * cell, rect.top() + y * cell, cell, cell));
        p.restore();
    }
    static void paintGroup(QPainter &p, const QRectF &rect, qreal radius)
    {
        const QRectF b = rect.adjusted(0.5, 0.5, -0.5, -0.5);
        QLinearGradient g(0, b.top(), 0, b.bottom());
        g.setColorAt(0.0, QColor("#222c47"));
        g.setColorAt(1.0, QColor("#192238"));
        p.setPen(Qt::NoPen);
        p.setBrush(g);
        p.drawRoundedRect(b, radius, radius);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("#10172a"), 1.4));
        p.drawRoundedRect(b, radius, radius);
        QLinearGradient hl(b.left(), b.top(), b.right(), b.top());
        hl.setColorAt(0.0, QColor(110, 126, 180, 0));
        hl.setColorAt(0.15, QColor(110, 126, 180, 190));
        hl.setColorAt(0.85, QColor(110, 126, 180, 190));
        hl.setColorAt(1.0, QColor(110, 126, 180, 0));
        p.setPen(QPen(QBrush(hl), 1.6));
        p.drawLine(QPointF(b.left() + radius, b.top() + 1.4), QPointF(b.right() - radius, b.top() + 1.4));
        p.setPen(QPen(QColor(80, 94, 140, 150), 1.2));
        p.drawLine(QPointF(b.left() + 1.0, b.top() + radius), QPointF(b.left() + 1.0, b.bottom() - radius));
    }
    static void paintSelected(QPainter &p, const QRectF &rect, qreal radius, qreal s)
    {
        Q_UNUSED(s);
        krimblePaintAqua(p, rect.adjusted(0.5, 0.5, -0.5, -0.5), radius);
    }
    static void paintColorBox(QPainter &p, const QRectF &rect, const QColor &c, qreal radius, qreal s, bool glow)
    {
        const QRectF b = rect.adjusted(0.5, 0.5, -0.5, -0.5);
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        p.drawRoundedRect(b, radius, radius);
        p.setBrush(Qt::NoBrush);
        if (!glow) {
            p.setPen(QPen(QColor("#e6ecfa"), qMax<qreal>(2.0, 3.0 * s)));
            p.drawRoundedRect(b, radius, radius);
        }
    }
    void drawIcon(QPainter &p, const QString &name, const QPointF &center, qreal factor = 1.0) const
    {
        const QPixmap pm = krimbleStudioIcon(name);
        if (pm.isNull()) return;
        const qreal s = scale() * factor;
        const QSizeF size(pm.width() * s, pm.height() * s);
        p.drawPixmap(QRectF(center.x() - size.width() / 2, center.y() - size.height() / 2, size.width(), size.height()), pm, QRectF(pm.rect()));
    }
};
