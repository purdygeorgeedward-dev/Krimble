#pragma once

#include <QAction>
#include <QCoreApplication>
#include <QEvent>
#include <QMap>
#include <QMenu>
#include <QPainter>
#include <QPointer>
#include <QWidget>
#include <QApplication>
#include "../KrimbleCyberTheme.h"
#include <QtMath>
#include <climits>
#include <cmath>

class KrimbleMenuColumnDividers : public QObject
{
public:
    explicit KrimbleMenuColumnDividers(QObject *parent) : QObject(parent) {}

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        QMenu *menu = qobject_cast<QMenu*>(watched);
        if (!menu) return false;
        if (event->type() == QEvent::Paint) return paintDividers(menu, event);
        return false;
    }

private:
    bool paintDividers(QMenu *menu, QEvent *event)
    {
        if (m_inside) return false;

        QMap<int, int> columns;
        int top = INT_MAX, bottom = INT_MIN;
        Q_FOREACH (QAction *action, menu->actions()) {
            if (!action->isVisible()) continue;
            const QRect r = menu->actionGeometry(action);
            if (r.isEmpty()) continue;
            columns[r.left()] = qMax(columns.value(r.left(), r.right()), r.right());
            top = qMin(top, r.top());
            bottom = qMax(bottom, r.bottom());
        }

        m_inside = true;
        QCoreApplication::sendEvent(menu, event);
        m_inside = false;

        QPainter p(menu);
        p.setPen(Qt::NoPen);
        if (qApp->property("krimbleCyberTheme").toBool()) {
            if (menu->activeAction()) krimbleCyberPaintMenuSelectionEnd(p, menu->actionGeometry(menu->activeAction()), menu->width());
            krimbleCyberPaintMenuStripe(p, menu->size());
        } else {
            p.fillRect(QRectF(0, 0, 1.0, menu->height()), QColor(255, 255, 255, 18));
        }
        const qreal thick = 1.0;
        QMap<int, int>::const_iterator previous = columns.constBegin();
        for (QMap<int, int>::const_iterator it = columns.constBegin() + 1; it != columns.constEnd(); ++it, ++previous) {
            const qreal x = (previous.value() + 1 + it.key()) / 2.0 - thick / 2.0;
            p.fillRect(QRectF(x - thick, top, 2 * thick, bottom + 1 - top), QColor(0, 0, 0));
            p.fillRect(QRectF(x + thick, top, thick, bottom + 1 - top), QColor(255, 255, 255, 110));
        }
        return true;
    }

    bool m_inside = false;
};
