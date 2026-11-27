/*
    Copyright (C) 2012 <hanna.et.scott@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later

*/

#include "TestSnapStrategy.h"
#include "KoPathPoint.h"
#include "KoPathShape.h"
#include "KoShapeControllerBase.h"
#include "KoSnapProxy.h"
#include "KoSnapStrategy.h"
#include "KoViewConverter.h"
#include "MockShapes.h"
#include <QPainterPath>
#include <simpletest.h>
#include <testflake.h>

#include <KoSnapData.h>

void TestSnapStrategy::testOrthogonalSnap()
{

    OrthogonalSnapStrategy toTest;
    const QPointF paramMousePosition;
    MockShapeController fakeShapeControllerBase;
    MockCanvas fakeKoCanvasBase(&fakeShapeControllerBase);
    KoSnapGuide aKoSnapGuide(&fakeKoCanvasBase);
    KoSnapProxy paramProxy(&aKoSnapGuide);
    qreal paramSnapDistance = 0;

    bool didSnap = toTest.snap(paramMousePosition, &paramProxy, paramSnapDistance);
    QVERIFY(!didSnap);

    OrthogonalSnapStrategy toTestTwo;
    const QPointF paramMousePositionTwo(3,3);
    MockShapeController fakeShapeControllerBaseTwo;

    MockCanvas fakeKoCanvasBaseTwo(&fakeShapeControllerBaseTwo);

    KoShapeManager *fakeShapeManager = fakeKoCanvasBaseTwo.shapeManager();
    MockShape fakeShapeOne;
    QList<QPointF> firstSnapPointList;
    firstSnapPointList.push_back(QPointF(1,2));
    firstSnapPointList.push_back(QPointF(2,2));
    firstSnapPointList.push_back(QPointF(3,2));
    firstSnapPointList.push_back(QPointF(4,2));

    fakeShapeOne.snapData().setSnapPoints(firstSnapPointList);
    fakeShapeOne.isVisible(true);
    fakeShapeManager->addShape(&fakeShapeOne);
    KoSnapGuide aKoSnapGuideTwo(&fakeKoCanvasBaseTwo);
    KoSnapProxy paramProxyTwo(&aKoSnapGuideTwo);

    qreal paramSnapDistanceTwo = 4;
    bool didSnapTwo = toTestTwo.snap(paramMousePositionTwo, &paramProxyTwo, paramSnapDistanceTwo);
    QVERIFY(didSnapTwo);

    fakeShapeManager->remove(&fakeShapeOne);
}
void TestSnapStrategy::testNodeSnap()
{

    NodeSnapStrategy toTest;
    const QPointF paramMousePos;
    MockShapeController fakeShapeControllerBase;
    MockCanvas fakeKoCanvasBase(&fakeShapeControllerBase);
    KoSnapGuide aKoSnapGuide(&fakeKoCanvasBase);
    KoSnapProxy paramProxy(&aKoSnapGuide);
    qreal paramSnapDistance = 0;

    bool didSnap = toTest.snap(paramMousePos, &paramProxy, paramSnapDistance);
    QVERIFY(!didSnap);

    NodeSnapStrategy toTestTwo;
    const QPointF paramMousePosTwo;
    MockShapeController fakeShapeControllerBaseTwo;
    MockCanvas fakeKoCanvasBaseTwo(&fakeShapeControllerBaseTwo);
    KoShapeManager *fakeShapeManager = fakeKoCanvasBaseTwo.shapeManager();
    MockShape fakeShapeOne;
    QList<QPointF> firstSnapPointList;
    firstSnapPointList.push_back(QPointF(1,2));
    firstSnapPointList.push_back(QPointF(2,2));
    firstSnapPointList.push_back(QPointF(3,2));
    firstSnapPointList.push_back(QPointF(4,2));

    qreal paramSnapDistanceTwo = 4;

    fakeShapeOne.snapData().setSnapPoints(firstSnapPointList);
    fakeShapeOne.isVisible(true);
    fakeShapeManager->addShape(&fakeShapeOne);
    KoSnapGuide aKoSnapGuideTwo(&fakeKoCanvasBaseTwo);
    KoSnapProxy paramProxyTwo(&aKoSnapGuideTwo);

    bool didSnapTwo = toTestTwo.snap(paramMousePosTwo, &paramProxyTwo, paramSnapDistanceTwo);
    QVERIFY(didSnapTwo);

    fakeShapeManager->remove(&fakeShapeOne);
}
void TestSnapStrategy::testExtensionSnap()
{
    ExtensionSnapStrategy toTest;
    const QPointF paramMousePos;
    MockShapeController fakeShapeControllerBase;
    MockCanvas fakeKoCanvasBase(&fakeShapeControllerBase);
    KoSnapGuide aKoSnapGuide(&fakeKoCanvasBase);
    KoSnapProxy paramProxy(&aKoSnapGuide);
    qreal paramSnapDistance = 0;
    bool didSnap = toTest.snap(paramMousePos, &paramProxy, paramSnapDistance);
    QVERIFY(!didSnap);

    ExtensionSnapStrategy toTestTwo;
    const QPointF paramMousePosTwo;
    MockShapeController fakeShapeControllerBaseTwo;
    MockCanvas fakeKoCanvasBaseTwo(&fakeShapeControllerBaseTwo);
    KoShapeManager *fakeShapeManager = fakeKoCanvasBaseTwo.shapeManager();
    KoPathShape fakeShapeOne;
    QList<QPointF> firstSnapPointList;
    firstSnapPointList.push_back(QPointF(1,2));
    firstSnapPointList.push_back(QPointF(2,2));
    firstSnapPointList.push_back(QPointF(3,2));
    firstSnapPointList.push_back(QPointF(4,2));

    qreal paramSnapDistanceTwo = 4;
    fakeShapeOne.snapData().setSnapPoints(firstSnapPointList);
    fakeShapeOne.isVisible(true);

    QPointF firstPoint(0,2);
    QPointF secondPoint(1,2);
    QPointF thirdPoint(2,3);
    QPointF fourthPoint(3,4);

    fakeShapeOne.moveTo(firstPoint);
    fakeShapeOne.lineTo(secondPoint);
    fakeShapeOne.lineTo(thirdPoint);
    fakeShapeOne.lineTo(fourthPoint);

    fakeShapeManager->addShape(&fakeShapeOne);
    KoSnapGuide aKoSnapGuideTwo(&fakeKoCanvasBaseTwo);
    KoSnapProxy paramProxyTwo(&aKoSnapGuideTwo);

    bool didSnapTwo = toTest.snap(paramMousePosTwo, &paramProxyTwo, paramSnapDistanceTwo);
    QVERIFY(didSnapTwo);

    fakeShapeManager->remove(&fakeShapeOne);
}
void TestSnapStrategy::testIntersectionSnap()
{
    IntersectionSnapStrategy toTest;
    const QPointF paramMousePos;
    MockShapeController fakeShapeControllerBase;
    MockCanvas fakeKoCanvasBase(&fakeShapeControllerBase);
    KoSnapGuide aKoSnapGuide(&fakeKoCanvasBase);
    KoSnapProxy paramProxy(&aKoSnapGuide);
    qreal paramSnapDistance = 0;
    bool didSnap = toTest.snap(paramMousePos, &paramProxy, paramSnapDistance);
    QVERIFY(!didSnap);

    IntersectionSnapStrategy toTestTwo;
    const QPointF paramMousePosTwo;
    MockShapeController fakeShapeControllerBaseTwo;
    MockCanvas fakeKoCanvasBaseTwo(&fakeShapeControllerBaseTwo);
    KoShapeManager *ShapeManager = fakeKoCanvasBaseTwo.shapeManager();

    qreal paramSnapDistanceTwo = 8;
    KoPathShape pathShapeOne;
    QList<QPointF> firstSnapPointList;

    pathShapeOne.moveTo(QPointF(1,2));
    pathShapeOne.lineTo(QPointF(2,2));
    pathShapeOne.lineTo(QPointF(3,2));
    pathShapeOne.lineTo(QPointF(4,2));

    pathShapeOne.isVisible(true);
    ShapeManager->addShape(&pathShapeOne);

    KoPathShape pathShapeTwo;
    QList<QPointF> secondSnapPointList;

    pathShapeTwo.moveTo(QPointF(1,1));
    pathShapeTwo.lineTo(QPointF(2,2));
    pathShapeTwo.lineTo(QPointF(3,3));
    pathShapeTwo.lineTo(QPointF(4,4));

    pathShapeTwo.isVisible(true);
    ShapeManager->addShape(&pathShapeTwo);

    KoPathShape pathShapeThree;
    QList<QPointF> thirdSnapPointList;
    pathShapeThree.moveTo(QPointF(5,5));
    pathShapeThree.lineTo(QPointF(6,6));
    pathShapeThree.lineTo(QPointF(7,7));
    pathShapeThree.lineTo(QPointF(8,8));

    pathShapeThree.isVisible(true);
    ShapeManager->addShape(&pathShapeThree);

    KoSnapGuide aKoSnapGuideTwo(&fakeKoCanvasBaseTwo);
    KoSnapProxy paramProxyTwo(&aKoSnapGuideTwo);
    bool didSnapTwo = toTestTwo.snap(paramMousePosTwo, &paramProxyTwo, paramSnapDistanceTwo);
    QVERIFY(didSnapTwo);

    ShapeManager->remove(&pathShapeOne);
    ShapeManager->remove(&pathShapeTwo);
    ShapeManager->remove(&pathShapeThree);
}
void TestSnapStrategy::testGridSnap()
{
    GridSnapStrategy toTest;
    const QPointF paramMousePos;
    MockShapeController fakeShapeControllerBase;
    MockCanvas fakeKoCanvasBase(&fakeShapeControllerBase);
    KoSnapGuide aKoSnapGuide(&fakeKoCanvasBase);
    KoSnapProxy paramProxy(&aKoSnapGuide);
    qreal paramSnapDistance = 0;
    bool didSnap = toTest.snap(paramMousePos, &paramProxy, paramSnapDistance);
    QVERIFY(!didSnap);

    GridSnapStrategy toTestTwo;
    const QPointF paramMousePosTwo(40,60);
    MockShapeController fakeShapeControllerBaseTwo;
    MockCanvas fakeKoCanvasBaseTwo(&fakeShapeControllerBaseTwo);
    fakeKoCanvasBaseTwo.setHorz(10);
    fakeKoCanvasBaseTwo.setVert(8);
    KoSnapGuide aKoSnapGuideTwo(&fakeKoCanvasBaseTwo);
    KoSnapProxy paramProxyTwo(&aKoSnapGuideTwo);
    qreal paramSnapDistanceTwo = 8;
    bool didSnapTwo = toTestTwo.snap(paramMousePosTwo, &paramProxyTwo, paramSnapDistanceTwo);
    QVERIFY(didSnapTwo);

}
void TestSnapStrategy::testBoundingBoxSnap()
{
    BoundingBoxSnapStrategy toTest;
    const QPointF paramMousePos;
    MockShapeController fakeShapeControllerBase;
    MockCanvas fakeKoCanvasBase(&fakeShapeControllerBase);
    KoSnapGuide aKoSnapGuide(&fakeKoCanvasBase);
    KoSnapProxy paramProxy(&aKoSnapGuide);
    qreal paramSnapDistance = 0;
    bool didSnap = toTest.snap(paramMousePos, &paramProxy, paramSnapDistance);
    QVERIFY(!didSnap);

    BoundingBoxSnapStrategy toTestTwo;
    const QPointF paramMousePosTwo;
    MockShapeController fakeShapeControllerBaseTwo;
    MockCanvas fakeKoCanvasBaseTwo(&fakeShapeControllerBaseTwo);
    KoShapeManager *ShapeManager = fakeKoCanvasBaseTwo.shapeManager();

    qreal paramSnapDistanceTwo = 8;
    KoPathShape pathShapeOne;
    QList<QPointF> firstSnapPointList;

    pathShapeOne.moveTo(QPointF(1,2));
    pathShapeOne.lineTo(QPointF(2,2));
    pathShapeOne.lineTo(QPointF(3,2));
    pathShapeOne.lineTo(QPointF(4,2));

    pathShapeOne.isVisible(true);
    ShapeManager->addShape(&pathShapeOne);

    KoPathShape pathShapeTwo;
    QList<QPointF> secondSnapPointList;

    pathShapeTwo.moveTo(QPointF(1,1));
    pathShapeTwo.lineTo(QPointF(2,2));
    pathShapeTwo.lineTo(QPointF(3,3));
    pathShapeTwo.lineTo(QPointF(4,4));

    pathShapeTwo.isVisible(true);
    ShapeManager->addShape(&pathShapeTwo);

    KoPathShape pathShapeThree;
    QList<QPointF> thirdSnapPointList;
    pathShapeThree.moveTo(QPointF(5,5));
    pathShapeThree.lineTo(QPointF(6,6));
    pathShapeThree.lineTo(QPointF(7,7));
    pathShapeThree.lineTo(QPointF(8,8));

    pathShapeThree.isVisible(true);
    ShapeManager->addShape(&pathShapeThree);

    KoSnapGuide aKoSnapGuideTwo(&fakeKoCanvasBaseTwo);
    KoSnapProxy paramProxyTwo(&aKoSnapGuideTwo);
    bool didSnapTwo = toTestTwo.snap(paramMousePosTwo, &paramProxyTwo, paramSnapDistanceTwo);
    QVERIFY(didSnapTwo);

    ShapeManager->remove(&pathShapeOne);
    ShapeManager->remove(&pathShapeTwo);
    ShapeManager->remove(&pathShapeThree);
}
void TestSnapStrategy::testLineGuideSnap()
{

}

