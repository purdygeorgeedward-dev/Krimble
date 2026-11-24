/*
 *  SPDX-FileCopyrightText: 2005 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_CONVOLUTION_PAINTER_H_
#define KIS_CONVOLUTION_PAINTER_H_

#include "kis_types.h"
#include "kis_painter.h"
#include "kis_image.h"
#include "kritaimage_export.h"

template<class factory> class KisConvolutionWorker;

enum KisConvolutionBorderOp {
    BORDER_IGNORE = 0,
    BORDER_REPEAT = 1
};

class KRITAIMAGE_EXPORT KisConvolutionPainter : public KisPainter
{

public:

    KisConvolutionPainter();
    KisConvolutionPainter(KisPaintDeviceSP device);
    KisConvolutionPainter(KisPaintDeviceSP device, KisSelectionSP selection);

    enum EnginePreference {
        NONE,
        SPATIAL,
        FFTW
    };

    KisConvolutionPainter(KisPaintDeviceSP device, EnginePreference enginePreference);

    void setEnginePreference(EnginePreference value);

    void applyMatrix(const KisConvolutionKernelSP kernel, const KisPaintDeviceSP src, QPoint srcPos, QPoint dstPos, QSize areaSize,
                     KisConvolutionBorderOp borderOp = BORDER_REPEAT);

    bool needsTransaction(const KisConvolutionKernelSP kernel) const;

    static bool supportsFFTW();

protected:
    friend class KisConvolutionPainterTest;

private:
    template<class factory>
        KisConvolutionWorker<factory>* createWorker(const KisConvolutionKernelSP kernel,
                                                    KisPainter *painter,
                                                    KoUpdater *progress);

     bool useFFTImplementation(const KisConvolutionKernelSP kernel) const;

private:
    EnginePreference m_enginePreference;
};
#endif
