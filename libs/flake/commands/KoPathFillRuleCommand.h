/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHFILLRULECOMMAND_H
#define KOPATHFILLRULECOMMAND_H

#include "kritaflake_export.h"

#include <kundo2command.h>
#include <QList>

class KoPathShape;

class KRITAFLAKE_EXPORT KoPathFillRuleCommand : public KUndo2Command
{
public:
    KoPathFillRuleCommand(const QList<KoPathShape*> &shapes, Qt::FillRule fillRule, KUndo2Command *parent = 0);
    ~KoPathFillRuleCommand() override;
    void redo() override;
    void undo() override;
private:
    class Private;
    Private * const d;
};

#endif
