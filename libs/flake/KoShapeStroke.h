/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006-2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2007, 2009 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPESTROKE_H
#define KOSHAPESTROKE_H

#include "KoFlakeTypes.h"
#include "KoShapeStrokeModel.h"

#include "kritaflake_export.h"

#include <QMetaType>
#include <QColor>

class KoShape;
class QPainter;
class QBrush;
class QPen;
struct KoInsets;

class KRITAFLAKE_EXPORT KoShapeStroke : public KoShapeStrokeModel
{
public:
    KoShapeStroke();

    KoShapeStroke(const KoShapeStroke &other);

    explicit KoShapeStroke(qreal lineWidth, const QColor &color = Qt::black);
    ~KoShapeStroke() override;

    KoShapeStroke& operator = (const KoShapeStroke &rhs);

    void setCapStyle(Qt::PenCapStyle style);
    Qt::PenCapStyle capStyle() const;
    void setJoinStyle(Qt::PenJoinStyle style);
    Qt::PenJoinStyle joinStyle() const;
    void setLineWidth(qreal lineWidth);
    qreal lineWidth() const;
    void setMiterLimit(qreal miterLimit);
    qreal miterLimit() const;
    void setLineStyle(Qt::PenStyle style, const QVector<qreal> &dashes);
    Qt::PenStyle lineStyle() const;
    QVector<qreal> lineDashes() const;
    void setDashOffset(qreal dashOffset);
    qreal dashOffset() const;

    QColor color() const;
    void setColor(const QColor &color);

    void setLineBrush(const QBrush & brush);
    QBrush lineBrush() const;

    void strokeInsets(const KoShape *shape, KoInsets &insets) const override;
    qreal strokeMaxMarkersInset(const KoShape *shape) const override;
    bool hasTransparency() const override;
    void paint(const KoShape *shape, QPainter &painter) const override;
    void paintMarkers(const KoShape *shape, QPainter &painter) const override;

    QPen resultLinePen() const;

    bool compareFillTo(const KoShapeStrokeModel *other) override;
    bool compareStyleTo(const KoShapeStrokeModel *other) override;
    bool isVisible() const override;

private:
    class Private;
    Private * const d;
};

Q_DECLARE_METATYPE( KoShapeStroke )

#endif
