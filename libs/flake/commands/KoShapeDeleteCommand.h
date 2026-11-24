/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEDELETECOMMAND_H
#define KOSHAPEDELETECOMMAND_H

#include "kritaflake_export.h"
#include <kundo2command.h>
#include <QList>

class KoShape;
class KoShapeControllerBase;

class KRITAFLAKE_EXPORT KoShapeDeleteCommand : public KUndo2Command
{
public:
    KoShapeDeleteCommand(KoShapeControllerBase *controller, KoShape *shape, KUndo2Command *parent = 0);
    KoShapeDeleteCommand(KoShapeControllerBase *controller, const QList<KoShape*> &shapes, KUndo2Command *parent = 0);
    ~KoShapeDeleteCommand() override;
    void redo() override;
    void undo() override;
private:
    class Private;
    Private * const d;
};

#endif
