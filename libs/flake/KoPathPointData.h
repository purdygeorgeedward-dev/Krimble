/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPATHPOINTDATA_H
#define KOPATHPOINTDATA_H

#include "KoPathShape.h"
#include <boost/operators.hpp>

class KoPathPointData : public boost::equality_comparable<KoPathPointData>,
                        public boost::partially_ordered<KoPathPointData>
{
public:
    KoPathPointData(KoPathShape * path, const KoPathPointIndex & pointIndex)
            : pathShape(path)
            , pointIndex(pointIndex) {}

    bool operator<(const KoPathPointData & other) const {
        return pathShape < other.pathShape ||
               (pathShape == other.pathShape &&
                (pointIndex.first < other.pointIndex.first ||
                 (pointIndex.first == other.pointIndex.first &&
                  pointIndex.second < other.pointIndex.second)));

    }
    bool operator==(const KoPathPointData & other) const {
        return pathShape == other.pathShape &&
               pointIndex.first == other.pointIndex.first &&
               pointIndex.second == other.pointIndex.second;
    }
    KoPathShape *pathShape;
    KoPathPointIndex pointIndex;
};

#endif
