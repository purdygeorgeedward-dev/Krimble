/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007, 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPECONTAINERMODEL_H
#define KOSHAPECONTAINERMODEL_H

#include "kritaflake_export.h"

#include <KoShape.h>

#include <QList>
#include <QPointF>

class KoShapeContainer;

class KRITAFLAKE_EXPORT KoShapeContainerModel
{
public:
    KoShapeContainerModel();

    virtual ~KoShapeContainerModel();

    void deleteOwnedShapes();

    virtual void add(KoShape *shape) = 0;

    virtual void remove(KoShape *shape) = 0;

    virtual void setClipped(const KoShape *shape, bool clipping) = 0;

    virtual bool isClipped(const KoShape *shape) const = 0;

    virtual void setInheritsTransform(const KoShape *shape, bool inherit) = 0;

    virtual bool inheritsTransform(const KoShape *shape) const = 0;

    virtual int count() const = 0;

    virtual QList<KoShape*> shapes() const = 0;

    virtual void containerChanged(KoShapeContainer *container, KoShape::ChangeType type) = 0;

    virtual void proposeMove(KoShape *shape, QPointF &move);

    virtual void childChanged(KoShape *shape, KoShape::ChangeType type);

    virtual void shapeHasBeenAddedToHierarchy(KoShape *shape, KoShapeContainer *addedToSubtree);
    virtual void shapeToBeRemovedFromHierarchy(KoShape *shape, KoShapeContainer *removedFromSubtree);

protected:
    KoShapeContainerModel(const KoShapeContainerModel &rhs);
};

#endif
