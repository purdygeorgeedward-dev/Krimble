// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-08: the Sliders and Values pages of the Color panel, from the new mockups (George: "make the new tabs work just like the mockups"). The measurements are the mockups'
// pixels converted to the card's 1178 design units (the card of each mockup scaled to the same width), see color_pages_measurements.md. Both pages are drawn at the same scale as
// the Picker page (krimble_color_paint.h) and share its bottom part (the foreground and background colors, recent colors, the palette).
#pragma once
#include <functional>
#include <QApplication>
#include <QClipboard>
#include <QLineEdit>
#include <QMouseEvent>
#include <QSignalBlocker>
#include <QSpinBox>
#include "krimble_color_paint.h"

// the color the panel is working on: opaque RGB, the alpha channel apart, and the hue to keep when a color is gray (a gray has no hue)
struct KrimbleColorState
{
    QColor color = QColor("#459ad3");
    int alpha = 255;
    int hue = 204;
};

// the channels of each color model: 0 RGB, 1 HSV, 2 HSL, 3 CMYK, 4 Grayscale
struct KrimbleChannels
{
    static int count(int model) { return model == 3 ? 4 : (model == 4 ? 1 : 3); }
    static QString modelName(int model)
    {
        static const char *names[] = {"RGB", "HSV", "HSL", "CMYK", "Grayscale"};
        return QString::fromLatin1(names[qBound(0, model, 4)]);
    }
    static QString label(int model, int i)
    {
        static const char *rgb[] = {"R", "G", "B"};
        static const char *hsv[] = {"H", "S", "V"};
        static const char *hsl[] = {"H", "S", "L"};
        static const char *cmyk[] = {"C", "M", "Y", "K"};
        switch (model) {
        case 0: return QString::fromLatin1(rgb[qBound(0, i, 2)]);
        case 1: return QString::fromLatin1(hsv[qBound(0, i, 2)]);
        case 2: return QString::fromLatin1(hsl[qBound(0, i, 2)]);
        case 3: return QString::fromLatin1(cmyk[qBound(0, i, 3)]);
        default: return QStringLiteral("Y");
        }
    }
    static int maximum(int model, int i)
    {
        if (model == 0 || model == 4) return 255;
        if ((model == 1 || model == 2) && i == 0) return 359;
        return 100;
    }
    static QString unit(int model, int i)
    {
        if ((model == 1 || model == 2) && i == 0) return QStringLiteral("\u00B0");
        if (model == 1 || model == 2 || model == 3) return QStringLiteral("%");
        return QString();
    }
    static int hueOf(const QColor &c, int hint, bool hsl)
    {
        const int h = hsl ? c.hslHue() : c.hsvHue();
        return h < 0 ? hint : h;
    }
    static int value(int model, int i, const QColor &c, int hint)
    {
        switch (model) {
        case 0: return i == 0 ? c.red() : (i == 1 ? c.green() : c.blue());
        case 1: return i == 0 ? hueOf(c, hint, false) : qRound((i == 1 ? c.hsvSaturationF() : c.valueF()) * 100.0);
        case 2: return i == 0 ? hueOf(c, hint, true) : qRound((i == 1 ? c.hslSaturationF() : c.lightnessF()) * 100.0);
        case 3: return qRound((i == 0 ? c.cyanF() : (i == 1 ? c.magentaF() : (i == 2 ? c.yellowF() : c.blackF()))) * 100.0);
        default: return qRound(0.299 * c.red() + 0.587 * c.green() + 0.114 * c.blue());
        }
    }
    // the color after channel i of the model is set to v (the other channels as they are)
    static QColor withValue(int model, int i, int v, const QColor &c, int hint)
    {
        v = qBound(0, v, maximum(model, i));
        switch (model) {
        case 0: {
            int r = c.red(), g = c.green(), b = c.blue();
            (i == 0 ? r : (i == 1 ? g : b)) = v;
            return QColor(r, g, b);
        }
        case 1: {
            int h = hueOf(c, hint, false), s = qRound(c.hsvSaturationF() * 100.0), val = qRound(c.valueF() * 100.0);
            (i == 0 ? h : (i == 1 ? s : val)) = v;
            return QColor::fromHsvF(qMin(0.9999, h / 360.0), s / 100.0, val / 100.0);
        }
        case 2: {
            int h = hueOf(c, hint, true), s = qRound(c.hslSaturationF() * 100.0), l = qRound(c.lightnessF() * 100.0);
            (i == 0 ? h : (i == 1 ? s : l)) = v;
            return QColor::fromHslF(qMin(0.9999, h / 360.0), s / 100.0, l / 100.0);
        }
        case 3: {
            double cc = c.cyanF(), m = c.magentaF(), y = c.yellowF(), k = c.blackF();
            (i == 0 ? cc : (i == 1 ? m : (i == 2 ? y : k))) = v / 100.0;
            return QColor::fromCmykF(cc, m, y, k);
        }
        default:
            return QColor(v, v, v);
        }
    }
    // the color at fraction t (0 to 1) along the track of channel i. RGB tracks are the mockup's: the channel's own color fading to a pale tint of it (measured: #F80302 to #FED2D8 for R,
    // #09E80C to #D0FED8 for G, #1010F9 to #A8AAFC for B); the others show what the channel does (hue: the rainbow; saturation: gray to the color; value and lightness: dark to light;
    // the inks: white to the ink; gray: black to white)
    static QColor trackAt(int model, int i, const QColor &c, int hint, double t)
    {
        if (model == 0) {
            static const QColor pure[3] = {QColor(248, 3, 2), QColor(9, 232, 12), QColor(16, 16, 249)};
            static const QColor pale[3] = {QColor(254, 210, 216), QColor(208, 254, 216), QColor(168, 170, 252)};
            const int k = qBound(0, i, 2);
            return QColor::fromRgbF(pure[k].redF() + (pale[k].redF() - pure[k].redF()) * t, pure[k].greenF() + (pale[k].greenF() - pure[k].greenF()) * t,
                                    pure[k].blueF() + (pale[k].blueF() - pure[k].blueF()) * t);
        }
        if (model == 3) {
            static const QColor ink[4] = {QColor(0, 200, 255), QColor(255, 0, 200), QColor(255, 230, 0), QColor(0, 0, 0)};
            const int k = qBound(0, i, 3);
            return QColor::fromRgbF(1.0 + (ink[k].redF() - 1.0) * t, 1.0 + (ink[k].greenF() - 1.0) * t, 1.0 + (ink[k].blueF() - 1.0) * t);
        }
        return withValue(model, i, qRound(t * maximum(model, i)), c, hint);
    }
};

