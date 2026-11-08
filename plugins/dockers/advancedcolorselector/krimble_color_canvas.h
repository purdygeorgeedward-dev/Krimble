// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-08: the Color panel's Picker page, built from the MEASUREMENTS of the mockup (color_panel_measurements.md; George: "Measure everything for proportions. Don't
// wing it. Sizing and scale matter." and "Remember this app asks the user to scale it as a percentage. Issues of scale are not a problem."). The canvas is the mockup's card:
// 1178 x 1100 design units (the mockup's own pixels). Every part is placed at its measured rectangle times the scale (the panel's width / 1178), so the proportions are exact at any
// size, and the symbols are the mockup's own (krimble_color_symbols.h). The color square, hue strip, number boxes, HEX box, color boxes, recent colors and palette cells are real
// widgets of krimble_color_studio.h; the canvas paints the frames, wells, labels and symbols under and around them.
#pragma once
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QPixmap>
#include <QToolButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QVector>
#include <QString>
#include <QSizePolicy>
#include <QPaintEvent>
#include <QResizeEvent>
#include "krimble_color_symbols.h"

class KrimbleColorCanvas : public QWidget
{
public:
    static constexpr double DW = 1178.0;   // the card's width in design units
    static constexpr double DH = 1100.0;   // the card's height

    // the parts the color studio hands over (all children of this canvas)
    QWidget *square = nullptr;
    QWidget *hue = nullptr;
    QSpinBox *hsv[3] = {nullptr, nullptr, nullptr};
    QSpinBox *rgb[3] = {nullptr, nullptr, nullptr};
    QLineEdit *hex = nullptr;
    QLabel *fg = nullptr;
    QLabel *bg = nullptr;
    QVector<QWidget *> *recentCells = nullptr;
    QVector<QWidget *> *paletteCells = nullptr;
    // buttons made here (transparent: the canvas paints them)
    QToolButton *eyedropper = nullptr;
    QToolButton *copy = nullptr;
    QToolButton *swap = nullptr;
    QToolButton *add = nullptr;
    QToolButton *trash = nullptr;
    QToolButton *mode = nullptr;
    QString modeText = QStringLiteral("HSV");
    QString paletteName = QStringLiteral("My Palette");

    explicit KrimbleColorCanvas(QWidget *parent = nullptr) : QWidget(parent)
    {
        QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        policy.setHeightForWidth(true);
        setSizePolicy(policy);
        setMinimumWidth(240);
        eyedropper = makeButton();
        copy = makeButton();
        swap = makeButton();
        add = makeButton();
        trash = makeButton();
        mode = makeButton();
    }
    bool hasHeightForWidth() const override { return true; }
    int heightForWidth(int w) const override { return qRound(w * DH / DW); }
    QSize sizeHint() const override { return QSize(330, heightForWidth(330)); }
    QSize minimumSizeHint() const override { return QSize(240, heightForWidth(240)); }
    qreal scale() const { return width() / DW; }

    // the up and down parts of the six number boxes: call once, after hsv[] and rgb[] are set
    void createSteppers()
    {
        QSpinBox *boxes[6] = {hsv[0], hsv[1], hsv[2], rgb[0], rgb[1], rgb[2]};
        for (int i = 0; i < 6; ++i) {
            QSpinBox *box = boxes[i];
            m_up[i] = makeButton();
            m_down[i] = makeButton();
            m_up[i]->setAutoRepeat(true);
            m_down[i]->setAutoRepeat(true);
            QObject::connect(m_up[i], &QToolButton::clicked, box, [box]() { box->stepUp(); });
            QObject::connect(m_down[i], &QToolButton::clicked, box, [box]() { box->stepDown(); });
        }
    }

    QColor fgColor;
    QColor bgColor;
    void setForeground(const QColor &c) { fgColor = c; if (fg) fg->setStyleSheet(foregroundStyle(c)); }
    void setBackground(const QColor &c) { bgColor = c; if (bg) bg->setStyleSheet(backgroundStyle(c)); }

