/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPARAMETERHANDLEMOVECOMMAND_H
#define KOPARAMETERHANDLEMOVECOMMAND_H

#include <kundo2command.h>
#include <QPointF>
#include "kritaflake_export.h"

class KoParameterShape;

class KoParameterHandleMoveCommand : public KUndo2Command
{
public:
    KoParameterHandleMoveCommand(KoParameterShape *shape, int handleId, const QPointF &startPoint, const QPointF &endPoint, Qt::KeyboardModifiers keyModifiers, KUndo2Command *parent = 0);
    ~KoParameterHandleMoveCommand() override;

    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;

private:
    KoParameterShape *m_shape;
    int m_handleId;
    QPointF m_startPoint;
    QPointF m_endPoint;
    Qt::KeyboardModifiers m_keyModifiers;
};

#endif