void TestSnapStrategy::testOrthogonalDecoration()
{
    OrthogonalSnapStrategy toTestTwo;
    const QPointF paramMousePositionTwo(3,3);
    MockShapeController fakeShapeControllerBaseTwo;
    MockCanvas fakeKoCanvasBaseTwo(&fakeShapeControllerBaseTwo);

    KoShapeManager *fakeShapeManager = fakeKoCanvasBaseTwo.shapeManager();
    MockShape fakeShapeOne;
    QList<QPointF> firstSnapPointList;
    firstSnapPointList.push_back(QPointF(1,2));
    firstSnapPointList.push_back(QPointF(2,2));
    firstSnapPointList.push_back(QPointF(3,2));
    firstSnapPointList.push_back(QPointF(4,2));

    fakeShapeOne.snapData().setSnapPoints(firstSnapPointList);
    fakeShapeOne.isVisible(true);
    fakeShapeManager->addShape(&fakeShapeOne);
    KoSnapGuide aKoSnapGuideTwo(&fakeKoCanvasBaseTwo);
    KoSnapProxy paramProxyTwo(&aKoSnapGuideTwo);

    qreal paramSnapDistanceTwo = 4;
    toTestTwo.snap(paramMousePositionTwo, &paramProxyTwo, paramSnapDistanceTwo);

    KoViewConverter irrelevantParameter;
    QPainterPath resultingDecoration = toTestTwo.decoration(irrelevantParameter);

    QVERIFY( resultingDecoration.isEmpty() );

    fakeShapeManager->remove(&fakeShapeOne);

}
void TestSnapStrategy::testNodeDecoration()
{
    NodeSnapStrategy toTest;
    KoViewConverter irrelevantParameter;
    QRectF originalRect = QRectF(-5.5, -5.5, 11, 11);
    QPainterPath resultingDecoration = toTest.decoration(irrelevantParameter);
    QRectF rectInsidePath = resultingDecoration.boundingRect();
    QVERIFY(originalRect==rectInsidePath);
}
void TestSnapStrategy::testExtensionDecoration()
{

    ExtensionSnapStrategy toTestTwo;
    const QPointF paramMousePosTwo;
    MockShapeController fakeShapeControllerBaseTwo;
    MockCanvas fakeKoCanvasBaseTwo(&fakeShapeControllerBaseTwo);
    KoShapeManager *fakeShapeManager = fakeKoCanvasBaseTwo.shapeManager();
    KoPathShape fakeShapeOne;
    QList<QPointF> firstSnapPointList;
    firstSnapPointList.push_back(QPointF(1,2));
    firstSnapPointList.push_back(QPointF(2,2));
    firstSnapPointList.push_back(QPointF(3,2));
    firstSnapPointList.push_back(QPointF(4,2));

    qreal paramSnapDistanceTwo = 4;
    fakeShapeOne.snapData().setSnapPoints(firstSnapPointList);
    fakeShapeOne.isVisible(true);

    QPointF firstPoint(0,2);
    QPointF secondPoint(1,2);
    QPointF thirdPoint(2,3);
    QPointF fourthPoint(3,4);

    fakeShapeOne.moveTo(firstPoint);
    fakeShapeOne.lineTo(secondPoint);
    fakeShapeOne.lineTo(thirdPoint);
    fakeShapeOne.lineTo(fourthPoint);

    fakeShapeManager->addShape(&fakeShapeOne);
    KoSnapGuide aKoSnapGuideTwo(&fakeKoCanvasBaseTwo);
    KoSnapProxy paramProxyTwo(&aKoSnapGuideTwo);

    toTestTwo.snap(paramMousePosTwo, &paramProxyTwo, paramSnapDistanceTwo);

    const KoViewConverter aConverter;
    QPainterPath resultingDecoration = toTestTwo.decoration(aConverter);
    QPointF resultDecorationLastPoint = resultingDecoration.currentPosition();

    QVERIFY( resultDecorationLastPoint == QPointF(0,2) );

    fakeShapeManager->remove(&fakeShapeOne);
}
void TestSnapStrategy::testIntersectionDecoration()
{
    IntersectionSnapStrategy toTest;
    KoViewConverter irrelevantParameter;
    QRectF originalRect = QRectF(-5.5,-5.5,11,11);
    QPainterPath resultingDecoration = toTest.decoration(irrelevantParameter);
    QRectF rectInsidePath = resultingDecoration.boundingRect();
    QVERIFY(originalRect==rectInsidePath);
}
void TestSnapStrategy::testGridDecoration()
{
    GridSnapStrategy toTest;
    const QPointF paramMousePosTwo(40,60);
    MockShapeController fakeShapeControllerBaseTwo;
    MockCanvas fakeKoCanvasBaseTwo(&fakeShapeControllerBaseTwo);
    fakeKoCanvasBaseTwo.setHorz(10);
    fakeKoCanvasBaseTwo.setVert(8);
    KoSnapGuide aKoSnapGuideTwo(&fakeKoCanvasBaseTwo);
    KoSnapProxy paramProxyTwo(&aKoSnapGuideTwo);
    qreal paramSnapDistanceTwo = 8;
    toTest.snap(paramMousePosTwo, &paramProxyTwo, paramSnapDistanceTwo);

    KoViewConverter viewConverter;
    QSizeF unzoomedSize = viewConverter.viewToDocument(QSizeF(5, 5));
    QPointF snappedPos(40, 56);
    QPointF originalEndPoint(snappedPos + QPointF(0, unzoomedSize.height()));
    QPainterPath resultingDecoration = toTest.decoration(viewConverter);

    QVERIFY( resultingDecoration.currentPosition() == originalEndPoint );
}
void TestSnapStrategy::testBoundingBoxDecoration()
{
    BoundingBoxSnapStrategy toTest;

    KoViewConverter viewConverter;
    QSizeF unzoomedSize = viewConverter.viewToDocument(QSizeF(5, 5));
    QPointF snappedPos(0,0);
    QPointF originalEndPoint(snappedPos + QPointF(unzoomedSize.width(), -unzoomedSize.height()));
    QPainterPath resultingDecoration = toTest.decoration(viewConverter);

    QVERIFY( resultingDecoration.currentPosition() == originalEndPoint );
}

