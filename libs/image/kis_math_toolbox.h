/*
 *  This file is part of the KDE project
 *
 *  SPDX-FileCopyrightText: 2005 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_MATH_TOOLBOX_H
#define KIS_MATH_TOOLBOX_H

#include <QObject>
#include <QRect>

#include <new>

#include <KoColorSpace.h>

#include "kis_types.h"
#include "kis_paint_device.h"

#ifdef _MSC_VER
#pragma warning(disable: 4290)
#endif

#if !defined _MSC_VER
#pragma GCC diagnostic ignored "-Wcast-align"
#endif

typedef double(*PtrToDouble)(const quint8*, int);
typedef void (*PtrFromDouble)(quint8*, int, double);
typedef void (*PtrFromDoubleCheckNull)(quint8*, int, double, bool*);

class KRITAIMAGE_EXPORT KisMathToolbox
{

public:

    struct KisFloatRepresentation {

        KisFloatRepresentation(uint nsize, uint ndepth)
            : coeffs(new float[nsize*nsize*ndepth])
            , size(nsize)
            , depth(ndepth) {
            for (quint32 i = 0; i < nsize * nsize * ndepth; ++i) {
                coeffs[i] = 0;
            }
        }

        ~KisFloatRepresentation() {
            if (coeffs) delete[] coeffs;
        }

        float* coeffs;
        uint size;
        uint depth;
    };

    typedef KisFloatRepresentation KisWavelet;

    inline KisWavelet* initWavelet(KisPaintDeviceSP lay, const QRect&);

    inline uint fastWaveletTotalSteps(const QRect&);

    KisWavelet* fastWaveletTransformation(KisPaintDeviceSP src, const QRect&, KisWavelet* buff = 0);

    void fastWaveletUntransformation(KisPaintDeviceSP dst, const QRect&, KisWavelet* wav, KisWavelet* buff = 0);

    bool getToDoubleChannelPtr(QList<KoChannelInfo *> cis, QVector<PtrToDouble>& f);
    bool getFromDoubleChannelPtr(QList<KoChannelInfo *> cis, QVector<PtrFromDouble>& f);
    bool getFromDoubleCheckNullChannelPtr(QList<KoChannelInfo *> cis, QVector<PtrFromDoubleCheckNull>& f);

    double minChannelValue(KoChannelInfo *);
    double maxChannelValue(KoChannelInfo *);

private:

    void wavetrans(KisWavelet* wav, KisWavelet* buff, uint halfsize);
    void waveuntrans(KisWavelet* wav, KisWavelet* buff, uint halfsize);

    void transformToFR(KisPaintDeviceSP src, KisFloatRepresentation*, const QRect&);

    void transformFromFR(KisPaintDeviceSP dst, KisFloatRepresentation*, const QRect&);

};

inline KisMathToolbox::KisWavelet* KisMathToolbox::initWavelet(KisPaintDeviceSP src, const QRect& rect)
{
    int size;
    int maxrectsize = (rect.height() < rect.width()) ? rect.width() : rect.height();
    for (size = 2; size < maxrectsize; size *= 2) ;
    qint32 depth = src->colorSpace()->colorChannelCount();
    return new KisWavelet(size, depth);
}

inline uint KisMathToolbox::fastWaveletTotalSteps(const QRect& rect)
{
    int size, steps;
    int maxrectsize = (rect.height() < rect.width()) ? rect.width() : rect.height();
    steps = 0;
    for (size = 2; size < maxrectsize; size *= 2) steps += size / 2; ;
    return steps;
}

#endif
