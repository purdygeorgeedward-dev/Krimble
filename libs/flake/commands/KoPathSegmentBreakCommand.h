/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHSEGMENTBREAKCOMMAND_H
#define KOPATHSEGMENTBREAKCOMMAND_H

#include <kundo2command.h>
#include "KoPathPointData.h"

class KoPathSegmentBreakCommand : public KUndo2Command
{
public:
    explicit KoPathSegmentBreakCommand(const KoPathPointData &pointData, KUndo2Command *parent = 0);
    ~KoPathSegmentBreakCommand() override;

    void redo() override;
    void undo() override;
private:
    KoPathPointData m_pointData;
    KoPathPointIndex m_startIndex;
    bool m_broken;
};

#endif
