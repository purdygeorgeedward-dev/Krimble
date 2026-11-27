/*
 *  SPDX-FileCopyrightText: 2025 Agata Cacko <cacko.azh@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisSpatialContainerTest.h"

#include <QtMath>
#include <QRandomGenerator>
#include <simpletest.h>

#include "kis_algebra_2d.h"
#include "kis_debug.h"

#include "kis_grid_interpolation_tools.h"
#include "KisSpatialContainer.h"

void writeOutVector(QVector<QPointF> &vector)
{
    int n = vector.length();
    for (int i = 0; i < n; i++) {
        qCritical() << vector[i];
    }
}

void KisSpatialContainerTest::testSpatialContainerAddMovePoints()
{
    QRandomGenerator gen(1000);
    int n = 30;
    QVector<QPointF> initial;
    QVector<QPointF> firstChange;
    QVector<QPointF> secondChange;

    initial.reserve(n);
    firstChange.reserve(n);
    secondChange.reserve(n);

    double max = 100.0;

    for (int i = 0; i < n; i++) {
        initial << QPointF(gen.bounded(max), gen.bounded(max));
        firstChange << QPointF(gen.bounded(max), gen.bounded(max));
        secondChange << QPointF(gen.bounded(max), gen.bounded(max));
    }

    KisSpatialContainer container(QRectF(0, 0, max, max), 5);

    QRectF boundsInitial;
    for (int i = 0; i < n; i++)
    {
        container.addPoint(i, initial[i]);
        KisAlgebra2D::accumulateBounds(initial[i], &boundsInitial);
    }

    QCOMPARE(boundsInitial, container.exactBounds());

    for (int i = 0; i < n; i++) {
        container.movePoint(i, initial[i], firstChange[i]);
    }

    QCOMPARE(KisAlgebra2D::accumulateBounds(firstChange), container.exactBounds());

    for (int i = 0; i < n; i++) {
        container.movePoint(i, firstChange[i], secondChange[i]);
    }

    QCOMPARE(KisAlgebra2D::accumulateBounds(secondChange), container.exactBounds());

    QVector<QPointF> inContainer = container.toVector();

    for (int i = 0; i < n; i++) {
        QCOMPARE(inContainer[i], secondChange[i]);
    }

}

void KisSpatialContainerTest::testSpatialContainerSearchPoints()
{
    QRandomGenerator gen(1000);
    int n = 1000;
    QVector<QPointF> initial;
    QVector<QPointF> firstChange;
    QVector<QPointF> secondChange;

    initial.reserve(n);
    firstChange.reserve(n);
    secondChange.reserve(n);

    double max = 100.0;

    for (int i = 0; i < n; i++) {
        initial << QPointF(gen.bounded(max), gen.bounded(max));
        firstChange << QPointF(gen.bounded(max), gen.bounded(max));
        secondChange << QPointF(gen.bounded(max), gen.bounded(max));
    }

    KisSpatialContainer container(QRectF(0, 0, max, max));

    for (int i = 0; i < n; i++)
    {
        container.addPoint(i, initial[i]);
    }

    for (int i = 0; i < n; i++) {
        container.movePoint(i, initial[i], firstChange[i]);
    }

    for (int i = 0; i < n; i++) {
        container.movePoint(i, firstChange[i], secondChange[i]);
    }

}

struct AllPointsFetcherOp
{
    AllPointsFetcherOp(QRectF srcRect) : m_srcRect(srcRect) {}

    inline void processPoint(int col, int row,
                             int prevCol, int prevRow,
                             int colIndex, int rowIndex) {

        Q_UNUSED(prevCol);
        Q_UNUSED(prevRow);
        Q_UNUSED(colIndex);
        Q_UNUSED(rowIndex);

        QPointF pt(col, row);
        m_points << pt;
    }

    inline void nextLine() {
    }

    QVector<QPointF> m_points;
    QRectF m_srcRect;
};

QString pointToString(const QPointF &p) {
    return QString("(").append(QString::number(p.x())).append(", ").append(QString::number(p.y())).append(") ");
}

QString pointsVectorToString(const QVector<QPointF> &points, QString vectorTitle = "") {
    QString list = "";
    if (vectorTitle != "") {
        list = vectorTitle.append(" = ");
    }
    for (int i = 0; i < points.count(); i++) {
        list.append(pointToString((points[i])));
    }
    return list;
}

void KisSpatialContainerTest::testSpatialContainerInitializeWithGridPoints()
{
    int size = 5000;
    int pixelPrecision = 8;
    int countPointsInNode = 100;
    QPoint startingPoint = QPoint(123, 256);
    QRect srcBounds = QRect(startingPoint, QSize(size, 2*size + 10));
    QSize gridSize =
        GridIterationTools::calcGridSize(srcBounds, pixelPrecision);

    AllPointsFetcherOp pointsOp(srcBounds);
    GridIterationTools::processGrid(pointsOp, srcBounds, pixelPrecision);

    QVector<QPointF> points = pointsOp.m_points;
    const int numPoints = points.size();

    if (numPoints != gridSize.width() * gridSize.height()) {
    }
    KIS_ASSERT_RECOVER_RETURN(numPoints == gridSize.width() * gridSize.height());
    QRectF gridArea = srcBounds;

    KisSpatialContainer container(gridArea, countPointsInNode);

    container.initializeWithGridPoints(gridArea, pixelPrecision);

    QVector<QPointF> pointsInContainer = container.toVector();

    if (container.count() <= 200) {
        qCritical() << pointsVectorToString(points, "original points");
        qCritical() << pointsVectorToString(pointsInContainer, "container points");
    }

    QCOMPARE(pointsInContainer.length(), points.length());

    for (int i = 0; i < numPoints; i++) {
        if (pointsInContainer[i] != points[i]) {
            qCritical() << "It happened at index: " << i;
        }
        QCOMPARE(pointsInContainer[i], points[i]);
    }

}

void KisSpatialContainerTest::testMemoryCleanup()
{
    {
        QRandomGenerator gen(1000);
        int n = 1000;
        QVector<QPointF> initial;
        QVector<QPointF> firstChange;
        QVector<QPointF> secondChange;

        initial.reserve(n);
        firstChange.reserve(n);
        secondChange.reserve(n);

        double max = 100.0;

        for (int i = 0; i < n; i++) {
            initial << QPointF(gen.bounded(max), gen.bounded(max));
            firstChange << QPointF(gen.bounded(max), gen.bounded(max));
            secondChange << QPointF(gen.bounded(max), gen.bounded(max));
        }

        KisSpatialContainer container(QRectF(0, 0, max, max));

        for (int i = 0; i < n; i++)
        {
            container.addPoint(i, initial[i]);
        }

        for (int i = 0; i < n; i++)
        {
            container.movePoint(i, initial[i], firstChange[i]);
        }

        for (int i = 0; i < n; i++)
        {
            container.movePoint(i, firstChange[i], secondChange[i]);
        }

        KisSpatialContainer copy(container);

        QVector<QPointF> original = container.toVector();
        QVector<QPointF> copied = copy.toVector();

        for (int i = 0; i < n; i++)
        {
            QCOMPARE(original[i], copied[i]);
        }

    }

}

void KisSpatialContainerTest::testDeepCopy()
{
    QRandomGenerator gen(1000);
    int n = 1000;
    QVector<QPointF> initial;
    QVector<QPointF> firstChange;
    QVector<QPointF> secondChange;

    initial.reserve(n);
    firstChange.reserve(n);
    secondChange.reserve(n);

    double max = 100.0;

    for (int i = 0; i < n; i++) {
        initial << QPointF(gen.bounded(max), gen.bounded(max));
        firstChange << QPointF(gen.bounded(max), gen.bounded(max));
        secondChange << QPointF(gen.bounded(max), gen.bounded(max));
    }

    KisSpatialContainer container(QRectF(0, 0, max, max));

    for (int i = 0; i < n; i++)
    {
        container.addPoint(i, initial[i]);
    }

    for (int i = 0; i < n; i++) {
        container.movePoint(i, initial[i], firstChange[i]);
    }

    for (int i = 0; i < n; i++) {
        container.movePoint(i, firstChange[i], secondChange[i]);
    }

    KisSpatialContainer containerCopy(container);

    QVector<QPointF> pointsInOriginalContainer = container.toVector();
    QVector<QPointF> pointsInCopiedContainer = container.toVector();

    for (int i = 0; i < n; i++) {
        QCOMPARE(pointsInOriginalContainer[i], secondChange[i]);
        QCOMPARE(pointsInCopiedContainer[i], secondChange[i]);
    }

}

SIMPLE_TEST_MAIN(KisSpatialContainerTest)
