/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPECONNECTIONCHANGECOMMAND_H
#define KOSHAPECONNECTIONCHANGECOMMAND_H

#include "kritaflake_export.h"
#include <kundo2command.h>
#include "KoConnectionShape.h"

class KRITAFLAKE_EXPORT KoShapeConnectionChangeCommand : public KUndo2Command
{
public:
    KoShapeConnectionChangeCommand(KoConnectionShape *connection, KoConnectionShape::HandleId connectionHandle,
                                   KoShape *oldConnectedShape, int oldConnectionPointId,
                                   KoShape *newConnectedShape, int newConnectionPointId, KUndo2Command *parent = 0);

    ~KoShapeConnectionChangeCommand() override;

    void redo() override;
    void undo() override;

private:
    class Private;
    Private * const d;
};

#endif