// a channel bar: a pill track filled with the colors the channel runs through (left to right), a glossy ball
class KrimbleSliderBar : public QWidget
{
public:
    std::function<QColor(double)> trackColor;   // the color at 0 to 1 along the track (alpha included for the alpha track)
    std::function<void(int)> onChanged;
    bool alphaTrack = false;
    QColor ballOverride;   // the ball's own color when it is not the track's color at the ball (the RGB bars have the channel's color: the mockup's red, green and blue balls)
    int maximum = 255;
    explicit KrimbleSliderBar(QWidget *parent = nullptr) : QWidget(parent) { setCursor(Qt::PointingHandCursor); }
    void setValue(int v)
    {
        v = qBound(0, v, maximum);
        if (v != m_value) { m_value = v; update(); }
    }
    int value() const { return m_value; }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHints(QPainter::Antialiasing);
        const qreal h = height();
        const qreal ballR = h * 0.39;
        const QRectF track(1.5, h / 2 - h * 0.19, width() - 3.0, h * 0.38);
        const qreal rad = track.height() / 2;
        if (alphaTrack) KrimbleColorBase::paintChecker(p, track, track.height() / 2.4, rad);
        QLinearGradient g(track.left(), 0, track.right(), 0);
        for (int i = 0; i <= 24; ++i) {
            const double t = i / 24.0;
            g.setColorAt(t, trackColor ? trackColor(t) : QColor(Qt::gray));
        }
        p.setPen(Qt::NoPen);
        p.setBrush(g);
        p.drawRoundedRect(track, rad, rad);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(6, 10, 24, 220), 1.4));
        p.drawRoundedRect(track, rad, rad);
        p.setPen(QPen(QColor(255, 255, 255, 70), 1.2));
        p.drawLine(QPointF(track.left() + rad, track.top() + 1.6), QPointF(track.right() - rad, track.top() + 1.6));
        // the ball
        const double t = maximum > 0 ? double(m_value) / maximum : 0.0;
        const QPointF center(ballR + (width() - 2 * ballR) * t, h / 2);
        QColor ballColor = ballOverride.isValid() ? ballOverride : (trackColor ? trackColor(t) : QColor(Qt::gray));
        ballColor.setAlpha(255);
        QRadialGradient ball(center + QPointF(-ballR * 0.25, -ballR * 0.35), ballR * 1.25);
        ball.setColorAt(0.0, ballColor.lighter(190));
        ball.setColorAt(0.45, ballColor);
        ball.setColorAt(1.0, ballColor.darker(165));
        p.setPen(QPen(QColor(6, 10, 24, 230), 2.0));
        p.setBrush(ball);
        p.drawEllipse(center, ballR, ballR);
        p.setPen(QPen(QColor(235, 240, 255, 200), 1.6));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(center, ballR - 1.6, ballR - 1.6);
    }
    void mousePressEvent(QMouseEvent *e) override { pick(e->pos().x()); }
    void mouseMoveEvent(QMouseEvent *e) override { if (e->buttons() & Qt::LeftButton) pick(e->pos().x()); }
private:
    void pick(int x)
    {
        const qreal ballR = height() * 0.39;
        const double t = qBound(0.0, (x - ballR) / qMax<qreal>(1.0, width() - 2 * ballR), 1.0);
        const int v = qRound(t * maximum);
        setValue(v);
        if (onChanged) onChanged(v);
    }
    int m_value = 0;
};

