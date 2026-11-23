/*
 * Krimble 2026-11-23: two additions to every menu (QMenu), installed by KrimbleGlowFilter when a menu is first polished.
 *
 * 868th: lines between the columns of a menu that is too tall for the screen and wraps into columns (the Panels menu).
 * George: "Need vertical dividers for menus that expand wide." / "Dark vertical lines." / "Add a hilight edge like the separator has.
 * Dark line and a light line." / "I can't see the dark line." Qt has no style sheet setting for this, so the lines are painted after the
 * menu's own paint. Only between columns: nothing at the top edge, where the menu meets its title
 * ("THE MENUS TOUCH THEIR TITLE AREAS WITH NO DIVIDING LINES IN BETWEEN EVER.").
 *
 * 876th: a barely visible light line down the left edge of every menu (only the left edge).
 *
 * 875th: a small soft cast shadow on the left, right and bottom of a menu, never on the top (the menu joins its title there).
 * George: "Can menus have a small shadow?" / "Can it be softer and a little bigger?" / "Add it. Keep it small."
 * The shadow is its own see-through window just behind the menu, so the menu itself (its size, its place, its submenus) is unchanged.
 */
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
#include <QtMath>
#include <climits>
#include <cmath>

// a soft shadow on the left, right and bottom of 'menu'; nothing above its top edge.
// Thin layers stacked from the outside in: darkest at the menu's edge, fading smoothly to nothing.
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
    static constexpr int kSize = 6;   // units around the menu (about 18 real px on the phone): "Keep it small."

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
        if (qApp->property("krimbleCyberTheme").toBool()) return;   // Krimble Cyber: its menus are see-through with their own glow
        if (!m_shadow) m_shadow = new KrimbleMenuShadowWindow();
        m_shadow->follow(menu);
        if (!m_shadow->isVisible()) {
            m_shadow->show();
            menu->raise();   // the menu stays in front of its shadow
        }
        m_shadow->update();
    }

    bool paintDividers(QMenu *menu, QEvent *event)
    {
        if (m_inside) return false;

        // column left edge -> its right edge, and the span the items cover from top to bottom
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
        // if (columns.size() < 2) return false;   // one column: the menu is painted as always, nothing added
        // (876th: every menu is painted here now, for its faint left edge)

        m_inside = true;
        QCoreApplication::sendEvent(menu, event);   // the menu's own paint first
        m_inside = false;

        QPainter p(menu);
        p.setPen(Qt::NoPen);
        // 876th: a barely visible light line down the menu's left edge only, not around it.
        // George: "On the left edge of the menu add a barely perceptible light edge. Just barely and only the left edge. Not all around."
        if (!qApp->property("krimbleCyberTheme").toBool()) p.fillRect(QRectF(0, 0, 1.0, menu->height()), QColor(255, 255, 255, 18));
        const qreal thick = 1.0;   // one unit, like the lines of the menu's horizontal separator
        QMap<int, int>::const_iterator previous = columns.constBegin();
        for (QMap<int, int>::const_iterator it = columns.constBegin() + 1; it != columns.constEnd(); ++it, ++previous) {
            const qreal x = (previous.value() + 1 + it.key()) / 2.0 - thick / 2.0;   // midway between the two columns
            // the dark line twice as wide as the light one: one unit of black did not show on the dark menu ("I can't see the dark line.")
            p.fillRect(QRectF(x - thick, top, 2 * thick, bottom + 1 - top), QColor(0, 0, 0));
            p.fillRect(QRectF(x + thick, top, thick, bottom + 1 - top), QColor(255, 255, 255, 110));
        }
        return true;
    }

    bool m_inside = false;
    QPointer<KrimbleMenuShadowWindow> m_shadow;
};
