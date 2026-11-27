/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_EDGE_DETECTION_KERNEL_H
#define KIS_EDGE_DETECTION_KERNEL_H

#include "kritaimage_export.h"
#include "kis_types.h"

#include <Eigen/Core>
#include <boost/optional.hpp>

class QRect;

class KRITAIMAGE_EXPORT KisEdgeDetectionKernel
{
public:
    KisEdgeDetectionKernel();

    enum FilterType {
        Simple,
        Prewitt,
        SobelVector
    };

    enum FilterOutput {
        pythagorean,
        xGrowth,
        xFall,
        yGrowth,
        yFall,
        radian
    };

    static Eigen::Matrix<qreal, Eigen::Dynamic, Eigen::Dynamic>
        createHorizontalMatrix(qreal radius, FilterType type, bool reverse = false);
    static Eigen::Matrix<qreal, Eigen::Dynamic, Eigen::Dynamic>
        createVerticalMatrix(qreal radius, FilterType type, bool reverse = false);

    static KisConvolutionKernelSP
        createHorizontalKernel(qreal radius, FilterType type, bool denormalize = true, bool reverse = false);

    static KisConvolutionKernelSP
        createVerticalKernel(qreal radius, FilterType type, bool denormalize = true, bool reverse = false);

    static int kernelSizeFromRadius(qreal radius);
    static qreal sigmaFromRadius(qreal radius);

    static void applyEdgeDetection(KisPaintDeviceSP device,
                              const QRect& rect,
                              qreal xRadius, qreal yRadius,
                              FilterType type,
                              const QBitArray &channelFlags,
                              KoUpdater *progressUpdater,
                              FilterOutput output = pythagorean,
                              bool writeToAlpha = false);
    static void convertToNormalMap(KisPaintDeviceSP device,
                                  const QRect & rect,
                                  qreal xRadius,
                                  qreal yRadius,
                                  FilterType type,
                                  int channelToConvert,
                                  QVector<int> channelOrder,
                                  QVector<bool> channelFlip,
                                  const QBitArray &channelFlags,
                                  KoUpdater *progressUpdater,
                                  boost::optional<bool> useFftw = boost::none);
};

#endif
