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
#include <functional>

#include "KoToolBoxButton_p.h"
#include <KoToolManager.h>

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
            // "QToolButton { color: #ffffff; font-size: 15px; padding: 4px 14px 4px 8px; border: none; background: transparent; text-align: left; }"
            "QToolButton { color: #ffffff; font-size: 12px; padding: 1px 10px 1px 2px; border: none; background: transparent; text-align: left; }"
            "QToolButton:pressed { background: #10498c; }"
            // "QToolButton:checked { background: #0b2f66; }"));
            "QToolButton:checked { background: #0b2f66; }"
            "QToolButton#krimbleFlyoutCurrent { background: #0b87dd; }"));   // the row in use: the flat selection blue (a checked row was painted Aqua)
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(2, 2, 2, 2);
        layout->setSpacing(0);
        Q_FOREACH (KoToolBoxButton *member, members) {
            QString name = member->toolTip();
            name.remove(QRegularExpression(QStringLiteral("<[^>]*>")));   // the tool tip may carry markup
            if (name.trimmed().isEmpty() && member->krimbleToolAction()) name = member->krimbleToolAction()->toolTip();
            if (name.trimmed().isEmpty()) name = member->objectName().section(QLatin1Char('/'), -1);
            name.replace(QRegularExpression(QStringLiteral("\\s*\\(\\s*\\)\\s*$")), QString());
            name.replace(QRegularExpression(QStringLiteral("\\sTool\\s*$")), QString());
            // QToolButton *row = new QToolButton(this);
            // row->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            // row->setIcon(member->icon());
            // row->setIconSize(member->iconSize());
            // row->setText(name);
            // row->setCheckable(true);
            // row->setChecked(member == current);
            // // row->setMinimumHeight(qMax(44, member->iconSize().height() * 3 / 2 + 14));
            // row->setMinimumHeight(qMax(member->height(), member->iconSize().height() + 6));
            // row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            // layout->addWidget(row);
            // // a tap: close the flyout, then click the member's own button (it may be hidden: click() works on a hidden button)
            // QObject::connect(row, &QToolButton::clicked, this, [this, member]() {
            //     close();
            //     member->click();
            // });
            // a tap: close the flyout, then click the member's own button (it may be hidden: click() works on a hidden button)
            addRow(member->icon(), name, member->iconSize(), member->height(), member == current, [member]() { member->click(); });
        }
    }

    // one row: picture and name; the row in use is marked in the flat selection blue (not a checked button, which is painted Aqua)
    void addRow(const QIcon &icon, const QString &name, const QSize &iconSize, int minHeight, bool isCurrent, const std::function<void()> &onTap)
    {
        QToolButton *row = new QToolButton(this);
        row->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        row->setIcon(icon);
        row->setIconSize(iconSize);
        row->setText(name);
        if (isCurrent) row->setObjectName(QStringLiteral("krimbleFlyoutCurrent"));
        row->setMinimumHeight(qMax(minHeight, iconSize.height() + 6));
        row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        layout()->addWidget(row);
        QObject::connect(row, &QToolButton::clicked, this, [this, onTap]() {
            close();
            onTap();
        });
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
        int y = topLeft.y();
        if (y + height() > avail.bottom()) y = topLeft.y() + button->height() - height();
        y = qBound(avail.top(), y, qMax(avail.top(), avail.bottom() - height()));
        move(x, y);
        show();
    }
};
