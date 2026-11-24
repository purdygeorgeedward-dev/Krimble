/*
 *  SPDX-FileCopyrightText: 2025 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOVIEWTRANSFORMSTILLPOINT_H
#define KOVIEWTRANSFORMSTILLPOINT_H

#include <kritaflake_export.h>
#include <QPointF>
#include <utility>

class KisCoordinatesConverter;

class KRITAFLAKE_EXPORT KoViewTransformStillPoint : public std::pair<QPointF, QPointF>
{
public:
    KoViewTransformStillPoint() = default;
    KoViewTransformStillPoint(const QPointF &docPoint, const QPointF &viewPoint);
    KoViewTransformStillPoint(const std::pair<QPointF, QPointF> &rhs);

    KoViewTransformStillPoint(const KoViewTransformStillPoint &rhs) = default;
    KoViewTransformStillPoint(KoViewTransformStillPoint &&rhs) = default;
    KoViewTransformStillPoint& operator=(const KoViewTransformStillPoint &rhs) = default;
    KoViewTransformStillPoint& operator=(KoViewTransformStillPoint &&rhs) = default;

    QPointF docPoint() const;
    QPointF viewPoint() const;
};

KRITAFLAKE_EXPORT QDebug operator<<(QDebug dbg, const KoViewTransformStillPoint &point);

#endif