/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include <QStyleOptionSpinBox>
#include <QAbstractSpinBox>
#include "KisWideDockSeparatorStyle.h"

#include <QStyleFactory>
#include <QPainter>
#include <QPixmap>
#include <QWidget>
#include <QStyleOption>

KisWideDockSeparatorStyle::KisWideDockSeparatorStyle(QStyle *baseStyle)
    : QProxyStyle(QStyleFactory::create(baseStyle->objectName()))
{
}

int KisWideDockSeparatorStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
#ifdef Q_OS_ANDROID
    if (metric == QStyle::PM_IconViewIconSize) {
        return 64;
    }
#endif
#ifdef Q_OS_ANDROID
    if (metric == QStyle::PM_MenuDesktopFrameWidth && widget) {
        const int krimbleFrame = widget->property("krimbleMenuDeskFrame").toInt();
        if (krimbleFrame > 0) return qMax(krimbleFrame, QProxyStyle::pixelMetric(metric, option, widget));
    }
#endif
    if (metric == QStyle::PM_DockWidgetSeparatorExtent) {
#ifdef Q_OS_ANDROID
        return QProxyStyle::pixelMetric(metric, option, widget);
#else
        return QProxyStyle::pixelMetric(metric, option, widget) * 2;
#endif
    }

#ifdef Q_OS_ANDROID
    if (metric == QStyle::PM_MdiSubWindowFrameWidth) {
        return qMax(QProxyStyle::pixelMetric(metric, option, widget), 16);
    }
#endif

    return QProxyStyle::pixelMetric(metric, option, widget);
}

void KisWideDockSeparatorStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
#ifdef Q_OS_ANDROID
    if (element == QStyle::PE_FrameWindow) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->fillRect(option->rect, option->palette.window());
        painter->setPen(QColor(76, 82, 117));
        painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
        painter->restore();
        return;
    }
#endif
#ifdef Q_OS_ANDROID
    if (element == QStyle::PE_FrameMenu) {
        QProxyStyle::drawPrimitive(element, option, painter, widget);
        const qreal dpr = painter->device() ? painter->device()->devicePixelRatioF() : 1.0;
        const qreal one = 1.0 / qMax<qreal>(1.0, dpr);
        const QRectF r = option->rect;
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->fillRect(QRectF(r.left(), r.top(), r.width(), one), QColor(110, 114, 156));
        painter->fillRect(QRectF(r.left(), r.top(), one, r.height()), QColor(110, 114, 156));
        painter->fillRect(QRectF(r.left(), r.bottom() + 1.0 - one, r.width(), one), QColor(18, 19, 34));
        painter->fillRect(QRectF(r.right() + 1.0 - one, r.top(), one, r.height()), QColor(18, 19, 34));
        painter->restore();
        return;
    }
#endif
#ifdef Q_OS_ANDROID
    if (element == QStyle::PE_IndicatorDockWidgetResizeHandle) {
        if (element == QStyle::PE_IndicatorDockWidgetResizeHandle) return;
        {
            const int originalWidth = QProxyStyle::pixelMetric(QStyle::PM_DockWidgetSeparatorExtent, option, widget) * 2;
            QStyleOption originalLook(*option);
            QRect r = option->rect;
            if (r.height() >= r.width()) {
                r.setX(r.center().x() - originalWidth / 2);
                r.setWidth(qMin(originalWidth, option->rect.width()));
            } else {
                r.setY(r.center().y() - originalWidth / 2);
                r.setHeight(qMin(originalWidth, option->rect.height()));
            }
            originalLook.rect = r;
            QProxyStyle::drawPrimitive(element, &originalLook, painter, widget);
            return;
        }

#if 0
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        const qreal dpr = painter->device()->devicePixelRatioF();
        QColor tinyDotColor = option->palette.color(QPalette::WindowText);
        tinyDotColor.setAlpha(150);
        painter->setBrush(tinyDotColor);
        const QRect tinyRect = option->rect;
        const bool tinyTall = tinyRect.height() >= tinyRect.width();
        const QPointF tinyCenter = tinyRect.center();
        for (int i = -1; i <= 1; ++i) {
            const qreal offset = i * 25.0 / dpr;
            const QPointF p = tinyTall ? QPointF(tinyCenter.x(), tinyCenter.y() + offset) : QPointF(tinyCenter.x() + offset, tinyCenter.y());
            painter->drawEllipse(p, 3.0 / dpr, 3.0 / dpr);
        }
        painter->restore();
        return;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        QColor dotColor = option->palette.color(QPalette::WindowText);
        dotColor.setAlpha(150);
        painter->setBrush(dotColor);
        const QRect r = option->rect;
        const bool tall = r.height() >= r.width();
        const QPointF c = r.center();
        for (int i = -1; i <= 1; ++i) {
            const QPointF p = tall ? QPointF(c.x(), c.y() + i * 14) : QPointF(c.x() + i * 14, c.y());
            painter->drawEllipse(p, 3.0, 3.0);
        }
        painter->restore();
#endif
    }
