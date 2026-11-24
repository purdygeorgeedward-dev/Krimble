/*
 * KDE. Krita Project.
 *
 * SPDX-FileCopyrightText: 2020 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISGRADIENTCONVERSION_H
#define KISGRADIENTCONVERSION_H

#include <QGradient>

#include <KoAbstractGradient.h>
#include <KoStopGradient.h>
#include <KoSegmentGradient.h>
#include <KoCanvasResourcesInterface.h>
#include "kritapigment_export.h"

namespace KisGradientConversion
{
    KRITAPIGMENT_EXPORT QGradientStops toQGradientStops(KoAbstractGradientSP gradient,
                                                        KoCanvasResourcesInterfaceSP canvasResourcesInterface = nullptr);

    KRITAPIGMENT_EXPORT QGradientStops toQGradientStops(KoStopGradientSP gradient,
                                                        KoCanvasResourcesInterfaceSP canvasResourcesInterface = nullptr);

    KRITAPIGMENT_EXPORT QGradientStops toQGradientStops(KoSegmentGradientSP gradient,
                                                        KoCanvasResourcesInterfaceSP canvasResourcesInterface = nullptr);

    KRITAPIGMENT_EXPORT QGradient* toQGradient(KoAbstractGradientSP gradient,
                                               KoCanvasResourcesInterfaceSP canvasResourcesInterface = nullptr);

    KRITAPIGMENT_EXPORT QGradient* toQGradient(KoStopGradientSP gradient,
                                               KoCanvasResourcesInterfaceSP canvasResourcesInterface = nullptr);

    KRITAPIGMENT_EXPORT QGradient* toQGradient(KoSegmentGradientSP gradient,
                                               KoCanvasResourcesInterfaceSP canvasResourcesInterface = nullptr);

    KRITAPIGMENT_EXPORT KoAbstractGradientSP toAbstractGradient(const QGradientStops &gradient);

    KRITAPIGMENT_EXPORT KoAbstractGradientSP toAbstractGradient(const QGradient *gradient);

    KRITAPIGMENT_EXPORT KoAbstractGradientSP toAbstractGradient(KoStopGradientSP gradient);

    KRITAPIGMENT_EXPORT KoAbstractGradientSP toAbstractGradient(KoSegmentGradientSP gradient);

    KRITAPIGMENT_EXPORT KoStopGradientSP toStopGradient(const QGradientStops &gradient);

    KRITAPIGMENT_EXPORT KoStopGradientSP toStopGradient(const QGradient *gradient);

    KRITAPIGMENT_EXPORT KoStopGradientSP toStopGradient(KoAbstractGradientSP gradient,
                                                        KoCanvasResourcesInterfaceSP canvasResourcesInterface = nullptr);

    KRITAPIGMENT_EXPORT KoStopGradientSP toStopGradient(KoSegmentGradientSP gradient,
                                                        KoCanvasResourcesInterfaceSP canvasResourcesInterface = nullptr);

    KRITAPIGMENT_EXPORT KoSegmentGradientSP toSegmentGradient(const QGradientStops &gradient);

    KRITAPIGMENT_EXPORT KoSegmentGradientSP toSegmentGradient(const QGradient *gradient);

    KRITAPIGMENT_EXPORT KoSegmentGradientSP toSegmentGradient(KoAbstractGradientSP gradient);

    KRITAPIGMENT_EXPORT KoSegmentGradientSP toSegmentGradient(KoStopGradientSP gradient);
}

#endif
