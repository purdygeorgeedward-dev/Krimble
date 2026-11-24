/*
 *  SPDX-FileCopyrightText: 2015 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef LIBKIS_EXTENSION_H
#define LIBKIS_EXTENSION_H

#include "kritalibkis_export.h"

#include <QObject>
#include <Window.h>

class KRITALIBKIS_EXPORT Extension : public QObject
{
    Q_OBJECT
public:

    explicit Extension(QObject *parent = 0);
    ~Extension() override;

    virtual void setup() = 0;

    virtual void createActions(Window *window) = 0;

};

#endif
