/*
 *  SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2006-2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KO_CONVOLUTION_OP_H
#define KO_CONVOLUTION_OP_H

class KoConvolutionOp
{
public:
    virtual ~KoConvolutionOp() { }
    virtual void convolveColors(const quint8* const* colors, const qreal* kernelValues, quint8 *dst, qreal factor, qreal offset, qint32 nColors, const QBitArray & channelFlags) const = 0;
};

#endif
