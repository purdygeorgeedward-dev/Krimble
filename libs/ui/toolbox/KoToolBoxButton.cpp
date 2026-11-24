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
    setCheckable(true);
    setAutoRaise(true);
    setIcon(KrimbleToolboxIcons::icon(m_toolAction->iconName()));

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
    if (m_krimbleLongPressFired) {
        m_krimbleLongPressFired = false;
        event->accept();
        return;
    }
    QToolButton::mouseReleaseEvent(event);
}

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
    setIcon(KrimbleToolboxIcons::icon(m_krimbleIconOverride.isEmpty() ? m_toolAction->iconName() : m_krimbleIconOverride));
    krimbleFitIconSize();
}

void KoToolBoxButton::krimbleFitIconSize()
{
    if (!m_toolAction) return;
    const bool picture = KrimbleToolboxIcons::colorMode() && !KrimbleToolboxIcons::grayMode() && KrimbleToolboxIcons::hasPicture(m_toolAction->iconName())
        && !(KrimbleToolboxIcons::iconStyle() == KrimbleToolboxIcons::Classic && KrimbleToolboxIcons::classicLineArt(m_toolAction->iconName()))
        && !(KrimbleToolboxIcons::iconStyle() == KrimbleToolboxIcons::Photorealistic && !KrimbleToolboxIcons::hasPhoto(m_toolAction->iconName())
             && KrimbleToolboxIcons::classicLineArt(m_toolAction->iconName()));
    if (picture) {
        const int side = qMin(width(), height());
        if (side < 8) return;
        if (!m_krimbleColorFit || iconSize() != m_krimbleSetIconSize) m_krimbleBaseIconSize = iconSize();
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
    if (styleSheet() != sheet) setStyleSheet(sheet);
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
    const bool krimbleLight = KrimbleToolboxIcons::lightGray() || krimbleWindow.lightness() > 150;
    const QColor face = KrimbleToolboxIcons::customFace() ? KrimbleToolboxIcons::lightGrayFace()
                      : (krimbleLight ? krimbleWindow.darker(108) : krimbleWindow.lighter(150));
    const int t = qMax(2, qRound(qMin(r.width(), r.height()) * 0.075));
    if (KrimbleToolboxIcons::glowFace()) KrimbleToolboxIcons::paintGlowTile(box, QRectF(r));
    else box.fillRect(r, face);
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
    } else if (KrimbleToolboxIcons::glowFace() && !isChecked()) {
        QPainter face(this);
        KrimbleToolboxIcons::paintGlowTile(face, QRectF(rect()).adjusted(2, 2, -2, -2));
    } else if (KrimbleToolboxIcons::customFace() && !isChecked()) {
        QPainter face(this);
        face.fillRect(rect().adjusted(1, 1, -1, -1), KrimbleToolboxIcons::lightGrayFace());
    }
    if (isChecked()) {
        QPainter painter(this);
        const qreal s = qMin(width(), height());
        painter.fillRect(QRectF(rect()).adjusted(s * 0.05, s * 0.05, -s * 0.05, -s * 0.05), QColor(11, 135, 221));
    }
    Q_UNUSED(event);
    {
        QStylePainter stylePainter(this);
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        if (!isDown()) opt.state &= ~QStyle::State_MouseOver;
        if (isChecked() && qApp->palette().color(QPalette::Window).lightness() > 160 && !opt.icon.isNull()) {
            const qreal dpr = devicePixelRatioF();
            QPixmap white(opt.iconSize * dpr);
            white.fill(Qt::transparent);
            {
                QPainter wp(&white);
                opt.icon.paint(&wp, QRect(QPoint(0, 0), white.size()));
                wp.setCompositionMode(QPainter::CompositionMode_SourceIn);
                wp.fillRect(white.rect(), Qt::white);
            }
            white.setDevicePixelRatio(dpr);
            opt.icon = QIcon(white);
        }
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
