// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-08: the History page of the Color panel, from the History mockup (George: "make the new tabs work just like the mockups"). Every color that becomes the foreground
// color is recorded with the tool that was active and the time; the list shows them (newest first) with a pin to keep a color in the Pinned Colors, the options group duplicates, show
// the tool and show the time, a limit, and Save History as Palette and Clear History. Measured in the mockup (the card scaled to 1178 units wide, 1062 tall).
#pragma once
#include <functional>
#include <QDateTime>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSettings>
#include <QSpinBox>
#include <QWheelEvent>
#include "krimble_color_pages.h"
#include "krimble_color_palettes.h"

struct KrimbleHistoryEntry
{
    QColor color;
    int alpha = 255;
    QString tool;
    QDateTime time;
};

class KrimbleHistoryStore
{
public:
    static QVector<KrimbleHistoryEntry> load()
    {
        QVector<KrimbleHistoryEntry> out;
        for (const QString &line : QSettings().value(QStringLiteral("Krimble/ColorStudioHistory")).toStringList()) {
            const QStringList f = line.split(QLatin1Char('|'));
            if (f.size() < 4) continue;
            KrimbleHistoryEntry e;
            e.color = QColor(f[0]);
            e.alpha = f[1].toInt();
            e.tool = f[2];
            e.time = QDateTime::fromString(f[3], Qt::ISODate);
            if (e.color.isValid()) out.append(e);
        }
        return out;
    }
    static void save(const QVector<KrimbleHistoryEntry> &entries)
    {
        QStringList lines;
        for (const KrimbleHistoryEntry &e : entries)
            lines << QStringLiteral("%1|%2|%3|%4").arg(e.color.name(QColor::HexRgb)).arg(e.alpha).arg(e.tool, e.time.toString(Qt::ISODate));
        QSettings().setValue(QStringLiteral("Krimble/ColorStudioHistory"), lines);
    }
    static QVector<QColor> pinned()
    {
        QVector<QColor> out;
        for (const QString &h : QSettings().value(QStringLiteral("Krimble/ColorStudioPinned")).toStringList()) { const QColor c(h); if (c.isValid()) out.append(c); }
        return out;
    }
    static void setPinned(const QVector<QColor> &colors)
    {
        QStringList l;
        for (const QColor &c : colors) l << c.name(QColor::HexRgb);
        QSettings().setValue(QStringLiteral("Krimble/ColorStudioPinned"), l);
    }
    static bool option(const char *key, bool fallback) { return QSettings().value(QStringLiteral("Krimble/ColorStudioHistory_") + QLatin1String(key), fallback).toBool(); }
    static void setOption(const char *key, bool v) { QSettings().setValue(QStringLiteral("Krimble/ColorStudioHistory_") + QLatin1String(key), v); }
    static int limit() { return QSettings().value(QStringLiteral("Krimble/ColorStudioHistory_limitValue"), 100).toInt(); }
    static void setLimit(int v) { QSettings().setValue(QStringLiteral("Krimble/ColorStudioHistory_limitValue"), v); }
    // record a color that became the foreground color (a color equal to the newest entry only updates its time when duplicates are grouped)
    static void add(const QColor &c, int alpha, const QString &tool, const QDateTime &when = QDateTime::currentDateTime())
    {
        QVector<KrimbleHistoryEntry> list = load();
        QColor opaque = c;
        opaque.setAlpha(255);
        if (option("group", true) && !list.isEmpty() && list.first().color == opaque && list.first().alpha == alpha) {
            list[0].time = when;
            list[0].tool = tool;
        } else {
            KrimbleHistoryEntry e;
            e.color = opaque;
            e.alpha = alpha;
            e.tool = tool;
            e.time = when;
            list.prepend(e);
        }
        int cap = option("limitOn", false) ? qMax(1, limit()) : 1000;
        while (list.size() > cap) list.removeLast();
        save(list);
    }
};

