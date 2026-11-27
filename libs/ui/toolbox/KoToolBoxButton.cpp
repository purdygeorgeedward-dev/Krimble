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

KoToolBoxButton::KoToolBoxButton(KoToolAction *toolAction, QWidget *parent)
    : QToolButton(parent)
    , m_toolAction(toolAction)
{
    setObjectName(m_toolAction->id());
    // ensure same L&F
    setCheckable(true);
    setAutoRaise(true);
    setProperty("krimbleGlowHooked", true);
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
    const bool picture = KrimbleToolboxIcons::colorMode() && KrimbleToolboxIcons::hasPicture(m_toolAction->iconName());
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

void KoToolBoxButton::krimblePaintRaisedBox(QWidget *widget, bool down)
{
    QPainter box(widget);
    box.setRenderHint(QPainter::Antialiasing, false);
    const QRect r = widget->rect().adjusted(1, 1, -1, -1);
    const QColor krimbleWindow = qApp->palette().color(QPalette::Window);
    const bool krimbleLight = krimbleWindow.lightness() > 150;
    const QColor face = krimbleLight ? krimbleWindow.darker(108) : krimbleWindow.lighter(150);
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
    }
    // The first version of the Raised style (near-white and near-black sides, a near-black outline), not used any more:
    if (isChecked()) {
        QPainter painter(this);
        painter.fillRect(rect(), QColor(16, 17, 28));
    }
    Q_UNUSED(event);
    {
        QStylePainter stylePainter(this);
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        if (!isDown()) opt.state &= ~QStyle::State_MouseOver;
        if (isChecked()) opt.state &= ~(QStyle::State_On | QStyle::State_Sunken | QStyle::State_Raised);
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
