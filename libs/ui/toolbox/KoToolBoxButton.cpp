/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoToolBoxButton_p.h"

#include <kis_debug.h>

#include <KoToolManager.h>
#include <QAction>
#include <QPalette>
#include <QPainter>   // Krimble 2026-11-07
#include <QRegularExpression>   // Krimble 2026-11-08
#include <QtMath>
#include <functional>
#include <QConicalGradient>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPaintEvent>
#include <QApplication>
#include <klocalizedstring.h>
#include <QKeySequence>
#include <KoIcon.h>
#include "KrimbleToolboxIcons.h"   // Krimble 2026-10-30

KoToolBoxButton::KoToolBoxButton(KoToolAction *toolAction, QWidget *parent)
    : QToolButton(parent)
    , m_toolAction(toolAction)
{
    setObjectName(m_toolAction->id());
    // ensure same L&F
    setCheckable(true);
    setAutoRaise(true);
    // setIcon(kisIcon(m_toolAction->iconName()));
    // Krimble 2026-10-30: the Full color or the Clear style (see KrimbleToolboxIcons.h)
    setIcon(KrimbleToolboxIcons::icon(m_toolAction->iconName()));

    // only a fallback
    setToolTip(m_toolAction->toolTip());

    connect(this, SIGNAL(clicked(bool)), m_toolAction, SLOT(trigger()));
}

// Krimble 2026-10-30: the icon again, after the user changed the icon style in the toolbox's menu
void KoToolBoxButton::krimbleRefreshIcon()
{
    setIcon(KrimbleToolboxIcons::icon(m_toolAction->iconName()));
}

void KoToolBoxButton::attachAction(QAction *action)
{
    if (!action) {
        return;
    }

    connect(action, SIGNAL(changed()), SLOT(slotUpdateActionData()));
    setDataFromToolAction(action);
}

void KoToolBoxButton::setHighlightColor()
{
    // Krimble 2026-11-06: George: "Light blue areas are selected areas" (the mockups: the selected tool of the Mixer's tool strip, the selected tab, the selected color well). A
    // selected tool in the toolbox is drawn like the mockup's selected tool: the dark panel color inside and a 2 px light blue (#38C6FF) outline with rounded corners. The old
    // version below filled the button with the theme's highlight color.
    // QPalette p = qApp->palette();
    // if (isChecked()) {
    //     QPalette palette_highlight(p);
    //     QColor c = p.color(QPalette::Highlight);
    //     palette_highlight.setColor(QPalette::Button, c);
    //     setPalette(palette_highlight);
    // }
    // else {
    //     setPalette(p);
    // }
    setPalette(qApp->palette());
    if (isChecked()) {
        // setStyleSheet(QStringLiteral("QToolButton { border: 2px solid #38c6ff; border-radius: 6px; background-color: #151a33; }"));   // first version: dark inside, George: "It's not just the outline. It's the shading."
        // setStyleSheet(QStringLiteral("QToolButton { border: 2px solid #3bbcf5; border-radius: 6px;"
        //                              " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #0585ce, stop:0.25 #016cac, stop:0.5 #015c96, stop:0.75 #01548c, stop:1 #0165a5); }"));   // sampled from the Mixer mockup
        // Krimble 2026-11-06 (2nd): George: "I don't want an outline around the gradient icon in the toolbox." The same blue gradient, no outline.
        // setStyleSheet(QStringLiteral("QToolButton { border: none; border-radius: 6px;"
        //                              " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #0585ce, stop:0.25 #016cac, stop:0.5 #015c96, stop:0.75 #01548c, stop:1 #0165a5); }"));
        // Krimble 2026-11-06 (3rd): George: "blue selections should go to a deeper blue." (the bottom is now #05429D, the mockup's OK button blue)
        // setStyleSheet(QStringLiteral("QToolButton { border: none; border-radius: 6px;"
        //                              " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #0b87dd, stop:0.4 #0867b8, stop:0.75 #0652ad, stop:1 #05429d); }"));
        // Krimble 2026-11-07: George: "The selected tool in the toolbox looks bad." and, asked what looks bad about it: "Selections are better looking in the mockups." The selected tool is
        // drawn by paintEvent() below like the mockup's selected tool (sampled from the mockup picture: a deep blue fill, darker in the middle, a bright light blue outline, and a soft glow
        // around it); the style sheet only makes the button's own background empty.
        setStyleSheet(QStringLiteral("QToolButton { border: none; background: transparent; }"));
    } else {
        setStyleSheet(QString());
    }
}

