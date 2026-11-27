/*
 *  SPDX-FileCopyrightText: 2021 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KoOptimizedPixelDataScalerU8ToU16Base_H
#define KoOptimizedPixelDataScalerU8ToU16Base_H

#include <QtGlobal>
#include "kritapigment_export.h"

class KRITAPIGMENT_EXPORT KoOptimizedPixelDataScalerU8ToU16Base
{
public:
    KoOptimizedPixelDataScalerU8ToU16Base(int channelsPerPixel);

    virtual ~KoOptimizedPixelDataScalerU8ToU16Base();

    virtual void convertU8ToU16(const quint8 *src, int srcRowStride,
                                quint8 *dst, int dstRowStride,
                                int numRows, int numColumns) const = 0;

    virtual void convertU16ToU8(const quint8 *src, int srcRowStride,
                                quint8 *dst, int dstRowStride,
                                int numRows, int numColumns) const = 0;

    int channelsPerPixel() const;

protected:
    int m_channelsPerPixel;
};

#endif