// the name of the tool for the list ("KritaShape/KisToolBrush" is "Brush")
inline QString krimbleToolName(const QString &id)
{
    if (id.contains(QLatin1String("Brush"), Qt::CaseInsensitive)) return QStringLiteral("Brush");
    if (id.contains(QLatin1String("Gradient"), Qt::CaseInsensitive)) return QStringLiteral("Gradient");
    if (id.contains(QLatin1String("Fill"), Qt::CaseInsensitive)) return QStringLiteral("Fill");
    if (id.contains(QLatin1String("ColorSampler"), Qt::CaseInsensitive) || id.contains(QLatin1String("Picker"), Qt::CaseInsensitive)) return QStringLiteral("Picker");
    QString name = id.section(QLatin1Char('/'), -1);
    name.remove(QLatin1String("KisTool"));
    return name.isEmpty() ? QStringLiteral("Brush") : name;
}

class KrimbleHistoryPage : public KrimbleColorBase
{
public:
    KrimbleColorState *state = nullptr;
    std::function<void(const QColor &)> onColor;      // a color chosen here becomes the foreground color
    std::function<void()> onPaletteChanged;           // a palette was made from the history
    QLineEdit *hex = nullptr;
    QToolButton *copy = nullptr;
    QSpinBox *limitBox = nullptr;

    double designHeight() const override { return 1062.0; }

    explicit KrimbleHistoryPage(QWidget *parent = nullptr) : KrimbleColorBase(parent)
    {
        hex = new QLineEdit(this);
        hex->setMaxLength(9);
        copy = makeButton();
        copy->setToolTip(tr("Copy the HEX code"));
        QObject::connect(copy, &QToolButton::clicked, this, [this]() { QApplication::clipboard()->setText(hex->text()); });
        QObject::connect(hex, &QLineEdit::editingFinished, this, [this]() { const QColor c(hex->text()); if (c.isValid() && onColor) onColor(c); });
        limitBox = new QSpinBox(this);
        limitBox->setRange(1, 1000);
        limitBox->setValue(KrimbleHistoryStore::limit());
        QObject::connect(limitBox, qOverload<int>(&QSpinBox::valueChanged), this, [this](int v) { KrimbleHistoryStore::setLimit(v); });
        m_limitUp = makeButton();
        m_limitDown = makeButton();
        m_limitUp->setAutoRepeat(true);
        m_limitDown->setAutoRepeat(true);
        QObject::connect(m_limitUp, &QToolButton::clicked, limitBox, [this]() { limitBox->stepUp(); });
        QObject::connect(m_limitDown, &QToolButton::clicked, limitBox, [this]() { limitBox->stepDown(); });
        setMouseTracking(false);
    }

    void refresh()
    {
        m_entries = KrimbleHistoryStore::load();
        m_pinned = KrimbleHistoryStore::pinned();
        if (state) {
            QSignalBlocker b(hex);
            hex->setText(state->color.name(QColor::HexRgb).toUpper());
        }
        clampScroll();
        update();
    }

