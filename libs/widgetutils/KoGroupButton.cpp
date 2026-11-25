/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 2007 Aurélien Gâteau <agateau@kde.org>
   SPDX-FileCopyrightText: 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
   SPDX-FileCopyrightText: 2012 Jarosław Staniek <staniek@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/
#include "KoGroupButton.h"

#include <QAction>
#include <QStyleOptionToolButton>
#include <QStylePainter>
#include "KrimbleAquaPaint.h"

#include <KLocalizedString>

class Q_DECL_HIDDEN KoGroupButton::Private
{
public:
    Private(KoGroupButton *qq, const GroupPosition position) : groupPosition(position)
    {
        qq->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    }
    GroupPosition groupPosition;
};

KoGroupButton::KoGroupButton(GroupPosition position, QWidget* parent)
    : KisHighlightedToolButton(parent), d(new Private(this, position))
{
}

KoGroupButton::KoGroupButton(QWidget* parent)
    : KisHighlightedToolButton(parent), d(new Private(this, NoGroup))
{
}

KoGroupButton::~KoGroupButton()
{
    delete d;
}

void KoGroupButton::setGroupPosition(KoGroupButton::GroupPosition groupPosition)
{
    d->groupPosition = groupPosition;
}

KoGroupButton::GroupPosition KoGroupButton::groupPosition() const
{
    return d->groupPosition;
}

void KoGroupButton::paintEvent(QPaintEvent* event)
{
    if (groupPosition() == NoGroup) {
        QToolButton::paintEvent(event);
        return;
    }
    QStylePainter painter(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    QStyleOptionToolButton panelOpt = opt;

    QRect& panelRect = panelOpt.rect;
    switch (groupPosition()) {
    case GroupLeft:
        panelRect.setWidth(panelRect.width() * 2);
        break;
    case GroupCenter:
        panelRect.setLeft(panelRect.left() - panelRect.width());
        panelRect.setWidth(panelRect.width() * 3);
        break;
    case GroupRight:
        panelRect.setLeft(panelRect.left() - panelRect.width());
        break;
    case NoGroup:
        Q_ASSERT(0);
    }
    if (autoRaise()) {
        if (!isChecked() && !isDown() && !(panelOpt.state & QStyle::State_MouseOver)) {
            panelOpt.state |= (QStyle::State_On | QStyle::State_Sunken);
            QPalette panelPal(panelOpt.palette);
            QColor c;
            c = panelPal.color(QPalette::Button);
            c.setAlpha(50);
            panelPal.setColor(QPalette::Button, c);
            c = panelPal.color(QPalette::Window);
            c.setAlpha(50);
            panelPal.setColor(QPalette::Window, c);
            panelOpt.palette = panelPal;
            painter.setOpacity(0.5);
        }
    } else {

        if (!isChecked() && !isDown() && !(panelOpt.state & QStyle::State_MouseOver)) {

        } else {
            panelOpt.state |= (QStyle::State_On | QStyle::State_Sunken);
            QPalette panelPal(panelOpt.palette);
            QColor c;
            c = panelPal.color(QPalette::Button);
            c.setAlpha(50);
            panelPal.setColor(QPalette::Button, c);
            c = panelPal.color(QPalette::Window);
            c.setAlpha(50);
            panelPal.setColor(QPalette::Window, c);
            panelOpt.palette = panelPal;
            painter.setOpacity(0.5);
        }
    }

    if (isChecked()) {
        painter.setOpacity(1.0);
        krimblePaintSelectedGlow(painter, QRectF(rect()), devicePixelRatioF());
    } else {
        painter.drawPrimitive(QStyle::PE_PanelButtonTool, panelOpt);
    }
    painter.setOpacity(1.0);

    const int y1 = opt.rect.top() + 1;
    const int y2 = opt.rect.bottom() - 1;
    painter.setOpacity(0.4);
    if (d->groupPosition != GroupRight) {
        const int x = opt.rect.right();
        painter.setPen(QPen(opt.palette.color(QPalette::Dark), 0));
        painter.drawLine(x, y1, x, y2);
    }
    painter.setOpacity(1.0);

    painter.drawControl(QStyle::CE_ToolButtonLabel, opt);

    if (!actions().isEmpty()) {
        QAction* action = actions().first();
        setToolTip(i18nc("@info:tooltip of custom triple button", "%1", action->toolTip()));
    }
}
