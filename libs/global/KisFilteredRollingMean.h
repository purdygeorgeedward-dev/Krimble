/*
 *  SPDX-FileCopyrightText: 2022 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISFILTEREDROLLINGMEAN_H
#define KISFILTEREDROLLINGMEAN_H

#include "kritaglobal_export.h"

#include <vector>
#include <boost/circular_buffer.hpp>
#include <QtGlobal>

class KRITAGLOBAL_EXPORT KisFilteredRollingMean
{
public:
    KisFilteredRollingMean(int windowSize, qreal effectivePortion);

    void addValue(qreal value);

    qreal filteredMean() const;

    bool isEmpty() const;

private:
    boost::circular_buffer<qreal> m_values;
    qreal m_rollingSum;
    qreal m_effectivePortion;
    mutable std::vector<qreal> m_cutOffBuffer;
};

#endif
