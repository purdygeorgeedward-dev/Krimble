/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPESIZECOMMAND_H
#define KOSHAPESIZECOMMAND_H

#include "kritaflake_export.h"

#include <kundo2command.h>
#include <QList>

class KoShape;

class KRITAFLAKE_EXPORT KoShapeSizeCommand : public KUndo2Command
{
public:
    KoShapeSizeCommand(const QList<KoShape*> &shapes, const QList<QSizeF> &previousSizes,
            const QList<QSizeF> &newSizes, KUndo2Command *parent = 0);
    ~KoShapeSizeCommand() override;

    void redo() override;
    void undo() override;

private:
    class Private;
    Private * const d;
};

#endif
