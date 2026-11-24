/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPECLIPCOMMAND_H
#define KOSHAPECLIPCOMMAND_H

#include "kritaflake_export.h"
#include <kundo2command.h>
#include <QList>

class KoShape;
class KoPathShape;
class KoShapeControllerBase;

class KRITAFLAKE_EXPORT KoShapeClipCommand : public KUndo2Command
{
public:
    KoShapeClipCommand(KoShapeControllerBase *controller, const QList<KoShape*> &shapes, const QList<KoPathShape*> &clipPathShapes, KUndo2Command *parent = 0);

    KoShapeClipCommand(KoShapeControllerBase *controller, KoShape *shape, const QList<KoPathShape*> &clipPathShapes, KUndo2Command *parent = 0);

    ~KoShapeClipCommand() override;

    void redo() override;
    void undo() override;

private:
    class Private;
    Private * const d;
};

#endif
