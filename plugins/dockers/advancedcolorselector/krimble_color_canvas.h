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
#include <QMouseEvent>
#include <QWheelEvent>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QVector>
#include <QString>
#include <QSizePolicy>
#include <QPaintEvent>
#include <QResizeEvent>
#include "krimble_color_symbols.h"
#include "krimble_color_paint.h"
#include "krimble_color_pages.h"

class KrimbleColorCanvas : public KrimbleColorBase
{
public:
    static constexpr double DH = 1100.0;   // the card's height
    double designHeight() const override { return DH; }

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
    QToolButton *paletteMenu = nullptr;   // 2026-11-08: the palette name box (opens the list of palettes)
    QToolButton *pencil = nullptr;        // rename the palette
    QToolButton *duplicate = nullptr;     // duplicate the palette
    QToolButton *more = nullptr;          // New, Open, Import, Export, Delete
    QToolButton *paletteUp = nullptr;     // 2026-11-08: George: "you better be able to scroll through 256 swatches." The palette area shows 4 rows of 19; the scroll bar, the wheel
    QToolButton *paletteDown = nullptr;   // and the arrows move it by rows
    int paletteScroll = 0;                // the first visible row
    void setPaletteScroll(int row)
    {
        const int maxRow = qMax(0, paletteRowCount() - 4);
        row = qBound(0, row, maxRow);
        if (row == paletteScroll) return;
        paletteScroll = row;
        relayout();
    }
    int paletteRowCount() const { return paletteCells ? (paletteCells->size() + 18) / 19 : 0; }
    // Krimble 2026-11-08: the top part of the card is the Picker (painted here), the Sliders page or the Values page; the bottom part (colors, recent, palette) is shared
    int topPage = 0;
    KrimbleSlidersTop *sliders = nullptr;
    KrimbleValuesTop *values = nullptr;
    void setTopPage(int page)
    {
        topPage = qBound(0, page, 2);
        relayout();
    }
    QString modeText = QStringLiteral("HSV");
    QString paletteName = QStringLiteral("My Palette");