    // the sizes of the color boxes' own frames follow the scale (the style sheet text is made here)
    QString foregroundStyle(const QColor &c) const
    {
        const qreal s = scale();
        return QStringLiteral("background:%1;border:%2px solid #4fd8fb;border-radius:%3px;")
            .arg(c.name()).arg(qMax(2, qRound(3 * s))).arg(qMax(3, qRound(12 * s)));
    }
    QString backgroundStyle(const QColor &c) const
    {
        const qreal s = scale();
        return QStringLiteral("background:%1;border:%2px solid #e6ecfa;border-radius:%3px;")
            .arg(c.name()).arg(qMax(2, qRound(3 * s))).arg(qMax(3, qRound(14 * s)));
    }

    void relayout()
    {
        const qreal s = scale();
        QFont f = font();
        f.setPixelSize(qMax(8, qRound(38 * s)));
        if (square) square->setGeometry(r(29, 27, 617, 557).toAlignedRect());
        // the hue strip is 82 units wide; its widget is 4 units wider at each side and each end, so the white handle can reach beyond the strip like the mockup's
        if (hue) hue->setGeometry(r(671, 21, 90, 570).toAlignedRect());
        mode->setGeometry(r(804, 26, 237, 62).toAlignedRect());
        eyedropper->setGeometry(r(1073, 26, 78, 62).toAlignedRect());
        static const double spinY[6] = {111, 168, 225, 310, 366, 423};
        static const double spinH[6] = {51, 50, 50, 50, 51, 51};
        QSpinBox *boxes[6] = {hsv[0], hsv[1], hsv[2], rgb[0], rgb[1], rgb[2]};
        const int arrowW = qRound(38 * s);
        for (int i = 0; i < 6; ++i) {
            if (!boxes[i]) continue;
            const QRect whole = r(876, spinY[i], 229, spinH[i]).toAlignedRect();
            boxes[i]->setGeometry(whole.left(), whole.top(), whole.width() - arrowW, whole.height());
            boxes[i]->setFont(f);
            boxes[i]->setStyleSheet(QStringLiteral("QSpinBox { background: transparent; border: none; padding: 0px 0px 0px %1px; color: #f4f6fc; selection-background-color: #1196d8; }"
                                                    "QSpinBox QLineEdit { background: transparent; border: none; padding: 0px; }"
                                                    "QSpinBox::up-button, QSpinBox::down-button { width: 0px; height: 0px; border: none; }").arg(qRound(18 * s)));
            if (m_up[i] && m_down[i]) {
                m_up[i]->setGeometry(whole.right() - arrowW + 1, whole.top(), arrowW, whole.height() / 2);
                m_down[i]->setGeometry(whole.right() - arrowW + 1, whole.top() + whole.height() / 2, arrowW, whole.height() - whole.height() / 2);
            }
        }
        if (hex) {
            hex->setGeometry(r(876, 520, 214, 54).toAlignedRect());
            hex->setFont(f);
            hex->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; padding: 0px 0px 0px %1px; color: #f4f6fc; selection-background-color: #1196d8; }").arg(qRound(22 * s)));
        }
        copy->setGeometry(r(1100, 520, 56, 54).toAlignedRect());
        if (fg) fg->setGeometry(r(33, 615, 119, 112).toAlignedRect());
        if (bg) bg->setGeometry(r(247, 613, 117, 118).toAlignedRect());
        if (fg && fgColor.isValid()) fg->setStyleSheet(foregroundStyle(fgColor));   // the frames' sizes follow the scale
        if (bg && bgColor.isValid()) bg->setStyleSheet(backgroundStyle(bgColor));
        swap->setGeometry(r(176, 646, 54, 52).toAlignedRect());
        add->setGeometry(r(788, 764, 54, 56).toAlignedRect());
        trash->setGeometry(r(1018, 764, 56, 56).toAlignedRect());
        if (recentCells) layoutCells(*recentCells, 10, 1, QRectF(428, 663, 658, 55), 7.0, 0.0);
        if (paletteCells) layoutCells(*paletteCells, 19, 4, QRectF(29, 850, 1070, 206), 3.5, 4.2);
        update();
    }

protected:
    void resizeEvent(QResizeEvent *e) override
    {
        QWidget::resizeEvent(e);
        relayout();
    }

    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
        const qreal s = scale();
        QFont f = font();
        f.setPixelSize(qMax(8, qRound(38 * s)));
        p.setFont(f);

