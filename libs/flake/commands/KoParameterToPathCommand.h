/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPARAMETERTOPATHCOMMAND_H
#define KOPARAMETERTOPATHCOMMAND_H

#include <kundo2command.h>
#include <QList>

#include "kritaflake_export.h"

class KoParameterShape;
class KoParameterToPathCommandPrivate;

class KRITAFLAKE_EXPORT KoParameterToPathCommand : public KUndo2Command
{
public:
    explicit KoParameterToPathCommand(KoParameterShape *shape, KUndo2Command *parent = 0);
    explicit KoParameterToPathCommand(const QList<KoParameterShape*> &shapes, KUndo2Command *parent = 0);
    ~KoParameterToPathCommand() override;

    void redo() override;
    void undo() override;
private:
    KoParameterToPathCommandPrivate * const d;
};

#endif
