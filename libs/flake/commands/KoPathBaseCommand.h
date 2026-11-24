/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHBASECOMMAND_H
#define KOPATHBASECOMMAND_H

#include <kundo2command.h>
#include <QSet>

class KoPathShape;

class KoPathBaseCommand : public KUndo2Command
{
public:
    explicit KoPathBaseCommand(KUndo2Command *parent = 0);

    explicit KoPathBaseCommand(KoPathShape *shape, KUndo2Command *parent = 0);

protected:
    void repaint(bool normalizeShapes);

    QSet<KoPathShape*> m_shapes;
};

#endif