// Sliders page: the color model buttons at the left; at the right the channel sliders with their number boxes, then the color preview with HEX, copy and the alpha preview
class KrimbleSlidersTop : public KrimbleColorBase
{
public:
    KrimbleColorState *state = nullptr;
    std::function<void(const QColor &)> onColor;   // a new opaque color from a slider or a box
    std::function<void(int)> onAlpha;
    QLineEdit *hex = nullptr;
    QToolButton *copy = nullptr;
    int model = 0;

    double designHeight() const override { return 590.0; }

    explicit KrimbleSlidersTop(QWidget *parent = nullptr) : KrimbleColorBase(parent)
    {
        for (int i = 0; i < 5; ++i) {
            m_model[i] = makeButton();
            QObject::connect(m_model[i], &QToolButton::clicked, this, [this, i]() { setModel(i); });
            m_bar[i] = new KrimbleSliderBar(this);
            m_spin[i] = new QSpinBox(this);
            m_spin[i]->setRange(0, 255);
            m_bar[i]->onChanged = [this, i](int v) { channelChanged(i, v); };
            QObject::connect(m_spin[i], qOverload<int>(&QSpinBox::valueChanged), this, [this, i](int v) { if (!m_syncing) channelChanged(i, v); });
            m_up[i] = makeButton();
            m_down[i] = makeButton();
            m_up[i]->setAutoRepeat(true);
            m_down[i]->setAutoRepeat(true);
            QSpinBox *box = m_spin[i];
            QObject::connect(m_up[i], &QToolButton::clicked, box, [box]() { box->stepUp(); });
            QObject::connect(m_down[i], &QToolButton::clicked, box, [box]() { box->stepDown(); });
        }
        hex = new QLineEdit(this);
        hex->setMaxLength(9);
        copy = makeButton();
        copy->setToolTip(tr("Copy the HEX code"));
        QObject::connect(copy, &QToolButton::clicked, this, [this]() { QApplication::clipboard()->setText(hex->text()); });
        QObject::connect(hex, &QLineEdit::editingFinished, this, [this]() {
            QColor c(hex->text());
            if (c.isValid() && onColor) onColor(c);
        });
        setModel(0);
    }

    void setModel(int m)
    {
        model = qBound(0, m, 4);
        refresh();
    }
    int rows() const { return KrimbleChannels::count(model) + 1; }   // the channels and the alpha

    // read the state into the bars, boxes and HEX
    void refresh()
    {
        if (!state) return;
        m_syncing = true;
        const int n = KrimbleChannels::count(model);
        for (int i = 0; i < 5; ++i) {
            const bool channel = i < n;
            const bool alphaRow = (i == n);
            const bool used = channel || alphaRow;
            m_bar[i]->setVisible(used);
            m_spin[i]->setVisible(used);
            m_up[i]->setVisible(used);
            m_down[i]->setVisible(used);
            if (!used) continue;
            if (channel) {
                const int maxv = KrimbleChannels::maximum(model, i);
                m_bar[i]->maximum = maxv;
                m_bar[i]->alphaTrack = false;
                {
                    static const QColor rgbBall[3] = {QColor(240, 40, 50), QColor(30, 220, 50), QColor(40, 60, 240)};
                    m_bar[i]->ballOverride = (model == 0) ? rgbBall[qBound(0, i, 2)] : QColor();
                }
                m_spin[i]->setRange(0, maxv);
                const int v = KrimbleChannels::value(model, i, state->color, state->hue);
                m_bar[i]->setValue(v);
                QSignalBlocker b(m_spin[i]);
                m_spin[i]->setValue(v);
                const int mo = model;
                const int ch = i;
                m_bar[i]->trackColor = [this, mo, ch](double t) { return KrimbleChannels::trackAt(mo, ch, state->color, state->hue, t); };
            } else {
                m_bar[i]->maximum = 255;
                m_bar[i]->alphaTrack = true;
                m_bar[i]->ballOverride = QColor(240, 40, 190);   // the mockup's magenta ball
                m_spin[i]->setRange(0, 255);
                m_bar[i]->setValue(state->alpha);
                QSignalBlocker b(m_spin[i]);
                m_spin[i]->setValue(state->alpha);
                m_bar[i]->trackColor = [this](double t) { QColor c = state->color; c.setAlpha(qRound(t * 255)); return c; };
            }
            m_bar[i]->update();
        }
        {
            QSignalBlocker b(hex);
            hex->setText(state->color.name(QColor::HexRgb).toUpper());
        }
        m_syncing = false;
        relayout();
        update();
    }