        // the card: a smooth gradient (measured #283248, #242A3F, #1B2235), a dark outline and a light line along the top that fades toward both ends
        {
            const QRectF card(0.5, 0.5, width() - 1.0, DH * s - 1.0);
            QPainterPath path;
            path.addRoundedRect(card, 22 * s, 22 * s);
            QLinearGradient g(0, 0, 0, DH * s);
            g.setColorAt(0.0, QColor("#283248"));
            g.setColorAt(0.5, QColor("#242a3f"));
            g.setColorAt(1.0, QColor("#1b2235"));
            p.fillPath(path, g);
            p.setPen(QPen(QColor("#0c1022"), 1.5));
            p.setBrush(Qt::NoBrush);
            p.drawPath(path);
            QLinearGradient top(0, 0, width(), 0);
            top.setColorAt(0.0, QColor(115, 130, 171, 0));
            top.setColorAt(0.5, QColor(115, 130, 171, 210));
            top.setColorAt(1.0, QColor(115, 130, 171, 0));
            p.setPen(QPen(QBrush(top), 1.6));
            p.drawLine(QPointF(30 * s, 1.6), QPointF(width() - 30 * s, 1.6));
        }

        const QColor label("#d9deee");

        // the mode drop-down (HSV or HSL) and the eyedropper button
        paintButton(p, r(804, 26, 237, 62), 12 * s);
        p.setPen(label);
        p.drawText(r(826 - 22 + 22, 26, 150, 62), Qt::AlignVCenter | Qt::AlignLeft, modeText);
        {
            const qreal x = (1010 - 22) * s;
            p.setPen(QPen(QColor(10, 14, 30, 200), 1.2));
            p.drawLine(QPointF(x, 36 * s), QPointF(x, 78 * s));
            p.setPen(QPen(QColor(110, 125, 175, 110), 1.0));
            p.drawLine(QPointF(x + 1.5, 36 * s), QPointF(x + 1.5, 78 * s));
            drawSymbol(p, QStringLiteral("chev_drop"), QPointF((1037 - 22) * s, 58 * s), 1.0);
        }
        paintButton(p, r(1073, 26, 78, 62), 12 * s);
        drawSymbol(p, QStringLiteral("eyedropper"), r(1073, 26, 78, 62).center(), 1.0);

        // the number boxes: H S V, then R G B, with their labels and units
        static const double spinY[6] = {111, 168, 225, 310, 366, 423};
        static const double spinH[6] = {51, 50, 50, 50, 51, 51};
        const QString names[6] = {QStringLiteral("H:"), QStringLiteral("S:"), modeText == QLatin1String("HSL") ? QStringLiteral("L:") : QStringLiteral("V:"),
                                  QStringLiteral("R:"), QStringLiteral("G:"), QStringLiteral("B:")};
        const QString units[6] = {QStringLiteral("\u00B0"), QStringLiteral("%"), QStringLiteral("%"), QString(), QString(), QString()};
        for (int i = 0; i < 6; ++i) {
            const QRectF box = r(876, spinY[i], 229, spinH[i]);
            paintField(p, box, 9 * s);
            // the arrow section: the right 38 units
            const QRectF arrows(box.right() - 38 * s, box.top(), 38 * s, box.height());
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
            p.setPen(label);
            p.drawText(QRectF(700 * s, box.top(), (858 - 700) * s, box.height()), Qt::AlignRight | Qt::AlignVCenter, names[i]);
            if (!units[i].isEmpty()) {
                p.setPen(QColor("#e8ebf6"));
                p.drawText(QRectF(900 * s, box.top(), (1052 - 900) * s, box.height()), Qt::AlignRight | Qt::AlignVCenter, units[i]);
            }
        }

        // the two thin dividers of the value column, HEX with its copy button
        paintLine(p, 804, 292, 1154, s);
        paintLine(p, 804, 496, 1154, s);
        p.setPen(label);
        p.drawText(QRectF(700 * s, 520 * s, (866 - 700) * s, 54 * s), Qt::AlignRight | Qt::AlignVCenter, QStringLiteral("HEX:"));
        paintField(p, r(876, 520, 214, 54), 9 * s);
        paintButton(p, r(1100, 520, 56, 54), 11 * s);
        drawSymbol(p, QStringLiteral("copy"), r(1100, 520, 56, 54).center(), 1.0);

