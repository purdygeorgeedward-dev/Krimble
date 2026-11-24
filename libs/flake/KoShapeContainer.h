/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPECONTAINER_H
#define KOSHAPECONTAINER_H

#include "KoShape.h"

#include <QList>

#include "kritaflake_export.h"

class QPainter;
class KoShapeContainerModel;
class KoShapeContainerPrivate;

class KRITAFLAKE_EXPORT KoShapeContainer : public KoShape
{
public:

    explicit KoShapeContainer(KoShapeContainerModel *model = 0);

    ~KoShapeContainer() override;

    void addShape(KoShape *shape);

    void removeShape(KoShape *shape);

    int shapeCount() const;

    void setClipped(const KoShape *child, bool clipping);

    bool isClipped(const KoShape *child) const;

    void setInheritsTransform(const KoShape *shape, bool inherit);

    bool inheritsTransform(const KoShape *shape) const;

    void paint(QPainter &painter) const override;

    virtual void paintComponent(QPainter &painter) const = 0;

    using KoShape::update;
    void update() const override;

    QList<KoShape*> shapes() const;

    KoShapeContainerModel *model() const;

protected:

    void setModel(KoShapeContainerModel *model);
    void setModelInit(KoShapeContainerModel *model);

public:

    struct ShapeInterface {
        ShapeInterface(KoShapeContainer *_q);

        void addShape(KoShape *shape);

        void removeShape(KoShape *shape);

    protected:
        KoShapeContainer *q;
    };

    ShapeInterface* shapeInterface();

protected:
    KoShapeContainer(const KoShapeContainer &rhs);

    virtual void shapeCountChanged() { }

    void shapeChanged(ChangeType type, KoShape *shape = 0) override;

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif
