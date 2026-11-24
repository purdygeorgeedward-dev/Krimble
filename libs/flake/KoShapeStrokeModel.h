/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007, 2009 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPESTROKEMODEL_H
#define KOSHAPESTROKEMODEL_H

#include "kritaflake_export.h"

#include <QtGlobal>

class KoShape;
class KoShapeSavingContext;
struct KoInsets;

class QColor;
class QPainter;

class KRITAFLAKE_EXPORT KoShapeStrokeModel
{
public:
    virtual ~KoShapeStrokeModel();

    virtual void strokeInsets(const KoShape *shape, KoInsets &insets) const = 0;

    virtual qreal strokeMaxMarkersInset(const KoShape *shape) const = 0;

    virtual bool hasTransparency() const = 0;
    virtual void paint(const KoShape *shape, QPainter &painter) const = 0;

    virtual void paintMarkers(const KoShape *shape, QPainter &painter) const = 0;

    virtual bool compareFillTo(const KoShapeStrokeModel *other) = 0;
    virtual bool compareStyleTo(const KoShapeStrokeModel *other) = 0;
    virtual bool isVisible() const = 0;
};

#endif
