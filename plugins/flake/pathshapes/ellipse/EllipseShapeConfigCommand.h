/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ELLIPSESHAPECONFIGCOMMAND_H
#define ELLIPSESHAPECONFIGCOMMAND_H

#include "EllipseShape.h"
#include <kundo2command.h>

class EllipseShapeConfigCommand : public KUndo2Command
{
public:
    EllipseShapeConfigCommand(EllipseShape *ellipse, EllipseShape::EllipseType type, qreal startAngle, qreal startEndAngle, KUndo2Command *parent = 0);
    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;

private:
    EllipseShape *m_ellipse;
    EllipseShape::EllipseType m_oldType;
    qreal m_oldStartAngle;
    qreal m_oldEndAngle;
    EllipseShape::EllipseType m_newType;
    qreal m_newStartAngle;
    qreal m_newEndAngle;
};

#endif

