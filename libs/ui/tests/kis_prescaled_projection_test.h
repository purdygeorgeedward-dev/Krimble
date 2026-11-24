/*
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISPRESCALED_PROJECTION_TEST_H
#define KISPRESCALED_PROJECTION_TEST_H

#include <simpletest.h>

class KisPrescaledProjection;
class KoZoomHandler;
class QString;

class KisPrescaledProjectionTest : public QObject
{
    Q_OBJECT

private:

    bool testProjectionScenario(KisPrescaledProjection & projection, KoZoomHandler * viewConverter, const QString & name);

private Q_SLOTS:

    void testCreation();

    void testScalingUndeferredSmoothingPixelForPixel();

    void testScalingUndeferredSmoothing();

    void benchmarkUpdate();

    void testScrollingZoom100();
    void testScrollingZoom50();
    void testUpdates();

    void testQtScaling();
};

#endif