    void relayout()
    {
        const qreal s = scale();
        QFont f = font();
        f.setPixelSize(qMax(8, qRound(35 * s)));
        const int n = rows();
        const double firstY = 111.5;
        const double pitch = n >= 5 ? 53.0 : 64.4;
        for (int i = 0; i < 5; ++i) {
            const double cy = firstY + pitch * i;
            m_bar[i]->setGeometry(r(389, cy - 23, 599, 46).toAlignedRect());
            const QRect whole = r(1004.6, cy - 25.5, 149, 51).toAlignedRect();
            const int arrowW = qRound(38 * s);
            m_spin[i]->setGeometry(whole.left(), whole.top(), whole.width() - arrowW, whole.height());
            m_spin[i]->setFont(f);
            m_spin[i]->setStyleSheet(QStringLiteral("QSpinBox { background: transparent; border: none; padding: 0px 0px 0px %1px; color: #f4f6fc; selection-background-color: #1196d8; }"
                                                    "QSpinBox QLineEdit { background: transparent; border: none; padding: 0px; }"
                                                    "QSpinBox::up-button, QSpinBox::down-button { width: 0px; height: 0px; border: none; }").arg(qRound(18 * s)));
            m_up[i]->setGeometry(whole.right() - arrowW + 1, whole.top(), arrowW, whole.height() / 2);
            m_down[i]->setGeometry(whole.right() - arrowW + 1, whole.top() + whole.height() / 2, arrowW, whole.height() - whole.height() / 2);
        }
        for (int i = 0; i < 5; ++i) m_model[i]->setGeometry(modelRect(i).toAlignedRect());
        hex->setGeometry(r(640, 461, 255, 57).toAlignedRect());
        hex->setFont(f);
        hex->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; padding: 0px 0px 0px %1px; color: #f4f6fc; selection-background-color: #1196d8; }").arg(qRound(22 * s)));
        copy->setGeometry(r(901.5, 461, 54, 57).toAlignedRect());
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
        f.setPixelSize(qMax(8, qRound(35 * s)));
        p.setFont(f);
        const QColor label("#d9deee");
        // the two groups
        paintGroup(p, r(14, 15, 274, 559), 14 * s);
        paintGroup(p, r(305, 18, 868, 556), 14 * s);
        p.setPen(label);
        p.setFont(fittedFont(f, tr("Color Model"), 175 * s));
        p.drawText(r(35, 15, 220, 54), Qt::AlignLeft | Qt::AlignVCenter, tr("Color Model"));
        p.setFont(fittedFont(f, tr("RGB Sliders"), 185 * s));
        p.drawText(r(334, 18, 500, 54), Qt::AlignLeft | Qt::AlignVCenter, tr("%1 Sliders").arg(KrimbleChannels::modelName(model)));
        p.setFont(f);
        // the model buttons
        static const char *icons[] = {"model_rgb", "model_hsv", "model_hsl", "model_cmyk", "model_gray"};
        for (int i = 0; i < 5; ++i) {
            const QRectF rect = modelRect(i);
            if (i == model) paintSelected(p, rect, 13 * s, s);
            else paintDarkButton(p, rect, 13 * s);
            drawIcon(p, QString::fromLatin1(icons[i]), QPointF(rect.left() + 46 * s, rect.center().y()), 1.0);
            p.setPen(i == model ? QColor("#ffffff") : label);
            p.setFont(fittedFont(f, QStringLiteral("Grayscale"), 133 * s));
            p.drawText(QRectF(rect.left() + 98 * s, rect.top(), rect.width() - 100 * s, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, KrimbleChannels::modelName(i));
            p.setFont(f);
        }
        // the rows: the label, then the box frame (the bar and the number are widgets above)
        const int n = rows();
        const double firstY = 111.5;
        const double pitch = n >= 5 ? 53.0 : 64.4;
        for (int i = 0; i < n; ++i) {
            const double cy = firstY + pitch * i;
            const QString name = (i < n - 1) ? KrimbleChannels::label(model, i) : QStringLiteral("A");
            p.setPen(label);
            p.drawText(r(300, cy - 25, 69, 50), Qt::AlignRight | Qt::AlignVCenter, name + QStringLiteral(":"));
            paintSpinFrame(p, r(1004.6, cy - 25.5, 149, 51));
        }
        // the divider, Color Preview and its line, the preview parts
        paintBrightLine(p, 327, 355, 1154, s);
        p.setPen(label);
        p.setFont(fittedFont(f, tr("Color Preview"), 189 * s));
        p.drawText(r(330, 372, 400, 34), Qt::AlignLeft | Qt::AlignVCenter, tr("Color Preview"));
        p.setFont(f);
        paintBrightLine(p, 327, 416, 1154, s);
        const QColor shown = state ? state->color : QColor("#459ad3");
        paintColorBox(p, r(364, 431, 139, 126), shown, 18 * s, s, true);
        p.setPen(label);
        p.drawText(r(500, 461, 120, 57), Qt::AlignRight | Qt::AlignVCenter, tr("HEX:"));
        paintField(p, r(640, 461, 262, 57), 9 * s);
        paintButton(p, r(901.5, 461, 54, 57), 11 * s);
        drawSymbol(p, QStringLiteral("copy"), r(901.5, 461, 54, 57).center(), 1.0);
        p.setPen(QPen(QColor(8, 11, 26, 200), 1.2));
        p.drawLine(QPointF(975.5 * s, 443 * s), QPointF(975.5 * s, 551 * s));
        p.setPen(QPen(QColor(110, 125, 175, 95), 1.0));
        p.drawLine(QPointF(975.5 * s + 1.6, 443 * s), QPointF(975.5 * s + 1.6, 551 * s));
        // the alpha preview: a checker with the color (with its alpha) in the middle and a white frame
        {
            const QRectF box = r(998, 437, 115, 116);
            paintChecker(p, box, 11 * s, 14 * s);
            QColor c = shown;
            c.setAlpha(state ? state->alpha : 255);
            p.setPen(Qt::NoPen);
            p.setBrush(c);
            p.drawRoundedRect(box.adjusted(18 * s, 18 * s, -18 * s, -18 * s), 8 * s, 8 * s);
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(QColor("#f4f6fc"), qMax<qreal>(2.0, 3.0 * s)));
            p.drawRoundedRect(box.adjusted(0.5, 0.5, -0.5, -0.5), 14 * s, 14 * s);
        }
    }

private:
    QRectF modelRect(int i) const { return r(28.6, 76.7 + 75.2 * i, 246.5, 64.4); }
    void channelChanged(int i, int v)
    {
        if (!state) return;
        const int n = KrimbleChannels::count(model);
        if (i == n) { if (onAlpha) onAlpha(v); return; }
        const QColor c = KrimbleChannels::withValue(model, i, v, state->color, state->hue);
        if (onColor) onColor(c);
    }
    QToolButton *m_model[5] = {};
    KrimbleSliderBar *m_bar[5] = {};
    QSpinBox *m_spin[5] = {};
    QToolButton *m_up[5] = {};
    QToolButton *m_down[5] = {};
    bool m_syncing = false;
};

