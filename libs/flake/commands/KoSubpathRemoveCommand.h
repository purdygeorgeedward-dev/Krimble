/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSUBPATHREMOVECOMMAND_H
#define KOSUBPATHREMOVECOMMAND_H

#include <kundo2command.h>
#include "KoPathShape.h"
#include "kritaflake_export.h"

class KRITAFLAKE_EXPORT KoSubpathRemoveCommand : public KUndo2Command
{
public:
    KoSubpathRemoveCommand(KoPathShape *pathShape, int subpathIndex, KUndo2Command *parent = 0);
    ~KoSubpathRemoveCommand() override;

    void redo() override;
    void undo() override;

private:
    KoPathShape *m_pathShape;
    int m_subpathIndex;
    KoSubpath *m_subpath;
};

#endif
