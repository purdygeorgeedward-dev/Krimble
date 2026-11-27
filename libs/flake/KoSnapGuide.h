/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSNAPGUIDE_H
#define KOSNAPGUIDE_H

#include "kritaflake_export.h"

#include <QScopedPointer>
#include <QList>
#include <Qt>

class KoSnapStrategy;
class KoShape;
class KoPathPoint;
class KoViewConverter;
class KoCanvasBase;
class QPainter;
class QPointF;
class QRectF;
class KoColorDisplayRendererInterface;

class KRITAFLAKE_EXPORT KoSnapGuide
{
public:
    enum Strategy
    {
        OrthogonalSnapping = 1,
        NodeSnapping = 2,
        ExtensionSnapping = 4,
        IntersectionSnapping = 8,
        GridSnapping = 0x10,
        BoundingBoxSnapping = 0x20,
        GuideLineSnapping = 0x40,
        DocumentBoundsSnapping = 0x80,
        DocumentCenterSnapping = 0x100,
        CustomSnapping = 0x200,
        PixelSnapping = 0x400
    };
    Q_DECLARE_FLAGS(Strategies, Strategy)

    explicit KoSnapGuide(KoCanvasBase *canvas);

    virtual ~KoSnapGuide();

    QPointF snap(const QPointF &mousePosition, Qt::KeyboardModifiers modifiers);

    QPointF snap(const QPointF &mousePosition, const QPointF &dragOffset, Qt::KeyboardModifiers modifiers);

    void paint(QPainter &painter, const KoViewConverter &converter, const KoColorDisplayRendererInterface *displayRenderer);

    QRectF boundingRect();

    void setAdditionalEditedShape(KoShape *shape);

    KoShape *additionalEditedShape() const;

    void enableSnapStrategy(Strategy type, bool value);
    bool isStrategyEnabled(Strategy type) const;

    void enableSnapStrategies(Strategies strategies);

    KoSnapGuide::Strategies enabledSnapStrategies() const;

    bool addCustomSnapStrategy(KoSnapStrategy *customStrategy);

    void overrideSnapStrategy(Strategy type, KoSnapStrategy *strategy);

    void enableSnapping(bool on);

    bool isSnapping() const;

    void setSnapDistance(int distance);

    int snapDistance() const;

    KoCanvasBase *canvas() const;

    void setIgnoredPathPoints(const QList<KoPathPoint*> &ignoredPoints);

    QList<KoPathPoint*> ignoredPathPoints() const;

    void setIgnoredShapes(const QList<KoShape*> &ignoredShapes);

    QList<KoShape*> ignoredShapes() const;

    void reset();

private:
    class Private;
    const QScopedPointer<Private> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KoSnapGuide::Strategies)

#endif
