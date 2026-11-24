/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHPOINTTYPECOMMAND_H
#define KOPATHPOINTTYPECOMMAND_H

#include <kundo2command.h>
#include <QList>
#include "KoPathBaseCommand.h"
#include "KoPathPoint.h"
#include "KoPathPointData.h"
#include "kritaflake_export.h"

class KRITAFLAKE_EXPORT KoPathPointTypeCommand : public KoPathBaseCommand
{
public:
    enum PointType {
        Corner,
        Smooth,
        Symmetric,
        Line,
        Curve
    };
    KoPathPointTypeCommand(const QList<KoPathPointData> &pointDataList, PointType pointType, KUndo2Command *parent = 0);
    ~KoPathPointTypeCommand() override;

    void redo() override;
    void undo() override;

    static void makeCubicPointSmooth(KoPathPoint *point);

private:
    struct PointData {
        PointData(const KoPathPointData pointData)
                : m_pointData(pointData) {}
        KoPathPointData m_pointData;
        QPointF m_oldControlPoint1;
        QPointF m_oldControlPoint2;
        KoPathPoint::PointProperties m_oldProperties;
        bool m_hadControlPoint1 {false};
        bool m_hadControlPoint2 {false};
    };

    bool appendPointData(KoPathPointData data);
    void undoChanges(const QList<PointData> &data);

    PointType m_pointType;
    QList<PointData> m_oldPointData;
    QList<PointData> m_additionalPointData;
};

#endif
