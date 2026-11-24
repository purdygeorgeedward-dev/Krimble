/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATTERNBACKGROUND_H
#define KOPATTERNBACKGROUND_H

#include "KoShapeBackground.h"
#include "kritaflake_export.h"

#include <QSharedDataPointer>

class KoPatternBackgroundPrivate;

class QTransform;
class QImage;
class QPointF;
class QRectF;

class KRITAFLAKE_EXPORT KoPatternBackground : public KoShapeBackground
{
public:
    enum PatternRepeat {
        Original,
        Tiled,
        Stretched
    };
    enum ReferencePoint {
        TopLeft,
        Top,
        TopRight,
        Left,
        Center,
        Right,
        BottomLeft,
        Bottom,
        BottomRight
    };

    explicit KoPatternBackground();

    ~KoPatternBackground() override;

    KoPatternBackground(const KoPatternBackground &);
    KoPatternBackground& operator=(const KoPatternBackground &);

    bool compareTo(const KoShapeBackground *other) const override;

    void setTransform(const QTransform &matrix);

    QTransform transform() const;

    void setPattern(const QImage &pattern);

    QImage pattern() const;

    void setRepeat(PatternRepeat repeat);

    PatternRepeat repeat() const;

    ReferencePoint referencePoint() const;

    void setReferencePoint(ReferencePoint referencePoint);

    QPointF referencePointOffset() const;

    void setReferencePointOffset(const QPointF &offset);

    QPointF tileRepeatOffset() const;

    void setTileRepeatOffset(const QPointF &offset);

    QSizeF patternDisplaySize() const;

    void setPatternDisplaySize(const QSizeF &size);

    QSizeF patternOriginalSize() const;

    void paint(QPainter &painter, const QPainterPath &fillPath) const override;

    QRectF patternRectFromFillSize(const QSizeF &size);
private:
    class Private;
    QSharedDataPointer<Private> d;
};

#endif
