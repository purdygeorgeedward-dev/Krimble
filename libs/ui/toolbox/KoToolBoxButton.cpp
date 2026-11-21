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
#include <QPainter>
#include <QRegularExpression>
#include <QtMath>
#include <functional>
#include <QConicalGradient>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QApplication>
#include <klocalizedstring.h>
#include <QKeySequence>
#include <KoIcon.h>
#include "KrimbleToolboxIcons.h"
#include "KrimbleAquaPaint.h"

KoToolBoxButton::KoToolBoxButton(KoToolAction *toolAction, QWidget *parent)
    : QToolButton(parent)
    , m_toolAction(toolAction)
{
    setObjectName(m_toolAction->id());
    // ensure same L&F
    setCheckable(true);
    setAutoRaise(true);
    setIcon(KrimbleToolboxIcons::icon(m_toolAction->iconName()));

    // only a fallback
    setToolTip(m_toolAction->toolTip());

    connect(this, SIGNAL(clicked(bool)), m_toolAction, SLOT(trigger()));

    m_krimbleTimer.setSingleShot(true);
    m_krimbleTimer.setInterval(250);
    connect(&m_krimbleTimer, &QTimer::timeout, this, [this]() {
        if (!m_krimbleLongPress || !isDown()) return;
        m_krimbleLongPressFired = true;
        setDown(false);
        Q_EMIT krimbleLongPressed();
    });
}

void KoToolBoxButton::mousePressEvent(QMouseEvent *event)
{
    m_krimbleLongPressFired = false;
    if (m_krimbleLongPress && event->button() == Qt::LeftButton) {
        m_krimblePressPos = event->pos();
        m_krimbleTimer.start();
    }
    QToolButton::mousePressEvent(event);
}

void KoToolBoxButton::mouseMoveEvent(QMouseEvent *event)
{
    if (m_krimbleTimer.isActive() && (event->pos() - m_krimblePressPos).manhattanLength() > QApplication::startDragDistance() * 2) {
        m_krimbleTimer.stop();
    }
    QToolButton::mouseMoveEvent(event);
}

void KoToolBoxButton::mouseReleaseEvent(QMouseEvent *event)
{
    m_krimbleTimer.stop();
    if (m_krimbleLongPressFired) {   // the long press opened the flyout: this release is not a tap on the button
        m_krimbleLongPressFired = false;
        event->accept();
        return;
    }
    QToolButton::mouseReleaseEvent(event);
}

// the system also turns a long press into a context menu event, which would open the toolbox's own menu on top of the flyout
void KoToolBoxButton::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_krimbleLongPress) {
        event->accept();
        return;
    }
    QToolButton::contextMenuEvent(event);
}

void KoToolBoxButton::krimbleRefreshIcon()
{
    setIcon(KrimbleToolboxIcons::icon(m_toolAction->iconName()));
    krimbleFitIconSize();
}

void KoToolBoxButton::krimbleFitIconSize()
{
    if (!m_toolAction) return;
    const bool picture = KrimbleToolboxIcons::colorMode() && !KrimbleToolboxIcons::grayMode() && KrimbleToolboxIcons::hasPicture(m_toolAction->iconName());
    if (picture) {
        const int side = qMin(width(), height());
        if (side < 8) return;   // not laid out yet; resizeEvent comes later
        if (!m_krimbleColorFit || iconSize() != m_krimbleSetIconSize) m_krimbleBaseIconSize = iconSize();   // a size from the toolbox since we set ours is the new base
        const int wanted = qRound(side * 0.92);
        m_krimbleSetIconSize = QSize(wanted, wanted);
        m_krimbleColorFit = true;
        if (iconSize() != m_krimbleSetIconSize) setIconSize(m_krimbleSetIconSize);
    } else if (m_krimbleColorFit) {
        m_krimbleColorFit = false;
        if (m_krimbleBaseIconSize.isValid()) setIconSize(m_krimbleBaseIconSize);
    }
}

void KoToolBoxButton::resizeEvent(QResizeEvent *event)
{
    QToolButton::resizeEvent(event);
    krimbleFitIconSize();
}

