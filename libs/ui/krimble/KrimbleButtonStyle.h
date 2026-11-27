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
        for (const QWidget *p = w; p; p = p->parentWidget()) {
            if (p->inherits("KisColorSelectorNgDock")) return false;
        }
        return true;
    }

    static void krimblePaintPill(QPainter *p, const QRectF &rect, bool selected, bool enabled, qreal dpr)
    {
        const qreal one = 1.0 / qMax<qreal>(1.0, dpr);
        const qreal ring = selected ? 2.0 * one : 0.0;
        const QRectF r = rect.adjusted(ring, ring, -ring, -ring);
        const qreal rad = r.height() / 2.0;
        QPainterPath path;
        path.addRoundedRect(r, rad, rad);

        p->save();
        p->setRenderHint(QPainter::Antialiasing, true);

        QLinearGradient body(r.topLeft(), r.bottomLeft());
        if (selected) {
            body.setColorAt(0.0, QColor(40, 150, 220));
            body.setColorAt(0.5, QColor(18, 110, 190));
            body.setColorAt(1.0, QColor(8, 70, 140));
        } else if (enabled) {
            body.setColorAt(0.0, QColor(88, 91, 124));
            body.setColorAt(0.5, QColor(66, 69, 99));
            body.setColorAt(1.0, QColor(46, 48, 72));
        } else {
            body.setColorAt(0.0, QColor(62, 64, 88));
            body.setColorAt(1.0, QColor(44, 46, 66));
        }
        p->fillPath(path, body);

        p->save();
        p->setClipPath(path);
        const QRectF shine(r.left() + rad * 0.35, r.top() + r.height() * 0.06, r.width() - rad * 0.7, r.height() * 0.46);
        QPainterPath shinePath;
        shinePath.addRoundedRect(shine, shine.height() / 2.0, shine.height() / 2.0);
        QLinearGradient shineFill(shine.topLeft(), shine.bottomLeft());
        shineFill.setColorAt(0.0, QColor(255, 255, 255, selected ? 110 : (enabled ? 60 : 30)));
        shineFill.setColorAt(1.0, QColor(255, 255, 255, 0));
        p->fillPath(shinePath, shineFill);
        p->restore();

        const qreal w = 2.0 * one;
        const QRectF ri = r.adjusted(w / 2.0, w / 2.0, -w / 2.0, -w / 2.0);
        QPainterPath bevel;
        bevel.addRoundedRect(ri, ri.height() / 2.0, ri.height() / 2.0);
        QLinearGradient edge(r.topLeft(), r.bottomLeft());
        edge.setColorAt(0.0, QColor(255, 255, 255, selected ? 150 : (enabled ? 95 : 45)));
        edge.setColorAt(0.45, QColor(255, 255, 255, 0));
        edge.setColorAt(0.55, QColor(0, 0, 0, 0));
        edge.setColorAt(1.0, QColor(0, 0, 0, selected ? 150 : 130));
        p->setPen(QPen(QBrush(edge), w));
        p->setBrush(Qt::NoBrush);
        p->drawPath(bevel);

        if (selected) {
            QPainterPath outer;
            const QRectF ro = rect.adjusted(one, one, -one, -one);
            outer.addRoundedRect(ro, ro.height() / 2.0, ro.height() / 2.0);
            p->setPen(QPen(QColor(120, 215, 255), 2.0 * one));
            p->drawPath(outer);
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
        if ((type == CT_PushButton || type == CT_ToolButton) && krimbleHandles(widget)) {
            const int h = qMax(s.height(), 36);
            s.setHeight(h);
            s.setWidth(qMax(s.width() + h / 2, h + h / 2));
        }
        return s;
    }
};