#endif
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void KisWideDockSeparatorStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
#ifdef Q_OS_ANDROID
    if (element == QStyle::CE_MenuItem) {
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            if (menuItem->menuItemType == QStyleOptionMenuItem::Separator && menuItem->text.isEmpty()) {
                const QRect r = menuItem->rect;
                const int y = r.center().y();
                painter->save();
                painter->setPen(Qt::NoPen);
                const qreal px = 1.0 / painter->device()->devicePixelRatioF();
                painter->fillRect(QRectF(r.left() + 6, y - px, r.width() - 12, px), QColor(0, 0, 0));
                painter->fillRect(QRectF(r.left() + 6, y, r.width() - 12, px), QColor(255, 255, 255, 110));
                painter->restore();
                return;
            }
            QStyleOptionMenuItem plain(*menuItem);
            const int tab = plain.text.indexOf(QLatin1Char('\t'));
            if (tab >= 0) plain.text = plain.text.left(tab);
            plain.tabWidth = 0;
            QProxyStyle::drawControl(element, &plain, painter, widget);
            return;
        }
    }
#endif
    QProxyStyle::drawControl(element, option, painter, widget);
}

QSize KisWideDockSeparatorStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
#ifdef Q_OS_ANDROID
    if (type == QStyle::CT_MenuItem) {
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            if (menuItem->menuItemType == QStyleOptionMenuItem::Separator && menuItem->text.isEmpty()) {
                QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
                s.setHeight(qMax(s.height(), 6));
                return s;
            }
            QStyleOptionMenuItem plain(*menuItem);
            const int tab = plain.text.indexOf(QLatin1Char('\t'));
            if (tab >= 0) plain.text = plain.text.left(tab);
            plain.tabWidth = 0;
            QSize base = QProxyStyle::sizeFromContents(type, &plain, size, widget);
            base.setWidth(qMin(base.width(), size.width() + 60));
            return base;
        }
    }
#endif
    return QProxyStyle::sizeFromContents(type, option, size, widget);
}

QIcon KisWideDockSeparatorStyle::standardIcon(StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const
{
#ifdef Q_OS_ANDROID
    if (standardIcon == SP_TitleBarCloseButton || standardIcon == SP_TitleBarMinButton
        || standardIcon == SP_TitleBarMaxButton || standardIcon == SP_TitleBarNormalButton) {
        const qreal dpr = widget ? widget->devicePixelRatioF() : 2.0;
        const int logicalSize = 20;
        QPixmap pixmap(int(logicalSize * dpr), int(logicalSize * dpr));
        pixmap.setDevicePixelRatio(dpr);
        pixmap.fill(Qt::transparent);
        QPainter p(&pixmap);
        p.setRenderHint(QPainter::Antialiasing, true);
        QColor color = option ? option->palette.color(QPalette::WindowText) : QColor(230, 230, 235);
        QPen pen(color);
        pen.setWidthF(2.2);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        const qreal a = 4.5, b = 15.5;
        if (standardIcon == SP_TitleBarCloseButton) {
            p.drawLine(QPointF(a, a), QPointF(b, b));
            p.drawLine(QPointF(b, a), QPointF(a, b));
        } else if (standardIcon == SP_TitleBarMinButton) {
            p.drawLine(QPointF(a, b - 1), QPointF(b, b - 1));
        } else if (standardIcon == SP_TitleBarMaxButton) {
            p.drawRect(QRectF(a, a, b - a, b - a));
        } else {
            p.drawRect(QRectF(a, a + 3, b - a - 3, b - a - 3));
            p.drawLine(QPointF(a + 3, a), QPointF(b, a));
            p.drawLine(QPointF(b, a), QPointF(b, b - 3));
        }
        p.end();
        return QIcon(pixmap);
    }
#endif
    return QProxyStyle::standardIcon(standardIcon, option, widget);
}

QRect KisWideDockSeparatorStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget) const
{
#ifdef Q_OS_ANDROID
    if (control == QStyle::CC_SpinBox) {
        if (const QStyleOptionSpinBox *spin = qstyleoption_cast<const QStyleOptionSpinBox*>(option)) {
            if (spin->buttonSymbols != QAbstractSpinBox::NoButtons) {
                const int buttonWidth = 26;
                const QRect r = spin->rect;
                const int h = r.height();
                switch (subControl) {
                case QStyle::SC_SpinBoxUp:
                    return QRect(r.right() - buttonWidth + 1, r.top(), buttonWidth, h / 2);
                case QStyle::SC_SpinBoxDown:
                    return QRect(r.right() - buttonWidth + 1, r.top() + h / 2, buttonWidth, h - h / 2);
                case QStyle::SC_SpinBoxEditField:
                    return QRect(r.left(), r.top(), r.width() - buttonWidth, h);
                case QStyle::SC_SpinBoxFrame:
                    return r;
                default:
                    break;
                }
            }
        }
    }
#endif
    return QProxyStyle::subControlRect(control, option, subControl, widget);
}
