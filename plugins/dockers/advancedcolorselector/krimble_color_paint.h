// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-08: the shared base of the pages of the Color panel (Picker, Sliders, Values, Palette, History): a page is a card of 1178 design units wide (the mockups' card, scaled
// to the same width) that places its parts at the measured rectangles times the scale (the width / 1178), and paints with the same frames, wells, lines and symbols.
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

class KrimbleColorBase : public QWidget
{
public:
    static constexpr double DW = 1178.0;   // the card's width in design units
    explicit KrimbleColorBase(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual double designHeight() const = 0;
    bool hasHeightForWidth() const override { return true; }
    int heightForWidth(int w) const override { return qRound(w * designHeight() / DW); }
    qreal scale() const { return width() / DW; }
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
    // the largest font (up to the base size) in which the text is not wider than the width
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
    // a raised button frame: a gradient from lighter to darker, a dark outline and a light line along the inside top
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
    // a dark inset field (number boxes, HEX, the name of the palette): dark at the top, a lighter outline, a light line along the bottom edge
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
        // p.setPen(QPen(QColor("#4a557b"), 1.3));
        p.setPen(QPen(QColor("#5b6ea6"), 1.5));   // 2026-11-08: the rim measured in the Sliders mockup is #6175B0
        p.drawRoundedRect(b, radius, radius);
        p.setPen(QPen(QColor(6, 8, 18, 210), 1.2));
        p.drawLine(QPointF(b.left() + radius, b.top() + 1.6), QPointF(b.right() - radius, b.top() + 1.6));
    }
    // a recessed well (recent colors, palette): very dark, a dark outline and a light line along the bottom edge
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
    // a dark raised button (the model buttons of the Sliders page, measured): a bright line along the top, a lighter line along the bottom, dark in between
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
    // a number box with its arrow section (the right 38 units): the field, a lighter section with a divider, and the up and down arrows
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
    // a divider of the groups (measured in the Sliders mockup: a bright line with a dark one under it)
    static void paintBrightLine(QPainter &p, double x0, double y, double x1, qreal s)
    {
        p.setPen(QPen(QColor(120, 138, 192, 200), 1.6));
        p.drawLine(QPointF(x0 * s, y * s), QPointF(x1 * s, y * s));
        p.setPen(QPen(QColor(8, 11, 26, 190), 1.2));
        p.drawLine(QPointF(x0 * s, y * s + 1.8), QPointF(x1 * s, y * s + 1.8));
    }
    // a checker pattern (transparency): two grays, square cells, clipped to a rounded rectangle
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
    // a titled group: a frame a little darker than the card, a bright line along its top edge, a lighter left edge
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
    // the look of a selected item: a deep blue fill, a bright cyan outline and a glow (measured in the mockups' selected buttons and tabs)
    static void paintSelected(QPainter &p, const QRectF &rect, qreal radius, qreal s)
    {
        for (int i = 6; i >= 1; --i) {
            p.setPen(QPen(QColor(40, 190, 255, 20 + (6 - i) * 9), 2.0));
            p.setBrush(Qt::NoBrush);
            p.drawRoundedRect(rect.adjusted(-i * 1.1 * s, -i * 1.1 * s, i * 1.1 * s, i * 1.1 * s), radius + i * s, radius + i * s);
        }
        const QRectF b = rect.adjusted(0.5, 0.5, -0.5, -0.5);
        QLinearGradient g(0, b.top(), 0, b.bottom());
        g.setColorAt(0.0, QColor("#1196d8"));
        g.setColorAt(0.12, QColor("#0a78bb"));
        g.setColorAt(0.55, QColor("#073d6b"));
        g.setColorAt(1.0, QColor("#074a85"));
        p.setPen(Qt::NoPen);
        p.setBrush(g);
        p.drawRoundedRect(b, radius, radius);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("#6fe3ff"), 2.4 * qMax<qreal>(0.5, s * 2)));
        p.drawRoundedRect(b, radius, radius);
    }
    // a pill that keeps the color and the glow of the item it shows (the preview boxes): a colored rounded square with the cyan outline and its glow
    static void paintColorBox(QPainter &p, const QRectF &rect, const QColor &c, qreal radius, qreal s, bool glow)
    {
        // Krimble 2026-11-13 (task 53): George: "In the ColorLab panel the left color well had a weird blue box around it. Remove that. No color crap around the color wells. no glow." The preview boxes (the wells of the pages) have no glow and no outline now; swatch cells (glow false) keep their outline.
        if (false && glow) {
            for (int i = 9; i >= 1; --i) {
                p.setPen(QPen(QColor(40, 190, 255, 26 + (9 - i) * 8), 2.2));
                p.setBrush(Qt::NoBrush);
                p.drawRoundedRect(rect.adjusted(-i * 1.3 * s, -i * 1.3 * s, i * 1.3 * s, i * 1.3 * s), radius + i * s, radius + i * s);
            }
        }
        const QRectF b = rect.adjusted(0.5, 0.5, -0.5, -0.5);
        p.setPen(Qt::NoPen);
        p.setBrush(c);
        p.drawRoundedRect(b, radius, radius);
        p.setBrush(Qt::NoBrush);
        if (!glow) {   // Krimble 2026-11-13 (task 53): George: "In the ColorLab panel the left color well had a weird blue box around it. Remove that. No color crap around the color wells. no glow."
            p.setPen(QPen(QColor("#e6ecfa"), qMax<qreal>(2.0, 3.0 * s)));
            p.drawRoundedRect(b, radius, radius);
        }
        // p.setPen(QPen(glow ? QColor("#6fe3ff") : QColor("#e6ecfa"), qMax<qreal>(2.0, 3.0 * s)));
        // p.drawRoundedRect(b, radius, radius);
    }
    // the icon pictures (colored, from krimble_color_icons.h) are drawn at the mockup's own size times the scale
    void drawIcon(QPainter &p, const QString &name, const QPointF &center, qreal factor = 1.0) const
    {
        const QPixmap pm = krimbleStudioIcon(name);
        if (pm.isNull()) return;
        const qreal s = scale() * factor;
        const QSizeF size(pm.width() * s, pm.height() * s);
        p.drawPixmap(QRectF(center.x() - size.width() / 2, center.y() - size.height() / 2, size.width(), size.height()), pm, QRectF(pm.rect()));
    }
};
