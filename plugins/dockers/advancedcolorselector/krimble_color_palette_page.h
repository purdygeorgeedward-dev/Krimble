// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-08: the Palette page of the Color panel, from the Palette mockup (George: "make the new tabs work just like the mockups"), measured and scaled to the card's 1178
// units (1098 tall). It works on the named palettes (krimble_color_palettes.h): choose a palette, make a New one, Open, Import, Export, Duplicate, Rename or Delete it; the info shows
// its name, colors and format; the selected color can be changed (index, HEX, R G B); the swatch tools pick from the image, replace the swatch with the foreground color, reload the
// palette from its file, or change the number of columns; the grid shows the swatches (tap one to use it, drag to scroll); the recent colors are below.
#pragma once
#include <functional>
#include <QMenu>
#include <QMouseEvent>
#include <QSpinBox>
#include <QWheelEvent>
#include "krimble_color_pages.h"
#include "krimble_color_palette_actions.h"

class KrimblePalettePage : public KrimbleColorBase
{
public:
    KrimbleColorState *state = nullptr;
    std::function<void(const QColor &)> onColor;      // a swatch chosen here becomes the foreground color
    std::function<void()> onPaletteChanged;           // the active palette or its colors changed
    std::function<void()> onPickFromImage;            // the eyedropper of the swatch tools
    QVector<QColor> *recent = nullptr;                // the recent colors (the panel's list)
    QLineEdit *hex = nullptr;
    QToolButton *copy = nullptr;
    QSpinBox *indexBox = nullptr;
    QSpinBox *rgbBox[3] = {nullptr, nullptr, nullptr};
    QSpinBox *cmykBox[4] = {nullptr, nullptr, nullptr, nullptr};   // 2026-11-08: the four plates of a CMYK palette's selected swatch

    double designHeight() const override { return 1098.0; }

    explicit KrimblePalettePage(QWidget *parent = nullptr) : KrimbleColorBase(parent)
    {
        hex = new QLineEdit(this);
        hex->setMaxLength(9);
        copy = makeButton();
        copy->setToolTip(tr("Copy the HEX code"));
        QObject::connect(copy, &QToolButton::clicked, this, [this]() { QApplication::clipboard()->setText(hex->text()); });
        QObject::connect(hex, &QLineEdit::editingFinished, this, [this]() {
            const QColor c(hex->text());
            if (c.isValid()) editSelected(c);
        });
        indexBox = new QSpinBox(this);
        QObject::connect(indexBox, qOverload<int>(&QSpinBox::valueChanged), this, [this](int v) { if (!m_syncing) selectIndex(v, true); });
        m_indexUp = makeButton();
        m_indexDown = makeButton();
        for (int i = 0; i < 3; ++i) {
            rgbBox[i] = new QSpinBox(this);
            rgbBox[i]->setRange(0, 255);
            QObject::connect(rgbBox[i], qOverload<int>(&QSpinBox::valueChanged), this, [this](int) {
                if (!m_syncing) editSelected(QColor(rgbBox[0]->value(), rgbBox[1]->value(), rgbBox[2]->value()));
            });
            m_rgbUp[i] = makeButton();
            m_rgbDown[i] = makeButton();
        }
        for (int i = 0; i < 4; ++i) {
            cmykBox[i] = new QSpinBox(this);
            cmykBox[i]->setRange(0, 100);
            QObject::connect(cmykBox[i], qOverload<int>(&QSpinBox::valueChanged), this, [this](int) {
                if (!m_syncing) editSelectedCmyk({cmykBox[0]->value(), cmykBox[1]->value(), cmykBox[2]->value(), cmykBox[3]->value()});
            });
            m_cmykUp[i] = makeButton();
            m_cmykDown[i] = makeButton();
        }
        QSpinBox *boxes[8] = {indexBox, rgbBox[0], rgbBox[1], rgbBox[2], cmykBox[0], cmykBox[1], cmykBox[2], cmykBox[3]};
        QToolButton *ups[8] = {m_indexUp, m_rgbUp[0], m_rgbUp[1], m_rgbUp[2], m_cmykUp[0], m_cmykUp[1], m_cmykUp[2], m_cmykUp[3]};
        QToolButton *downs[8] = {m_indexDown, m_rgbDown[0], m_rgbDown[1], m_rgbDown[2], m_cmykDown[0], m_cmykDown[1], m_cmykDown[2], m_cmykDown[3]};
        for (int i = 0; i < 8; ++i) {
            ups[i]->setAutoRepeat(true);
            downs[i]->setAutoRepeat(true);
            QSpinBox *box = boxes[i];
            QObject::connect(ups[i], &QToolButton::clicked, box, [box]() { box->stepUp(); });
            QObject::connect(downs[i], &QToolButton::clicked, box, [box]() { box->stepDown(); });
        }
    }

