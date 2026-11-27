/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOSNAPPROXY_H
#define KOSNAPPROXY_H

class KoSnapGuide;
class KoShape;
class KoPathSegment;
class KoCanvasBase;
#include <QList>
#include "kritaflake_export.h"

class QPointF;
class QRectF;
class KRITAFLAKE_EXPORT KoSnapProxy
{
public:
    KoSnapProxy(KoSnapGuide *snapGuide);

    QList<QPointF> pointsInRect(const QRectF &rect, bool omitEditedShape);

    QList<KoShape*> shapesInRect(const QRectF &rect, bool omitEditedShape = false);

    QList<QPointF> pointsFromShape(KoShape *shape);

    QList<KoPathSegment> segmentsInRect(const QRectF &rect, bool omitEditedShape);

    QList<KoShape*> shapes(bool omitEditedShape = false);

    KoCanvasBase *canvas();

private:
    KoSnapGuide *m_snapGuide;
};

#endif
