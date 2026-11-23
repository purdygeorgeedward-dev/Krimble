/*
 *  SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_button.h"

#include <QMouseEvent>
#include <QStyleOptionToolButton>

KisToolButton::KisToolButton(QWidget *parent) :
    QToolButton(parent)
{
    m_tabletContact = false;
}

void KisToolButton::mousePressEvent(QMouseEvent *e)
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    QRect popupr = style()->subControlRect(QStyle::CC_ToolButton, &opt,
                                           QStyle::SC_ToolButtonMenu, this);
    // if (popupr.isValid() && !popupr.contains(e->pos())) {
    // Krimble 2026-11-23: a button without a menu is a plain button everywhere (the Layers + had a dead strip where its menu used to be)
    if (!menu() || (popupr.isValid() && !popupr.contains(e->pos()))) {
        QToolButton::mousePressEvent(e);
    } else {
        m_tabletContact = true;
    }
}

void KisToolButton::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_tabletContact) {
        QToolButton::mousePressEvent(e);
    } else {
        QToolButton::mouseReleaseEvent(e);
    }
    m_tabletContact = false;
}
