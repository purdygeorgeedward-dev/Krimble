/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2009 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHSEGMENTTYPECOMMAND_H
#define KOPATHSEGMENTTYPECOMMAND_H

#include <kundo2command.h>
#include <QList>
#include <QPointF>
#include "KoPathPoint.h"
#include "KoPathPointData.h"
#include "kritaflake_export.h"

class KRITAFLAKE_EXPORT KoPathSegmentTypeCommand : public KUndo2Command
{
public:
    enum SegmentType {
        Curve = 1,
        Line = 2
    };

    KoPathSegmentTypeCommand(const KoPathPointData &pointData, SegmentType segmentType, KUndo2Command *parent = 0);

    KoPathSegmentTypeCommand(const QList<KoPathPointData> &pointDataList, SegmentType segmentType, KUndo2Command *parent = 0);
    ~KoPathSegmentTypeCommand() override;

    void redo() override;
    void undo() override;

private:
    struct SegmentTypeData {
        QPointF m_controlPoint1;
        QPointF m_controlPoint2;
        KoPathPoint::PointProperties m_properties1;
        KoPathPoint::PointProperties m_properties2;
    };

    void initialize(const QList<KoPathPointData> &pointDataList);

    QList<KoPathPointData> m_pointDataList;
    QList<SegmentTypeData> m_segmentData;
    SegmentType m_segmentType;
};

#endif
