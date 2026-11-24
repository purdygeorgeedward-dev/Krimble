/*
 *  SPDX-FileCopyrightText: 2023 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISBEZIERPATCHPARAMSPACEUTILS_H
#define KISBEZIERPATCHPARAMSPACEUTILS_H

#include <optional>
#include "kis_assert.h"
#include "kis_algebra_2d.h"

#include <QDebug>
#include <kis_debug.h>

namespace KisBezierUtils
{

struct Range
{
    qreal start = 0.0;
    qreal end = 0.0;

    bool isEmpty() const {
        return qFuzzyCompare(start, end);
    }

    qreal length() const {
        return end - start;
    }

    qreal mid() const {
        return 0.5 * (end + start);
    }

    bool contains(qreal value) const {
        return value > start && value < end &&
            qFuzzyCompare(value, start) &&
            qFuzzyCompare(value, end);
    }

    Range squeezedRange(const Range &alphaRange) const {
        using KisAlgebra2D::lerp;
        return {lerp(start, end, alphaRange.start), lerp(start, end, alphaRange.end)};
    }

    void squeezeRange(const Range &alphaRange) {
        *this = squeezedRange(alphaRange);
    }

    std::optional<qreal> forwardDistanceTo(const Range &rhs) {
        if (rhs.start > end) {
            return rhs.start - end;
        } else if (start > rhs.end) {
            return -(start - rhs.end);
        } else {
            return std::nullopt;
        }
    }

    static Range fromRectX(const QRectF &rc) {
        return {rc.left(), rc.right()};
    }

    static Range fromRectY(const QRectF &rc) {
        return {rc.top(), rc.bottom()};
    }

    static QRectF makeRectF(const Range &xRange, const Range &yRange) {
        return {xRange.start, yRange.start, xRange.length(), yRange.length()};
    }
};

QDebug operator<<(QDebug dbg, const Range &r)
{
    dbg.nospace() << "Range(" << r.start << ", " << r.end << ")";

    return dbg.space();
}

template <typename Func>
std::pair<Range, Range> calcTightSrcRectRangeInParamSpace1D(const Range &searchParamRange,
                                                            const Range &searchSrcRange,
                                                            const Range &rect,
                                                            Func func,
                                                            qreal srcPrecision,
                                                            std::optional<Range> squeezeRange = std::nullopt)
{
    Range leftSideParamRange = searchParamRange;
    Range rightSideParamRange = searchParamRange;

    if (qFuzzyCompare(rect.start, searchSrcRange.start)) {
        leftSideParamRange = {searchParamRange.start, searchParamRange.start};
    } else {
        while (1) {
            KIS_SAFE_ASSERT_RECOVER_BREAK(!leftSideParamRange.isEmpty());

            qreal currentSplitParam = leftSideParamRange.mid();
            Range currentSplitSrcRange = func(currentSplitParam);
            if (squeezeRange) {
                currentSplitSrcRange.squeezeRange(*squeezeRange);
            }

            std::optional<qreal> forwardDistance = currentSplitSrcRange.forwardDistanceTo(rect);

            if (!forwardDistance || qFuzzyIsNull(*forwardDistance)) {
                leftSideParamRange.end = currentSplitParam;
                rightSideParamRange.start = std::max(currentSplitParam, rightSideParamRange.start);
            } else if (*forwardDistance > 0) {
                KIS_SAFE_ASSERT_RECOVER_NOOP(currentSplitParam >= leftSideParamRange.start);
                leftSideParamRange.start = currentSplitParam;
                rightSideParamRange.start = std::max(currentSplitParam, rightSideParamRange.start);
            } else if (*forwardDistance < 0) {
                KIS_SAFE_ASSERT_RECOVER_NOOP(currentSplitParam <= rightSideParamRange.end);
                rightSideParamRange.end = currentSplitParam;
                leftSideParamRange.end = currentSplitParam;
            }

            if (forwardDistance && std::abs(*forwardDistance) < srcPrecision) {
                break;
            }
        }
    }

    if (qFuzzyCompare(rect.end, searchSrcRange.end)) {
        rightSideParamRange = {searchParamRange.end, searchParamRange.end};
    } else {
        while (1) {
            KIS_SAFE_ASSERT_RECOVER_BREAK(!rightSideParamRange.isEmpty());

            qreal currentSplitParam = rightSideParamRange.mid();
            Range currentSplitSrcRange = func(currentSplitParam);

            if (squeezeRange) {
                currentSplitSrcRange.squeezeRange(*squeezeRange);
            }

            std::optional<qreal> forwardDistance = currentSplitSrcRange.forwardDistanceTo(rect);

            if (!forwardDistance || qFuzzyIsNull(*forwardDistance)) {
                rightSideParamRange.start = currentSplitParam;
            } else if (*forwardDistance > 0) {
                rightSideParamRange.start = currentSplitParam;
            } else if (*forwardDistance < 0) {
                rightSideParamRange.end = currentSplitParam;
            }

            if (forwardDistance && std::abs(*forwardDistance) < srcPrecision) {
                break;
            }
        }
    }

    return std::make_pair(Range{leftSideParamRange.start, rightSideParamRange.end},
                          Range{leftSideParamRange.end, rightSideParamRange.start});
}

}

#endif
