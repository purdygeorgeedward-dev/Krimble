/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2005, 2007-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGSTYLEPARSER_H
#define SVGSTYLEPARSER_H

#include "kritaflake_export.h"
#include <QMap>
#include <QGradient>

#include <QDomDocument>

typedef QMap<QString, QString> SvgStyles;

class SvgLoadingContext;
class SvgGraphicsContext;
class QColor;
class QGradient;

class KRITAFLAKE_EXPORT SvgStyleParser
{
public:
    explicit SvgStyleParser(SvgLoadingContext &context);
    ~SvgStyleParser();

    void parseStyle(const SvgStyles &styles, const bool inheritByDefault = false);

    void parseFont(const SvgStyles &styles);

    bool parseColor(QColor &, const QString &);

    QPair<qreal, QColor> parseColorStop(const QDomElement&, SvgGraphicsContext* context, qreal& previousOffset);

    void parseColorStops(QGradient *, const QDomElement &, SvgGraphicsContext *context, const QGradientStops &defaultStops);

    SvgStyles collectStyles(const QDomElement &);

    SvgStyles mergeStyles(const SvgStyles &, const SvgStyles &);

    SvgStyles mergeStyles(const QDomElement &, const QDomElement &);

    SvgStyles parseOneCssStyle(const QString &style, const QStringList &interestingAttributes);
private:

    void parsePA(SvgGraphicsContext *, const QString &, const QString &);

    QString inheritedAttribute(const QString &attributeName, const QDomElement &e);

    class Private;
    Private * const d;
};

#endif
