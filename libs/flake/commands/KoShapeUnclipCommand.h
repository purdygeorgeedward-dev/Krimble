/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEUNCLIPCOMMAND_H
#define KOSHAPEUNCLIPCOMMAND_H

#include "kritaflake_export.h"
#include <kundo2command.h>
#include <QList>

class KoShape;
class KoShapeControllerBase;

class KRITAFLAKE_EXPORT KoShapeUnclipCommand : public KUndo2Command
{
public:
    KoShapeUnclipCommand(KoShapeControllerBase *controller, const QList<KoShape*> &shapes, KUndo2Command *parent = 0);

    KoShapeUnclipCommand(KoShapeControllerBase *controller, KoShape *shape, KUndo2Command *parent = 0);

    ~KoShapeUnclipCommand() override;

    void redo() override;
    void undo() override;

private:
    class Private;
    Private * const d;
};

#endif
