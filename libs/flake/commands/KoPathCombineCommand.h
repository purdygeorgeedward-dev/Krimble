/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHCOMBINECOMMAND_H
#define KOPATHCOMBINECOMMAND_H

#include <kundo2command.h>
#include <QList>
#include "kritaflake_export.h"

class KoShapeControllerBase;
class KoPathShape;
class KoPathPointData;

class KRITAFLAKE_EXPORT KoPathCombineCommand : public KUndo2Command
{
public:
    KoPathCombineCommand(KoShapeControllerBase *controller, const QList<KoPathShape*> &paths, KUndo2Command *parent = 0);
    ~KoPathCombineCommand() override;
    void redo() override;
    void undo() override;

    KoPathShape *combinedPath() const;
    KoPathPointData originalToCombined(KoPathPointData pd) const;

private:
    class Private;
    Private * const d;
};

#endif
