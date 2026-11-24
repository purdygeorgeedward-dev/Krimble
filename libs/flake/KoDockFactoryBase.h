/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Peter Simonsson <peter.simonsson@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KODOCKFACTORY_H
#define KODOCKFACTORY_H

#include "kritaflake_export.h"

#include <QString>

class QDockWidget;

class KRITAFLAKE_EXPORT KoDockFactoryBase
{
public:
    enum DockPosition {
        DockTornOff,
        DockTop,
        DockBottom,
        DockRight,
        DockLeft,
        DockMinimized
    };

    KoDockFactoryBase();
    virtual ~KoDockFactoryBase();

    virtual QString id() const = 0;

    virtual DockPosition defaultDockPosition() const = 0;

    virtual QDockWidget* createDockWidget() = 0;
};

#endif
