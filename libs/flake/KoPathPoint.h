/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006-2008 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPATHPOINT_H
#define KOPATHPOINT_H

#include "kritaflake_export.h"

#include <QFlags>

class KoPathShape;
class QPointF;
class QTransform;
class QRectF;
class QPainter;
class KisHandlePainterHelper;

class KRITAFLAKE_EXPORT KoPathPoint
{
public:
    enum PointProperty {
        Normal = 0,
        StartSubpath = 1,
        StopSubpath = 2,
        CloseSubpath = 8,
        IsSmooth = 16,
        IsSymmetric = 32
    };
    Q_DECLARE_FLAGS(PointProperties, PointProperty)

    enum PointType {
        None = 0,
        Node = 1,
        ControlPoint1 = 2,
        ControlPoint2 = 4,
        All = 7
    };
    Q_DECLARE_FLAGS(PointTypes, PointType)

    KoPathPoint();

    KoPathPoint(KoPathShape *path, const QPointF &point, PointProperties properties = Normal);

    KoPathPoint(const KoPathPoint &pathPoint);
    KoPathPoint(const KoPathPoint &pathPoint, KoPathShape *newParent);

    KoPathPoint& operator=(const KoPathPoint &other);

    bool operator == (const KoPathPoint &other) const;

    ~KoPathPoint();

    QPointF point() const;

    QPointF controlPoint1() const;

    QPointF controlPoint2() const;

    void setPoint(const QPointF &point);

    void setControlPoint1(const QPointF &point);

    void setControlPoint2(const QPointF &point);

    void removeControlPoint1();

    void removeControlPoint2();

    PointProperties properties() const;

    void setProperties(PointProperties properties);

    void setProperty(PointProperty property);

    void unsetProperty(PointProperty property);

    bool activeControlPoint1() const;

    bool activeControlPoint2() const;

    void map(const QTransform &matrix);

    void paint(KisHandlePainterHelper &handlesHelper, PointTypes types, bool active = true);

    void setParent(KoPathShape* parent);

    KoPathShape *parent() const;

    QRectF boundingRect(bool active = true) const;

    void reverse();

    bool isSmooth(KoPathPoint *previous, KoPathPoint *next) const;

protected:
    friend class KoPathShapePrivate;
private:
    class Private;
    Private * const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KoPathPoint::PointProperties)
Q_DECLARE_OPERATORS_FOR_FLAGS(KoPathPoint::PointTypes)

#endif