// the nearest named color (the SVG color names) with "(custom)" when it is not exactly that color; and the web safe color (each channel a multiple of 51)
inline QString krimbleColorName(const QColor &c)
{
    QString best;
    int bestDistance = 1 << 30;
    for (const QString &name : QColor::colorNames()) {
        const QColor n(name);
        const int dr = n.red() - c.red(), dg = n.green() - c.green(), db = n.blue() - c.blue();
        const int distance = dr * dr + dg * dg + db * db;
        if (distance < bestDistance) { bestDistance = distance; best = name; }
    }
    if (best.isEmpty()) return QString();
    best[0] = best[0].toUpper();
    return bestDistance == 0 ? best : best + QStringLiteral(" (custom)");
}
inline QString krimbleWebSafe(const QColor &c)
{
    auto snap = [](int v) { return ((v + 25) / 51) * 51; };
    return QColor(snap(c.red()), snap(c.green()), snap(c.blue())).name(QColor::HexRgb).toUpper();
}

// Values page: the color preview with its alpha at the left; at the right every color model as number boxes (RGB, HSV, HSL, CMYK), HEX, the color's name and its web safe color
class KrimbleValuesTop : public KrimbleColorBase
{
public:
    KrimbleColorState *state = nullptr;
    std::function<void(const QColor &)> onColor;
    std::function<void(int)> onAlpha;
    QLineEdit *hex = nullptr;
    QToolButton *hexCopy = nullptr;
    QToolButton *webCopy = nullptr;

    double designHeight() const override { return 590.0; }

    explicit KrimbleValuesTop(QWidget *parent = nullptr) : KrimbleColorBase(parent)
    {
        // the boxes: RGB 0 to 2, HSV 3 to 5, HSL 6 to 8, CMYK 9 to 12, alpha 13 (0 to 255) and 14 (0 to 100)
        for (int i = 0; i < 15; ++i) {
            m_spin[i] = new QSpinBox(this);
            m_up[i] = makeButton();
            m_down[i] = makeButton();
            m_up[i]->setAutoRepeat(true);
            m_down[i]->setAutoRepeat(true);
            QSpinBox *box = m_spin[i];
            QObject::connect(m_up[i], &QToolButton::clicked, box, [box]() { box->stepUp(); });
            QObject::connect(m_down[i], &QToolButton::clicked, box, [box]() { box->stepDown(); });
            QObject::connect(m_spin[i], qOverload<int>(&QSpinBox::valueChanged), this, [this, i](int) { if (!m_syncing) boxChanged(i); });
        }
        for (int m = 0; m < 4; ++m) {
            const int count = KrimbleChannels::count(m);
            for (int c = 0; c < count; ++c) m_spin[rowFirst(m) + c]->setRange(0, KrimbleChannels::maximum(m, c));
        }
        m_spin[13]->setRange(0, 255);
        m_spin[14]->setRange(0, 100);
        m_alphaBar = new KrimbleSliderBar(this);
        m_alphaBar->alphaTrack = true;
        m_alphaBar->ballOverride = QColor(240, 40, 190);
        m_alphaBar->onChanged = [this](int v) { if (onAlpha) onAlpha(v); };
        m_alphaBar->trackColor = [this](double t) { QColor c = state ? state->color : QColor("#f035be"); c.setAlpha(qRound(t * 255)); return c; };
        hex = new QLineEdit(this);
        hex->setMaxLength(9);
        hexCopy = makeButton();
        webCopy = makeButton();
        hexCopy->setToolTip(tr("Copy the HEX code"));
        webCopy->setToolTip(tr("Copy the web safe color"));
        QObject::connect(hexCopy, &QToolButton::clicked, this, [this]() { QApplication::clipboard()->setText(hex->text()); });
        QObject::connect(webCopy, &QToolButton::clicked, this, [this]() { if (state) QApplication::clipboard()->setText(krimbleWebSafe(state->color)); });
        QObject::connect(hex, &QLineEdit::editingFinished, this, [this]() {
            QColor c(hex->text());
            if (c.isValid() && onColor) onColor(c);
        });
    }

