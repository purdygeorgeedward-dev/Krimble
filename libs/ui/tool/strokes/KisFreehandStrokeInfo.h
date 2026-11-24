/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISPAINTINGSTROKEDATA_H
#define KISPAINTINGSTROKEDATA_H

#include "kritaui_export.h"

class KisPainter;
class KisDistanceInformation;

class KRITAUI_EXPORT KisFreehandStrokeInfo {
public:
    KisFreehandStrokeInfo();
    KisFreehandStrokeInfo(const KisDistanceInformation &startDist);
    KisFreehandStrokeInfo(KisFreehandStrokeInfo *rhs, int levelOfDetail);
    ~KisFreehandStrokeInfo();

    KisPainter *painter;
    KisDistanceInformation *dragDistance;

    KisDistanceInformation* buddyDragDistance();

private:
    KisFreehandStrokeInfo *m_parentStrokeInfo {0};
    KisFreehandStrokeInfo *m_childStrokeInfo {0};
};

#endif
