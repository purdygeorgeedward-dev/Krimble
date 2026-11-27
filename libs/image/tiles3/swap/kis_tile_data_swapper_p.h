/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_TILE_DATA_SWAPPER_P_H_
#define KIS_TILE_DATA_SWAPPER_P_H_

#include "kis_image_config.h"
#include "tiles3/kis_tile_data.h"

class KisStoreLimits
{
public:
    KisStoreLimits() {
        KisImageConfig config(true);

        m_emergencyThreshold = MiB_TO_METRIC(config.tilesHardLimit());

        m_hardLimitThreshold = m_emergencyThreshold - (m_emergencyThreshold / 8);
        m_hardLimit = m_hardLimitThreshold - (m_hardLimitThreshold / 8);

        m_softLimitThreshold = qBound(0, MiB_TO_METRIC(config.tilesSoftLimit()), m_hardLimitThreshold);
        m_softLimit = m_softLimitThreshold - m_softLimitThreshold / 8;
    }

    inline qint32 emergencyThreshold() {
        return m_emergencyThreshold;
    }

    inline qint32 hardLimitThreshold() {
        return m_hardLimitThreshold;
    }

    inline qint32 hardLimit() {
        return m_hardLimit;
    }

    inline qint32 softLimitThreshold() {
        return m_softLimitThreshold;
    }

    inline qint32 softLimit() {
        return m_softLimit;
    }

private:
    qint32 m_emergencyThreshold;
    qint32 m_hardLimitThreshold;
    qint32 m_hardLimit;
    qint32 m_softLimitThreshold;
    qint32 m_softLimit;
};

#endif

