/*
 *  SPDX-FileCopyrightText: 2011 Hanna Skott <hannaetscott@gmail.com>
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "TestInputDevice.h"

#include <simpletest.h>
#include "KoInputDevice.h"

void TestInputDevice::testTabletConstructor()
{
    KoInputDevice::InputDevice parameterTabletDevice = KoInputDevice::InputDevice::Stylus;
    KoInputDevice::Pointer parameterPointerType = KoInputDevice::Pointer::Eraser;
    qint64 parameterUniqueTabletId = 0;
    KoInputDevice toTest(parameterTabletDevice, parameterPointerType, parameterUniqueTabletId);
    bool isMouse = toTest.isMouse();
    QVERIFY(!isMouse);
}
void TestInputDevice::testNoParameterConstructor()
{
    KoInputDevice toTest;
    bool isMouse = toTest.isMouse();
    qint64 theUniqueTabletId = toTest.uniqueTabletId();
    qint64 toCompareWith = -1;
    QVERIFY(isMouse);
    QCOMPARE(theUniqueTabletId, toCompareWith);
}
void TestInputDevice::testConstructorWithSingleReference()
{
    KoInputDevice::InputDevice parameterTabletDevice = KoInputDevice::InputDevice::Stylus;
    KoInputDevice::Pointer parameterPointerType = KoInputDevice::Pointer::Eraser;
    qint64 parameterUniqueTabletId = 0;
    KoInputDevice parameterTabletDevice2(parameterTabletDevice, parameterPointerType, parameterUniqueTabletId);
    KoInputDevice toTest(parameterTabletDevice2);
    bool isMouse = toTest.isMouse();
    QVERIFY(!isMouse);
    KoInputDevice parameterMouseDevice;
    toTest = KoInputDevice(parameterMouseDevice);
    isMouse = toTest.isMouse();
    QVERIFY(isMouse);
}
void TestInputDevice::testEqualityCheckOperator()
{
    KoInputDevice::InputDevice parameterTabletDevice = KoInputDevice::InputDevice::Stylus;
    KoInputDevice::Pointer parameterPointerType = KoInputDevice::Pointer::Eraser;
    qint64 parameterUniqueTabletId = 0;
    KoInputDevice parameterTabletDevice2(parameterTabletDevice, parameterPointerType, parameterUniqueTabletId);
    KoInputDevice toTest(parameterTabletDevice2);
    KoInputDevice copy = toTest;
    KoInputDevice notSame;
    QVERIFY(toTest==copy);
    QVERIFY(toTest!=notSame);
}
void TestInputDevice::testDevice()
{
    KoInputDevice::InputDevice parameterTabletDevice = KoInputDevice::InputDevice::Stylus;
    KoInputDevice::Pointer parameterPointerType = KoInputDevice::Pointer::Eraser; ;
    qint64 parameterUniqueTabletId = 0;
    KoInputDevice parameterTabletDevice2(parameterTabletDevice, parameterPointerType, parameterUniqueTabletId);
    KoInputDevice toTest(parameterTabletDevice2);
    KoInputDevice::InputDevice aTabletDevice = toTest.device();

    QCOMPARE(aTabletDevice, KoInputDevice::InputDevice::Stylus);
}
void TestInputDevice::testPointer()
{
    KoInputDevice::InputDevice parameterTabletDevice = KoInputDevice::InputDevice::Stylus;
    KoInputDevice::Pointer parameterPointerType = KoInputDevice::Pointer::Eraser;
    qint64 parameterUniqueTabletId = 0;
    KoInputDevice parameterTabletDevice2(parameterTabletDevice, parameterPointerType, parameterUniqueTabletId);
    KoInputDevice toTest(parameterTabletDevice2);
    KoInputDevice::Pointer aTabletPointer= toTest.pointer();

    QCOMPARE(aTabletPointer, KoInputDevice::Pointer::Eraser);
}
void TestInputDevice::testMouse()
{
    KoInputDevice toTest = KoInputDevice::mouse();
    bool isMouse = toTest.isMouse();
    QVERIFY(isMouse);
}
void TestInputDevice::testStylus()
{
    KoInputDevice toTest = KoInputDevice::stylus();
    bool isMouse = toTest.isMouse();
    QVERIFY(!isMouse);
}
void TestInputDevice::testEraser()
{
    KoInputDevice toTest = KoInputDevice::eraser();
    bool isMouse = toTest.isMouse();
    QVERIFY(!isMouse);
}

SIMPLE_TEST_MAIN(TestInputDevice)
