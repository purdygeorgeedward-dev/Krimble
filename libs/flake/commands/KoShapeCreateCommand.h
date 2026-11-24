/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPECREATECOMMAND_H
#define KOSHAPECREATECOMMAND_H

#include "kritaflake_export.h"
#include <kundo2command.h>

class KoShape;
class KoShapeContainer;
class KoShapeControllerBase;

class KRITAFLAKE_EXPORT KoShapeCreateCommand : public KUndo2Command
{
public:
    KoShapeCreateCommand(KoShapeControllerBase *controller, KoShape *shape,
                         KoShapeContainer *parentShape = 0,
                         KUndo2Command *parent = 0);

    KoShapeCreateCommand(KoShapeControllerBase *controller, const QList<KoShape*> shape,
                         KoShapeContainer *parentShape = 0,
                         KUndo2Command *parent = 0);

    ~KoShapeCreateCommand() override;
    void redo() override;
    void undo() override;

protected:
    KoShapeCreateCommand(KoShapeControllerBase *controller, const QList<KoShape *> shapes,
                         KoShapeContainer *parentShape, KUndo2Command *parent,
                         const KUndo2MagicString &undoString);

private:
    class Private;
    Private * const d;
};

#endif
