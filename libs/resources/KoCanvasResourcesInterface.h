/*
 *  SPDX-FileCopyrightText: 2020 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOCANVASRESOURCESINTERFACE_H
#define KOCANVASRESOURCESINTERFACE_H

#include "kritaresources_export.h"
#include <QSharedPointer>

class QVariant;

#include <kritaresources_export.h>

class KRITARESOURCES_EXPORT KoCanvasResourcesInterface
{
public:
    virtual ~KoCanvasResourcesInterface();

    virtual QVariant resource(int key) const = 0;
};

using KoCanvasResourcesInterfaceSP = QSharedPointer<KoCanvasResourcesInterface>;

#endif
