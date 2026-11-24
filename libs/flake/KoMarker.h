/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOMARKER_H
#define KOMARKER_H

#include <QMetaType>
#include <QSharedData>

#include "kritaflake_export.h"
#include <KoFlake.h>

#include <QDomDocument>

class KoShapeLoadingContext;
class KoShapeSavingContext;
class QString;
class QPainterPath;
class KoShape;
class QPainter;
class KoShapeStroke;

class  KRITAFLAKE_EXPORT KoMarker : public QSharedData
{
public:
    KoMarker();
    ~KoMarker();

    QString name() const;

    KoMarker(const KoMarker &rhs);
    bool operator==(const KoMarker &other) const;

    enum MarkerCoordinateSystem {
        StrokeWidth,
        UserSpaceOnUse
    };

    void setCoordinateSystem(MarkerCoordinateSystem value);
    MarkerCoordinateSystem coordinateSystem() const;

    static MarkerCoordinateSystem coordinateSystemFromString(const QString &value);
    static QString coordinateSystemToString(MarkerCoordinateSystem value);

    void setReferencePoint(const QPointF &value);
    QPointF referencePoint() const;

    void setReferenceSize(const QSizeF &size);
    QSizeF referenceSize() const;

    bool hasAutoOrientation() const;
    void setAutoOrientation(bool value);

    qreal explicitOrientation() const;

    void setExplicitOrientation(qreal value);

    void setShapes(const QList<KoShape*> &shapes);
    QList<KoShape*> shapes() const;

    void paintAtPosition(QPainter *painter, const QPointF &pos, qreal strokeWidth, qreal nodeAngle);

    qreal maxInset(qreal strokeWidth) const;

    QRectF boundingRect(qreal strokeWidth, qreal nodeAngle) const;

    QPainterPath outline(qreal strokeWidth, qreal nodeAngle) const;

    void drawPreview(QPainter *painter, const QRectF &previewRect,
                     const QPen &pen, KoFlake::MarkerPosition position);

    void applyShapeStroke(const KoShape *shape, KoShapeStroke *stroke, const QPointF &pos, qreal strokeWidth, qreal nodeAngle);

private:
    class Private;
    Private * const d;
};

Q_DECLARE_METATYPE(KoMarker*)

#endif
