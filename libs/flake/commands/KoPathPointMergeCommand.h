/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHPOINTMERGECOMMAND_H
#define KOPATHPOINTMERGECOMMAND_H

#include <kundo2command.h>

#include "kritaflake_export.h"

class KoPathPointData;

class KRITAFLAKE_EXPORT KoPathPointMergeCommand : public KUndo2Command
{
public:
    KoPathPointMergeCommand(const KoPathPointData &pointData1, const KoPathPointData &pointData2, KUndo2Command *parent = 0);
    ~KoPathPointMergeCommand() override;

    void redo() override;
    void undo() override;

    KoPathPointData mergedPointData() const;

private:

    class Private;
    Private * const d;
};

#endif
