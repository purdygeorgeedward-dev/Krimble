/*
 *  SPDX-FileCopyrightText: 2005 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_convolution_painter.h"

#include <stdlib.h>
#include <string.h>
#include <cfloat>

#include <QBrush>
#include <QColor>
#include <QPen>
#include <QMap>
#include <QPainter>
#include <QRect>

#include <kis_debug.h>
#include <klocalizedstring.h>

#include "kis_convolution_kernel.h"
#include "kis_global.h"
#include "kis_image.h"
#include "kis_layer.h"
#include "kis_paint_device.h"
#include "kis_painter.h"
#include "KoColorSpace.h"
#include <KoChannelInfo.h>
#include "kis_types.h"
#include "kis_default_bounds.h"

#include "kis_selection.h"

#include "kis_convolution_worker.h"
#include "kis_convolution_worker_spatial.h"

#include "config_convolution.h"

#ifdef HAVE_FFTW3
#include "kis_convolution_worker_fft.h"
#endif

bool KisConvolutionPainter::useFFTImplementation(const KisConvolutionKernelSP kernel) const
{
    bool result = false;

#ifdef HAVE_FFTW3
    #define THRESHOLD_SIZE 5

    result =
        m_enginePreference == FFTW ||
        (m_enginePreference == NONE &&
         (kernel->width() > THRESHOLD_SIZE ||
          kernel->height() > THRESHOLD_SIZE));
#else
    Q_UNUSED(kernel);
#endif

    return result;
}

template<class factory>
KisConvolutionWorker<factory>* KisConvolutionPainter::createWorker(const KisConvolutionKernelSP kernel,
                                                                   KisPainter *painter,
                                                                   KoUpdater *progress)
{
    KisConvolutionWorker<factory> *worker;

#ifdef HAVE_FFTW3
    if (useFFTImplementation(kernel)) {
        worker = new KisConvolutionWorkerFFT<factory>(painter, progress);
    } else {
        worker = new KisConvolutionWorkerSpatial<factory>(painter, progress);
    }
#else
    Q_UNUSED(kernel);
    worker = new KisConvolutionWorkerSpatial<factory>(painter, progress);
#endif

    return worker;
}

bool KisConvolutionPainter::supportsFFTW()
{
#ifdef HAVE_FFTW3
    return true;
#else
    return false;
#endif
}

KisConvolutionPainter::KisConvolutionPainter()
    : KisPainter(),
      m_enginePreference(NONE)
{
}

KisConvolutionPainter::KisConvolutionPainter(KisPaintDeviceSP device)
    : KisPainter(device),
      m_enginePreference(NONE)
{
}

KisConvolutionPainter::KisConvolutionPainter(KisPaintDeviceSP device, KisSelectionSP selection)
    : KisPainter(device, selection),
      m_enginePreference(NONE)
{
}

KisConvolutionPainter::KisConvolutionPainter(KisPaintDeviceSP device, EnginePreference enginePreference)
    : KisPainter(device),
      m_enginePreference(enginePreference)
{
}

void KisConvolutionPainter::setEnginePreference(EnginePreference value)
{
    m_enginePreference = value;
}

void KisConvolutionPainter::applyMatrix(const KisConvolutionKernelSP kernel, const KisPaintDeviceSP src, QPoint srcPos, QPoint dstPos, QSize areaSize, KisConvolutionBorderOp borderOp)
{
    if (src->defaultBounds()->wrapAroundMode() && src->supportsWraproundMode()) {
        borderOp = BORDER_IGNORE;
    }

    switch (borderOp) {
    case BORDER_REPEAT: {
        const QRect boundsRect = src->defaultBounds()->bounds();
        const QRect requestedRect = QRect(srcPos, areaSize);
        QRect dataRect = requestedRect | boundsRect;

        KIS_SAFE_ASSERT_RECOVER(boundsRect != KisDefaultBounds().bounds()) {
            dataRect = requestedRect | src->exactBounds();
        }

        if(dataRect.isValid()) {
            KisConvolutionWorker<RepeatIteratorFactory> *worker;
            worker = createWorker<RepeatIteratorFactory>(kernel, this, progressUpdater());
            worker->execute(kernel, src, srcPos, dstPos, areaSize, dataRect);
            delete worker;
        }
        break;
    }
    case BORDER_IGNORE:
    default: {
        KisConvolutionWorker<StandardIteratorFactory> *worker;
        worker = createWorker<StandardIteratorFactory>(kernel, this, progressUpdater());
        worker->execute(kernel, src, srcPos, dstPos, areaSize, QRect());
        delete worker;
    }
    }
}

bool KisConvolutionPainter::needsTransaction(const KisConvolutionKernelSP kernel) const
{
    return !useFFTImplementation(kernel);
}
