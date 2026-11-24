/* This file is part of the KDE project
 *   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 *   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPARAMETERCHANGESTRATEGYPRIVATE_H
#define KOPARAMETERCHANGESTRATEGYPRIVATE_H

#include "KoInteractionStrategy_p.h"
#include "KoParameterShape.h"

class KoParameterChangeStrategyPrivate : public KoInteractionStrategyPrivate
{
public:
    KoParameterChangeStrategyPrivate(KoToolBase *owner, KoParameterShape *paramShape, int handle)
        : KoInteractionStrategyPrivate(owner)
        , parameterShape(paramShape)
        , handleId(handle)
        , startPoint(paramShape->shapeToDocument(paramShape->handlePosition(handle)))
    {
        releasePoint = startPoint;
    }
    KoParameterShape * const parameterShape;
    const int handleId;
    const QPointF startPoint;
    QPointF releasePoint;
    Qt::KeyboardModifiers lastModifierUsed;
};

#endif
