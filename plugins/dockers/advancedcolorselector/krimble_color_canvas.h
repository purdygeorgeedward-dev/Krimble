// SPDX-License-Identifier: GPL-2.0-or-later
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
    static constexpr double DH = 1100.0;
    static constexpr double PaletteRowH = 51.5;
    int extraPaletteRows = 0;
    double extraH() const { return extraPaletteRows * PaletteRowH; }
    int visibleRows() const { return 4 + extraPaletteRows; }
    void setExtraPaletteRows(int n)
    {
        n = qBound(0, n, 10);
        if (n == extraPaletteRows) return;
        extraPaletteRows = n;
        updateGeometry();
        relayout();
    }
    double designHeight() const override { return DH + extraH(); }

    QWidget *square = nullptr;
    QWidget *hue = nullptr;
    QSpinBox *hsv[3] = {nullptr, nullptr, nullptr};
    QSpinBox *rgb[3] = {nullptr, nullptr, nullptr};
    QLineEdit *hex = nullptr;
    QLabel *fg = nullptr;
    QLabel *bg = nullptr;
    QVector<QWidget *> *recentCells = nullptr;
    QVector<QWidget *> *paletteCells = nullptr;
    QToolButton *eyedropper = nullptr;
    QToolButton *copy = nullptr;
    QToolButton *swap = nullptr;
    QToolButton *add = nullptr;
    QToolButton *trash = nullptr;
    QToolButton *mode = nullptr;
    QToolButton *paletteMenu = nullptr;
    QToolButton *pencil = nullptr;
    QToolButton *duplicate = nullptr;
    QToolButton *more = nullptr;
    QToolButton *paletteUp = nullptr;
    QToolButton *paletteDown = nullptr;
    int paletteScroll = 0;
    void setPaletteScroll(int row)
    {
        const int maxRow = qMax(0, paletteRowCount() - visibleRows());
        row = qBound(0, row, maxRow);
        if (row == paletteScroll) return;
        paletteScroll = row;
        relayout();
    }
    int paletteRowCount() const { return paletteCells ? (paletteCells->size() + 18) / 19 : 0; }
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
    void setForeground(const QColor &c) { fgColor = c; if (fg) krimbleSetSheet(fg, foregroundStyle(c)); }
    void setBackground(const QColor &c) { bgColor = c; if (bg) krimbleSetSheet(bg, backgroundStyle(c)); }

    QString foregroundStyle(const QColor &c) const
    {
        const qreal s = scale();
        return QStringLiteral("background:%1;border:none;border-radius:%2px;").arg(c.name()).arg(qMax(3, qRound(12 * s)));
    }
    QString backgroundStyle(const QColor &c) const
    {
        const qreal s = scale();
        return QStringLiteral("background:%1;border:none;border-radius:%2px;").arg(c.name()).arg(qMax(3, qRound(14 * s)));
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
        if (square) square->setGeometry(r(75, 27, 557, 557).toAlignedRect());
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
            krimbleSetSheet(boxes[i], QStringLiteral("QSpinBox { background: transparent; border: none; padding: 0px 0px 0px %1px; color: #f4f6fc; selection-background-color: #0b87dd; }"
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
            krimbleSetSheet(hex, QStringLiteral("QLineEdit { background: transparent; border: none; padding: 0px 0px 0px %1px; color: #f4f6fc; selection-background-color: #0b87dd; }").arg(qRound(22 * s)));
        }
        copy->setGeometry(r(1100, 520, 56, 54).toAlignedRect());
        if (fg) fg->setGeometry(r(33, 615, 119, 112).toAlignedRect());
        if (bg) bg->setGeometry(r(247, 613, 117, 118).toAlignedRect());
        if (fg && fgColor.isValid()) krimbleSetSheet(fg, foregroundStyle(fgColor));
        if (bg && bgColor.isValid()) krimbleSetSheet(bg, backgroundStyle(bgColor));
        swap->setGeometry(r(176, 646, 54, 52).toAlignedRect());
        add->setGeometry(r(788, 764, 54, 56).toAlignedRect());
        paletteMenu->setGeometry(r(215, 761, 536, 64).toAlignedRect());
        pencil->setGeometry(r(866, 764, 50, 56).toAlignedRect());
        duplicate->setGeometry(r(944, 764, 50, 56).toAlignedRect());
        more->setGeometry(r(1097, 761, 59, 64).toAlignedRect());
        paletteUp->setGeometry(r(1116, 846, 39, 37).toAlignedRect());
        paletteDown->setGeometry(r(1116, 1026 + extraH(), 39, 37).toAlignedRect());
        paletteScroll = qBound(0, paletteScroll, qMax(0, paletteRowCount() - visibleRows()));
        trash->setGeometry(r(1018, 764, 56, 56).toAlignedRect());
        if (recentCells) layoutCells(*recentCells, 10, 1, QRectF(428, 663, 658, 55), 7.0, 0.0);
        if (paletteCells) layoutCells(*paletteCells, 19, visibleRows(), QRectF(29, 850, 1070, 206 + extraH()), 3.5, 4.2, paletteScroll);
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

        {
            const QRectF card(0.5, 0.5, width() - 1.0, designHeight() * s - 1.0);
            QPainterPath path;
            path.addRoundedRect(card, 22 * s, 22 * s);
            QLinearGradient g(0, 0, 0, designHeight() * s);
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

        static const double spinY[6] = {111, 168, 225, 310, 366, 423};
        static const double spinH[6] = {51, 50, 50, 50, 51, 51};
        const QString names[6] = {QStringLiteral("H:"), QStringLiteral("S:"), modeText == QLatin1String("HSL") ? QStringLiteral("L:") : QStringLiteral("V:"),
                                  QStringLiteral("R:"), QStringLiteral("G:"), QStringLiteral("B:")};
        const QString units[6] = {QStringLiteral("\u00B0"), QStringLiteral("%"), QStringLiteral("%"), QString(), QString(), QString()};
        for (int i = 0; i < 6; ++i) {
            const QRectF box = r(876, spinY[i], 229, spinH[i]);
            paintField(p, box, 9 * s);
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

        paintLine(p, 804, 292, 1154, s);
        paintLine(p, 804, 496, 1154, s);
        p.setPen(label);
        p.drawText(QRectF(700 * s, 520 * s, (866 - 700) * s, 54 * s), Qt::AlignRight | Qt::AlignVCenter, QStringLiteral("HEX:"));
        paintField(p, r(876, 520, 214, 54), 9 * s);
        paintButton(p, r(1100, 520, 56, 54), 11 * s);
        drawSymbol(p, QStringLiteral("copy"), r(1100, 520, 56, 54).center(), 1.0);

        }
        drawSymbol(p, QStringLiteral("swap"), r(176, 646, 54, 52).center(), 1.0);

        paintLine(p, 404, 603, 1158, s);
        paintLine(p, 404, 751, 1158, s);
        p.setPen(label);
        {
            p.setFont(fittedFont(f, QStringLiteral("Recent Colors"), 170 * s));
            p.drawText(QRectF(430 * s, 606 * s, 500 * s, 46 * s), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Recent Colors"));
            p.setFont(f);
        }
        paintWell(p, r(422, 657, 669, 64), 10 * s);
        paintEmptyCells(p, recentCells, 10, 1, QRectF(428, 663, 658, 55), 7.0, 0.0, QColor("#1a1f33"), 0);

        paintButton(p, r(27, 761, 188, 64), 12 * s);
        p.setPen(label);
        p.setFont(fittedFont(f, QStringLiteral("Palette:"), 98 * s));
        p.drawText(QRectF(102 * s, 764 * s, 150 * s, 56 * s), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Palette:"));
        p.setFont(f);
        drawSymbol(p, QStringLiteral("grid"), r(40, 768, 50, 48).center(), 1.0);
        paintButton(p, r(215, 761, 536, 64), 12 * s);
        p.setPen(QColor("#f1f4fc"));
        p.drawText(QRectF(236 * s, 761 * s, 480 * s, 64 * s), Qt::AlignLeft | Qt::AlignVCenter, paletteName);
        drawSymbol(p, QStringLiteral("plus"), r(788, 764, 54, 56).center(), 1.0);
        drawSymbol(p, QStringLiteral("trash"), r(1018, 764, 56, 56).center(), 1.0);
        drawSymbol(p, QStringLiteral("chev_palette"), r(715, 793, 0, 0).topLeft(), 1.0);
        p.setPen(QPen(QColor(8, 11, 26, 200), 1.2));
        p.drawLine(QPointF(775 * s, 770 * s), QPointF(775 * s, 816 * s));
        p.setPen(QPen(QColor(110, 125, 175, 95), 1.0));
        p.drawLine(QPointF(775 * s + 1.6, 770 * s), QPointF(775 * s + 1.6, 816 * s));
        drawSymbol(p, QStringLiteral("pencil"), r(866, 764, 50, 56).center(), 1.0);
        drawSymbol(p, QStringLiteral("duplicate"), r(944, 764, 50, 56).center(), 1.0);
        paintButton(p, r(1097, 761, 59, 64), 11 * s);
        drawSymbol(p, QStringLiteral("more"), r(1097, 761, 59, 64).center(), 1.0);
        paintWell(p, r(24, 843, 1081, 220 + extraH()), 10 * s);
        paintEmptyCells(p, paletteCells, 19, visibleRows(), QRectF(29, 850, 1070, 206 + extraH()), 3.5, 4.2, QColor("#0f1325"), 0, paletteScroll);
        {
            paintWell(p, r(1116, 846, 39, 217 + extraH()), 8 * s);
            paintButton(p, r(1116, 846, 39, 37), 8 * s);
            paintButton(p, r(1116, 1026 + extraH(), 39, 37), 8 * s);
            drawSymbol(p, QStringLiteral("scroll_up"), r(1116, 846, 39, 37).center(), 0.9);
            drawSymbol(p, QStringLiteral("scroll_down"), r(1116, 1026 + extraH(), 39, 37).center(), 0.9);
            const QRectF thumb = paletteThumb();
            QLinearGradient tg(0, thumb.top(), 0, thumb.bottom());
            tg.setColorAt(0.0, QColor("#59679a"));
            tg.setColorAt(1.0, QColor("#38446c"));
            p.setPen(QPen(QColor("#0b0f20"), 1.2));
            p.setBrush(tg);
            p.drawRoundedRect(thumb, 6 * s, 6 * s);
        }
    }

    QRectF paletteThumb() const
    {
        const int rows = paletteRowCount();
        const double trackTop = 886, trackH = 137 + extraH();
        const double ratio = rows > visibleRows() ? double(visibleRows()) / rows : 1.0;
        const double thumbH = qMax(30.0, trackH * ratio);
        const int maxRow = qMax(0, rows - visibleRows());
        const double y = trackTop + (trackH - thumbH) * (maxRow > 0 ? double(paletteScroll) / maxRow : 0.0);
        return r(1119, y, 33, thumbH);
    }
    void wheelEvent(QWheelEvent *e) override
    {
        if (topPage > 2) { QWidget::wheelEvent(e); return; }
        if (r(24, 843, 1135, 220 + extraH()).contains(QPointF(e->pos()))) {
            setPaletteScroll(paletteScroll - (e->angleDelta().y() > 0 ? 1 : -1));
            e->accept();
            return;
        }
        QWidget::wheelEvent(e);
    }
    void mousePressEvent(QMouseEvent *e) override
    {
        const QRectF track = r(1116, 883, 39, 143 + extraH());
        if (track.contains(QPointF(e->pos()))) {
            const QRectF thumb = paletteThumb();
            if (thumb.contains(QPointF(e->pos()))) { m_thumbDrag = true; m_thumbGrab = e->pos().y() - thumb.top(); }
            else setPaletteScroll(paletteScroll + (e->pos().y() < thumb.top() ? -visibleRows() : visibleRows()));
            return;
        }
        QWidget::mousePressEvent(e);
    }
    void mouseMoveEvent(QMouseEvent *e) override
    {
        if (!m_thumbDrag) { QWidget::mouseMoveEvent(e); return; }
        const int rows = paletteRowCount();
        const int maxRow = qMax(0, rows - visibleRows());
        const double trackTop = 886 * scale(), trackH = (137 + extraH()) * scale();
        const double thumbH = qMax(30.0 * scale(), trackH * (rows > visibleRows() ? double(visibleRows()) / rows : 1.0));
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
