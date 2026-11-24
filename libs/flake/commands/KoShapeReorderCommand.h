/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEREORDERCOMMAND_H
#define KOSHAPEREORDERCOMMAND_H

#include "kritaflake_export.h"

#include <boost/operators.hpp>
#include <kundo2command.h>
#include <QList>

class KoShape;
class KoShapeManager;
class KoShapeReorderCommandPrivate;

class KRITAFLAKE_EXPORT KoShapeReorderCommand : public KUndo2Command
{
public:
    struct KRITAFLAKE_EXPORT IndexedShape : boost::less_than_comparable<IndexedShape> {
        IndexedShape();
        IndexedShape(KoShape *_shape);

        bool operator<(const IndexedShape &rhs) const;

        int zIndex = 0;
        KoShape *shape = 0;
    };

public:
    KoShapeReorderCommand(const QList<KoShape*> &shapes, QList<int> &newIndexes, KUndo2Command *parent = 0);
    KoShapeReorderCommand(const QList<IndexedShape> &shapes, KUndo2Command *parent = 0);
    ~KoShapeReorderCommand() override;

    enum MoveShapeType  {
        RaiseShape,
        LowerShape,
        BringToFront,
        SendToBack
    };

    static KoShapeReorderCommand *createCommand(const QList<KoShape*> &shapes, KoShapeManager *manager,
            MoveShapeType move, KUndo2Command *parent = 0);

    static KoShapeReorderCommand *mergeInShape(QList<KoShape*> shapes, KoShape *newShape,
                                               KUndo2Command *parent = 0);

    static
    QList<KoShapeReorderCommand::IndexedShape>
    homogenizeZIndexes(QList<IndexedShape> shapes);

    static
    QList<KoShapeReorderCommand::IndexedShape>
    homogenizeZIndexesLazy(QList<IndexedShape> shapes);

    static QList<IndexedShape> mergeDownShapes(QList<KoShape*> shapesBelow, QList<KoShape*> shapesAbove);

    void redo() override;
    void undo() override;

private:
    KoShapeReorderCommandPrivate * const d;
};

KRITAFLAKE_EXPORT QDebug operator<<(QDebug dbg, const KoShapeReorderCommand::IndexedShape &indexedShape);

#endif
