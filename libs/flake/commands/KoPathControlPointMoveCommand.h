/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHCONTROLPOINTMOVECOMMAND_H
#define KOPATHCONTROLPOINTMOVECOMMAND_H

#include <kundo2command.h>
#include <QPointF>
#include "KoPathPointData.h"
#include "KoPathPoint.h"
#include "kritaflake_export.h"

class KRITAFLAKE_EXPORT KoPathControlPointMoveCommand : public KUndo2Command
{
public:
    KoPathControlPointMoveCommand(const KoPathPointData &pointData, const QPointF &offset,
            KoPathPoint::PointType pointType, KUndo2Command *parent = 0);
    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;

private:
    KoPathPointData m_pointData;
    QPointF m_offset;
    KoPathPoint::PointType m_pointType;
};

#endif
