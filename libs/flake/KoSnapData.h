/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSNAPDATA_H
#define KOSNAPDATA_H

#include <kritaflake_export.h>

#include "KoPathSegment.h"

class KRITAFLAKE_EXPORT KoSnapData
{
public:
    KoSnapData();
    ~KoSnapData();

    QList<QPointF> snapPoints() const;

    void setSnapPoints(const QList<QPointF> &snapPoints);

    QList<KoPathSegment> snapSegments() const;

    void setSnapSegments(const QList<KoPathSegment> &snapSegments);

private:
    QList<QPointF> m_points;
    QList<KoPathSegment> m_segments;
};

#endif
