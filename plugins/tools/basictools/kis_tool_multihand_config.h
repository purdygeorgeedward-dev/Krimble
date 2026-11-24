/*
    SPDX-FileCopyrightText: 2016 Scott Petrovic <scottpetrovic@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KISTOOLMULTIHANDCONFIG_H
#define KISTOOLMULTIHANDCONFIG_H

#include "ui_wdgmultihandtool.h"

class KisToolMultiHandConfigWidget : public QWidget, public Ui::WdgMultiHandTool
{
    Q_OBJECT

public:
    KisToolMultiHandConfigWidget(QWidget *parent=0);
    ~KisToolMultiHandConfigWidget() override;

};

#endif
