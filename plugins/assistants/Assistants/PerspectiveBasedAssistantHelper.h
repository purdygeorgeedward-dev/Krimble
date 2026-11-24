/*
 * SPDX-FileCopyrightText: 2022 Agata Cacko <cacko.azh@gmail.com>
 */

#ifndef _PERSPECTIVE_BASED_ASSISTANT_HELPER_H_
#define _PERSPECTIVE_BASED_ASSISTANT_HELPER_H_

#include <QObject>
#include <boost/optional.hpp>

#include "Ellipse.h"
#include "kis_abstract_perspective_grid.h"
#include "kis_painting_assistant.h"

#include "kritaassistanttool_export.h"

class KRITAASSISTANTTOOL_EXPORT PerspectiveBasedAssistantHelper
{
private:
    PerspectiveBasedAssistantHelper();
    ~PerspectiveBasedAssistantHelper();

public:

    class CacheData
    {
    public:
        boost::optional<QPointF> vanishingPoint1 {boost::none};
        boost::optional<QPointF> vanishingPoint2 {boost::none};

        QVector<qreal> distancesFromPoints;
        qreal maxDistanceFromPoint {0.0};

        QLineF horizon;

        QPolygonF polygon;

        typedef enum PerspectiveType {
            None,
            OneVp,
            TwoVps
        } PerspectiveType;

        PerspectiveType type {None};

    };

    static bool getTetragon(const QList<KisPaintingAssistantHandleSP> &handles, bool isAssistantComplete, QPolygonF& outPolygon);

    static QPolygonF getAllConnectedTetragon(const QList<KisPaintingAssistantHandleSP>& handles);

    static qreal distanceInGrid(const QList<KisPaintingAssistantHandleSP>& handles, bool isAssistantComplete, const QPointF &point);

    static qreal distanceInGrid(const CacheData& cache, const QPointF &point);

    static void updateCacheData(CacheData& cache, const QPolygonF& poly);

    static bool getVanishingPointsOptional(const QPolygonF &poly, boost::optional<QPointF>& vp1, boost::optional<QPointF>& vp2);

    static qreal localScale(const QTransform& transform, QPointF pt);

    static qreal inverseMaxLocalScale(const QTransform& transform);

    static qreal pdot(const QPointF& a, const QPointF& b);

};

#endif
