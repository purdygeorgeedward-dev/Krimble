/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2009 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHPOINTMOVECOMMAND_H
#define KOPATHPOINTMOVECOMMAND_H

#include "kritaflake_export.h"

#include <kundo2command.h>

#include "KoPathPointData.h"

class KoPathPointMoveCommandPrivate;
class QPointF;

class KRITAFLAKE_EXPORT KoPathPointMoveCommand : public KUndo2Command
{
public:
    KoPathPointMoveCommand(const QList<KoPathPointData> &pointData, const QPointF &offset, KUndo2Command *parent = 0);

    KoPathPointMoveCommand(const QList<KoPathPointData> &pointData, const QList<QPointF> &offsets, KUndo2Command *parent = 0);

    ~KoPathPointMoveCommand() override;

    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;

private:
    KoPathPointMoveCommandPrivate * const d;
};

#endif