void KoToolBoxButton::setDataFromToolAction(QAction *action)
{
    if (!action) {
        warnUI << "No tool action available, using fallback!";
        setToolTip(m_toolAction->toolTip());
        return;
    }
    // setToolTip(action->toolTip());
    // Krimble 2026-11-08: George: "Does it have to say \"Tool\" after every tool?" The word is left off the tool tip ("Freehand Brush Tool" is "Freehand Brush").
    {
        QString tip = action->toolTip();
        tip.replace(QRegularExpression(QStringLiteral("\\sTool(\\s*\\(|\\s*$)")), QStringLiteral("\\1"));
        setToolTip(tip);
    }
}

void KoToolBoxButton::slotUpdateActionData()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action) {
        setDataFromToolAction(action);
    }
}

// Krimble 2026-11-07: the selected tool, like the mockup (George: "Selections are better looking in the mockups.", "Your test render isn't perfect.", "See how the edges glow in slightly
// irregular ways?", "Show me a better render.", "not bright enough on the edges or dark enough in the middle.", "edges are too dark", "still not enough."). Drawn from the mockup picture (the selected lasso tool of the icon sheet,
// cut out and compared with a real Qt render five times, with the real colors measured in both): a rounded square (corner 11.5 percent of its width); a fill that is #10498C at the top,
// #07306F in the middle and #033075 at the bottom with a dark middle (a vignette, down to #01071E behind the icon, as in the mockup, where the icon casts a dark shadow); vivid blue just
// inside the edge (the mockup has #0D64BD and #1348B0 there) fading smoothly over 11.5 percent of the width, strong; a thin, bright outline (near-white cyan, #CDFFFF, where it is brightest, #69DCFF
// where it is dim) with a soft halo under it; and a glow outside, within 10 percent of the width (the mockup is dark again a few pixels outside its edge). The brightness of the outline and the strength of the
// outer glow are NOT even: they were measured all the way around the mockup's square (every 5 degrees, 0 = right, counter-clockwise, a median of three): the outer glow is strongest along
// the top, weak at the sides; the outline is brightest along the bottom and dimmest at the top right. All of it inside the button's own square (12 percent of its size is kept free around it
// for the glow). The icon is drawn over it by QToolButton as usual.
static const int krimbleSelSteps = 72;
static const qreal krimbleSelOutline[krimbleSelSteps] = { 0.79, 0.78, 0.79, 0.80, 0.81, 0.84, 0.85, 0.85, 0.81, 0.66, 0.66, 0.69, 0.70, 0.73, 0.73, 0.69, 0.67, 0.66, 0.66, 0.68, 0.73, 0.76, 0.76, 0.74, 0.72, 0.68, 0.66, 0.66, 0.73, 0.81, 0.81, 0.81, 0.81, 0.81, 0.79, 0.76, 0.74, 0.73, 0.69, 0.69, 0.69, 0.71, 0.71, 0.71, 0.71, 0.75, 0.72, 0.73, 0.73, 0.79, 0.92, 0.96, 0.96, 0.94, 0.94, 0.94, 0.98, 0.99, 0.99, 0.94, 0.83, 0.73, 0.68, 0.70, 0.70, 0.84, 0.84, 0.84, 0.84, 0.83, 0.80, 0.79 };
static const qreal krimbleSelGlow[krimbleSelSteps] = { 0.27, 0.27, 0.28, 0.28, 0.29, 0.32, 0.34, 0.34, 0.29, 0.29, 0.66, 0.81, 0.88, 0.88, 0.75, 0.62, 0.50, 0.48, 0.48, 0.58, 0.78, 0.90, 0.93, 0.93, 0.79, 0.67, 0.57, 0.26, 0.26, 0.34, 0.35, 0.34, 0.34, 0.33, 0.32, 0.31, 0.31, 0.30, 0.30, 0.30, 0.30, 0.31, 0.32, 0.32, 0.20, 0.20, 0.37, 0.41, 0.41, 0.41, 0.41, 0.39, 0.39, 0.36, 0.36, 0.36, 0.39, 0.41, 0.42, 0.41, 0.43, 0.42, 0.42, 0.24, 0.24, 0.36, 0.36, 0.34, 0.31, 0.29, 0.29, 0.28 };

