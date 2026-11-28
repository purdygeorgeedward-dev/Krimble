/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include <QProxyStyle>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QStyleOption>
#include <QPushButton>
#include <QToolButton>
#include <QDialog>
#include <QApplication>
#include <QMdiArea>
#include <QMenuBar>

class KrimbleButtonStyle : public QProxyStyle
{
public:
    explicit KrimbleButtonStyle(QStyle *base) : QProxyStyle(base) {}

    static bool krimbleHandles(const QWidget *w)
    {
        if (!w) return false;
        if (!qobject_cast<const QPushButton *>(w)) {
            if (!qobject_cast<const QToolButton *>(w)) return false;
            bool inDialog = false;
            for (const QWidget *p = w->parentWidget(); p; p = p->parentWidget()) {
                if (qobject_cast<const QDialog *>(p)) { inDialog = true; break; }
            }
            if (!inDialog) return false;
        }
        if (w->inherits("KisColorButton") || w->inherits("KoColorPopupButton") || w->inherits("KoDualColorButton")) return false;
        for (const QWidget *p = w; p; p = p->parentWidget()) {
            if (p->inherits("KisColorSelectorNgDock")) return false;
        }
        return true;
    }

    static void krimblePaintPill(QPainter *p, const QRectF &rect, bool selected, bool enabled, qreal dpr)
    {
        const qreal real = 1.0 / qMax<qreal>(1.0, dpr);
        const QRectF r = rect.adjusted(4.0 * real, 4.0 * real, -4.0 * real, -4.0 * real);
        if (r.width() < 2 || r.height() < 2) return;
        // rounded boxes: only the corners are rounded, not the sides (George, Nov 28 2026)
        const qreal radius = qMin<qreal>(6.0, qMin(r.width(), r.height()) / 2.0);
        p->save();
        p->setRenderHint(QPainter::Antialiasing, true);
        p->setPen(Qt::NoPen);
        if (selected) {
            for (int i = 4; i >= 1; --i) {
                QColor c(70, 175, 225);
                c.setAlpha(18 + (4 - i) * 22);
                p->setBrush(c);
                p->drawRoundedRect(r.adjusted(-i * real, -i * real, i * real, i * real), radius + i * real, radius + i * real);
            }
            QLinearGradient face(r.topLeft(), r.bottomLeft());
            face.setColorAt(0.0, QColor(13, 135, 198));
            face.setColorAt(0.49, QColor(9, 99, 157));
            face.setColorAt(0.5, QColor(7, 65, 115));
            face.setColorAt(1.0, QColor(0, 77, 129));
            p->setBrush(face);
            p->setPen(QPen(QColor(113, 226, 255), 2.0 * real));
            p->drawRoundedRect(r, radius, radius);
        } else {
            const QColor window = qApp->palette().color(QPalette::Window);
            QLinearGradient face(r.topLeft(), r.bottomLeft());
            face.setColorAt(0.0, enabled ? window.lighter(118) : window);
            face.setColorAt(1.0, enabled ? window.darker(135) : window.darker(115));
            p->setBrush(face);
            p->drawRoundedRect(r, radius, radius);
        }
        p->restore();
    }

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override
    {
        if ((element == PE_PanelButtonCommand || element == PE_PanelButtonTool || element == PE_PanelButtonBevel || element == PE_FrameButtonTool)
                && krimbleHandles(widget)) {
            if (element == PE_FrameButtonTool) return;
            const bool selected = option->state & (State_Sunken | State_On);
            const bool enabled = option->state & State_Enabled;
            krimblePaintPill(painter, QRectF(option->rect), selected, enabled, widget ? widget->devicePixelRatioF() : 1.0);
            return;
        }
        if (element == PE_FrameFocusRect && krimbleHandles(widget)) return;
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }

    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override
    {
        if (control == CC_ToolButton && krimbleHandles(widget)) {
            if (const QStyleOptionToolButton *tb = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
                QStyleOptionToolButton copy(*tb);
                const bool selected = tb->state & (State_Sunken | State_On);
                krimblePaintPill(painter, QRectF(tb->rect), selected, tb->state & State_Enabled, widget ? widget->devicePixelRatioF() : 1.0);
                copy.state &= ~(State_Sunken | State_On | State_Raised | State_MouseOver | State_AutoRaise | State_HasFocus);
                drawControl(CE_ToolButtonLabel, &copy, painter, widget);
                return;
            }
        }
        QProxyStyle::drawComplexControl(control, option, painter, widget);
    }

    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget = nullptr) const override
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == CT_MdiControls && widget && qobject_cast<const QMdiArea *>(widget)) {
            if (QMenuBar *bar = widget->window()->findChild<QMenuBar *>()) {
                const int h = bar->fontMetrics().height() + 8;
                return QSize(3 * h, h);
            }
        }
        if ((type == CT_PushButton || type == CT_ToolButton) && krimbleHandles(widget)) {
            const int h = qMax(s.height(), 36);
            s.setHeight(h);
            s.setWidth(qMax(s.width() + h / 2, h + h / 2));
        }
        return s;
    }
};