    void relayout()
    {
        const qreal s = scale();
        QFont f = font();
        f.setPixelSize(qMax(8, qRound(33 * s)));
        f = fittedFont(f, QStringLiteral("Group consecutive duplicates"), 286 * s);
        hex->setGeometry(r(195, 87, 133, 38).toAlignedRect());
        hex->setFont(f);
        hex->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; padding: 0px 0px 0px %1px; color: #f4f6fc; selection-background-color: #1196d8; }").arg(qRound(14 * s)));
        copy->setGeometry(r(328, 87, 38, 38).toAlignedRect());
        const QRect whole = r(255.6, 758, 107, 39).toAlignedRect();
        const int arrowW = qRound(38 * s);
        limitBox->setGeometry(whole.left(), whole.top(), whole.width() - arrowW, whole.height());
        limitBox->setFont(f);
        limitBox->setStyleSheet(QStringLiteral("QSpinBox { background: transparent; border: none; padding: 0px 0px 0px %1px; color: #f4f6fc; selection-background-color: #1196d8; }"
                                               "QSpinBox QLineEdit { background: transparent; border: none; padding: 0px; }"
                                               "QSpinBox::up-button, QSpinBox::down-button { width: 0px; height: 0px; border: none; }").arg(qRound(12 * s)));
        m_limitUp->setGeometry(whole.right() - arrowW + 1, whole.top(), arrowW, whole.height() / 2);
        m_limitDown->setGeometry(whole.right() - arrowW + 1, whole.top() + whole.height() / 2, arrowW, whole.height() - whole.height() / 2);
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
        // the card
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
        f.setPixelSize(qMax(8, qRound(33 * s)));
        f = fittedFont(f, QStringLiteral("Group consecutive duplicates"), 286 * s);   // 2026-11-08: the left column's text is one size (measured: that text is 286 units wide)
        p.setFont(f);
        const QColor label("#d9deee");
        const QColor shown = state ? state->color : QColor("#459ad3");
        const int alpha = state ? state->alpha : 255;
        QFont large = font();
        large.setPixelSize(qMax(8, qRound(60 * s)));   // the start of every fit below: the fit makes it smaller, to the width measured in the mockup
        // ---- the left group
        paintGroup(p, r(9, 13, 372, 1031), 14 * s);
        p.setPen(label);
        p.setFont(fittedFont(large, tr("Current Color"), 179 * s));
        p.drawText(r(25, 13, 300, 66), Qt::AlignLeft | Qt::AlignVCenter, tr("Current Color"));
        p.setFont(f);
        paintColorBox(p, r(23, 93, 95, 91), shown, 16 * s, s, true);
        p.setPen(label);
        p.drawText(r(134, 87, 60, 38), Qt::AlignLeft | Qt::AlignVCenter, tr("HEX:"));
        paintField(p, r(195, 87, 171, 38), 8 * s);
        paintButton(p, r(328, 87, 38, 38), 9 * s);
        drawSymbol(p, QStringLiteral("copy"), r(328, 87, 38, 38).center(), 0.8);
        const QColor opaque = shown;
        const QString rgbText = QStringLiteral("%1, %2, %3").arg(opaque.red()).arg(opaque.green()).arg(opaque.blue());
        const QString hsvText = QStringLiteral("%1\u00B0, %2%, %3%").arg(qMax(0, opaque.hsvHue() < 0 ? (state ? state->hue : 0) : opaque.hsvHue())).arg(qRound(opaque.hsvSaturationF() * 100)).arg(qRound(opaque.valueF() * 100));
        const QString hslText = QStringLiteral("%1\u00B0, %2%, %3%").arg(qMax(0, opaque.hslHue() < 0 ? (state ? state->hue : 0) : opaque.hslHue())).arg(qRound(opaque.hslSaturationF() * 100)).arg(qRound(opaque.lightnessF() * 100));
        static const char *labels[3] = {"RGB:", "HSV:", "HSL:"};
        const QString texts[3] = {rgbText, hsvText, hslText};
        for (int i = 0; i < 3; ++i) {
            const double cy = 154.6 + 44.8 * i;
            p.setPen(label);
            p.drawText(r(134, cy - 20, 60, 40), Qt::AlignLeft | Qt::AlignVCenter, tr(labels[i]));
            p.setPen(QColor("#f1f4fc"));
            p.drawText(r(195, cy - 20, 180, 40), Qt::AlignLeft | Qt::AlignVCenter, texts[i]);
        }
        paintBrightLine(p, 25, 286, 366, s);
        drawIcon(p, QStringLiteral("pin_title"), r(41, 319, 0, 0).topLeft(), 1.0);
        p.setPen(label);
        p.setFont(fittedFont(large, tr("Pinned Colors"), 150 * s));
        p.drawText(r(68, 297, 300, 44), Qt::AlignLeft | Qt::AlignVCenter, tr("Pinned Colors"));
        p.setFont(f);
        paintBrightLine(p, 25, 342.5, 366, s);
        for (int i = 0; i < 8; ++i) {
            const QRectF cell = pinnedRect(i);
            if (i < m_pinned.size()) paintColorBox(p, cell, m_pinned[i], 14 * s, s, false);
            else {
                p.setPen(QPen(QColor(8, 11, 26, 220), 1.4));
                p.setBrush(QColor("#161c2e"));
                p.drawRoundedRect(cell, 14 * s, 14 * s);
            }
        }
        paintBrightLine(p, 25, 550.6, 366, s);
        p.setPen(label);
        p.setFont(fittedFont(large, tr("History Options"), 190 * s));
        p.drawText(r(25, 560, 330, 46), Qt::AlignLeft | Qt::AlignVCenter, tr("History Options"));
        p.setFont(f);
        static const char *optionTexts[3] = {"Group consecutive duplicates", "Show source (tool)", "Show timestamp"};
        static const char *optionKeys[3] = {"group", "source", "time"};
        for (int i = 0; i < 3; ++i) {
            paintCheck(p, checkRect(i), KrimbleHistoryStore::option(optionKeys[i], true), s);
            p.setPen(label);
            p.drawText(r(74, checkRect(i).top() / s - 4, 300, 42), Qt::AlignLeft | Qt::AlignVCenter, tr(optionTexts[i]));
        }
        paintCheck(p, checkRect(3), KrimbleHistoryStore::option("limitOn", false), s);
        p.setPen(label);
        p.drawText(r(74, 757, 180, 42), Qt::AlignLeft | Qt::AlignVCenter, tr("Limit history to:"));
        paintSpinFrame(p, r(255.6, 758, 107, 39));
        static const char *buttonIcons[3] = {"pin_button", "row_palette", nullptr};
        static const char *buttonTexts[3] = {"Pin Current Color", "Save History as Palette...", "Clear History..."};
        for (int i = 0; i < 3; ++i) {
            const QRectF b = buttonRect(i);
            paintButton(p, b, 11 * s);
            if (buttonIcons[i]) drawIcon(p, QString::fromLatin1(buttonIcons[i]), QPointF(b.left() + 36 * s, b.center().y()), 0.9);
            else drawSymbol(p, QStringLiteral("trash"), QPointF(b.left() + 36 * s, b.center().y()), 0.8);
            p.setPen(QColor("#f1f4fc"));
            p.drawText(QRectF(b.left() + 72 * s, b.top(), b.width() - 76 * s, b.height()), Qt::AlignLeft | Qt::AlignVCenter, tr(buttonTexts[i]));
        }
        // ---- the right group: the title, the filter, the list or grid
        paintGroup(p, r(392, 13, 786, 1031), 14 * s);
        p.setPen(label);
        p.setFont(fittedFont(large, tr("Color History (42)"), 226 * s));
        p.drawText(r(405, 13, 400, 66), Qt::AlignLeft | Qt::AlignVCenter, tr("Color History (%1)").arg(m_entries.size()));
        p.setFont(f);
        paintButton(p, r(820, 23, 186, 48), 11 * s);
        drawIcon(p, QStringLiteral("filter"), r(846, 47, 0, 0).topLeft(), 0.8);
        p.setPen(QColor("#f1f4fc"));
        p.setFont(fittedFont(large, QStringLiteral("All Tools"), 90 * s));
        p.drawText(r(872, 23, 100, 48), Qt::AlignLeft | Qt::AlignVCenter, m_filter.isEmpty() ? tr("All Tools") : m_filter);
        p.setFont(f);
        drawSymbol(p, QStringLiteral("chev_drop"), r(985, 47, 0, 0).topLeft(), 0.8);
        if (!m_gridView) paintSelected(p, r(1043, 21, 53, 50), 10 * s, s); else paintButton(p, r(1043, 21, 53, 50), 10 * s);
        drawSymbol(p, QStringLiteral("grid"), r(1069.5, 46, 0, 0).topLeft(), 0.8);
        if (m_gridView) paintSelected(p, r(1106, 21, 53, 50), 10 * s, s); else paintButton(p, r(1106, 21, 53, 50), 10 * s);
        drawIcon(p, QStringLiteral("list"), r(1132.5, 46, 0, 0).topLeft(), 0.9);
        // the list
        p.save();
        p.setClipRect(r(396, 88, 770, 936));
        const QVector<int> shown_rows = visibleRows();
        bool selectedDone = false;
        if (!m_gridView) {
            for (int v = 0; v < shown_rows.size(); ++v) {
                const QRectF row = rowRect(v);
                if (row.bottom() < 88 * s || row.top() > 1024 * s) continue;
                const KrimbleHistoryEntry &e = m_entries[shown_rows[v]];
                const bool current = !selectedDone && e.color == shown && e.alpha == alpha;
                if (current) { selectedDone = true; paintSelected(p, row, 12 * s, s); }
                else paintGroup(p, row, 12 * s);
                paintColorBox(p, QRectF(row.left() + 9 * s, row.top() + 8 * s, 59 * s, row.height() - 16 * s), e.color, 10 * s, s, false);
                p.setPen(QColor("#f4f6fc"));
                QFont big = fittedFont(large, QStringLiteral("#F035BE"), 99 * s);   // measured: 99 units wide
                p.setFont(big);
                p.drawText(QRectF(row.left() + 90 * s, row.top(), 140 * s, row.height()), Qt::AlignLeft | Qt::AlignVCenter, e.color.name(QColor::HexRgb).toUpper());
                QFont small = fittedFont(large, QStringLiteral("HSL: 316\u00B0, 86%, 57%"), 186 * s);   // measured: 186 units wide
                p.setFont(small);
                p.setPen(QColor("#d9deee"));
                p.drawText(QRectF(row.left() + 228 * s, row.top() + 6 * s, 220 * s, row.height() / 2 - 6 * s), Qt::AlignLeft | Qt::AlignVCenter,
                           QStringLiteral("RGB: %1, %2, %3").arg(e.color.red()).arg(e.color.green()).arg(e.color.blue()));
                p.drawText(QRectF(row.left() + 228 * s, row.top() + row.height() / 2, 220 * s, row.height() / 2 - 6 * s), Qt::AlignLeft | Qt::AlignVCenter,
                           QStringLiteral("HSL: %1\u00B0, %2%, %3%").arg(qMax(0, e.color.hslHue())).arg(qRound(e.color.hslSaturationF() * 100)).arg(qRound(e.color.lightnessF() * 100)));
                p.setFont(f);
                if (KrimbleHistoryStore::option("source", true)) {
                    const QString icon = e.tool == QLatin1String("Gradient") ? QStringLiteral("tool_gradient") : (e.tool == QLatin1String("Fill") ? QStringLiteral("tool_fill")
                                          : (e.tool == QLatin1String("Picker") ? QStringLiteral("tool_picker") : QStringLiteral("tool_brush")));
                    drawIcon(p, icon, QPointF(row.left() + 463 * s, row.center().y()), 1.0);
                    p.setPen(QColor("#e8ebf6"));
                    p.setFont(fittedFont(large, QStringLiteral("Gradient"), 76 * s));
                    p.drawText(QRectF(row.left() + 486 * s, row.top(), 140 * s, row.height()), Qt::AlignLeft | Qt::AlignVCenter, e.tool);
                }
                if (KrimbleHistoryStore::option("time", true)) {
                    p.setPen(QColor("#e8ebf6"));
                    p.setFont(fittedFont(large, QStringLiteral("11:42 AM"), 79 * s));
                    p.drawText(QRectF(row.left() + 560 * s, row.top(), 105 * s, row.height()), Qt::AlignRight | Qt::AlignVCenter, e.time.toString(QStringLiteral("h:mm AP")));
                }
                drawIcon(p, isPinned(e.color) ? QStringLiteral("pin_on") : QStringLiteral("pin_off"), QPointF(row.left() + 711 * s, row.center().y()), 1.0);
            }
        } else {
            for (int v = 0; v < shown_rows.size(); ++v) {
                const QRectF cell = gridRect(v);
                if (cell.bottom() < 88 * s || cell.top() > 1024 * s) continue;
                paintColorBox(p, cell, m_entries[shown_rows[v]].color, 8 * s, s, false);
            }
        }
        p.restore();
        // the scroll bar at the right of the list
        {
            const double total = contentHeight();
            const double viewH = 930;
            const QRectF track = r(1154, 94, 15, 926);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor("#10162a"));
            p.drawRoundedRect(track, 7 * s, 7 * s);
            if (total > viewH) {
                const double ratio = viewH / total;
                const double thumbH = qMax(40.0, 926 * ratio);
                const double thumbY = 94 + (926 - thumbH) * (m_scroll / qMax(1.0, total - viewH));
                p.setBrush(QColor("#2f7fc6"));
                p.drawRoundedRect(r(1155, thumbY, 13, thumbH), 6 * s, 6 * s);
            }
        }
    }

    void wheelEvent(QWheelEvent *e) override
    {
        m_scroll -= e->angleDelta().y() / 120.0 * 74.0;
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
        if (!(e->buttons() & Qt::LeftButton)) return;
        const int dy = e->pos().y() - m_press.y();
        if (qAbs(dy) > 8) m_dragged = true;
        if (m_dragged && listArea().contains(m_press)) {
            m_scroll = m_pressScroll - dy / scale();
            clampScroll();
            update();
        }
    }
    void mouseReleaseEvent(QMouseEvent *e) override
    {
        if (m_dragged) return;
        click(e->pos());
    }
    void mouseDoubleClickEvent(QMouseEvent *e) override
    {
        // a double tap on a pinned color unpins it
        for (int i = 0; i < m_pinned.size(); ++i)
            if (pinnedRect(i).contains(e->pos())) {
                m_pinned.removeAt(i);
                KrimbleHistoryStore::setPinned(m_pinned);
                update();
                return;
            }
    }

