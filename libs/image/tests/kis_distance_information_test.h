/*
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KIS_DISTANCE_INFORMATION_TEST_H
#define KIS_DISTANCE_INFORMATION_TEST_H

#include <simpletest.h>

class KisPaintInformation;
class KisDistanceInformation;

class KisDistanceInformationTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testInitInfo();
    void testInterpolation();

private:
    void testInitInfoEquality() const;
    void testInitInfoXMLClone() const;

    void testInterpolationImpl(const KisPaintInformation &p1, const KisPaintInformation &p2,
                               KisDistanceInformation &dist, qreal interpFactor,
                               bool needSpacingUpdate, bool needTimingUpdate,
                               qreal interpTolerance) const;
};

#endif
