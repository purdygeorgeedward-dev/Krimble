/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOLOADINGSHAPEUPDATER_H
#define KOLOADINGSHAPEUPDATER_H

#include "kritaflake_export.h"

class KoShape;

class KRITAFLAKE_EXPORT KoLoadingShapeUpdater
{
public:
    KoLoadingShapeUpdater();
    virtual ~KoLoadingShapeUpdater();

    virtual void update(KoShape *shape) = 0;
};

#endif