    void refresh()
    {
        if (!state) return;
        m_syncing = true;
        for (int m = 0; m < 4; ++m) {
            const int count = KrimbleChannels::count(m);
            for (int c = 0; c < count; ++c) m_spin[rowFirst(m) + c]->setValue(KrimbleChannels::value(m, c, state->color, state->hue));
        }
        m_spin[13]->setValue(state->alpha);
        m_spin[14]->setValue(qRound(state->alpha * 100.0 / 255.0));
        m_alphaBar->setValue(state->alpha);
        m_alphaBar->update();
        hex->setText(state->color.name(QColor::HexRgb).toUpper());
        m_syncing = false;
        update();
    }

    void relayout()
    {
        const qreal s = scale();
        QFont f = font();
        f.setPixelSize(qMax(8, qRound(31 * s)));
        const int arrowW = qRound(38 * s);
        QFont narrowFont = f;
        narrowFont.setPixelSize(qMax(7, qRound(26 * s)));   // the narrow boxes (the inks, the alpha) have smaller text so three digits fit
        auto place = [&](int i, const QRectF &whole) {
            const QRect w = whole.toAlignedRect();
            const bool narrow = i >= 9;
            m_spin[i]->setGeometry(w.left(), w.top(), w.width() - arrowW, w.height());
            m_spin[i]->setFont(narrow ? narrowFont : f);
            m_spin[i]->setStyleSheet(QStringLiteral("QSpinBox { background: transparent; border: none; padding: 0px 0px 0px %1px; color: #f4f6fc; selection-background-color: #1196d8; }"
                                                    "QSpinBox QLineEdit { background: transparent; border: none; padding: 0px; }"
                                                    "QSpinBox::up-button, QSpinBox::down-button { width: 0px; height: 0px; border: none; }").arg(qRound((narrow ? 8 : 14) * s)));
            m_up[i]->setGeometry(w.right() - arrowW + 1, w.top(), arrowW, w.height() / 2);
            m_down[i]->setGeometry(w.right() - arrowW + 1, w.top() + w.height() / 2, arrowW, w.height() - w.height() / 2);
        };
        for (int m = 0; m < 4; ++m) {
            const int count = KrimbleChannels::count(m);
            for (int c = 0; c < count; ++c) place(rowFirst(m) + c, boxRect(m, c));
        }
        place(13, r(46, 504.6, 100, 37));
        place(14, r(190.4, 504.6, 110, 37));
        m_alphaBar->setGeometry(r(24, 546, 300, 46).toAlignedRect());
        hex->setGeometry(r(568, 334.8, 333, 51).toAlignedRect());
        hex->setFont(f);
        hex->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; padding: 0px 0px 0px %1px; color: #f4f6fc; selection-background-color: #1196d8; }").arg(qRound(26 * s)));
        hexCopy->setGeometry(r(900, 334.8, 49, 51).toAlignedRect());
        webCopy->setGeometry(r(1045, 526, 46, 44).toAlignedRect());
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
        f.setPixelSize(qMax(8, qRound(31 * s)));
        p.setFont(f);
        const QColor label("#d9deee");
        const QColor shown = state ? state->color : QColor("#459ad3");
        const int alpha = state ? state->alpha : 255;
        paintGroup(p, r(8, 13, 317, 580), 14 * s);
        paintGroup(p, r(340, 13, 828, 580), 14 * s);
        p.setPen(label);
        p.setFont(fittedFont(f, tr("Color Preview"), 204 * s));
        p.drawText(r(32, 13, 280, 50), Qt::AlignLeft | Qt::AlignVCenter, tr("Color Preview"));
        p.setFont(fittedFont(f, tr("Color Values & Conversion"), 387 * s));
        p.drawText(r(361, 13, 700, 46), Qt::AlignLeft | Qt::AlignVCenter, tr("Color Values & Conversion"));
        p.setFont(f);
        // the help mark of the group: a circle with a question mark
        p.setPen(QPen(QColor("#d9deee"), qMax<qreal>(1.5, 2.2 * s)));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPointF(1141.4 * s, 35 * s), 15 * s, 15 * s);
        p.drawText(QRectF(1126.4 * s, 20 * s, 30 * s, 30 * s), Qt::AlignCenter, QStringLiteral("?"));
        // the preview and the alpha
        paintColorBox(p, r(46, 80, 237, 230), shown, 22 * s, s, true);
        {
            const QRectF box = r(113, 332, 112, 113);
            paintChecker(p, box, 11 * s, 14 * s);
            QColor c = shown;
            c.setAlpha(alpha);
            p.setPen(Qt::NoPen);
            p.setBrush(c);
            p.drawRoundedRect(box.adjusted(17 * s, 17 * s, -17 * s, -17 * s), 8 * s, 8 * s);
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(QColor("#f4f6fc"), qMax<qreal>(2.0, 3.0 * s)));
            p.drawRoundedRect(box.adjusted(0.5, 0.5, -0.5, -0.5), 14 * s, 14 * s);
        }
        paintBrightLine(p, 30, 460.6, 309, s);
        p.setPen(label);
        p.setFont(fittedFont(f, tr("Alpha (Opacity)"), 179 * s));
        p.drawText(r(36, 466, 280, 40), Qt::AlignLeft | Qt::AlignVCenter, tr("Alpha (Opacity)"));
        p.setFont(f);
        paintSpinFrame(p, r(46, 504.6, 100, 37));
        paintSpinFrame(p, r(190.4, 504.6, 110, 37));
        p.setPen(label);
        p.drawText(r(146, 504.6, 44, 37), Qt::AlignCenter, QStringLiteral("/"));
        p.setPen(QColor("#e8ebf6"));
        {
            QFont uf = f;
            uf.setPixelSize(qMax(7, qRound(26 * s)));
            p.setFont(uf);
            p.drawText(r(190.4, 504.6, 110 - 38 - 5, 37), Qt::AlignRight | Qt::AlignVCenter, QStringLiteral("%"));
            p.setFont(f);
        }
        // the model rows
        static const char *rowIcons[5] = {"row_rgb", "model_hsv", "model_hsl", "model_cmyk", "row_hash"};
        static const double rowY[5] = {100.3, 164.8, 229.3, 294.8, 360.3};
        for (int m = 0; m < 5; ++m) {
            const QRectF rect = r(357, rowY[m] - 25.5, 179, 51);
            paintDarkButton(p, rect, 12 * s);
            drawIcon(p, QString::fromLatin1(rowIcons[m]), QPointF(rect.left() + 31 * s, rect.center().y()), m == 0 || m == 4 ? 1.0 : 0.6);
            p.setPen(label);
            const QString name = m == 4 ? QStringLiteral("HEX:") : KrimbleChannels::modelName(m) + QStringLiteral(":");
            p.drawText(QRectF(rect.left() + 62 * s, rect.top(), rect.width() - 64 * s, rect.height()), Qt::AlignLeft | Qt::AlignVCenter, name);
        }
        for (int m = 0; m < 4; ++m) {
            const int count = KrimbleChannels::count(m);
            for (int c = 0; c < count; ++c) {
                const QRectF box = boxRect(m, c);
                paintSpinFrame(p, box);
                p.setPen(label);
                p.drawText(QRectF(box.left() - 50 * s, box.top(), 46 * s, box.height()), Qt::AlignRight | Qt::AlignVCenter, KrimbleChannels::label(m, c) + QStringLiteral(":"));
                const QString unit = KrimbleChannels::unit(m, c);
                if (!unit.isEmpty()) {
                    p.setPen(QColor("#e8ebf6"));
                    QFont uf = f;
                    uf.setPixelSize(qMax(7, qRound((m == 3 ? 21 : 26) * s)));
                    p.setFont(uf);
                    p.drawText(QRectF(box.left() + (m == 3 ? 42 : 50) * s, box.top(), box.width() - 38 * s - (m == 3 ? 42 : 50) * s - 7 * s, box.height()), Qt::AlignRight | Qt::AlignVCenter, unit);
                    p.setFont(f);
                }
            }
        }
        // HEX
        paintField(p, r(568, 334.8, 333, 51), 9 * s);
        paintButton(p, r(900, 334.8, 49, 51), 11 * s);
        drawSymbol(p, QStringLiteral("copy"), r(900, 334.8, 49, 51).center(), 1.0);
        // Color Info
        paintBrightLine(p, 361, 400, 1163, s);
        p.setPen(label);
        p.setFont(fittedFont(f, tr("Color Info"), 135 * s));
        p.drawText(r(363, 405, 300, 42), Qt::AlignLeft | Qt::AlignVCenter, tr("Color Info"));
        p.setFont(f);
        paintGroup(p, r(359, 449, 806, 65), 12 * s);
        drawIcon(p, QStringLiteral("row_eyedropper"), r(401, 485, 0, 0).topLeft(), 1.0);
        p.setPen(label);
        p.setFont(fittedFont(f, tr("Name:"), 79 * s));
        p.drawText(r(443, 449, 160, 65), Qt::AlignLeft | Qt::AlignVCenter, tr("Name:"));
        p.setFont(f);
        paintField(p, r(611, 461.5, 481, 39), 8 * s);
        p.setPen(QColor("#f1f4fc"));
        p.drawText(r(633, 461.5, 450, 39), Qt::AlignLeft | Qt::AlignVCenter, krimbleColorName(shown));
        paintGroup(p, r(359, 518, 806, 62), 12 * s);
        drawIcon(p, QStringLiteral("row_palette"), r(400, 548, 0, 0).topLeft(), 1.0);
        p.setPen(label);
        p.setFont(fittedFont(f, tr("Web Safe:"), 126 * s));
        p.drawText(r(443, 518, 170, 62), Qt::AlignLeft | Qt::AlignVCenter, tr("Web Safe:"));
        p.setFont(f);
        paintField(p, r(611, 529, 481, 39), 8 * s);
        p.setPen(QColor("#f1f4fc"));
        p.drawText(r(633, 529, 400, 39), Qt::AlignLeft | Qt::AlignVCenter, krimbleWebSafe(shown));
        paintButton(p, r(1045, 526, 46, 44), 10 * s);
        drawSymbol(p, QStringLiteral("copy"), r(1045, 526, 46, 44).center(), 0.85);
    }

