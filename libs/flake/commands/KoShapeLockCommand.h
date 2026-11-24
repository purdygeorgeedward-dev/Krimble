/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPELOCKCOMMAND_H
#define KOSHAPELOCKCOMMAND_H

#include <kundo2command.h>
#include <QList>

class KoShape;

class KoShapeLockCommand : public KUndo2Command
{
public:
    KoShapeLockCommand(const QList<KoShape*> &shapes, const QList<bool> &oldLock, const QList<bool> &newLock,
                       KUndo2Command *parent = 0);
    ~KoShapeLockCommand() override;

    void redo() override;
    void undo() override;

private:
    QList<KoShape*> m_shapes;
    QList<bool> m_oldLock;
    QList<bool> m_newLock;
};

#endif
