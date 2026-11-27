/*
 *  SPDX-FileCopyrightText: Copyright (c) 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOPOLYGONUTILS_H
#define KOPOLYGONUTILS_H

#include <QPolygon>

class KoPolygonUtils
{
public:
    static QPolygon offsetPolygon(const QPolygon &polygon, int offset, bool rounded = true, int circleSegments = 16);

    static QList<QPolygon> offsetPolygons (const QList<QPolygon> polygons, int offset, bool rounded = true, int circleSegments = 16);
};

#endif