private:
    static int rowFirst(int model) { return model == 0 ? 0 : (model == 1 ? 3 : (model == 2 ? 6 : 9)); }
    // the box of channel c of a model row (the mockup's measured places)
    QRectF boxRect(int model, int c) const
    {
        static const double rowY[4] = {100.3, 164.8, 229.3, 294.8};
        static const double xs[3] = {603, 800, 999};
        static const double ws[3] = {132, 131, 136};
        static const double cx[4] = {591.5, 749, 900.6, 1064.4};
        static const double cw[4] = {102, 106.5, 107.5, 99.5};
        const double cy = rowY[qBound(0, model, 3)];
        if (model == 3) return r(cx[qBound(0, c, 3)], cy - 24, cw[qBound(0, c, 3)], 48);
        return r(xs[qBound(0, c, 2)], cy - 24, ws[qBound(0, c, 2)], 48);
    }
    void boxChanged(int i)
    {
        if (!state) return;
        if (i == 13) { if (onAlpha) onAlpha(m_spin[13]->value()); return; }
        if (i == 14) { if (onAlpha) onAlpha(qRound(m_spin[14]->value() * 255.0 / 100.0)); return; }
        const int model = i < 3 ? 0 : (i < 6 ? 1 : (i < 9 ? 2 : 3));
        const int c = i - rowFirst(model);
        if (onColor) onColor(KrimbleChannels::withValue(model, c, m_spin[i]->value(), state->color, state->hue));
    }
    QSpinBox *m_spin[15] = {};
    QToolButton *m_up[15] = {};
    QToolButton *m_down[15] = {};
    KrimbleSliderBar *m_alphaBar = nullptr;
    bool m_syncing = false;
};