void TestSnapStrategy::testLineGuideDecoration()
{

}

void TestSnapStrategy::testSquareDistance()
{
    OrthogonalSnapStrategy toTest;

    QPointF p1;
    QPointF p2;

    qreal resultingRealOne = toTest.squareDistance(p1, p2);
    QVERIFY(resultingRealOne == 0);
    OrthogonalSnapStrategy toTestTwo;

    QPointF p1_2(2,2);
    QPointF p2_2(1,1);

    qreal resultingRealTwo = toTestTwo.squareDistance(p1_2, p2_2);
    QVERIFY(resultingRealTwo == 2);
    OrthogonalSnapStrategy toTestThree;

    QPointF p1_3(2,2);
    QPointF p2_3(-2,-2);

    qreal resultingRealThree = toTestThree.squareDistance(p1_3, p2_3);
    QVERIFY(resultingRealThree == 32);

    OrthogonalSnapStrategy toTestFour;

    QPointF p1_4(2,2);
    QPointF p2_4(2,2);

    qreal resultingRealFour = toTestFour.squareDistance(p1_4, p2_4);
    QVERIFY(resultingRealFour == 0);
}
void TestSnapStrategy::testScalarProduct()
{
    OrthogonalSnapStrategy toTest;

    QPointF p1_5;
    QPointF p2_5;

    qreal resultingRealOne = toTest.squareDistance(p1_5, p2_5);
    QVERIFY(resultingRealOne == 0 );
    OrthogonalSnapStrategy toTestTwo;

    QPointF p1_6(2,2);
    QPointF p2_6(3,3);

    qreal resultingRealTwo = toTestTwo.squareDistance(p1_6, p2_6);
    QVERIFY(resultingRealTwo == 2 );
    OrthogonalSnapStrategy toTestThree;

    QPointF p1_7(2,2);
    QPointF p2_7(-2,-2);

    qreal resultingRealThree = toTestThree.squareDistance(p1_7, p2_7);
    QVERIFY(resultingRealThree == 32);
    OrthogonalSnapStrategy toTestFour;

    QPointF p1_8(1,1);
    QPointF p2_8(1,1);

    qreal resultingRealFour = toTestFour.squareDistance(p1_8, p2_8);
    QVERIFY(resultingRealFour == 0);
    OrthogonalSnapStrategy toTestFive;

    QPointF p1_9(1,1);
    QPointF p2_9(0,0);

    qreal resultingRealFive = toTestFive.squareDistance(p1_9, p2_9);
    QVERIFY(resultingRealFive == 2);
}