    explicit KrimbleColorCanvas(QWidget *parent = nullptr) : KrimbleColorBase(parent)
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
        paletteMenu = makeButton();
        pencil = makeButton();
        duplicate = makeButton();
        more = makeButton();
        paletteUp = makeButton();
        paletteDown = makeButton();
        paletteUp->setAutoRepeat(true);
        paletteDown->setAutoRepeat(true);
        QObject::connect(paletteUp, &QToolButton::clicked, this, [this]() { setPaletteScroll(paletteScroll - 1); });
        QObject::connect(paletteDown, &QToolButton::clicked, this, [this]() { setPaletteScroll(paletteScroll + 1); });
    }
    QSize sizeHint() const override { return QSize(330, heightForWidth(330)); }
    QSize minimumSizeHint() const override { return QSize(240, heightForWidth(240)); }

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
        // return QStringLiteral("background:%1;border:%2px solid #4fd8fb;border-radius:%3px;")
        //     .arg(c.name()).arg(qMax(2, qRound(3 * s))).arg(qMax(3, qRound(12 * s)));
        // 2026-11-09: George: "Why are the color wells rounded at the corners? You know I hate that." The wells have square corners.
        // return QStringLiteral("background:%1;border:%2px solid #4fd8fb;border-radius:0px;").arg(c.name()).arg(qMax(2, qRound(3 * s)));
        // Krimble 2026-11-11: George: "The two color wells under the big gradient area should have rounded corners."
        return QStringLiteral("background:%1;border:%2px solid #4fd8fb;border-radius:%3px;").arg(c.name()).arg(qMax(2, qRound(3 * s))).arg(qMax(3, qRound(12 * s)));
    }
    QString backgroundStyle(const QColor &c) const
    {
        const qreal s = scale();
        // return QStringLiteral("background:%1;border:%2px solid #e6ecfa;border-radius:%3px;")
        //     .arg(c.name()).arg(qMax(2, qRound(3 * s))).arg(qMax(3, qRound(14 * s)));
        // return QStringLiteral("background:%1;border:%2px solid #e6ecfa;border-radius:0px;").arg(c.name()).arg(qMax(2, qRound(3 * s)));
        return QStringLiteral("background:%1;border:%2px solid #e6ecfa;border-radius:%3px;").arg(c.name()).arg(qMax(2, qRound(3 * s))).arg(qMax(3, qRound(14 * s)));   // Krimble 2026-11-11: rounded again
    }

    void relayout()
    {
        const qreal s = scale();
        {
            const bool picker = (topPage == 0);
            for (QWidget *w : {square, hue, static_cast<QWidget *>(hex), static_cast<QWidget *>(mode), static_cast<QWidget *>(eyedropper), static_cast<QWidget *>(copy)})
                if (w) w->setVisible(picker);
            for (int i = 0; i < 3; ++i) { if (hsv[i]) hsv[i]->setVisible(picker); if (rgb[i]) rgb[i]->setVisible(picker); }
            for (int i = 0; i < 6; ++i) { if (m_up[i]) m_up[i]->setVisible(picker); if (m_down[i]) m_down[i]->setVisible(picker); }
            if (sliders) { sliders->setGeometry(r(0, 0, DW, 590).toAlignedRect()); sliders->setVisible(topPage == 1); }
            if (values) { values->setGeometry(r(0, 0, DW, 590).toAlignedRect()); values->setVisible(topPage == 2); }
        }
        QFont f = font();
        f.setPixelSize(qMax(8, qRound(38 * s)));
        // if (square) square->setGeometry(r(29, 27, 617, 557).toAlignedRect());
        // Krimble 2026-11-11: George: "It should be a strict square area. That square can expand in size with the panel scale, but should stay square." The area was 617 x 557 units; it is now
        // 557 x 557 (r() scales both sides by the same factor), and the square and the hue strip are centered in the same span as before (29 to 761).
        if (square) square->setGeometry(r(75, 27, 557, 557).toAlignedRect());
        // the hue strip is 82 units wide; its widget is 4 units wider at each side and each end, so the white handle can reach beyond the strip like the mockup's
        // if (hue) hue->setGeometry(r(671, 21, 90, 570).toAlignedRect());
        // Krimble 2026-11-11: George: "The rainbow hue line can be thinner." The widget is 58 units wide (was 90), so the rainbow is about 53 units (was 82).
        if (hue) hue->setGeometry(r(657, 21, 58, 570).toAlignedRect());
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
        paletteMenu->setGeometry(r(215, 761, 536, 64).toAlignedRect());
        pencil->setGeometry(r(866, 764, 50, 56).toAlignedRect());
        duplicate->setGeometry(r(944, 764, 50, 56).toAlignedRect());
        more->setGeometry(r(1097, 761, 59, 64).toAlignedRect());
        paletteUp->setGeometry(r(1116, 846, 39, 37).toAlignedRect());
        paletteDown->setGeometry(r(1116, 1026, 39, 37).toAlignedRect());
        paletteScroll = qBound(0, paletteScroll, qMax(0, paletteRowCount() - 4));
        trash->setGeometry(r(1018, 764, 56, 56).toAlignedRect());
        if (recentCells) layoutCells(*recentCells, 10, 1, QRectF(428, 663, 658, 55), 7.0, 0.0);
        // if (paletteCells) layoutCells(*paletteCells, 19, 4, QRectF(29, 850, 1070, 206), 3.5, 4.2);
        if (paletteCells) layoutCells(*paletteCells, 19, 4, QRectF(29, 850, 1070, 206), 3.5, 4.2, paletteScroll);
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

        // the mode drop-down (HSV or HSL) and the eyedropper button (the Picker page only: the other pages are widgets over this part)
        if (topPage == 0) {
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

        }   // the Picker page's own painting
        // the foreground color's glow, the swap arrows between the two colors
        if (fg) {
            const QRectF ring = QRectF(fg->geometry());
            for (int i = 9; i >= 1; --i) {
                p.setPen(QPen(QColor(40, 190, 255, 26 + (9 - i) * 8), 2.2));
                p.setBrush(Qt::NoBrush);
                // p.drawRoundedRect(ring.adjusted(-i * 1.3 * s, -i * 1.3 * s, i * 1.3 * s, i * 1.3 * s), (12 + i) * s, (12 + i) * s);
                p.drawRect(ring.adjusted(-i * 1.3 * s, -i * 1.3 * s, i * 1.3 * s, i * 1.3 * s));   // 2026-11-09: square corners
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
        // paintEmptyCells(p, recentCells, 10, 1, QRectF(428, 663, 658, 55), 7.0, 0.0, QColor("#1a1f33"), 5 * s);
        paintEmptyCells(p, recentCells, 10, 1, QRectF(428, 663, 658, 55), 7.0, 0.0, QColor("#1a1f33"), 0);   // 2026-11-09: square corners

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
        // 2026-11-08: the rest of the mockup's palette row: the chevron of the name box, a divider, the pencil, duplicate and "..." buttons
        drawSymbol(p, QStringLiteral("chev_palette"), r(715, 793, 0, 0).topLeft(), 1.0);
        p.setPen(QPen(QColor(8, 11, 26, 200), 1.2));
        p.drawLine(QPointF(775 * s, 770 * s), QPointF(775 * s, 816 * s));
        p.setPen(QPen(QColor(110, 125, 175, 95), 1.0));
        p.drawLine(QPointF(775 * s + 1.6, 770 * s), QPointF(775 * s + 1.6, 816 * s));
        drawSymbol(p, QStringLiteral("pencil"), r(866, 764, 50, 56).center(), 1.0);
        drawSymbol(p, QStringLiteral("duplicate"), r(944, 764, 50, 56).center(), 1.0);
        paintButton(p, r(1097, 761, 59, 64), 11 * s);
        drawSymbol(p, QStringLiteral("more"), r(1097, 761, 59, 64).center(), 1.0);
        paintWell(p, r(24, 843, 1081, 220), 10 * s);
        // paintEmptyCells(p, paletteCells, 19, 4, QRectF(29, 850, 1070, 206), 3.5, 4.2, QColor("#0f1325"), 3 * s, paletteScroll);
        paintEmptyCells(p, paletteCells, 19, 4, QRectF(29, 850, 1070, 206), 3.5, 4.2, QColor("#0f1325"), 0, paletteScroll);   // 2026-11-09: square corners
        // the scroll bar of the palette (the mockup's: up and down arrows and a thumb), measured: 39 units wide beside the well
        {
            paintWell(p, r(1116, 846, 39, 217), 8 * s);
            paintButton(p, r(1116, 846, 39, 37), 8 * s);
            paintButton(p, r(1116, 1026, 39, 37), 8 * s);
            drawSymbol(p, QStringLiteral("scroll_up"), r(1116, 846, 39, 37).center(), 0.9);
            drawSymbol(p, QStringLiteral("scroll_down"), r(1116, 1026, 39, 37).center(), 0.9);
            const QRectF thumb = paletteThumb();
            QLinearGradient tg(0, thumb.top(), 0, thumb.bottom());
            tg.setColorAt(0.0, QColor("#59679a"));
            tg.setColorAt(1.0, QColor("#38446c"));
            p.setPen(QPen(QColor("#0b0f20"), 1.2));
            p.setBrush(tg);
            p.drawRoundedRect(thumb, 6 * s, 6 * s);
        }
    }

    // the scroll bar: the thumb's place (its length is the part of the palette that shows)
    QRectF paletteThumb() const
    {
        const int rows = paletteRowCount();
        const double trackTop = 886, trackH = 137;
        const double ratio = rows > 4 ? 4.0 / rows : 1.0;
        const double thumbH = qMax(30.0, trackH * ratio);
        const int maxRow = qMax(0, rows - 4);
        const double y = trackTop + (trackH - thumbH) * (maxRow > 0 ? double(paletteScroll) / maxRow : 0.0);
        return r(1119, y, 33, thumbH);
    }
    void wheelEvent(QWheelEvent *e) override
    {
        if (topPage > 2) { QWidget::wheelEvent(e); return; }
        if (r(24, 843, 1135, 220).contains(QPointF(e->pos()))) {
            setPaletteScroll(paletteScroll - (e->angleDelta().y() > 0 ? 1 : -1));
            e->accept();
            return;
        }
        QWidget::wheelEvent(e);
    }
    void mousePressEvent(QMouseEvent *e) override
    {
        const QRectF track = r(1116, 883, 39, 143);
        if (track.contains(QPointF(e->pos()))) {
            const QRectF thumb = paletteThumb();
            if (thumb.contains(QPointF(e->pos()))) { m_thumbDrag = true; m_thumbGrab = e->pos().y() - thumb.top(); }
            else setPaletteScroll(paletteScroll + (e->pos().y() < thumb.top() ? -4 : 4));
            return;
        }
        QWidget::mousePressEvent(e);
    }
    void mouseMoveEvent(QMouseEvent *e) override
    {
        if (!m_thumbDrag) { QWidget::mouseMoveEvent(e); return; }
        const int rows = paletteRowCount();
        const int maxRow = qMax(0, rows - 4);
        const double trackTop = 886 * scale(), trackH = 137 * scale();
        const double thumbH = qMax(30.0 * scale(), trackH * (rows > 4 ? 4.0 / rows : 1.0));
        const double span = qMax(1.0, trackH - thumbH);
        setPaletteScroll(qRound(maxRow * qBound(0.0, (e->pos().y() - m_thumbGrab - trackTop) / span, 1.0)));
    }
    void mouseReleaseEvent(QMouseEvent *e) override
    {
        m_thumbDrag = false;
        QWidget::mouseReleaseEvent(e);
    }

private:
    bool m_thumbDrag = false;
    double m_thumbGrab = 0;
    void layoutCells(const QVector<QWidget *> &cells, int cols, int rows, const QRectF &host, double gapX, double gapY, int firstRow = 0)
    {
        const qreal s = scale();
        const double cellW = (host.width() - (cols - 1) * gapX) / cols;
        const double cellH = (host.height() - (rows - 1) * gapY) / rows;
        for (int i = 0; i < cells.size(); ++i) {
            QWidget *cell = cells[i];
            const int col = i % cols;
            const int row = i / cols - firstRow;
            if (row < 0 || row >= rows) { cell->hide(); continue; }
            // cell->setGeometry(QRectF((host.left() + col * (cellW + gapX)) * s, (host.top() + row * (cellH + gapY)) * s, cellW * s, cellH * s).toAlignedRect());
            // Krimble 2026-11-11: George: "It looks like ColorLab scaled down so far that some of the black lines dividing the color wells disappeared. Not good." The cells were placed with
            // toAlignedRect(), which rounds each cell OUTWARD to whole pixels, so at a scale where a cell is not a whole number of pixels neighbors overlapped by a pixel and the next cell's color
            // covered the black line (the 1 px dark border on the right and bottom of each cell) of the one before it. Now every cell edge is rounded to the nearest whole pixel by ONE formula, so
            // the right edge of a cell is exactly the left edge of the next: no overlap, no gap, and each cell keeps its own 1 px line.
            const double pitchX = cellW + gapX, pitchY = cellH + gapY;
            const int x0 = qRound((host.left() + col * pitchX) * s);
            const int x1 = qRound((host.left() + (col + 1) * pitchX - gapX) * s);
            const int y0 = qRound((host.top() + row * pitchY) * s);
            const int y1 = qRound((host.top() + (row + 1) * pitchY - gapY) * s);
            cell->setGeometry(x0, y0, qMax(1, x1 - x0), qMax(1, y1 - y0));
            cell->show();
        }
    }
    void paintEmptyCells(QPainter &p, const QVector<QWidget *> *cells, int cols, int rows, const QRectF &host, double gapX, double gapY, const QColor &color, qreal radius, int firstRow = 0)
    {
        const qreal s = scale();
        const int used = cells ? cells->size() : 0;
        const double cellW = (host.width() - (cols - 1) * gapX) / cols;
        const double cellH = (host.height() - (rows - 1) * gapY) / rows;
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        for (int i = qMax(used, firstRow * cols); i < (firstRow + rows) * cols; ++i) {
            const int col = i % cols;
            const int row = i / cols - firstRow;
            p.drawRoundedRect(QRectF((host.left() + col * (cellW + gapX)) * s, (host.top() + row * (cellH + gapY)) * s, cellW * s, cellH * s), radius, radius);
        }
    }
    QToolButton *m_up[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    QToolButton *m_down[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
};