    // read the active palette from its file
    void reload()
    {
        KrimblePaletteStore::ensureDefaults();
        if (!KrimblePaletteStore::load(KrimblePaletteStore::active(), m_pal)) m_pal = KrimblePalette();
        if (m_selected >= m_pal.colors.size()) m_selected = m_pal.colors.isEmpty() ? -1 : 0;
        clampScroll();
        syncBoxes();
        update();
    }
    void refresh()
    {
        if (KrimblePaletteStore::signature() != m_signature) { m_signature = KrimblePaletteStore::signature(); reload(); }
        else update();
    }
    const KrimblePalette &palette() const { return m_pal; }
    int selectedIndex() const { return m_selected; }

    // choose a swatch (by index); set the foreground color when asked
    void selectIndex(int i, bool useColor)
    {
        if (i < 0 || i >= m_pal.colors.size()) return;
        m_selected = i;
        ensureVisible(i);
        syncBoxes();
        update();
        if (useColor && onColor) onColor(m_pal.colors[i]);
    }

    void relayout()
    {
        const qreal s = scale();
        QFont f = font();
        f.setPixelSize(qMax(8, qRound(30 * s)));
        auto place = [&](QSpinBox *box, QToolButton *up, QToolButton *down, const QRectF &whole, double arrowUnits, double padUnits = 10.0) {
            const int arrowW = qRound(arrowUnits * s);
            const QRect w = whole.toAlignedRect();
            box->setGeometry(w.left(), w.top(), w.width() - arrowW, w.height());
            box->setFont(f);
            box->setStyleSheet(QStringLiteral("QSpinBox { background: transparent; border: none; padding: 0px 0px 0px %1px; color: #f4f6fc; selection-background-color: #1196d8; }"
                                              "QSpinBox QLineEdit { background: transparent; border: none; padding: 0px; }"
                                              "QSpinBox::up-button, QSpinBox::down-button { width: 0px; height: 0px; border: none; }").arg(qRound(padUnits * s)));
            up->setGeometry(w.right() - arrowW + 1, w.top(), arrowW, w.height() / 2);
            down->setGeometry(w.right() - arrowW + 1, w.top() + w.height() / 2, arrowW, w.height() - w.height() / 2);
        };
        place(indexBox, m_indexUp, m_indexDown, r(241, 674, 109, 36), 26);
        place(rgbBox[0], m_rgbUp[0], m_rgbDown[0], r(50.6, 839, 76, 34), 24);
        place(rgbBox[1], m_rgbUp[1], m_rgbDown[1], r(166, 839, 78, 34), 24);
        place(rgbBox[2], m_rgbUp[2], m_rgbDown[2], r(282.6, 839, 81, 34), 24);
        for (int i = 0; i < 4; ++i) place(cmykBox[i], m_cmykUp[i], m_cmykDown[i], r(45 + 80 * i, 839, 60, 34), 16, 4);
        hex->setGeometry(r(165, 751, 148, 39).toAlignedRect());
        hex->setFont(f);
        hex->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; padding: 0px 0px 0px %1px; color: #f4f6fc; selection-background-color: #1196d8; }").arg(qRound(12 * s)));
        copy->setGeometry(r(313, 751, 41, 39).toAlignedRect());
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
        QFont f = font();
        f.setPixelSize(qMax(8, qRound(29 * s)));   // titles
        QFont body = font();
        body.setPixelSize(qMax(8, qRound(23 * s)));   // the buttons, the info and the small labels
        p.setFont(body);
        const QColor label("#d9deee");
        const QColor selectedColor = (m_selected >= 0 && m_selected < m_pal.colors.size()) ? m_pal.colors[m_selected] : (state ? state->color : QColor("#459ad3"));
        // ---- the left group
        paintGroup(p, r(13, 11, 358, 1065), 14 * s);
        p.setPen(label);
        p.setFont(f);
        p.drawText(r(30, 11, 200, 46), Qt::AlignLeft | Qt::AlignVCenter, tr("Palette"));
        p.setFont(body);
        paintButton(p, r(29, 60, 325, 51), 11 * s);
        drawIcon(p, QStringLiteral("pal_grid"), r(64, 85.5, 0, 0).topLeft(), 0.8);
        p.setPen(QColor("#f1f4fc"));
        p.setFont(fittedFont(f, m_pal.name.isEmpty() ? tr("Palette") : m_pal.name, 190 * s));
        p.drawText(r(95, 60, 215, 51), Qt::AlignLeft | Qt::AlignVCenter, m_pal.name);
        p.setFont(body);
        drawSymbol(p, QStringLiteral("chev_drop"), r(330, 85.5, 0, 0).topLeft(), 0.9);
        static const char *commands[7] = {"New Palette...", "Open Palette...", "Import...", "Export...", "Duplicate...", "Rename...", "Delete..."};
        static const char *commandIcons[7] = {"pal_new", "pal_open", "pal_import", "pal_export", "pal_duplicate", "pal_rename", "pal_delete"};
        for (int i = 0; i < 7; ++i) {
            const QRectF b = commandRect(i);
            paintButton(p, b, 9 * s);
            drawIcon(p, QString::fromLatin1(commandIcons[i]), QPointF(b.left() + 34 * s, b.center().y()), 0.8);
            p.setPen(QColor("#f1f4fc"));
            p.drawText(QRectF(b.left() + 70 * s, b.top(), b.width() - 74 * s, b.height()), Qt::AlignLeft | Qt::AlignVCenter, tr(commands[i]));
        }
        paintBrightLine(p, 29, 478, 354, s);
        p.setPen(label);
        p.setFont(f);
        p.drawText(r(30, 486, 300, 28), Qt::AlignLeft | Qt::AlignVCenter, tr("Palette Info"));
        p.setFont(body);
        const int count = m_pal.colors.size();
        const int cols = qMax(1, m_pal.columns);
        const QString infoLabels[3] = {tr("Name:"), tr("Colors:"), tr("Format:")};
        const QString infoValues[3] = {m_pal.name, tr("%1 (%2 \u00D7 %3)").arg(count).arg(cols).arg((count + cols - 1) / cols), m_pal.isCmyk() ? QStringLiteral("GPL (CMYK)") : QStringLiteral("GPL")};
        for (int i = 0; i < 3; ++i) {
            const double cy = 536.8 + 31.5 * i;
            p.setPen(label);
            p.drawText(r(36, cy - 15, 85, 30), Qt::AlignLeft | Qt::AlignVCenter, infoLabels[i]);
            p.setPen(QColor("#f1f4fc"));
            p.drawText(r(124.6, cy - 15, 230, 30), Qt::AlignLeft | Qt::AlignVCenter, infoValues[i]);
        }
        paintBrightLine(p, 29, 623, 354, s);
        p.setPen(label);
        p.setFont(f);
        p.drawText(r(33, 631, 300, 30), Qt::AlignLeft | Qt::AlignVCenter, tr("Selected Color"));
        p.setFont(body);
        paintColorBox(p, r(27, 679, 112, 113), selectedColor, 16 * s, s, true);
        p.setPen(label);
        p.drawText(r(160, 674, 80, 36), Qt::AlignLeft | Qt::AlignVCenter, tr("Index:"));
        paintSpinFrame(p, r(241, 674, 109, 36), 26);
        p.drawText(r(160, 715, 80, 36), Qt::AlignLeft | Qt::AlignVCenter, tr("HEX:"));
        paintField(p, r(165, 751, 189, 39), 8 * s);
        paintButton(p, r(313, 751, 41, 39), 9 * s);
        drawSymbol(p, QStringLiteral("copy"), r(313, 751, 41, 39).center(), 0.75);
        p.setPen(label);
        p.drawText(r(36, 800, 200, 30), Qt::AlignLeft | Qt::AlignVCenter, m_pal.isCmyk() ? tr("CMYK (percent):") : tr("RGB:"));
        if (m_pal.isCmyk()) {
            static const char *cmykNames[4] = {"C", "M", "Y", "K"};
            for (int i = 0; i < 4; ++i) {
                p.setPen(label);
                p.drawText(r(29 + 80 * i, 839, 16, 34), Qt::AlignLeft | Qt::AlignVCenter, QString::fromLatin1(cmykNames[i]));
                paintSpinFrame(p, r(45 + 80 * i, 839, 60, 34), 16);
            }
        } else {
        static const double labelX[3] = {26, 140, 257};
        static const char *rgbNames[3] = {"R:", "G:", "B:"};
        static const double boxX[3] = {50.6, 166, 282.6};
        static const double boxW[3] = {76, 78, 81};
        for (int i = 0; i < 3; ++i) {
            p.setPen(label);
            p.drawText(r(labelX[i], 839, 24, 34), Qt::AlignLeft | Qt::AlignVCenter, QString::fromLatin1(rgbNames[i]));
            paintSpinFrame(p, r(boxX[i], 839, boxW[i], 34), 24);
        }
        }
        paintBrightLine(p, 29, 895, 354, s);
        p.setPen(label);
        p.setFont(f);
        p.drawText(r(33, 903, 300, 30), Qt::AlignLeft | Qt::AlignVCenter, tr("Swatch Tools"));
        p.setFont(body);
        static const char *toolIcons[4] = {"tool_eyedropper", "tool_pencil", "tool_refresh", "tool_grid"};
        for (int i = 0; i < 4; ++i) {
            const QRectF b = toolRect(i);
            paintButton(p, b, 11 * s);
            drawIcon(p, QString::fromLatin1(toolIcons[i]), b.center(), 0.9);
        }
        // ---- the grid group
        paintGroup(p, r(376, 13, 802, 920), 14 * s);
        paintWell(p, r(432.5, 60.8, 738, 863), 10 * s);
        {
            QFont small = f;
            small.setPixelSize(qMax(7, qRound(26 * s)));
            p.setFont(small);
            p.setPen(label);
            for (int c = 0; c < cols; ++c)
                p.drawText(QRectF(cellRect(0, c).left(), 20 * s, cellRect(0, c).width(), 40 * s), Qt::AlignCenter, QString::number(c));
            p.save();
            p.setClipRect(r(376, 60, 800, 862));
            for (int row = 0; row < visibleRowCount() + 1; ++row) {
                const int realRow = row + int(m_scroll);
                if (realRow * cols >= count && row > 0) break;
                const QRectF cell = cellRect(row, 0);
                p.setPen(label);
                p.drawText(QRectF(378 * s, cell.top(), 50 * s, cell.height()), Qt::AlignRight | Qt::AlignVCenter, QString::number(realRow * cols));
            }
            p.restore();
            p.setFont(f);
        }
        p.save();
        p.setClipRect(r(436, 62, 733, 860));
        for (int row = 0; row < 17; ++row) {
            for (int c = 0; c < cols; ++c) {
                const int i = (row + int(m_scroll)) * cols + c;
                const QRectF cell = cellRect(row, c);
                if (i >= count) {
                    p.setPen(Qt::NoPen);
                    p.setBrush(QColor("#0f1325"));
                    p.drawRoundedRect(cell, 3 * s, 3 * s);
                    continue;
                }
                p.setPen(Qt::NoPen);
                p.setBrush(m_pal.colors[i]);
                p.drawRoundedRect(cell, 3 * s, 3 * s);
                if (i == m_selected) {
                    p.setBrush(Qt::NoBrush);
                    p.setPen(QPen(QColor("#6fe3ff"), qMax<qreal>(2.0, 3.2 * s)));
                    p.drawRoundedRect(cell.adjusted(-1, -1, 1, 1), 4 * s, 4 * s);
                }
            }
        }
        p.restore();
        // ---- the recent colors
        paintGroup(p, r(376, 944, 802, 133), 14 * s);
        p.setPen(label);
        p.setFont(fittedFont(f, tr("Recent Colors"), 170 * s));
        p.drawText(r(397, 950, 400, 32), Qt::AlignLeft | Qt::AlignVCenter, tr("Recent Colors"));
        p.setFont(f);
        paintWell(p, r(396, 991, 737, 69), 10 * s);
        for (int i = 0; i < 10; ++i) {
            const QRectF cell = recentRect(i);
            p.setPen(Qt::NoPen);
            p.setBrush(recent && i < recent->size() ? (*recent)[i] : QColor("#1a1f33"));
            p.drawRoundedRect(cell, 6 * s, 6 * s);
        }
    }

