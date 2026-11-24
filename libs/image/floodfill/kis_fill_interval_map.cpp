/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_fill_interval_map.h"
#include "kis_fill_interval_map_p.h"

#include "kis_fill_sanity_checks.h"

KisFillIntervalMap::KisFillIntervalMap()
    : m_d(new Private)
{
}

KisFillIntervalMap::~KisFillIntervalMap()
{
}

void KisFillIntervalMap::insertInterval(const KisFillInterval &interval)
{
    Private::GlobalMap::iterator rowMap = m_d->map.find(interval.row);
    if (rowMap == m_d->map.end()) {
        rowMap = m_d->map.insert(interval.row, Private::LineIntervalMap());
    }

    rowMap->insert(interval.start, interval);
}

void KisFillIntervalMap::cropInterval(KisFillInterval *interval)
{
    Private::IteratorRange range;
    range = m_d->findFirstIntersectingInterval(*interval);

    Private::LineIntervalMap::iterator it = range.beginIt;

    while (interval->isValid() && it != range.endIt) {
        bool needsIncrement = true;

        if (it->start <= interval->start && it->end >= interval->start) {
            int savedIntervalStart = interval->start;
            interval->start = it->end + 1;

            if (it->end > interval->end) {
                KisFillInterval newInterval(interval->end + 1, it->end, it->row);
                range.rowMapIt->insert(newInterval.start, newInterval);
            }

            it->end = savedIntervalStart - 1;

            if (!it->isValid()) {
                it = range.rowMapIt->erase(it);
                needsIncrement = false;
            }
        } else if (it->start <= interval->end && it->end >= interval->end) {
            int savedIntervalEnd = interval->end;
            interval->end = it->start - 1;
            it->start = savedIntervalEnd + 1;

            if (!it->isValid()) {
                it = range.rowMapIt->erase(it);
                needsIncrement = false;
            }
        } else if (it->start > interval->end) {
            break;
        }

#ifdef ENABLE_FILL_SANITY_CHECKS
        else if (it->start > interval->start && it->end < interval->end) {
            SANITY_ASSERT_MSG(0, "FATAL: The backward interval cannot fully reside inside the forward interval");
            interval->invalidate();
            it->invalidate();
            it = range.rowMapIt->erase(it);
            needsIncrement = false;
        }

        KIS_SAFE_ASSERT_RECOVER((it == range.endIt || it->isValid()) &&
                                "FATAL: The backward interval cannot become "
                                "invalid during the crop action") {
            it = range.rowMapIt->erase(it);
            needsIncrement = false;
        }
#endif

        if (needsIncrement) {
            it++;
        }
    }
}

KisFillIntervalMap::Private::IteratorRange
KisFillIntervalMap::Private::findFirstIntersectingInterval(const KisFillInterval &interval)
{
    Private::GlobalMap::iterator rowMap = map.find(interval.row);
    if (rowMap == map.end()) {
        return IteratorRange();
    }

    LineIntervalMap::iterator it = rowMap->begin();
    LineIntervalMap::iterator end = rowMap->end();

    while(it != end) {
        if (it->end < interval.start) {
            ++it;
        } else if (it->start > interval.end) {
            it = end;
            break;
        } else {
            break;
        }
    }

    return IteratorRange(it, end, rowMap);
}

QStack<KisFillInterval> KisFillIntervalMap::fetchAllIntervals(int rowCorrection) const
{
    QStack<KisFillInterval> intervals;

    Private::GlobalMap::const_iterator rowMapIt = m_d->map.constBegin();
    Private::GlobalMap::const_iterator rowMapEndIt = m_d->map.constEnd();

    while (rowMapIt != rowMapEndIt) {
        Private::LineIntervalMap::const_iterator it = rowMapIt->constBegin();
        Private::LineIntervalMap::const_iterator end = rowMapIt->constEnd();

        while(it != end) {
            KisFillInterval interval = *it;
            interval.row += rowCorrection;
            intervals.append(interval);
            ++it;
        }

        ++rowMapIt;
    }

    return intervals;
}

void KisFillIntervalMap::clear()
{
    m_d->map.clear();
}
