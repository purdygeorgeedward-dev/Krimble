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

inline void krimblePaintMenuShadow(QPainter &p, const QRectF &menu, qreal size, qreal strength = 0.30)
{
    const int steps = qMax(1, qRound(size));
    const qreal each = 1.0 - std::pow(1.0 - strength, 1.0 / steps);
    for (int i = steps; i >= 1; --i) {
        const qreal t = qreal(i) / steps;
        QColor c(0, 0, 0);
        c.setAlphaF(qBound<qreal>(0.0, each * (1.0 - t * t) * 2.0, 1.0));
        p.fillRect(QRectF(menu.left() - i, menu.top(), menu.width() + 2 * i, menu.height() + i), c);
    }
}

class KrimbleMenuShadowWindow : public QWidget
{
public:
    static constexpr int kSize = 6;

    KrimbleMenuShadowWindow()
        : QWidget(nullptr, Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_ShowWithoutActivating);
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
    }
    void follow(QMenu *menu)
    {
        setGeometry(menu->geometry().adjusted(-kSize, 0, kSize, kSize));
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.fillRect(rect(), Qt::transparent);
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);
        krimblePaintMenuShadow(p, QRectF(kSize, 0, width() - 2 * kSize, height() - kSize), kSize);
    }
};

class KrimbleMenuColumnDividers : public QObject
{
public:
    explicit KrimbleMenuColumnDividers(QObject *parent) : QObject(parent) {}
    ~KrimbleMenuColumnDividers() override { delete m_shadow; }

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        QMenu *menu = qobject_cast<QMenu*>(watched);
        if (!menu) return false;
        switch (event->type()) {
        case QEvent::Show:
        case QEvent::Move:
        case QEvent::Resize:
            showShadow(menu);
            return false;
        case QEvent::Hide:
            if (m_shadow) m_shadow->hide();
            return false;
        case QEvent::Paint:
            return paintDividers(menu, event);
        default:
            return false;
        }
    }

private:
    void showShadow(QMenu *menu)
    {
        if (!menu->isVisible()) return;
        if (qApp->property("krimbleCyberTheme").toBool()) return;
        if (!m_shadow) m_shadow = new KrimbleMenuShadowWindow();
        m_shadow->follow(menu);
        if (!m_shadow->isVisible()) {
            m_shadow->show();
            menu->raise();
        }
        m_shadow->update();
    }

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
    QPointer<KrimbleMenuShadowWindow> m_shadow;
};