void KoToolBoxButton::showEvent(QShowEvent *event)
{
    QToolButton::showEvent(event);
    krimbleFitIconSize();
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
    setPalette(qApp->palette());
    const QString sheet = isChecked() ? QStringLiteral("QToolButton { border: none; background: transparent; }") : QString();
    if (styleSheet() != sheet) setStyleSheet(sheet);   // unchanged: no restyle
}

void KoToolBoxButton::setDataFromToolAction(QAction *action)
{
    if (!action) {
        warnUI << "No tool action available, using fallback!";
        setToolTip(m_toolAction->toolTip());
        return;
    }
    {
        QString tip = action->toolTip();
        if (tip.trimmed().isEmpty() && m_toolAction) tip = m_toolAction->toolTip();
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

static const int krimbleSelSteps = 72;
static const qreal krimbleSelOutline[krimbleSelSteps] = { 0.79, 0.78, 0.79, 0.80, 0.81, 0.84, 0.85, 0.85, 0.81, 0.66, 0.66, 0.69, 0.70, 0.73, 0.73, 0.69, 0.67, 0.66, 0.66, 0.68, 0.73, 0.76, 0.76, 0.74, 0.72, 0.68, 0.66, 0.66, 0.73, 0.81, 0.81, 0.81, 0.81, 0.81, 0.79, 0.76, 0.74, 0.73, 0.69, 0.69, 0.69, 0.71, 0.71, 0.71, 0.71, 0.75, 0.72, 0.73, 0.73, 0.79, 0.92, 0.96, 0.96, 0.94, 0.94, 0.94, 0.98, 0.99, 0.99, 0.94, 0.83, 0.73, 0.68, 0.70, 0.70, 0.84, 0.84, 0.84, 0.84, 0.83, 0.80, 0.79 };
static const qreal krimbleSelGlow[krimbleSelSteps] = { 0.27, 0.27, 0.28, 0.28, 0.29, 0.32, 0.34, 0.34, 0.29, 0.29, 0.66, 0.81, 0.88, 0.88, 0.75, 0.62, 0.50, 0.48, 0.48, 0.58, 0.78, 0.90, 0.93, 0.93, 0.79, 0.67, 0.57, 0.26, 0.26, 0.34, 0.35, 0.34, 0.34, 0.33, 0.32, 0.31, 0.31, 0.30, 0.30, 0.30, 0.30, 0.31, 0.32, 0.32, 0.20, 0.20, 0.37, 0.41, 0.41, 0.41, 0.41, 0.39, 0.39, 0.36, 0.36, 0.36, 0.39, 0.41, 0.42, 0.41, 0.43, 0.42, 0.42, 0.24, 0.24, 0.36, 0.36, 0.34, 0.31, 0.29, 0.29, 0.28 };

static QConicalGradient krimbleSelConical(const QPointF &center, const qreal *values, const std::function<QColor(qreal)> &color)
{
    QConicalGradient gradient(center, 0);
    for (int i = 0; i <= krimbleSelSteps; ++i) gradient.setColorAt(i / qreal(krimbleSelSteps), color(values[i % krimbleSelSteps]));
    return gradient;
}

static void krimbleMiterPen(QPainter &painter)
{
    QPen pen = painter.pen();
    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);
}

void KoToolBoxButton::krimblePaintRaisedBox(QWidget *widget, bool down)
{
    QPainter box(widget);
    box.setRenderHint(QPainter::Antialiasing, false);
    const QRect r = widget->rect().adjusted(1, 1, -1, -1);
    const QColor krimbleWindow = qApp->palette().color(QPalette::Window);
    // const bool krimbleLight = krimbleWindow.lightness() > 150;
    // const QColor face = krimbleLight ? krimbleWindow.darker(108) : krimbleWindow.lighter(150);
    const bool krimbleLight = KrimbleToolboxIcons::lightGray() || krimbleWindow.lightness() > 150;   // the Light gray switch gives the light face
    const QColor face = KrimbleToolboxIcons::lightGray() ? KrimbleToolboxIcons::lightGrayFace()
                      : (krimbleLight ? krimbleWindow.darker(108) : krimbleWindow.lighter(150));
    const int t = qMax(2, qRound(qMin(r.width(), r.height()) * 0.075));
    box.fillRect(r, face);
    const int hiTop = krimbleLight ? 255 : 85;
    const int hiLeft = krimbleLight ? 170 : 40;
    const QColor topTone    = down ? QColor(0, 0, 0, 90)            : QColor(255, 255, 255, hiTop);
    const QColor leftTone   = down ? QColor(0, 0, 0, 55)            : QColor(255, 255, 255, hiLeft);
    const QColor rightTone  = down ? QColor(255, 255, 255, hiLeft)  : QColor(0, 0, 0, 55);
    const QColor bottomTone = down ? QColor(255, 255, 255, hiTop)   : QColor(0, 0, 0, 90);
    box.setPen(Qt::NoPen);
    QPolygon poly;
    poly << r.topLeft() << r.topRight() << QPoint(r.right() - t, r.top() + t) << QPoint(r.left() + t, r.top() + t);
    box.setBrush(topTone); box.drawPolygon(poly);
    poly.clear(); poly << r.topLeft() << QPoint(r.left() + t, r.top() + t) << QPoint(r.left() + t, r.bottom() - t) << r.bottomLeft();
    box.setBrush(leftTone); box.drawPolygon(poly);
    poly.clear(); poly << r.topRight() << r.bottomRight() << QPoint(r.right() - t, r.bottom() - t) << QPoint(r.right() - t, r.top() + t);
    box.setBrush(rightTone); box.drawPolygon(poly);
    poly.clear(); poly << r.bottomLeft() << QPoint(r.left() + t, r.bottom() - t) << QPoint(r.right() - t, r.bottom() - t) << r.bottomRight();
    box.setBrush(bottomTone); box.drawPolygon(poly);
}

void KoToolBoxButton::paintEvent(QPaintEvent *event)
{
    if (KrimbleToolboxIcons::buttonStyle() == 1 && !isChecked() && !qApp->property("krimbleNoRaisedButtons").toBool()) {
        krimblePaintRaisedBox(this, isDown());
    } else if (KrimbleToolboxIcons::lightGray() && !isChecked()) {
        QPainter face(this);
        face.fillRect(rect().adjusted(1, 1, -1, -1), KrimbleToolboxIcons::lightGrayFace());   // Light gray, flat
    }
    // The first version of the Raised style (near-white and near-black sides, a near-black outline), not used any more:
    if (isChecked()) {
        QPainter painter(this);
        const qreal s = qMin(width(), height());
        // painter.fillRect(QRectF(rect()).adjusted(s * 0.12, s * 0.12, -s * 0.12, -s * 0.12), QColor("#0b87dd"));   // flat, the blue of the menu selection
        krimblePaintAqua(painter, QRectF(rect()).adjusted(s * 0.12, s * 0.12, -s * 0.12, -s * 0.12), s * 0.07);   // the Aqua-style selection
    }
    // if (isChecked()) {
    //     QPainter painter(this);
    //     painter.setRenderHint(QPainter::Antialiasing, true);
    //     const qreal s = qMin(width(), height());
    //     const QRectF base = QRectF(rect()).adjusted(s * 0.12, s * 0.12, -s * 0.12, -s * 0.12);
    //     const qreal bw = base.width();
    //     const qreal r = 0.0;
    //     const QPointF centre = base.center();
    //     // the glow outside the outline: tight and weak, fading out within 6 percent of the width, stronger where the mockup's is
    //     const int glowSteps = 18;
    //     for (int i = glowSteps; i >= 1; --i) {
    //         const qreal t = qreal(i) / glowSteps;
    //         const qreal w = bw * 0.05 * t;
    //         const qreal alpha = 255 * 0.6 * qExp(-3.4 * t);
    //         painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelGlow, [alpha](qreal v) { return QColor(28, 128, 235, qMin(255, int(alpha * (0.25 + 0.9 * v)))); })), qMax<qreal>(1.0, bw * 0.012)));
    //         krimbleMiterPen(painter);   // task 37: a pen's corners are cut off (bevel join) unless it says miter; that is what looked rounded
    //         painter.setBrush(Qt::NoBrush);
    //         painter.drawRoundedRect(base.adjusted(-w, -w, w, w), r, r);
    //     }
    //     // the fill, darker in the middle
    //     QLinearGradient fill(base.topLeft(), base.bottomLeft());
    //     fill.setColorAt(0.0, QColor("#10498c"));
    //     fill.setColorAt(0.5, QColor("#07306f"));
    //     fill.setColorAt(1.0, QColor("#033075"));
    //     painter.setPen(Qt::NoPen);
    //     painter.setBrush(fill);
    //     painter.drawRoundedRect(base, r, r);
    //     QRadialGradient vignette(centre, bw * 0.62);
    //     vignette.setColorAt(0.0, QColor(1, 8, 34, 235));
    //     vignette.setColorAt(0.3, QColor(1, 10, 40, 150));
    //     vignette.setColorAt(0.62, QColor(1, 12, 45, 34));
    //     vignette.setColorAt(0.85, QColor(1, 12, 45, 0));
    //     vignette.setColorAt(1.0, QColor(1, 12, 45, 0));
    //     painter.setBrush(vignette);
    //     painter.drawRoundedRect(base, r, r);
    //     // vivid blue just inside the edge, fading toward the middle, as strong as the outline is bright at each place
    //     const int innerSteps = 24;
    //     for (int k = 1; k <= innerSteps; ++k) {
    //         const qreal t = qreal(k) / innerSteps;
    //         const qreal d = bw * 0.06 * t;
    //         const qreal alpha = 255 * qExp(-4.5 * t);
    //         painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelOutline, [alpha](qreal v) { return QColor(36, 150, 248, qMax(0, qMin(255, int(alpha * (0.8 + 0.9 * (v - 0.55)))))); })), qMax<qreal>(1.0, bw * 0.011)));
    //         krimbleMiterPen(painter);   // task 37
    //         painter.setBrush(Qt::NoBrush);
    //         painter.drawRoundedRect(base.adjusted(d, d, -d, -d), r, r);
    //     }
    //     painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelOutline, [](qreal v) { return QColor(80, 200, 255, qMin(255, int(255 * v))); })), qMax<qreal>(3.0, bw * 0.022)));
    //     // qMax<qreal>(2.0, bw * 0.014)));   // the thinner try of task 37 (not wanted)
    //     krimbleMiterPen(painter);   // task 37
    //     painter.setBrush(Qt::NoBrush);
    //     painter.drawRoundedRect(base, r, r);
    //     painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelOutline, [](qreal v) {
    //         const qreal t = qBound<qreal>(0.0, (v - 0.62) / 0.38, 1.0);
    //         return QColor(int(105 + (205 - 105) * t), int(220 + (255 - 220) * t), 255);
    //     })), qMax<qreal>(2.2, bw * 0.018)));
    //     // qMax<qreal>(1.5, bw * 0.011)));   // the thinner try of task 37 (not wanted)
    //     krimbleMiterPen(painter);   // task 37
    //     painter.drawRoundedRect(base, r, r);
    //     // the dark shadow behind the icon (the mockup's icon casts one): the icon is drawn over it below
    //     QRadialGradient shadow(centre, s * 0.30);
    //     shadow.setColorAt(0.0, QColor(0, 4, 24, 170));
    //     shadow.setColorAt(1.0, QColor(0, 4, 24, 0));
    //     painter.setPen(Qt::NoPen);
    //     painter.setBrush(shadow);
    //     painter.drawEllipse(centre, s * 0.30, s * 0.30);
    // }
    Q_UNUSED(event);
    {
        QStylePainter stylePainter(this);
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        if (!isDown()) opt.state &= ~QStyle::State_MouseOver;
        stylePainter.drawComplexControl(QStyle::CC_ToolButton, opt);
    }
    if (m_krimbleLongPress) {
        QPainter mark(this);
        mark.setRenderHint(QPainter::Antialiasing, true);
        const qreal t = qMax<qreal>(5.0, qMin(width(), height()) * 0.16);
        const qreal e = qMax<qreal>(2.0, qMin(width(), height()) * 0.06);
        QPolygonF tri;
        tri << QPointF(width() - e, height() - e) << QPointF(width() - e - t, height() - e) << QPointF(width() - e, height() - e - t);
        mark.setPen(Qt::NoPen);
        mark.setBrush(QColor(215, 228, 255, 210));
        mark.drawPolygon(tri);
    }
}
