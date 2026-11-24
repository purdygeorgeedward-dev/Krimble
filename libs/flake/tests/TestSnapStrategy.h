/*
    Copyright (C) 2012 <hanna.et.scott@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later

*/

#ifndef TESTSNAPSTRATEGY_H
#define TESTSNAPSTRATEGY_H

#include <QObject>

class TestSnapStrategy : public QObject
{
    Q_OBJECT
  private Q_SLOTS:
    void testOrthogonalSnap();
    void testNodeSnap();
    void testExtensionSnap();
    void testIntersectionSnap();
    void testGridSnap();
    void testBoundingBoxSnap();
    void testLineGuideSnap();

    void testOrthogonalDecoration();
    void testNodeDecoration();
    void testExtensionDecoration();
    void testIntersectionDecoration();
    void testGridDecoration();
    void testBoundingBoxDecoration();
    void testLineGuideDecoration();
    void testSquareDistance();
    void testScalarProduct();
    void testSnapToExtension();
    void testProject();
    void testExtensionDirection();
    void testSquareDistanceToLine();

};

#endif
