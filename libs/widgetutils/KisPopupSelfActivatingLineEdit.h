/*
 * SPDX-FileCopyrightText: 2021 Alvin Wong <alvin@alvinhc.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KIS_POPUP_SELF_ACTIVATING_LINE_EDIT_H
#define KIS_POPUP_SELF_ACTIVATING_LINE_EDIT_H

#include <QLineEdit>

#include <kritawidgetutils_export.h>

class KRITAWIDGETUTILS_EXPORT KisPopupSelfActivatingLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    KisPopupSelfActivatingLineEdit(QWidget *parent = nullptr);
    virtual ~KisPopupSelfActivatingLineEdit();

protected:
    void focusInEvent(QFocusEvent *e) override;
};

#endif