// the tab strip above the card: Picker, Sliders, Values, Palette, History (measured in the Picker mockup: five tabs 220 units wide every 231, 75 tall; the selected one 235 x 82 with the
// cyan outline and glow of every selected item)
class KrimbleColorTabBar : public KrimbleColorBase
{
public:
    std::function<void(int)> onTab;
    int current = 0;
    double designHeight() const override { return 84.0; }
    explicit KrimbleColorTabBar(QWidget *parent = nullptr) : KrimbleColorBase(parent)
    {
        QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);   // (not Fixed: a fixed height would be the size hint's, not the height the width asks for)
        policy.setHeightForWidth(true);
        setSizePolicy(policy);
    }
    void setCurrent(int i)
    {
        current = qBound(0, i, 4);
        update();
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
        const qreal s = scale();
        QFont f = font();
        f.setPixelSize(qMax(8, qRound(40 * s)));
        p.setFont(f);
        static const char *names[5] = {"Picker", "Sliders", "Values", "Palette", "History"};
        // the unselected tabs first, then the selected one over its neighbors
        for (int i = 0; i < 5; ++i) {
            if (i == current) continue;
            const QRectF rect = tabRect(i, false);
            const QRectF b = rect.adjusted(0.5, 0.5, -0.5, -0.5);
            QLinearGradient g(0, b.top(), 0, b.bottom());
            g.setColorAt(0.0, QColor("#2e354d"));
            g.setColorAt(1.0, QColor("#1e2437"));
            p.setPen(Qt::NoPen);
            p.setBrush(g);
            p.drawRoundedRect(b, 14 * s, 14 * s);
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(QColor("#0d1223"), 1.4));
            p.drawRoundedRect(b, 14 * s, 14 * s);
            p.setPen(QPen(QColor(110, 128, 182, 120), 1.2));
            p.drawLine(QPointF(b.left() + 14 * s, b.top() + 1.4), QPointF(b.right() - 14 * s, b.top() + 1.4));
            p.setPen(QColor("#d9deee"));
            p.drawText(rect, Qt::AlignCenter, tr(names[i]));
        }
        const QRectF sel = tabRect(current, true);
        paintSelected(p, sel, 14 * s, s);
        p.setPen(QColor("#ffffff"));
        p.drawText(sel, Qt::AlignCenter, tr(names[current]));
    }
    void mousePressEvent(QMouseEvent *e) override
    {
        for (int i = 0; i < 5; ++i)
            if (tabRect(i, i == current).adjusted(-3, 0, 3, 6).contains(e->pos())) {
                if (i != current) { setCurrent(i); if (onTab) onTab(i); }
                return;
            }
    }
private:
    QRectF tabRect(int i, bool selected) const
    {
        const double x = 31.0 + 231.0 * i;
        return selected ? r(x - 7.5, 0, 235, 82) : r(x, 3, 220, 75);
    }
};
