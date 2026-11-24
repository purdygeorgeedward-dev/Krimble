/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEBACKGROUND_H
#define KOSHAPEBACKGROUND_H

#include "kritaflake_export.h"

#include <QtGlobal>

class QSizeF;
class QPainter;
class QPainterPath;
class KoShapeSavingContext;

class KRITAFLAKE_EXPORT KoShapeBackground
{
public:
    KoShapeBackground();
    virtual ~KoShapeBackground();

    virtual void paint(QPainter &painter, const QPainterPath &fillPath) const = 0;

    virtual bool hasTransparency() const;

    virtual bool compareTo(const KoShapeBackground *other) const = 0;

    virtual explicit operator bool() const { return true; }

};

#endif
