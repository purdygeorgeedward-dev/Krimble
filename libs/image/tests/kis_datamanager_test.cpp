/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt boud @valdyas.org
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_datamanager_test.h"

#include <simpletest.h>
#include "kis_datamanager.h"

void KisDataManagerTest::testCreation()
{
    quint8 c = 0;
    KisDataManager test(1, &c);
}

void KisDataManagerTest::testDefaultPixel()
{
    {
        quint8 c = 0;
        KisDataManager dm(1, &c);
        QVERIFY(dm.pixelSize() == 1);
        QVERIFY(*dm.defaultPixel() == 0);
    }
    {
        quint8 * p = new quint8[3];
        memset(p, 0, 3);

        KisDataManager dm(3, p);
        QVERIFY(dm.pixelSize() == 3);

        const quint8 * defaultPixelC = dm.defaultPixel();
        QVERIFY(defaultPixelC[0] == 0);
        QVERIFY(defaultPixelC[1] == 0);
        QVERIFY(defaultPixelC[2] == 0);

        quint8 * defaultPixel = const_cast<quint8*>(defaultPixelC);
        defaultPixel[0] = 50;
        defaultPixel[1] = 150;
        defaultPixel[2] = 200;

        QVERIFY(p[0] == 0);
        QVERIFY(p[1] == 0);
        QVERIFY(p[2] == 0);

        dm.setDefaultPixel(p);
        defaultPixelC = dm.defaultPixel();
        QVERIFY(defaultPixelC[0] == 0);
        QVERIFY(defaultPixelC[1] == 0);
        QVERIFY(defaultPixelC[2] == 0);

        delete[]p;

    }
}

SIMPLE_TEST_MAIN(KisDataManagerTest)
