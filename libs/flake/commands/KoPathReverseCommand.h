/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHREVERSECOMMAND_H
#define KOPATHREVERSECOMMAND_H

#include <kundo2command.h>
#include <QList>
#include "kritaflake_export.h"

class KoPathShape;

class KRITAFLAKE_EXPORT KoPathReverseCommand : public KUndo2Command
{
public:
    explicit KoPathReverseCommand(const QList<KoPathShape*> &paths, KUndo2Command *parent = 0);
    ~KoPathReverseCommand() override;
    void redo() override;
    void undo() override;
private:
    class Private;
    Private * const d;
};

#endif
