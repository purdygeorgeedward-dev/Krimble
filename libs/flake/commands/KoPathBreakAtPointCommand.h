/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHBREAKATPOINTCOMMAND_H
#define KOPATHBREAKATPOINTCOMMAND_H

#include <kundo2command.h>
#include <QList>
#include "KoPathPointData.h"

class KoPathPoint;

class KoPathBreakAtPointCommand : public KUndo2Command
{
public:
    explicit KoPathBreakAtPointCommand(const QList<KoPathPointData> &pointDataList, KUndo2Command *parent = 0);
    ~KoPathBreakAtPointCommand() override;

    void redo() override;
    void undo() override;

private:
    QList<KoPathPointData> m_pointDataList;
    QList<KoPathPoint*> m_points;
    QList<KoPathPointIndex> m_closedIndex;
    bool m_deletePoints;
};

#endif
