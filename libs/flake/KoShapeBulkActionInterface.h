/* This file is part of the KDE project
   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSHAPEBULKACTIONINTERFACE_H
#define KOSHAPEBULKACTIONINTERFACE_H

#include <QRect>

struct KoShapeBulkActionInterface
{
    virtual ~KoShapeBulkActionInterface() = default;

    virtual void startBulkAction() = 0;

    virtual QRectF endBulkAction() = 0;
};

#endif