void TestSnapStrategy::testSnapToExtension()
{
}
void TestSnapStrategy::testProject()
{
    ExtensionSnapStrategy toTestOne;
    qreal toCompWithOne = -1;
    QPointF lineStart(4,4);
    QPointF lineEnd(2,2);
    QPointF comparisonPoint(6,6);

    qreal resultingRealOne = toTestOne.project(lineStart, lineEnd, comparisonPoint);
    QCOMPARE(resultingRealOne, toCompWithOne);
    ExtensionSnapStrategy toTestTwo;
    qreal toCompWithTwo = -4;
    QPointF lineStart_2(-2,-2);
    QPointF lineEnd_2(-4,-4);
    QPointF comparisonPoint_2(6,6);

    qreal resultingRealTwo = toTestTwo.project(lineStart_2, lineEnd_2, comparisonPoint_2);
    QCOMPARE(resultingRealTwo, toCompWithTwo);
    ExtensionSnapStrategy toTestThree;
    qreal toCompWithThree = (10*(6/sqrt(72.0)) + 10*(6/sqrt(72.0))) / sqrt(72.0);
    QPointF lineStart_3(-2,-2);
    QPointF lineEnd_3(4, 4);
    QPointF comparisonPoint_3(8,8);

    qreal resultingRealThree = toTestThree.project(lineStart_3, lineEnd_3, comparisonPoint_3);
    QCOMPARE(resultingRealThree, toCompWithThree);

    ExtensionSnapStrategy toTestFour;
    qreal toCompWithFour = 0.5;
    QPointF lineStart_4(2,1);
    QPointF lineEnd_4(6,3);
    QPointF comparisonPoint_4(3,4);

    qreal resultingRealFour = toTestFour.project(lineStart_4, lineEnd_4, comparisonPoint_4);
    QCOMPARE(resultingRealFour, toCompWithFour);
}