private:
    QRectF pinnedRect(int i) const { return r(23.2 + 86.9 * (i % 4), 353.6 + 90.0 * (i / 4), 74.8, 75.8); }
    QRectF checkRect(int i) const { return r(29, 613 + 48 * i, 34, 34); }
    QRectF buttonRect(int i) const { return r(23, 833 + 66 * i, 350, 53); }
    QRectF listArea() const { return r(396, 88, 770, 936); }
    QRectF rowRect(int v) const { return r(401, 94 + 74.3 * v - m_scroll, 743, 72.8); }
    QRectF gridRect(int v) const { return r(410 + 76.5 * (v % 9), 100 + 76.5 * (v / 9) - m_scroll, 70, 70); }
    double contentHeight() const
    {
        const int n = visibleRows().size();
        return m_gridView ? 8 + 76.5 * ((n + 8) / 9) : 74.3 * n;
    }
    void clampScroll() { m_scroll = qBound(0.0, m_scroll, qMax(0.0, contentHeight() - 930.0)); }
    QVector<int> visibleRows() const
    {
        QVector<int> rows;
        for (int i = 0; i < m_entries.size(); ++i)
            if (m_filter.isEmpty() || m_entries[i].tool == m_filter) rows.append(i);
        return rows;
    }
    bool isPinned(const QColor &c) const { return m_pinned.contains(c); }
    void paintCheck(QPainter &p, const QRectF &box, bool on, qreal s)
    {
        paintField(p, box, 8 * s);
        if (!on) return;
        p.setPen(QPen(QColor("#35d8f8"), qMax<qreal>(2.5, 4.5 * s), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        QPainterPath tick;
        tick.moveTo(box.left() + box.width() * 0.22, box.top() + box.height() * 0.54);
        tick.lineTo(box.left() + box.width() * 0.43, box.top() + box.height() * 0.74);
        tick.lineTo(box.left() + box.width() * 0.80, box.top() + box.height() * 0.28);
        p.drawPath(tick);
    }
    void click(const QPoint &pos)
    {
        // the options
        static const char *keys[3] = {"group", "source", "time"};
        for (int i = 0; i < 3; ++i)
            if (checkRect(i).adjusted(-4, -6, 280 * scale(), 6).contains(pos)) {
                KrimbleHistoryStore::setOption(keys[i], !KrimbleHistoryStore::option(keys[i], true));
                update();
                return;
            }
        if (checkRect(3).adjusted(-4, -6, 190 * scale(), 6).contains(pos)) {
            KrimbleHistoryStore::setOption("limitOn", !KrimbleHistoryStore::option("limitOn", false));
            update();
            return;
        }
        // the pinned colors
        for (int i = 0; i < m_pinned.size(); ++i)
            if (pinnedRect(i).contains(pos)) { if (onColor) onColor(m_pinned[i]); return; }
        // the three buttons
        if (buttonRect(0).contains(pos)) { pinCurrent(); return; }
        if (buttonRect(1).contains(pos)) { saveAsPalette(); return; }
        if (buttonRect(2).contains(pos)) { clearHistory(); return; }
        // the filter and the view buttons
        if (r(820, 23, 186, 48).contains(pos)) { chooseFilter(pos); return; }
        if (r(1043, 21, 53, 50).contains(pos)) { m_gridView = true; clampScroll(); update(); return; }
        if (r(1106, 21, 53, 50).contains(pos)) { m_gridView = false; clampScroll(); update(); return; }
        // the list: a tap on the pin toggles it, a tap elsewhere in a row sets the color
        if (!listArea().contains(pos)) return;
        const QVector<int> rows = visibleRows();
        for (int v = 0; v < rows.size(); ++v) {
            const KrimbleHistoryEntry &e = m_entries[rows[v]];
            if (!m_gridView) {
                const QRectF row = rowRect(v);
                if (!row.contains(pos)) continue;
                if (pos.x() > row.left() + 670 * scale()) togglePin(e.color);
                else if (onColor) onColor(e.color);
                return;
            } else if (gridRect(v).contains(pos)) {
                if (onColor) onColor(e.color);
                return;
            }
        }
    }
    void togglePin(const QColor &c)
    {
        if (m_pinned.contains(c)) m_pinned.removeAll(c);
        else {
            if (m_pinned.size() >= 8) m_pinned.removeLast();
            m_pinned.append(c);
        }
        KrimbleHistoryStore::setPinned(m_pinned);
        update();
    }
    void pinCurrent()
    {
        if (state) togglePin(state->color);
    }
    void chooseFilter(const QPoint &pos)
    {
        QMenu menu(this);
        QAction *all = menu.addAction(tr("All Tools"));
        QStringList tools;
        for (const KrimbleHistoryEntry &e : m_entries) if (!tools.contains(e.tool)) tools << e.tool;
        for (const QString &t : tools) menu.addAction(t);
        QAction *chosen = menu.exec(mapToGlobal(pos));
        if (!chosen) return;
        m_filter = chosen == all ? QString() : chosen->text();
        m_scroll = 0;
        update();
    }
    void saveAsPalette()
    {
        bool ok = false;
        const QString name = QInputDialog::getText(this, tr("Save History as Palette"), tr("Palette name:"), QLineEdit::Normal, tr("History"), &ok).trimmed();
        if (!ok || name.isEmpty()) return;
        KrimblePalette pal;
        pal.name = KrimblePaletteStore::uniqueName(name);
        pal.columns = 16;
        for (const KrimbleHistoryEntry &e : m_entries) if (!pal.colors.contains(e.color)) pal.colors.append(e.color);
        if (KrimblePaletteStore::save(pal)) {
            KrimblePaletteStore::setActive(pal.name);
            if (onPaletteChanged) onPaletteChanged();
        }
    }
    void clearHistory()
    {
        if (QMessageBox::question(this, tr("Clear History"), tr("Remove all %1 colors from the history? Pinned colors stay.").arg(m_entries.size())) != QMessageBox::Yes) return;
        KrimbleHistoryStore::save(QVector<KrimbleHistoryEntry>());
        refresh();
    }

    QVector<KrimbleHistoryEntry> m_entries;
    QVector<QColor> m_pinned;
    QString m_filter;
    bool m_gridView = false;
    double m_scroll = 0;
    QPoint m_press;
    double m_pressScroll = 0;
    bool m_dragged = false;
    QToolButton *m_limitUp = nullptr;
    QToolButton *m_limitDown = nullptr;
};
