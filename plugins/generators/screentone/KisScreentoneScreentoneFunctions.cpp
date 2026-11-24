/*
 * KDE. Krita Project.
 *
 * SPDX-FileCopyrightText: 2020 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisScreentoneScreentoneFunctions.h"

#include <cmath>

namespace KisScreentoneScreentoneFunctions {

qreal sin(qreal x)
{
    x = std::cos(x * M_PI);
    return  x * x;
}

qreal triangle(qreal x)
{
    return 1.0 - 2.0 * std::abs(x - std::floor(x + 0.5));
}

qreal sawTooth(qreal x)
{
    constexpr qreal peakXOffset = 0.9;
    constexpr qreal peakYOffset = 0.5;
    x = x - std::floor(x);
    return (x < peakXOffset ? 1.0 / peakXOffset * x : -1.0 / (1.0 - peakXOffset) * (x - 1.0)) * peakYOffset;
}

qreal DotsRoundLinear::operator()(qreal x, qreal y) const
{
    x = triangle(x);
    y = triangle(y);
    return std::sqrt(x * x + y * y) / M_SQRT2;
}

qreal DotsRoundLinearEqualized::operator()(qreal x, qreal y) const
{
    const qreal z = DotsRoundLinear::operator()(x, y);
    const qreal zOverSqrt2 = z / M_SQRT2;
    const qreal zOverSqrt2Squared = zOverSqrt2 * zOverSqrt2;
    if (z <= M_SQRT2 / 2.0) {
        return M_PI * zOverSqrt2Squared;
    } else {
        return M_PI * zOverSqrt2Squared -
               4.0 * (zOverSqrt2Squared * std::acos(M_SQRT2 / (2.0 * z)) -
                      0.5 * std::sqrt(zOverSqrt2Squared - 0.25));
    }
}

qreal DotsRoundSinusoidal::operator()(qreal x, qreal y) const
{
    return (sin(x) + sin(y)) / 2.;
}

qreal DotsRoundSinusoidalEqualized::operator()(qreal x, qreal y) const
{
    const qreal z = DotsRoundSinusoidal::operator()(x, y);
    if (z <= 0.5) {
        return M_SQRT2 / 2.0 - std::sqrt(-(z - 0.5469) / 1.0938);
    } else {
        return (1.0 - M_SQRT2 / 2.0) + std::sqrt((z - (1.0 - 0.5469)) / 1.0938);
    }
}

qreal DotsEllipseLinear::operator()(qreal x, qreal y) const
{
    constexpr qreal aspectRatio = 1.25;
    constexpr qreal factor = 0.625;
    x = triangle(x);
    y = triangle(y) * aspectRatio;
    return std::sqrt(x * x + y * y) * factor;
}

qreal DotsEllipseLinearEqualized::operator()(qreal x, qreal y) const
{
    const qreal z = DotsEllipseLinear::operator()(x, y);
    constexpr qreal factor = 0.625;
    constexpr qreal factorTimes2 = factor * 2.0;
    const qreal zOverFactorTimes2 = z / factorTimes2;
    const qreal zTimesPoint8OverFactorTimes2 = 0.8 * zOverFactorTimes2;
    qreal result = M_PI * zOverFactorTimes2 * zTimesPoint8OverFactorTimes2;
    if (z > 0.625) {
        const qreal zOverFactorTimes2Squared = zOverFactorTimes2 * zOverFactorTimes2;
        result -= 2.0 * (zOverFactorTimes2Squared * std::acos(factor / z) -
                         0.5 * std::sqrt(zOverFactorTimes2Squared - 0.25)) * 0.8;
    }
    if (z > 0.78125) {
        const qreal zTimesPoint8OverFactorTimes2Squared =
            zTimesPoint8OverFactorTimes2 * zTimesPoint8OverFactorTimes2;
        result -= 2.0 * (zTimesPoint8OverFactorTimes2Squared * std::acos(factor / (0.8 * z)) -
                         0.5 * std::sqrt(zTimesPoint8OverFactorTimes2Squared - 0.25)) / 0.8;
    }
    return result;
}

qreal DotsEllipseSinusoidal::operator()(qreal x, qreal y) const
{
    x = sin(x) * 0.4;
    y = sin(y) * 0.6;
    return (x + y);
}

qreal DotsEllipseSinusoidalEqualized::operator()(qreal x, qreal y) const
{
    const qreal z = DotsEllipseSinusoidal::operator()(x, y);
    const qreal z2 = z * z;
    const qreal z3 = z * z2;
    if (z <= 0.3) {
        return 0.8795 * z3 + 0.1825 * z2 + 0.6649 * z + 0.0008;
    } else if (z <= 0.4) {
        return 32.0507 * z3 - 30.3781 * z2 + 10.6756 * z - 1.0937;
    } else if (z <= 0.5) {
        return 27.8089 * z3 - 39.4726 * z2 + 19.8992 * z - 3.0553;
    } else if (z <= 0.6) {
        return 35.1490 * z3 - 55.6810 * z2 + 30.6244 * z - 5.2839;
    } else if (z <= 0.7) {
        return 24.3210 * z3 - 50.1381 * z2 + 35.6452 * z - 7.9322;
    } else {
        return 0.7457 * z3 - 2.4792 * z2 + 3.3748 * z - 0.6402;
    }
}

qreal DotsEllipseLinear_Legacy::operator()(qreal x, qreal y) const
{
    constexpr qreal ellipseRatioX = 0.4 / M_SQRT2;
    constexpr qreal ellipseRatioY = 0.6 / M_SQRT2;
    x = triangle(x) * ellipseRatioX;
    y = triangle(y) * ellipseRatioY;
    return std::sqrt(x * x + y * y) * M_SQRT2;
}

qreal DotsDiamond::operator()(qreal x, qreal y) const
{
    return (triangle(x) + triangle(y)) / 2.;
}

qreal DotsDiamondEqualized::operator()(qreal x, qreal y) const
{
    const qreal z = DotsDiamond::operator()(x, y);
    if (z <= 0.5) {
        return 2.0 * z * z;
    } else {
        return -2.0 * z * z + 4.0 * z - 1.0;
    }
}

qreal DotsSquare::operator()(qreal x, qreal y) const
{
    return std::max(triangle(x), triangle(y));
}

qreal DotsSquareEqualized::operator()(qreal x, qreal y) const
{
    const qreal z = DotsSquare::operator()(x, y);
    return z * z;
}

qreal LinesStraightLinear::operator()(qreal x, qreal y) const
{
    Q_UNUSED(x);
    return triangle(y);
}

qreal LinesStraightSinusoidal::operator()(qreal x, qreal y) const
{
    Q_UNUSED(x);
    return sin(y);
}

qreal LinesSineWaveLinear::operator()(qreal x, qreal y) const
{
    return triangle(y + sin(x));
}

qreal LinesSineWaveSinusoidal::operator()(qreal x, qreal y) const
{
    return sin(y + sin(x));
}

qreal LinesTriangularWaveLinear::operator()(qreal x, qreal y) const
{
    return triangle(y + triangle(x));
}

qreal LinesTriangularWaveSinusoidal::operator()(qreal x, qreal y) const
{
    return sin(y + triangle(x));
}

qreal LinesSawToothWaveLinear::operator()(qreal x, qreal y) const
{
    return triangle(y + sawTooth(x));
}

qreal LinesSawToothWaveSinusoidal::operator()(qreal x, qreal y) const
{
    return sin(y + sawTooth(x));
}

qreal LinesCurtainsLinear::operator()(qreal x, qreal y) const
{
    x = triangle(x);
    return triangle(y + x * x);
}

qreal LinesCurtainsSinusoidal::operator()(qreal x, qreal y) const
{
    x = triangle(x);
    return sin(y + x * x);
}

}