void TestSnapStrategy::testExtensionDirection()
{
    ExtensionSnapStrategy toTestOne;
    KoPathShape uninitiatedPathShape;
    KoPathPoint::PointProperties normal = KoPathPoint::Normal;
    const QPointF initiatedPoint0(0,0);
    KoPathPoint initiatedPoint(&uninitiatedPathShape, initiatedPoint0, normal);
    const QTransform initiatedMatrix(1, 1, 1, 1, 1, 1);
    QPointF direction2 = toTestOne.extensionDirection( &initiatedPoint, initiatedMatrix);
    QVERIFY(direction2.isNull());

    ExtensionSnapStrategy toTestTwo;
    QPointF expectedPointTwo(0,0);
    KoPathShape shapeOne;

    QPointF firstPoint(0,1);
    QPointF secondPoint(1,2);
    QPointF thirdPoint(2,3);
    QPointF fourthPoint(3,4);

    shapeOne.moveTo(firstPoint);
    shapeOne.lineTo(secondPoint);
    shapeOne.lineTo(thirdPoint);
    shapeOne.lineTo(fourthPoint);

    QPointF paramPositionTwo(0,1);
    KoPathPoint paramPointTwo;
    paramPointTwo.setPoint(paramPositionTwo);
    paramPointTwo.setParent(&shapeOne);

    const QTransform paramTransMatrix(1,2,3,4,5,6);
    QPointF directionTwo = toTestTwo.extensionDirection( &paramPointTwo, paramTransMatrix);
    QCOMPARE(directionTwo, expectedPointTwo);

    ExtensionSnapStrategy toTestThree;
    QPointF expectedPointThree(0,0);
    QPointF paramPositionThree(1,1);
    KoPathPoint paramPointThree;
    paramPointThree.setPoint(paramPositionThree);
    paramPointThree.setParent(&shapeOne);
    QPointF directionThree = toTestThree.extensionDirection( &paramPointThree, paramTransMatrix);
    QCOMPARE(directionThree, expectedPointThree);

}

