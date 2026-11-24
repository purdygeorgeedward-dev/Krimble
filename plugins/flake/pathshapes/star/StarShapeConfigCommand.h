/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STARSHAPECONFIGCOMMAND_H
#define STARSHAPECONFIGCOMMAND_H

#include <kundo2command.h>

class StarShape;

class StarShapeConfigCommand : public KUndo2Command
{
public:
    StarShapeConfigCommand(StarShape *star, uint cornerCount, qreal innerRadius, qreal outerRadius, bool convex, KUndo2Command *parent = 0);
    void redo() override;
    void undo() override;
private:
    StarShape *m_star;
    uint m_oldCornerCount;
    qreal m_oldInnerRadius;
    qreal m_oldOuterRadius;
    bool m_oldConvex;
    uint m_newCornerCount;
    qreal m_newInnerRadius;
    qreal m_newOuterRadius;
    bool m_newConvex;
};

#endif

