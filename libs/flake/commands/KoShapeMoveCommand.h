/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEMOVECOMMAND_H
#define KOSHAPEMOVECOMMAND_H

#include "kritaflake_export.h"

#include <kundo2command.h>
#include <QList>
#include <QPointF>
#include <KoFlake.h>

class KoShape;

class KRITAFLAKE_EXPORT KoShapeMoveCommand : public KUndo2Command
{
public:
    KoShapeMoveCommand(const QList<KoShape*> &shapes, QList<QPointF> &previousPositions, QList<QPointF> &newPositions,
                       KoFlake::AnchorPosition anchor = KoFlake::Center, KUndo2Command *parent = 0);

    KoShapeMoveCommand(const QList<KoShape*> &shapes, const QPointF &offset, KUndo2Command *parent = 0);

    ~KoShapeMoveCommand() override;
    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;

private:
    class Private;
    Private * const d;
};

#endif
