/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __KOSHAPELAYER_H__
#define __KOSHAPELAYER_H__

#include "KoShapeContainer.h"
#include "kritaflake_export.h"

class KRITAFLAKE_EXPORT KoShapeLayer : public KoShapeContainer
{
public:
    KoShapeLayer();
    explicit KoShapeLayer(KoShapeContainerModel *model);

    void paintComponent(QPainter &painter) const override;
    bool hitTest(const QPointF &position) const override;
    QRectF boundingRect() const override;
};

#endif

