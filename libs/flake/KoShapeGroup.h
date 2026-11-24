/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEGROUP_H
#define KOSHAPEGROUP_H

#include "KoShapeContainer.h"

#include "kritaflake_export.h"

class KoShapeSavingContext;
class KoShapeLoadingContext;
class KoShapeGroupPrivate;

class KRITAFLAKE_EXPORT KoShapeGroup : public KoShapeContainer
{
public:
    KoShapeGroup();
    ~KoShapeGroup() override;

    KoShape* cloneShape() const override;

    void paintComponent(QPainter &painter) const override;
    bool hitTest(const QPointF &position) const override;
    QSizeF size() const override;
    void setSize(const QSizeF &size) override;
    QRectF outlineRect() const override;
    QRectF boundingRect() const override;

private:
    friend class ShapeGroupContainerModel;

    void invalidateSizeCache();

private:
    KoShapeGroup(const KoShapeGroup &rhs);

private:
    void tryUpdateCachedSize() const;

    void shapeChanged(ChangeType type, KoShape *shape = 0) override;

    class Private;
    QScopedPointer<Private> d;
};

#endif