void TestSnapStrategy::testSquareDistanceToLine()
{
    BoundingBoxSnapStrategy toTestOne;

    const QPointF lineA(4,1);
    const QPointF lineB(6,3);
    const QPointF point(5,8);
    QPointF pointOnLine(0,0);

    qreal result = toTestOne.squareDistanceToLine(lineA, lineB, point, pointOnLine);
    QVERIFY(result == HUGE_VAL);

    BoundingBoxSnapStrategy toTestTwo;
    QPointF lineA2(4,4);
    QPointF lineB2(4,4);
    QPointF point2(5,8);
    QPointF pointOnLine2(0,0);

    qreal result2 = toTestTwo.squareDistanceToLine(lineA2, lineB2, point2, pointOnLine2);
    QVERIFY(result2 == HUGE_VAL);

    BoundingBoxSnapStrategy toTestThree;
    QPointF lineA3(6,4);
    QPointF lineB3(8,6);
    QPointF point3(2,2);
    QPointF pointOnLine3(0,0);

    qreal result3 = toTestThree.squareDistanceToLine(lineA3, lineB3, point3, pointOnLine3);
    QVERIFY(result3 == HUGE_VAL);

    BoundingBoxSnapStrategy toTestFour;
    QPointF lineA4(2,2);
    QPointF lineB4(8,6);
    QPointF point4(3,4);
    QPointF pointOnLine4(0,0);

    QPointF diff(6,4);

    QPointF distToPointOnLine = (lineA4 + ((1*(6/sqrt(52.0)) + 2*(4/sqrt(52.0))) / sqrt(52.0)) * diff)-point4;
    qreal toCompWithFour = distToPointOnLine.x()*distToPointOnLine.x()+distToPointOnLine.y()*distToPointOnLine.y();

    qreal result4 = toTestFour.squareDistanceToLine(lineA4, lineB4, point4, pointOnLine4);
    QVERIFY(qFuzzyCompare(result4, toCompWithFour));

}
KISTEST_MAIN(TestSnapStrategy)
