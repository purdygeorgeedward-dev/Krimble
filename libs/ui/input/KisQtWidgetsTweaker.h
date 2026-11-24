/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2017 Nikita Vertikov <kitmouse.nikita@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KISQTWIDGETTWEAKER_H
#define KISQTWIDGETTWEAKER_H

#include <QObject>
#include "kritaui_export.h"

class QEvent;

class KRITAUI_EXPORT KisQtWidgetsTweaker : public QObject
{
    Q_OBJECT
public:
    KisQtWidgetsTweaker(QObject* parent = nullptr);
    ~KisQtWidgetsTweaker();
    bool eventFilter(QObject *receiver, QEvent* event) override;
    static KisQtWidgetsTweaker *instance();

private:
    struct Private;
    Private* d;
};

#endif
