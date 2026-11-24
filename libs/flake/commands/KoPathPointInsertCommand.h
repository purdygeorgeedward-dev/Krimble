/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHPOINTINSERTCOMMAND_H
#define KOPATHPOINTINSERTCOMMAND_H

#include <kundo2command.h>
#include <QList>
#include "KoPathPointData.h"
#include "kritaflake_export.h"

class KoPathPointInsertCommandPrivate;
class KoPathPoint;

class KRITAFLAKE_EXPORT KoPathPointInsertCommand : public KUndo2Command
{
public:
    KoPathPointInsertCommand(const QList<KoPathPointData> &pointDataList, qreal insertPosition, KUndo2Command *parent = 0);
    ~KoPathPointInsertCommand() override;

    void redo() override;
    void undo() override;

    QList<KoPathPoint*> insertedPoints() const;

private:
    KoPathPointInsertCommandPrivate * const d;
};

#endif
