/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHPOINTREMOVECOMMAND_H
#define KOPATHPOINTREMOVECOMMAND_H

#include <kundo2command.h>
#include <QList>
#include "KoPathPointData.h"
#include "kritaflake_export.h"

class KoShapeController;
class KoPathPointRemoveCommandPrivate;

class KRITAFLAKE_EXPORT KoPathPointRemoveCommand : public KUndo2Command
{
public:
    static KUndo2Command *createCommand(const QList<KoPathPointData> &pointDataList, KoShapeController *shapeController, KUndo2Command *parent = 0);

    explicit KoPathPointRemoveCommand(const QList<KoPathPointData> &pointDataList, KUndo2Command *parent = 0);
    ~KoPathPointRemoveCommand() override;

    void redo() override;
    void undo() override;

private:
    KoPathPointRemoveCommandPrivate *d;
};

#endif
