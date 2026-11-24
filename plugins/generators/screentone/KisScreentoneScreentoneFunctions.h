/*
 * KDE. Krita Project.
 *
 * SPDX-FileCopyrightText: 2020 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISSCREENTONESCREENTONEFUNCTIONS_H
#define KISSCREENTONESCREENTONEFUNCTIONS_H

#include <QtGlobal>

#include "KisScreentoneGeneratorTemplate.h"

namespace KisScreentoneScreentoneFunctions {

qreal sin(qreal x);
qreal triangle(qreal x);
qreal sawTooth(qreal x);

class DotsRoundLinear
{
public:
    qreal operator()(qreal x, qreal y) const;
};

class DotsRoundLinearEqualized : public DotsRoundLinear
{
public:
    qreal operator()(qreal x, qreal y) const;
};

class DotsRoundSinusoidal
{
public:
    qreal operator()(qreal x, qreal y) const;
};

class DotsRoundSinusoidalEqualized : public DotsRoundSinusoidal
{
public:
    qreal operator()(qreal x, qreal y) const;
};

class DotsEllipseLinear
{
public:
    qreal operator()(qreal x, qreal y) const;
};

class DotsEllipseLinearEqualized : public DotsEllipseLinear
{
public:
    qreal operator()(qreal x, qreal y) const;
};

class DotsEllipseSinusoidal
{
public:
    qreal operator()(qreal x, qreal y) const;
};

class DotsEllipseSinusoidalEqualized : public DotsEllipseSinusoidal
{
public:
    qreal operator()(qreal x, qreal y) const;
};

class DotsEllipseLinear_Legacy
{
public:
    qreal operator()(qreal x, qreal y) const;
};

using DotsEllipseLinearEqualized_Legacy = DotsEllipseLinear_Legacy;

using DotsEllipseSinusoidal_Legacy = DotsEllipseSinusoidal;

using DotsEllipseSinusoidalEqualized_Legacy = DotsEllipseSinusoidal;

class DotsDiamond
{
public:
    qreal operator()(qreal x, qreal y) const;
};

class DotsDiamondEqualized : public DotsDiamond
{
public:
    qreal operator()(qreal x, qreal y) const;
};

class DotsSquare
{
public:
    qreal operator()(qreal x, qreal y) const;
};

class DotsSquareEqualized : public DotsSquare
{
public:
    qreal operator()(qreal x, qreal y) const;
};

class LinesStraightLinear
{
public:
    qreal operator()(qreal x, qreal y) const;
};

using LinesStraightLinearEqualized = LinesStraightLinear;

class LinesStraightSinusoidal
{
public:
    qreal operator()(qreal x, qreal y) const;
};

using LinesStraightSinusoidalEqualized = LinesStraightLinear;

class LinesSineWaveLinear
{
public:
    qreal operator()(qreal x, qreal y) const;
};

using LinesSineWaveLinearEqualized = LinesSineWaveLinear;

class LinesSineWaveSinusoidal
{
public:
    qreal operator()(qreal x, qreal y) const;
};

using LinesSineWaveSinusoidalEqualized = LinesSineWaveLinear;

class LinesTriangularWaveLinear
{
public:
    qreal operator()(qreal x, qreal y) const;
};

using LinesTriangularWaveLinearEqualized = LinesTriangularWaveLinear;

class LinesTriangularWaveSinusoidal
{
public:
    qreal operator()(qreal x, qreal y) const;
};

using LinesTriangularWaveSinusoidalEqualized = LinesTriangularWaveLinear;

class LinesSawToothWaveLinear
{
public:
    qreal operator()(qreal x, qreal y) const;
};

using LinesSawToothWaveLinearEqualized = LinesSawToothWaveLinear;

class LinesSawToothWaveSinusoidal
{
public:
    qreal operator()(qreal x, qreal y) const;
};

using LinesSawToothWaveSinusoidalEqualized = LinesSawToothWaveLinear;

class LinesCurtainsLinear
{
public:
    qreal operator()(qreal x, qreal y) const;
};

using LinesCurtainsLinearEqualized = LinesCurtainsLinear;

class LinesCurtainsSinusoidal
{
public:
    qreal operator()(qreal x, qreal y) const;
};

using LinesCurtainsSinusoidalEqualized = LinesCurtainsLinear;

template <typename T>
class TemplateBasedFunction
{
public:
    TemplateBasedFunction(const T &the_template)
        : m_template(the_template)
    {}

    qreal operator()(qreal x, qreal y) const
    {
        return m_template(x, y);
    }

private:
    const T& m_template;
};

}

#endif