static QConicalGradient krimbleSelConical(const QPointF &center, const qreal *values, const std::function<QColor(qreal)> &color)
{
    QConicalGradient gradient(center, 0);
    for (int i = 0; i <= krimbleSelSteps; ++i) gradient.setColorAt(i / qreal(krimbleSelSteps), color(values[i % krimbleSelSteps]));
    return gradient;
}

// Krimble 2026-11-12 (task 37): George: "I previously asked for thinner glow edges (especially in toolbox) and no rounded corners in the toolbox glow." Qt's pens cut the corners (bevel join) of a rectangle drawn with them: the glow's corners looked rounded although the rectangle's radius is 0. This sets the miter join (sharp corners) on the painter's current pen.
static void krimbleMiterPen(QPainter &painter)
{
    QPen pen = painter.pen();
    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);
}

void KoToolBoxButton::paintEvent(QPaintEvent *event)
{
    if (isChecked()) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        const qreal s = qMin(width(), height());
        const QRectF base = QRectF(rect()).adjusted(s * 0.12, s * 0.12, -s * 0.12, -s * 0.12);
        const qreal bw = base.width();
        // const qreal r = bw * 0.115;
        // Krimble 2026-11-11: George: "Can't a square without rounded corners still glow? Try square selection with corners in toolbox." The selected tool is a square with sharp corners (radius 0) and
        // still glows: the glow, the fill, the inner blue band and the outline are all drawn with square corners.
        const qreal r = 0.0;
        const QPointF centre = base.center();
        // the glow outside the outline: tight and weak, fading out within 6 percent of the width, stronger where the mockup's is
        const int glowSteps = 18;
        for (int i = glowSteps; i >= 1; --i) {
            const qreal t = qreal(i) / glowSteps;
            // const qreal w = bw * 0.10 * t;
            // const qreal alpha = 255 * qExp(-2.9 * t);
            // Krimble 2026-11-11: George (of the zoomed Pan tool): "The hand (Pan tool) example shows too much glow." The outer glow reaches half as far (5 percent of the width, was 10) and is weaker.
            // const qreal w = bw * 0.05 * t;
            // Krimble 2026-11-12 (task 37): George: "I previously asked for thinner glow edges (especially in toolbox) and no rounded corners in the toolbox glow." The outer glow reaches 3 percent of the width (was 5).
            const qreal w = bw * 0.03 * t;
            const qreal alpha = 255 * 0.6 * qExp(-3.4 * t);
            painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelGlow, [alpha](qreal v) { return QColor(28, 128, 235, qMin(255, int(alpha * (0.25 + 0.9 * v)))); })), qMax<qreal>(1.0, bw * 0.012)));
            krimbleMiterPen(painter);   // task 37: a pen's corners are cut off (bevel join) unless it says miter; that is what looked rounded
            painter.setBrush(Qt::NoBrush);
            // painter.drawRoundedRect(base.adjusted(-w, -w, w, w), r + w, r + w);
            painter.drawRoundedRect(base.adjusted(-w, -w, w, w), r, r);   // Krimble 2026-11-11: sharp corners on the glow too
        }
        // the fill, darker in the middle
        QLinearGradient fill(base.topLeft(), base.bottomLeft());
        fill.setColorAt(0.0, QColor("#10498c"));
        fill.setColorAt(0.5, QColor("#07306f"));
        fill.setColorAt(1.0, QColor("#033075"));
        painter.setPen(Qt::NoPen);
        painter.setBrush(fill);
        painter.drawRoundedRect(base, r, r);
        QRadialGradient vignette(centre, bw * 0.62);
        vignette.setColorAt(0.0, QColor(1, 8, 34, 235));
        vignette.setColorAt(0.3, QColor(1, 10, 40, 150));
        vignette.setColorAt(0.62, QColor(1, 12, 45, 34));
        vignette.setColorAt(0.85, QColor(1, 12, 45, 0));
        vignette.setColorAt(1.0, QColor(1, 12, 45, 0));
        painter.setBrush(vignette);
        painter.drawRoundedRect(base, r, r);
        // vivid blue just inside the edge, fading toward the middle, as strong as the outline is bright at each place
        const int innerSteps = 24;
        for (int k = 1; k <= innerSteps; ++k) {
            const qreal t = qreal(k) / innerSteps;
            // const qreal d = bw * 0.115 * t;
            // const qreal alpha = 255 * qExp(-3.4 * t);
            // Krimble 2026-11-11: the vivid blue band inside the edge is about half as wide (6 percent of the width, was 11.5) and fades faster.
            // const qreal d = bw * 0.06 * t;
            // task 37: the vivid blue band inside the edge is 3.5 percent of the width (was 6).
            const qreal d = bw * 0.035 * t;
            const qreal alpha = 255 * qExp(-4.5 * t);
            painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelOutline, [alpha](qreal v) { return QColor(36, 150, 248, qMax(0, qMin(255, int(alpha * (0.8 + 0.9 * (v - 0.55)))))); })), qMax<qreal>(1.0, bw * 0.011)));
            krimbleMiterPen(painter);   // task 37
            painter.setBrush(Qt::NoBrush);
            // painter.drawRoundedRect(base.adjusted(d, d, -d, -d), qMax<qreal>(1, r - d), qMax<qreal>(1, r - d));
            painter.drawRoundedRect(base.adjusted(d, d, -d, -d), r, r);   // Krimble 2026-11-11: sharp corners
        }
        // the outline: a soft halo under a thin bright core, light cyan where it is bright
        // painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelOutline, [](qreal v) { return QColor(80, 200, 255, qMin(255, int(255 * v))); })), qMax<qreal>(4.0, bw * 0.034)));
        // Krimble 2026-11-11: the soft halo under the bright core is thinner (2.2 percent of the width, at least 3 px; was 3.4 percent, at least 4 px).
        painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelOutline, [](qreal v) { return QColor(80, 200, 255, qMin(255, int(255 * v))); })), // qMax<qreal>(3.0, bw * 0.022)));
        qMax<qreal>(2.0, bw * 0.014)));   // task 37: the halo is 1.4 percent of the width, at least 2 px (was 2.2 percent, at least 3 px)
        krimbleMiterPen(painter);   // task 37
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(base, r, r);
        painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelOutline, [](qreal v) {
            const qreal t = qBound<qreal>(0.0, (v - 0.62) / 0.38, 1.0);
            return QColor(int(105 + (205 - 105) * t), int(220 + (255 - 220) * t), 255);
        })), // qMax<qreal>(2.2, bw * 0.018)));
        qMax<qreal>(1.5, bw * 0.011)));   // task 37: the bright core line is 1.1 percent of the width, at least 1.5 px (was 1.8 percent, at least 2.2 px)
        krimbleMiterPen(painter);   // task 37
        painter.drawRoundedRect(base, r, r);
        // the dark shadow behind the icon (the mockup's icon casts one): the icon is drawn over it below
        QRadialGradient shadow(centre, s * 0.30);
        shadow.setColorAt(0.0, QColor(0, 4, 24, 170));
        shadow.setColorAt(1.0, QColor(0, 4, 24, 0));
        painter.setPen(Qt::NoPen);
        painter.setBrush(shadow);
        painter.drawEllipse(centre, s * 0.30, s * 0.30);
    }
    QToolButton::paintEvent(event);
}
