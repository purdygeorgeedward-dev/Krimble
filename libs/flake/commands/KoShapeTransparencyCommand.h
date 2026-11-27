/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPETRANSPARENCYCOMMAND_H
#define KOSHAPETRANSPARENCYCOMMAND_H

#include "kritaflake_export.h"

#include <kundo2command.h>
#include <QList>

class KoShape;

class KRITAFLAKE_EXPORT KoShapeTransparencyCommand : public KUndo2Command
{
public:
    KoShapeTransparencyCommand(const QList<KoShape*> &shapes, qreal transparency, KUndo2Command *parent = 0);

    KoShapeTransparencyCommand(KoShape *shape, qreal transparency, KUndo2Command *parent = 0);

    KoShapeTransparencyCommand(const QList<KoShape*> &shapes, const QList<qreal> &transparencies, KUndo2Command *parent = 0);

    ~KoShapeTransparencyCommand() override;
    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;

private:
    class Private;
    Private * const d;
};

#endif