        // the foreground color's glow, the swap arrows between the two colors
        if (fg) {
            const QRectF ring = QRectF(fg->geometry());
            for (int i = 9; i >= 1; --i) {
                p.setPen(QPen(QColor(40, 190, 255, 26 + (9 - i) * 8), 2.2));
                p.setBrush(Qt::NoBrush);
                p.drawRoundedRect(ring.adjusted(-i * 1.3 * s, -i * 1.3 * s, i * 1.3 * s, i * 1.3 * s), (12 + i) * s, (12 + i) * s);
            }
        }
        drawSymbol(p, QStringLiteral("swap"), r(176, 646, 54, 52).center(), 1.0);

        // Recent Colors: a divider above and below, the label, the dark well with the ten cells (empty places are dark), the chevron of the mockup is left out
        paintLine(p, 404, 603, 1158, s);
        paintLine(p, 404, 751, 1158, s);
        p.setPen(label);
        {
            // the label is as wide as the mockup's (170 units for "Recent Colors")
            p.setFont(fittedFont(f, QStringLiteral("Recent Colors"), 170 * s));
            p.drawText(QRectF(430 * s, 606 * s, 500 * s, 46 * s), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Recent Colors"));
            p.setFont(f);
        }
        paintWell(p, r(422, 657, 669, 64), 10 * s);
        paintEmptyCells(p, recentCells, 10, 1, QRectF(428, 663, 658, 55), 7.0, 0.0, QColor("#1a1f33"), 5 * s);

        // the palette row: grid symbol, the name, the add and remove buttons; then the palette well
        // the mockup's palette row: a raised frame with the grid symbol and "Palette:", joined to the raised name box
        paintButton(p, r(27, 761, 188, 64), 12 * s);
        p.setPen(label);
        p.setFont(fittedFont(f, QStringLiteral("Palette:"), 98 * s));
        p.drawText(QRectF(102 * s, 764 * s, 150 * s, 56 * s), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Palette:"));
        p.setFont(f);
        drawSymbol(p, QStringLiteral("grid"), r(40, 768, 50, 48).center(), 1.0);
        // paintField(p, r(215, 761, 536, 64), 10 * s);
        paintButton(p, r(215, 761, 536, 64), 12 * s);
        p.setPen(QColor("#f1f4fc"));
        p.drawText(QRectF(236 * s, 761 * s, 480 * s, 64 * s), Qt::AlignLeft | Qt::AlignVCenter, paletteName);
        drawSymbol(p, QStringLiteral("plus"), r(788, 764, 54, 56).center(), 1.0);
        drawSymbol(p, QStringLiteral("trash"), r(1018, 764, 56, 56).center(), 1.0);
        paintWell(p, r(24, 843, 1081, 220), 10 * s);
        paintEmptyCells(p, paletteCells, 19, 4, QRectF(29, 850, 1070, 206), 3.5, 4.2, QColor("#0f1325"), 3 * s);
    }

private:
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
    void layoutCells(const QVector<QWidget *> &cells, int cols, int rows, const QRectF &host, double gapX, double gapY)
    {
        const qreal s = scale();
        const double cellW = (host.width() - (cols - 1) * gapX) / cols;
        const double cellH = (host.height() - (rows - 1) * gapY) / rows;
        for (int i = 0; i < cells.size(); ++i) {
            QWidget *cell = cells[i];
            const int col = i % cols;
            const int row = i / cols;
            if (row >= rows) { cell->hide(); continue; }
            cell->setGeometry(QRectF((host.left() + col * (cellW + gapX)) * s, (host.top() + row * (cellH + gapY)) * s, cellW * s, cellH * s).toAlignedRect());
            cell->show();
        }
    }
    void paintEmptyCells(QPainter &p, const QVector<QWidget *> *cells, int cols, int rows, const QRectF &host, double gapX, double gapY, const QColor &color, qreal radius)
    {
        const qreal s = scale();
        const int used = cells ? cells->size() : 0;
        const double cellW = (host.width() - (cols - 1) * gapX) / cols;
        const double cellH = (host.height() - (rows - 1) * gapY) / rows;
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        for (int i = used; i < cols * rows; ++i) {
            const int col = i % cols;
            const int row = i / cols;
            p.drawRoundedRect(QRectF((host.left() + col * (cellW + gapX)) * s, (host.top() + row * (cellH + gapY)) * s, cellW * s, cellH * s), radius, radius);
        }
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
        p.setPen(QPen(QColor("#4a557b"), 1.3));
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

    QToolButton *m_up[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    QToolButton *m_down[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
};
