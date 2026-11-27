/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPathPointRemoveCommand.h"
#include "KoSubpathRemoveCommand.h"
#include "KoShapeController.h"
#include "KoPathPoint.h"
#include <klocalizedstring.h>

class KoPathPointRemoveCommandPrivate
{
public:
    KoPathPointRemoveCommandPrivate() : deletePoints(false) { }
    ~KoPathPointRemoveCommandPrivate() {
        if (deletePoints)
            qDeleteAll(points);
    }
    QList<KoPathPointData> pointDataList;
    QList<KoPathPoint*> points;
    bool deletePoints;
};

KUndo2Command *KoPathPointRemoveCommand::createCommand(
    const QList<KoPathPointData> &pointDataList,
    KoShapeController *shapeController,
    KUndo2Command *parent)
{

    QList<KoPathPointData> sortedPointData(pointDataList);
    std::sort(sortedPointData.begin(), sortedPointData.end());

    KoPathPointData last(0, KoPathPointIndex(-1, -1));
    sortedPointData.append(last);

    QList<KoPathPointData> pointsOfSubpath;
    QList<KoPathPointData> subpathsOfPath;
    QList<KoPathPointData> pointsToDelete;
    QList<KoPathPointData> subpathToDelete;
    QList<KoShape*> shapesToDelete;

    last = sortedPointData.first();

    QList<KoPathPointData>::const_iterator it(sortedPointData.constBegin());
    for (; it != sortedPointData.constEnd(); ++it) {
        if (last.pathShape != it->pathShape || last.pointIndex.first != it->pointIndex.first) {
            if (last.pathShape->subpathPointCount(last.pointIndex.first) == pointsOfSubpath.size()) {
                subpathsOfPath.append(pointsOfSubpath.first());
            } else {
                pointsToDelete += pointsOfSubpath;
            }
            pointsOfSubpath.clear();
        }

        if (last.pathShape != it->pathShape) {
            if (last.pathShape->subpathCount() == subpathsOfPath.size()) {
                shapesToDelete.append(last.pathShape);
            } else {
                subpathToDelete += subpathsOfPath;
            }
            subpathsOfPath.clear();
        }
        if (! it->pathShape)
            continue;
        last = *it;
        pointsOfSubpath.append(*it);
    }

    KUndo2Command *cmd = new KUndo2Command(kundo2_i18n("Remove points"), parent);

    if (pointsToDelete.size() > 0) {
        new KoPathPointRemoveCommand(pointsToDelete, cmd);
    }

    std::sort(subpathToDelete.begin(), subpathToDelete.end(), std::greater<>{});

    Q_FOREACH (const KoPathPointData & pd, subpathToDelete) {
        new KoSubpathRemoveCommand(pd.pathShape, pd.pointIndex.first, cmd);
    }
    if (shapesToDelete.size() > 0) {
        shapeController->removeShapes(shapesToDelete, cmd);
    }

    return cmd;
}

KoPathPointRemoveCommand::KoPathPointRemoveCommand(const QList<KoPathPointData> & pointDataList,
        KUndo2Command *parent)
    : KUndo2Command(parent),
    d(new KoPathPointRemoveCommandPrivate())
{
    QList<KoPathPointData>::const_iterator it(pointDataList.begin());
    for (; it != pointDataList.end(); ++it) {
        KoPathPoint *point = it->pathShape->pointByIndex(it->pointIndex);
        if (point) {
            d->pointDataList.append(*it);
            d->points.append(0);
        }
    }
    std::sort(d->pointDataList.begin(), d->pointDataList.end());
    setText(kundo2_i18n("Remove points"));
}

KoPathPointRemoveCommand::~KoPathPointRemoveCommand()
{
    delete d;
}

void KoPathPointRemoveCommand::redo()
{
    KUndo2Command::redo();
    KoPathShape * lastPathShape = 0;
    int updateBefore = d->pointDataList.size();
    for (int i = d->pointDataList.size() - 1; i >= 0; --i) {
        const KoPathPointData &pd = d->pointDataList.at(i);
        pd.pathShape->update();
        d->points[i] = pd.pathShape->removePoint(pd.pointIndex);

        if (lastPathShape != pd.pathShape) {
            if (lastPathShape) {
                QPointF offset = lastPathShape->normalize();

                QTransform matrix;
                matrix.translate(-offset.x(), -offset.y());
                for (int j = i + 1; j < updateBefore; ++j) {
                    d->points.at(j)->map(matrix);
                }
                lastPathShape->update();
                updateBefore = i + 1;
            }
            lastPathShape = pd.pathShape;
        }
    }

    if (lastPathShape) {
        QPointF offset = lastPathShape->normalize();

        QTransform matrix;
        matrix.translate(-offset.x(), -offset.y());
        for (int j = 0; j < updateBefore; ++j) {
            d->points.at(j)->map(matrix);
        }
        lastPathShape->update();
    }

    d->deletePoints = true;
}

void KoPathPointRemoveCommand::undo()
{
    KUndo2Command::undo();
    KoPathShape * lastPathShape = 0;

    QMap<KoPathShape *, QList<KoPathPointIndex>> pointsMap;

    for (int i = 0; i < d->pointDataList.size(); ++i) {
        const KoPathPointData &pd = d->pointDataList.at(i);
        if (lastPathShape && lastPathShape != pd.pathShape) {
            lastPathShape->normalize();
            lastPathShape->update();
        }
        pd.pathShape->insertPoint(d->points[i], pd.pointIndex);
        lastPathShape = pd.pathShape;

        pointsMap[pd.pathShape].append(pd.pointIndex);
    }

    if (lastPathShape) {
        lastPathShape->normalize();
        lastPathShape->update();
    }

    for (auto it = pointsMap.constBegin(); it != pointsMap.constEnd(); ++it) {
        it.key()->recommendPointSelectionChange(it.value());
    }

    d->deletePoints = false;
}
