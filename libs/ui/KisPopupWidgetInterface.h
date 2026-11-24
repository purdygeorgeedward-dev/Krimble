/*
 *  SPDX-FileCopyrightText: 2021 Emmet O 'Neill <emmetoneill.pdx@gmail.com>
 *  SPDX-FileCopyrightText: 2021 Eoin O 'Neill <eoinoneill1991@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISPOPUPWIDGETINTERFACE_H
#define KISPOPUPWIDGETINTERFACE_H

#include <QWidget>
#include <QGridLayout>

class KisPopupWidgetInterface {
public:
    virtual ~KisPopupWidgetInterface() {}

    virtual void popup(const QPoint& position) = 0;

    virtual bool onScreen() = 0;

    virtual void dismiss() = 0;

Q_SIGNALS:
    virtual void finished() = 0;
};

Q_DECLARE_INTERFACE(KisPopupWidgetInterface, "KisPopupWidgetInterface")

#endif
