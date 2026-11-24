/*
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KIS_TIMING_INFORMATION_H
#define KIS_TIMING_INFORMATION_H

#include "kritaimage_export.h"

const qreal LONG_TIME = 320000000000.0;

class KRITAIMAGE_EXPORT KisTimingInformation
{
public:

    explicit KisTimingInformation()
        : m_timedSpacingEnabled(false)
        , m_timedSpacingInterval(LONG_TIME)
    {
    }

    explicit KisTimingInformation(qreal interval)
        : m_timedSpacingEnabled(true)
        , m_timedSpacingInterval(interval)
    {
    }

    inline bool isTimedSpacingEnabled() const {
        return m_timedSpacingEnabled;
    }

    inline qreal timedSpacingInterval() const {
        return isTimedSpacingEnabled() ?
                    m_timedSpacingInterval :
                    LONG_TIME;
    }

private:
    bool m_timedSpacingEnabled;
    qreal m_timedSpacingInterval;
};

#endif
