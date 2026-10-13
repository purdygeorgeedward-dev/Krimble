/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
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
    if (metric == QStyle::PM_DockWidgetSeparatorExtent) {
        // Krimble 2026-10-10: on Android at least 20 px (the same touch size as the dialog edges), so a finger can grab it.
        // return QProxyStyle::pixelMetric(metric, option, widget) * 2;
#ifdef Q_OS_ANDROID
        // Krimble 2026-10-10 (3rd): 14 (was 20): each strip is about 32 device pixels narrower, which gives the toolbox room to widen.
        // return qMax(QProxyStyle::pixelMetric(metric, option, widget) * 2, 20);
        // return qMax(QProxyStyle::pixelMetric(metric, option, widget) * 2, 14);
        // Krimble 2026-10-12: George: no "UI fat" along the edge. The strip is the base style's own thin one again; the touch
        // zone is made by an invisible grab area inside each panel instead (KrimbleDockEdgeGrab in KisMainWindow.cpp).
        return QProxyStyle::pixelMetric(metric, option, widget);
#else
        return QProxyStyle::pixelMetric(metric, option, widget) * 2;
#endif
    }

    return QProxyStyle::pixelMetric(metric, option, widget);
}

void KisWideDockSeparatorStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
#ifdef Q_OS_ANDROID
    if (element == QStyle::PE_IndicatorDockWidgetResizeHandle) {
        // Krimble 2026-10-12: the strip draws nothing now (invisible); the code below is kept, not deleted, and is not reached.
        return;
        // Krimble 2026-10-11: George wants the ORIGINAL default look of these edges, with only the grabbing made bigger. The
        // strip is wider so a finger can catch it, but what is drawn is exactly what the base style drew before: its own
        // handle, in a strip of the original width, centered in the wide one.
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
        // (4th) the three tiny dots, 6 device pixels across and 25 apart, and no line:
        // Krimble 2026-10-10 (4th): George: the original tiny dots were visually right
        // (4th) comment:, only hard to grab. The strip is 20 px
        // wide so a finger can grab it; what is drawn is the same tiny grip as before: three dots, each 6 device pixels
        // across and 25 device pixels apart, and no line or band.
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

        // (3rd) draw nothing at all, and the older versions below, are kept, not deleted. They are not reached.
        // (2nd) three larger dots (radius 3 logical pixels, which is about 8 device pixels: too big):
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
                // Krimble 2026-10-10: the divider between menu items: a black line with a highlight line below it.
                const QRect r = menuItem->rect;
                const int y = r.center().y();
                painter->save();
                painter->setPen(Qt::NoPen);
                // Krimble 2026-10-10 (2nd): each line is exactly one device pixel thick (the 2 logical pixel lines were about 5).
                const qreal px = 1.0 / painter->device()->devicePixelRatioF();
                // painter->fillRect(QRect(r.left() + 6, y - 1, r.width() - 12, 2), QColor(0, 0, 0));
                // painter->fillRect(QRect(r.left() + 6, y + 1, r.width() - 12, 2), QColor(255, 255, 255, 110));
                painter->fillRect(QRectF(r.left() + 6, y - px, r.width() - 12, px), QColor(0, 0, 0));
                painter->fillRect(QRectF(r.left() + 6, y, r.width() - 12, px), QColor(255, 255, 255, 110));
                painter->restore();
                return;
            }
            // Krimble 2026-10-10: draw the item without its shortcut text (no keyboard on a touch screen).
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
                // s.setHeight(qMax(s.height(), 8));
                s.setHeight(qMax(s.height(), 6));   // Krimble 2026-10-10 (2nd): slot 6 (was 8) for the 1 pixel lines
                return s;
            }
            // Krimble 2026-10-10: no keyboard on a touch screen, so no shortcut text and no column reserved for it.
            QStyleOptionMenuItem plain(*menuItem);
            const int tab = plain.text.indexOf(QLatin1Char('\t'));
            if (tab >= 0) plain.text = plain.text.left(tab);
            plain.tabWidth = 0;
            // return QProxyStyle::sizeFromContents(type, &plain, size, widget);
            // Krimble 2026-10-12: George: "Menus are still too wide." The width of an item is at most its text plus 60 px (the
            // check/icon margin on the left and the arrow on the right), however much the base style adds.
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
    // Krimble 2026-10-11: the window buttons (close, minimize, maximize, restore) drawn by the base style came out as dashes and
    // blobs on a phone; the close button was not an X. They are drawn here: lines in the text color on a transparent square.
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
