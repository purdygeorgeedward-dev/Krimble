/*
 *  SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2006-2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KO_MIX_COLORS_OP_H
#define KO_MIX_COLORS_OP_H

#include <limits.h>

class KoMixColorsOp
{
public:

    class Mixer
    {
    public:
        virtual ~Mixer() {}
        virtual void accumulate(const quint8 *data, const qint16 *weights, int weightSum, int nPixels) = 0;

        virtual void accumulateAverage(const quint8 *data, int nPixels) = 0;

        virtual void computeMixedColor(quint8 *data) = 0;

        virtual qint64 currentWeightsSum() const = 0;
    };

    virtual Mixer* createMixer() const = 0;

public:
    virtual ~KoMixColorsOp() { }
    virtual void mixColors(const quint8 * const*colors, const qint16 *weights, int nColors, quint8 *dst, int weightSum = 255) const = 0;
    virtual void mixColors(const quint8 *colors, const qint16 *weights, int nColors, quint8 *dst, int weightSum = 255) const = 0;

    virtual void mixColors(const quint8 * const*colors, int nColors, quint8 *dst) const = 0;
    virtual void mixColors(const quint8 *colors, int nColors, quint8 *dst) const = 0;

    virtual void mixTwoColorArrays(const quint8* colorsA, const quint8* colorsB, int nColors, qreal weight, quint8* dst) const = 0;

    virtual void mixArrayWithColor(const quint8* colorArray, const quint8* color, int nColors, qreal weight, quint8* dst) const = 0;
};

#endif
