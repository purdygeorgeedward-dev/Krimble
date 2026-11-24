/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_GREEN_COORDINATES_MATH_H
#define __KIS_GREEN_COORDINATES_MATH_H

#include <QScopedPointer>
#include <QVector>
#include <QPointF>

#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisGreenCoordinatesMath
{
public:
    KisGreenCoordinatesMath();
    ~KisGreenCoordinatesMath();

    void precalculateGreenCoordinates(const QVector<QPointF> &originalCage, const QVector<QPointF> &points);

    void generateTransformedCageNormals(const QVector<QPointF> &transformedCage);

    QPointF transformedPoint(int pointIndex, const QVector<QPointF> &transformedCage);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
