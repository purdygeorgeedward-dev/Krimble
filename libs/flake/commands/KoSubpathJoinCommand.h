/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSUBPATHJOINCOMMAND_H
#define KOSUBPATHJOINCOMMAND_H

#include <kundo2command.h>
#include <QPointF>
#include "KoPathPoint.h"
#include "KoPathPointData.h"
#include <boost/optional.hpp>

class KoSubpathJoinCommand : public KUndo2Command
{
public:
    KoSubpathJoinCommand(const KoPathPointData &pointData1, const KoPathPointData &pointData2, KUndo2Command *parent = 0);
    ~KoSubpathJoinCommand() override;

    void redo() override;
    void undo() override;

private:
    bool closeSubpathMode() const;

private:
    KoPathPointData m_pointData1;
    KoPathPointData m_pointData2;
    KoPathPointIndex m_splitIndex;

    boost::optional<QPointF> m_savedControlPoint1;
    boost::optional<QPointF> m_savedControlPoint2;

    KoPathPoint::PointProperties m_oldProperties1;
    KoPathPoint::PointProperties m_oldProperties2;
    enum Reverse {
        ReverseFirst = 1,
        ReverseSecond = 2
    };
    int m_reverse;
};

#endif