    void wheelEvent(QWheelEvent *e) override
    {
        m_scroll -= e->angleDelta().y() / 120.0 * 2.0;
        clampScroll();
        update();
    }
    void mousePressEvent(QMouseEvent *e) override
    {
        m_press = e->pos();
        m_pressScroll = m_scroll;
        m_dragged = false;
    }
    void mouseMoveEvent(QMouseEvent *e) override
    {
        if (!(e->buttons() & Qt::LeftButton) || !gridArea().contains(m_press)) return;
        const double dy = (e->pos().y() - m_press.y()) / scale();
        if (qAbs(dy) > 8) m_dragged = true;
        if (m_dragged) {
            m_scroll = m_pressScroll - dy / 53.3;
            clampScroll();
            update();
        }
    }
    void mouseReleaseEvent(QMouseEvent *e) override
    {
        if (!m_dragged) click(e->pos());
    }

private:
    QRectF commandRect(int i) const { return r(29, 127.6 + 48.6 * i, 325, 40); }
    QRectF toolRect(int i) const
    {
        static const double xs[4] = {29.4, 114.5, 201.6, 283.6};
        return r(xs[i], 976.6, 72.5, 55.8);
    }
    QRectF gridArea() const { return r(432, 60, 740, 864); }
    QRectF cellRect(int row, int col) const
    {
        const int cols = qMax(1, m_pal.columns);
        const double gapX = 3.45, gapY = 3.5;
        const double w = (731 - (cols - 1) * gapX) / cols;
        return r(436.5 + col * (w + gapX), 64.8 + row * 53.3, w, 49.8);
    }
    QRectF recentRect(int i) const { return r(400 + 66.9 * i, 996, 60, 59); }
    int visibleRowCount() const { return 16; }
    int totalRows() const { return (m_pal.colors.size() + qMax(1, m_pal.columns) - 1) / qMax(1, m_pal.columns); }
    void clampScroll() { m_scroll = qBound(0.0, m_scroll, double(qMax(0, totalRows() - visibleRowCount()))); }
    void ensureVisible(int index)
    {
        const int row = index / qMax(1, m_pal.columns);
        if (row < int(m_scroll)) m_scroll = row;
        else if (row >= int(m_scroll) + visibleRowCount()) m_scroll = row - visibleRowCount() + 1;
        clampScroll();
    }
    void syncBoxes()
    {
        m_syncing = true;
        const int n = m_pal.colors.size();
        indexBox->setRange(0, qMax(0, n - 1));
        indexBox->setValue(qMax(0, m_selected));
        const QColor c = (m_selected >= 0 && m_selected < n) ? m_pal.colors[m_selected] : QColor(0, 0, 0);
        rgbBox[0]->setValue(c.red());
        rgbBox[1]->setValue(c.green());
        rgbBox[2]->setValue(c.blue());
        hex->setText(c.name(QColor::HexRgb).toUpper());
        // a CMYK palette's swatch is edited with its four plates, an RGB palette's with R, G, B
        const bool cmyk = m_pal.isCmyk();
        for (int i = 0; i < 3; ++i) { rgbBox[i]->setVisible(!cmyk); m_rgbUp[i]->setVisible(!cmyk); m_rgbDown[i]->setVisible(!cmyk); }
        for (int i = 0; i < 4; ++i) {
            cmykBox[i]->setVisible(cmyk);
            m_cmykUp[i]->setVisible(cmyk);
            m_cmykDown[i]->setVisible(cmyk);
            cmykBox[i]->setValue((cmyk && m_selected >= 0 && m_selected < m_pal.cmyk.size()) ? m_pal.cmyk[m_selected][i] : 0);
        }
        m_syncing = false;
    }
    // change the selected swatch's color and save the palette
    void editSelected(const QColor &c)
    {
        if (m_selected < 0 || m_selected >= m_pal.colors.size()) return;
        QColor opaque = c;
        opaque.setAlpha(255);
        // m_pal.colors[m_selected] = opaque;
        m_pal.replace(m_selected, opaque);   // 2026-11-08: through the palette: a CMYK palette keeps the plates (made from this color)
        KrimblePaletteStore::save(m_pal);
        m_signature = KrimblePaletteStore::signature();
        syncBoxes();
        update();
        if (onPaletteChanged) onPaletteChanged();
        if (onColor) onColor(opaque);
    }
    // 2026-11-08: the four plates (percent) of the selected swatch of a CMYK palette
    void editSelectedCmyk(const KrimbleCmyk &v)
    {
        if (m_selected < 0 || m_selected >= m_pal.colors.size() || !m_pal.isCmyk()) return;
        m_pal.replaceCmyk(m_selected, v);
        KrimblePaletteStore::save(m_pal);
        m_signature = KrimblePaletteStore::signature();
        m_syncing = true;
        hex->setText(m_pal.colors[m_selected].name(QColor::HexRgb).toUpper());
        m_syncing = false;
        update();
        if (onPaletteChanged) onPaletteChanged();
        if (onColor) onColor(m_pal.colors[m_selected]);
    }
    void changed()
    {
        m_signature = QString();
        reload();
        if (onPaletteChanged) onPaletteChanged();
    }
    void click(const QPoint &pos)
    {
        // the palette chooser
        if (r(29, 60, 325, 51).contains(pos)) {
            QMenu menu(this);
            for (const QString &name : KrimblePaletteStore::names()) {
                QAction *a = menu.addAction(name);
                a->setCheckable(true);
                a->setChecked(name == m_pal.name);
            }
            QAction *chosen = menu.exec(mapToGlobal(pos));
            if (chosen) { KrimblePaletteStore::setActive(chosen->text()); changed(); }
            return;
        }
        // the seven commands
        for (int i = 0; i < 7; ++i)
            if (commandRect(i).contains(pos)) {
                bool did = false;
                switch (i) {
                case 0: did = KrimblePaletteActions::newPalette(this); break;
                case 1: did = KrimblePaletteActions::openPalette(this); break;
                case 2: did = KrimblePaletteActions::importPalette(this); break;
                case 3: KrimblePaletteActions::exportPalette(this); break;
                case 4: did = KrimblePaletteActions::duplicatePalette(this); break;
                case 5: did = KrimblePaletteActions::renamePalette(this); break;
                default: did = KrimblePaletteActions::deletePalette(this); break;
                }
                if (did) changed();
                return;
            }
        // the swatch tools: pick from the image, replace the swatch with the foreground color, reload from the file, the number of columns
        for (int i = 0; i < 4; ++i)
            if (toolRect(i).contains(pos)) {
                if (i == 0) { if (onPickFromImage) onPickFromImage(); }
                else if (i == 1) { if (state) editSelected(state->color); }
                else if (i == 2) { changed(); }
                else {
                    QMenu menu(this);
                    static const int choices[] = {8, 10, 12, 16, 19, 24, 32};
                    for (int c : choices) {
                        QAction *a = menu.addAction(tr("%1 columns").arg(c));
                        a->setData(c);
                        a->setCheckable(true);
                        a->setChecked(c == m_pal.columns);
                    }
                    QAction *chosen = menu.exec(mapToGlobal(pos));
                    if (chosen) {
                        m_pal.columns = chosen->data().toInt();
                        KrimblePaletteStore::save(m_pal);
                        changed();
                    }
                }
                return;
            }
        // the recent colors
        for (int i = 0; recent && i < recent->size() && i < 10; ++i)
            if (recentRect(i).contains(pos)) { if (onColor) onColor((*recent)[i]); return; }
        // the grid
        if (gridArea().contains(pos)) {
            const int cols = qMax(1, m_pal.columns);
            for (int row = 0; row < 17; ++row)
                for (int c = 0; c < cols; ++c)
                    if (cellRect(row, c).adjusted(-2, -2, 2, 2).contains(pos)) {
                        const int i = (row + int(m_scroll)) * cols + c;
                        if (i < m_pal.colors.size()) selectIndex(i, true);
                        return;
                    }
        }
    }

    KrimblePalette m_pal;
    int m_selected = -1;
    double m_scroll = 0;
    QString m_signature;
    QPoint m_press;
    double m_pressScroll = 0;
    bool m_dragged = false;
    bool m_syncing = false;
    QToolButton *m_indexUp = nullptr;
    QToolButton *m_indexDown = nullptr;
    QToolButton *m_rgbUp[3] = {nullptr, nullptr, nullptr};
    QToolButton *m_rgbDown[3] = {nullptr, nullptr, nullptr};
    QToolButton *m_cmykUp[4] = {nullptr, nullptr, nullptr, nullptr};
    QToolButton *m_cmykDown[4] = {nullptr, nullptr, nullptr, nullptr};
};
