// SPDX-License-Identifier: GPL-2.0-or-later
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
            "QToolButton { color: #ffffff; font-size: 12px; padding: 1px 10px 1px 2px; border: none; background: transparent; text-align: left; }"
            "QToolButton:pressed { background: #10498c; }"
            "QToolButton:checked { background: #0b2f66; }"
            "QToolButton#krimbleFlyoutCurrent { background: #0b87dd; }"));
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(2, 2, 2, 2);
        layout->setSpacing(0);
        Q_FOREACH (KoToolBoxButton *member, members) {
            QString name = member->toolTip();
            name.remove(QRegularExpression(QStringLiteral("<[^>]*>")));
            if (name.trimmed().isEmpty() && member->krimbleToolAction()) name = member->krimbleToolAction()->toolTip();
            if (name.trimmed().isEmpty()) name = member->objectName().section(QLatin1Char('/'), -1);
            name.replace(QRegularExpression(QStringLiteral("\\s*\\(\\s*\\)\\s*$")), QString());
            name.replace(QRegularExpression(QStringLiteral("\\sTool\\s*$")), QString());
            addRow(member->icon(), name, member->iconSize(), member->height(), member == current, [member]() { member->click(); });
        }
    }

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
