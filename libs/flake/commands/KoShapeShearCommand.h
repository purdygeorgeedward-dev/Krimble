/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPESHEARCOMMAND_H
#define KOSHAPESHEARCOMMAND_H

#include "kritaflake_export.h"

#include <kundo2command.h>
#include <QList>

class KoShape;
class KoShapeShearCommandPrivate;

class KRITAFLAKE_EXPORT KoShapeShearCommand : public KUndo2Command
{
public:
    KoShapeShearCommand(const QList<KoShape*> &shapes, const QList<qreal> &previousShearXs, const QList<qreal> &previousShearYs, const QList<qreal> &newShearXs, const QList<qreal> &newShearYs, KUndo2Command *parent = 0);

    ~KoShapeShearCommand() override;
    void redo() override;
    void undo() override;

private:
    KoShapeShearCommandPrivate * const d;
};

#endif
