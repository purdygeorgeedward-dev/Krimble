/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGUTIL_H
#define SVGUTIL_H

#include "kritaflake_export.h"
#include <QRectF>
#include <KoSvgText.h>
#include <KisQStringListFwd.h>

class QString;
class QTransform;
class KoXmlWriter;
class KoSvgTextProperties;

#include <QDomDocument>

class SvgGraphicsContext;

class KRITAFLAKE_EXPORT SvgUtil
{
public:

    static double fromUserSpace(double value);
    static double toUserSpace(double value);

    static double ptToPx(SvgGraphicsContext *gc, double value);

    static QPointF toUserSpace(const QPointF &point);

    static QRectF toUserSpace(const QRectF &rect);

    static QSizeF toUserSpace(const QSizeF &size);

    static QString toPercentage(qreal value);

    static double fromPercentage(QString s, bool *ok=nullptr);

    static QPointF objectToUserSpace(const QPointF &position, const QRectF &objectBound);

    static QSizeF objectToUserSpace(const QSizeF &size, const QRectF &objectBound);

    static QPointF userSpaceToObject(const QPointF &position, const QRectF &objectBound);

    static QSizeF userSpaceToObject(const QSizeF &size, const QRectF &objectBound);

    static QString transformToString(const QTransform &transform);

    static void writeTransformAttributeLazy(const QString &name, const QTransform &transform, KoXmlWriter &shapeWriter);

    static bool parseViewBox(const QDomElement &e, const QRectF &elementBounds, QRectF *_viewRect, QTransform *_viewTransform);

    struct PreserveAspectRatioParser;
    static void parseAspectRatio(const PreserveAspectRatioParser &p, const QRectF &elementBounds, const QRectF &viewRect, QTransform *_viewTransform);

    static qreal parseUnit(SvgGraphicsContext *gc,
                           const KoSvgTextProperties &resolved,
                           QStringView,
                           bool horiz = false,
                           bool vert = false,
                           const QRectF &bbox = QRectF());
    static KoSvgText::CssLengthPercentage parseUnitStruct(SvgGraphicsContext *gc,
                           QStringView unit,
                           bool horiz = false,
                           bool vert = false,
                           const QRectF &bbox = QRectF());

    static KoSvgText::CssLengthPercentage parseTextUnitStruct(SvgGraphicsContext *gc, QStringView unit);

    static KoSvgText::CssLengthPercentage parseUnitStructImpl(SvgGraphicsContext *gc,
                                                          QStringView,
                                                          bool horiz = false,
                                                          bool vert = false,
                                                          const QRectF &bbox = QRectF(),
                                                          bool percentageViewBox = false);

    static qreal parseUnitX(SvgGraphicsContext *gc, const KoSvgTextProperties &resolved, const QString &unit);

    static qreal parseUnitY(SvgGraphicsContext *gc, const KoSvgTextProperties &resolved, const QString &unit);

    static qreal parseUnitXY(SvgGraphicsContext *gc, const KoSvgTextProperties &resolved, const QString &unit);

    static qreal parseUnitAngular(SvgGraphicsContext *gc, const QString &unit);

    static const char * parseNumber(const char *ptr, qreal &number);

    static qreal parseNumber(const QString &string);

    static QString mapExtendedShapeTag(const QString &tagName, const QDomElement &element);

    static QStringList simplifyList(const QString &str);

    struct KRITAFLAKE_EXPORT PreserveAspectRatioParser
    {
        PreserveAspectRatioParser(const QString &str);

        enum Alignment {
            Min,
            Middle,
            Max
        };

        bool defer = false;
        Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio;
        Alignment xAlignment = Min;
        Alignment yAlignment = Min;

        QPointF rectAnchorPoint(const QRectF &rc) const;

        QString toString() const;

    private:
        Alignment alignmentFromString(const QString &str) const;
        QString alignmentToString(Alignment alignment) const;
        static qreal alignedValue(qreal min, qreal max, Alignment alignment);
    };
};

#endif
