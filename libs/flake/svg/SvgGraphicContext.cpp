/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2003, 2005 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2007, 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SvgGraphicContext.h"

#include "kis_pointer_utils.h"

SvgGraphicsContext::SvgGraphicsContext()
: stroke(toQShared(new KoShapeStroke()))
, textProperties(KoSvgTextProperties())
{
    stroke->setLineStyle(Qt::NoPen, QVector<qreal>());
    stroke->setLineWidth(1.0);
    stroke->setCapStyle(Qt::FlatCap);
    stroke->setJoinStyle(Qt::MiterJoin);
}

SvgGraphicsContext::SvgGraphicsContext(const SvgGraphicsContext &gc)
    : stroke(toQShared(new KoShapeStroke(*(gc.stroke.data()))))
{
    KoShapeStrokeSP newStroke = stroke;
    *this = gc;
    this->stroke = newStroke;
}

void SvgGraphicsContext::workaroundClearInheritedFillProperties()
{

    strokeType = None;

    stroke = toQShared(new KoShapeStroke());
    stroke->setLineStyle(Qt::NoPen, QVector<qreal>());
    stroke->setLineWidth(1.0);
    stroke->setCapStyle(Qt::FlatCap);
    stroke->setJoinStyle(Qt::MiterJoin);

    fillType = Solid;
    fillRule = Qt::WindingFill;
    fillColor = QColor(Qt::black);

    opacity = 1.0;

    currentColor = Qt::black;
}
