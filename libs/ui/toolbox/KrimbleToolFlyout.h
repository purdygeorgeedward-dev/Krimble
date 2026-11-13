// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-12 (task 38): George: "I want long press to open a flyout of other buttons. Then a tap selects one or misses and cancels selection."
// The flyout of a tool group: a small popup beside the toolbox button that was long pressed, one big row per tool of the group (picture and name, the tool in use marked).
// A tap on a row picks that tool (its own, hidden toolbox button is clicked, so the tool changes exactly as if it had been tapped), a tap anywhere else only closes the popup:
// it is a Qt popup, which closes on a press outside it, and the press is not handed on to what is underneath (WA_NoMouseReplay), so a miss changes nothing.
#pragma once

#include <QFrame>
#include <QToolButton>
#include <QVBoxLayout>
#include <QRegularExpression>
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QList>

#include "KoToolBoxButton_p.h"

class KrimbleToolFlyout : public QFrame
{
public:
    KrimbleToolFlyout(const QList<KoToolBoxButton*> &members, KoToolBoxButton *current, QWidget *parent)
        : QFrame(parent, Qt::Popup | Qt::FramelessWindowHint)
    {
        setAttribute(Qt::WA_DeleteOnClose);
        setAttribute(Qt::WA_NoMouseReplay);
        setObjectName(QStringLiteral("krimbleToolFlyout"));
        setStyleSheet(QStringLiteral(
            "QFrame#krimbleToolFlyout { background: #181b2e; border: 1px solid #6058aa; }"
            "QToolButton { color: #ffffff; font-size: 15px; padding: 4px 14px 4px 8px; border: none; background: transparent; text-align: left; }"
            "QToolButton:pressed { background: #10498c; }"
            "QToolButton:checked { background: #0b2f66; }"));
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(2, 2, 2, 2);
        layout->setSpacing(0);
        Q_FOREACH (KoToolBoxButton *member, members) {
            QString name = member->toolTip();
            name.remove(QRegularExpression(QStringLiteral("<[^>]*>")));   // the tool tip may carry markup
            QToolButton *row = new QToolButton(this);
            row->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            row->setIcon(member->icon());
            row->setIconSize(member->iconSize() * 3 / 2);
            row->setText(name);
            row->setCheckable(true);
            row->setChecked(member == current);
            row->setMinimumHeight(qMax(44, member->iconSize().height() * 3 / 2 + 14));
            row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            layout->addWidget(row);
            // a tap: close the flyout, then click the member's own button (it may be hidden: click() works on a hidden button)
            QObject::connect(row, &QToolButton::clicked, this, [this, member]() {
                close();
                member->click();
            });
        }
    }

    // beside the button: to the right of it, or to the left when there is no room, kept inside the screen
    void showBeside(QWidget *button)
    {
        adjustSize();
        const QRect avail = (button->screen() ? button->screen()->availableGeometry() : QGuiApplication::primaryScreen()->availableGeometry());
        const QPoint topRight = button->mapToGlobal(QPoint(button->width(), 0));
        const QPoint topLeft = button->mapToGlobal(QPoint(0, 0));
        int x = topRight.x();
        if (x + width() > avail.right()) x = topLeft.x() - width();
        x = qBound(avail.left(), x, qMax(avail.left(), avail.right() - width()));
        // int y = qBound(avail.top(), topLeft.y(), qMax(avail.top(), avail.bottom() - height()));
        // Krimble 2026-11-12 (task 38): George: "Why isn't the flyout next to the tool?" (the mockup of a tall flyout was moved up and ended far from the button). A flyout that does not fit below the button's top now grows UPWARD from the button's bottom edge, so the button is still beside it; only if it does not fit there either is it moved onto the screen.
        int y = topLeft.y();
        if (y + height() > avail.bottom()) y = topLeft.y() + button->height() - height();
        y = qBound(avail.top(), y, qMax(avail.top(), avail.bottom() - height()));
        move(x, y);
        show();
    }
};
