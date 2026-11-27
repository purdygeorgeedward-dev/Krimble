/*
 * SPDX-FileCopyrightText: 2020 Ashwin Dhakaita <ashwingpdhakaita@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef MY_PAINTOP_PLUGIN_H_
#define MY_PAINTOP_PLUGIN_H_

#include <QObject>
#include <QVariant>

class MyPaintOpPlugin : public QObject
{
    Q_OBJECT
public:
    MyPaintOpPlugin(QObject *parent, const QVariantList &);
    ~MyPaintOpPlugin() override;
};

#endif
