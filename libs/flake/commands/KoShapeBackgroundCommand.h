/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006, 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEBACKGROUNDCOMMAND_H
#define KOSHAPEBACKGROUNDCOMMAND_H

#include "kritaflake_export.h"

#include <kundo2command.h>
#include <QList>
#include <QSharedPointer>

class KoShape;
class KoShapeBackground;

class KRITAFLAKE_EXPORT KoShapeBackgroundCommand : public KUndo2Command
{
public:
    KoShapeBackgroundCommand(const QList<KoShape*> &shapes, QSharedPointer<KoShapeBackground> fill, KUndo2Command *parent = 0);

    KoShapeBackgroundCommand(KoShape *shape, QSharedPointer<KoShapeBackground> fill, KUndo2Command *parent = 0);

    KoShapeBackgroundCommand(const QList<KoShape*> &shapes, const QList<QSharedPointer<KoShapeBackground> > &fills, KUndo2Command *parent = 0);

    ~KoShapeBackgroundCommand() override;
    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;

private:
    class Private;
    Private * const d;
};

#endif
