/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QEvent>
#include <QMap>
#include <QMenu>
#include <QPainter>
#include <climits>

class KrimbleMenuColumnDividers : public QObject
{
public:
    explicit KrimbleMenuColumnDividers(QObject *parent) : QObject(parent) {}

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::Polish) {
            if (QMenu *menu = qobject_cast<QMenu*>(watched)) {
                if (!menu->property("krimbleColumnDividers").toBool()) {
                    menu->setProperty("krimbleColumnDividers", true);
                    menu->installEventFilter(this);
                }
            }
            return false;
        }
        if (event->type() == QEvent::Paint) {
            if (QMenu *menu = qobject_cast<QMenu*>(watched)) return paintDividers(menu, event);
        }
        return false;
    }

    static void install()
    {
        if (qApp->findChild<KrimbleMenuColumnDividers*>(QStringLiteral("krimbleMenuColumnDividers"), Qt::FindDirectChildrenOnly)) return;
        KrimbleMenuColumnDividers *dividers = new KrimbleMenuColumnDividers(qApp);
        dividers->setObjectName(QStringLiteral("krimbleMenuColumnDividers"));
        qApp->installEventFilter(dividers);
        Q_FOREACH (QWidget *w, QApplication::allWidgets()) {
            if (QMenu *menu = qobject_cast<QMenu*>(w)) {
                if (!menu->property("krimbleColumnDividers").toBool()) {
                    menu->setProperty("krimbleColumnDividers", true);
                    menu->installEventFilter(dividers);
                }
            }
        }
    }

private:
    bool paintDividers(QMenu *menu, QEvent *event)
    {
        if (m_inside) return false;

        QMap<int, int> columns;
        Q_FOREACH (QAction *action, menu->actions()) {
            if (!action->isVisible()) continue;
            const QRect r = menu->actionGeometry(action);
            if (r.isEmpty()) continue;
            columns[r.left()] = qMax(columns.value(r.left(), r.right()), r.right());
        }

        m_inside = true;
        QCoreApplication::sendEvent(menu, event);
        m_inside = false;

        if (columns.size() < 2) return true;
        QPainter p(menu);
        p.setPen(Qt::NoPen);
        const qreal thick = 1.0;
        const qreal top = 0.0;
        const qreal height = menu->height();
        QMap<int, int>::const_iterator previous = columns.constBegin();
        for (QMap<int, int>::const_iterator it = columns.constBegin() + 1; it != columns.constEnd(); ++it, ++previous) {
            const qreal x = (previous.value() + 1 + it.key()) / 2.0 - thick / 2.0;
            p.fillRect(QRectF(x - thick, top, 2 * thick, height), QColor(0, 0, 0));
            p.fillRect(QRectF(x + thick, top, thick, height), QColor(255, 255, 255, 110));
        }
        return true;
    }

    bool m_inside = false;
};
