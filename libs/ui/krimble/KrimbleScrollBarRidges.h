/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include <QObject>
#include <QEvent>
#include <QPainter>
#include <QScrollBar>
#include <QStyleOptionSlider>
#include <QApplication>

class KrimbleScrollBarGrip : public QObject
{
public:
    explicit KrimbleScrollBarGrip(QObject *parent) : QObject(parent) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() != QEvent::Paint || m_busy) return false;
        QScrollBar *bar = qobject_cast<QScrollBar*>(watched);
        if (!bar || bar->maximum() <= bar->minimum()) return false;
        m_busy = true;
        watched->event(event);
        m_busy = false;
        QStyleOptionSlider opt;
        opt.initFrom(bar);
        opt.orientation = bar->orientation();
        opt.minimum = bar->minimum();
        opt.maximum = bar->maximum();
        opt.sliderPosition = bar->sliderPosition();
        opt.sliderValue = bar->value();
        opt.pageStep = bar->pageStep();
        opt.singleStep = bar->singleStep();
        opt.upsideDown = bar->invertedAppearance();
        opt.subControls = QStyle::SC_All;
        const QRectF handle = bar->style()->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSlider, bar);
        if (handle.width() < 6 || handle.height() < 6) return true;
        const QColor base = bar->palette().color(QPalette::Window);
        const QColor light = base.lighter(165);
        const QColor dark = base.darker(230);
        QPainter p(bar);
        const QRectF r = handle.adjusted(0.5, 0.5, -0.5, -0.5);
        p.setPen(QPen(light, 1));
        p.drawLine(r.topLeft(), r.topRight());
        p.drawLine(r.topLeft(), r.bottomLeft());
        p.setPen(QPen(dark, 1));
        p.drawLine(r.bottomLeft(), r.bottomRight());
        p.drawLine(r.topRight(), r.bottomRight());
        const bool vertical = bar->orientation() == Qt::Vertical;
        const qreal gap = 3.0;
        const int ridges = qMax(2, int((vertical ? handle.height() : handle.width()) * 0.40 / gap));
        const qreal across = (vertical ? handle.width() : handle.height()) * 0.55;
        const QPointF c = handle.center();
        for (int i = 0; i < ridges; ++i) {
            const qreal offset = (i - (ridges - 1) / 2.0) * gap;
            if (vertical) {
                const qreal y = qRound(c.y() + offset);
                p.setPen(QPen(light, 1));
                p.drawLine(QPointF(c.x() - across / 2, y), QPointF(c.x() + across / 2, y));
                p.setPen(QPen(dark, 1));
                p.drawLine(QPointF(c.x() - across / 2, y + 1), QPointF(c.x() + across / 2, y + 1));
            } else {
                const qreal x = qRound(c.x() + offset);
                p.setPen(QPen(light, 1));
                p.drawLine(QPointF(x, c.y() - across / 2), QPointF(x, c.y() + across / 2));
                p.setPen(QPen(dark, 1));
                p.drawLine(QPointF(x + 1, c.y() - across / 2), QPointF(x + 1, c.y() + across / 2));
            }
        }
        return true;
    }
private:
    bool m_busy {false};
};

class KrimbleScrollBarHooks : public QObject
{
public:
    explicit KrimbleScrollBarHooks(QObject *parent) : QObject(parent), m_grip(new KrimbleScrollBarGrip(this)) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::Polish) {
            if (QScrollBar *bar = qobject_cast<QScrollBar*>(watched)) {
                if (!bar->property("krimbleGripHooked").toBool()) {
                    bar->setProperty("krimbleGripHooked", true);
                    bar->installEventFilter(m_grip);
                    bar->update();
                }
            }
        }
        return false;
    }
    static void install()
    {
        if (qApp->findChild<KrimbleScrollBarHooks*>(QString(), Qt::FindDirectChildrenOnly)) return;
        qApp->installEventFilter(new KrimbleScrollBarHooks(qApp));
    }
private:
    KrimbleScrollBarGrip *m_grip;
};
