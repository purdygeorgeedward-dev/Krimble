/*
 *  This file is part of the KDE project
 *
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_RANDOM_SUB_ACCESSOR_H
#define KIS_RANDOM_SUB_ACCESSOR_H

#include "kis_random_accessor_ng.h"
#include "kis_types.h"
#include <kritaimage_export.h>
#include "kis_shared.h"

class  KRITAIMAGE_EXPORT KisRandomSubAccessor : public KisShared
{
public:
    KisRandomSubAccessor(KisPaintDeviceSP device);
    ~KisRandomSubAccessor();
    void sampledOldRawData(quint8* dst);

    void sampledRawData(quint8* dst);

    inline void moveTo(qreal x, qreal y) {
        m_currentPoint.setX(x); m_currentPoint.setY(y);
    }
    inline void moveTo(const QPointF& p) {
        m_currentPoint = p;
    }

private:
    KisPaintDeviceSP m_device;
    QPointF m_currentPoint;
    KisRandomConstAccessorSP m_randomAccessor;
};

#endif
