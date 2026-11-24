/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEGROUPCOMMAND_H
#define KOSHAPEGROUPCOMMAND_H

#include "kritaflake_export.h"

#include <QList>
#include <QScopedPointer>
#include <kundo2command.h>

class KoShape;
class KoShapeGroup;
class KoShapeContainer;
class KoShapeGroupCommandPrivate;

class KRITAFLAKE_EXPORT KoShapeGroupCommand : public KUndo2Command
{
public:
    static KoShapeGroupCommand *createCommand(KoShapeContainer *container, const QList<KoShape *> &shapes, bool shouldNormalize = false);

    KoShapeGroupCommand(KoShapeContainer *container, const QList<KoShape *> &shapes, bool shouldNormalize, KUndo2Command *parent = 0);

    KoShapeGroupCommand(KoShapeContainer *container, const QList<KoShape *> &shapes, KUndo2Command *parent = 0);
    ~KoShapeGroupCommand() override;

    void redo() override;

    void undo() override;

protected:
    const QScopedPointer<KoShapeGroupCommandPrivate> d;
};

#endif
