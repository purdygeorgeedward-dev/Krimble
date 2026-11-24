/*
 *  SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_BUTTON_H
#define KIS_TOOL_BUTTON_H

#include <QToolButton>
#include <kritaui_export.h>

class KRITAUI_EXPORT KisToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit KisToolButton(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    bool m_tabletContact;
};

#endif
