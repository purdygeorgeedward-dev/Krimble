/*
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2007 Emanuele Tamponi <emanuele@valinor.it>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KO_CONVOLUTION_OP_IMPL_H
#define KO_CONVOLUTION_OP_IMPL_H

#include "DebugPigment.h"
#include "KoColorSpaceMaths.h"
#include "KoConvolutionOp.h"
#include "KoColorSpaceTraits.h"

template<class _CSTrait>
class KoConvolutionOpImpl : public KoConvolutionOp
{
    typedef typename KoColorSpaceMathsTraits<typename _CSTrait::channels_type>::compositetype compositetype;
    typedef typename _CSTrait::channels_type channels_type;
public:

    KoConvolutionOpImpl() { }

    ~KoConvolutionOpImpl() override { }

    void convolveColors(const quint8* const* colors, const qreal* kernelValues, quint8 *dst, qreal factor, qreal offset, qint32 nPixels, const QBitArray & channelFlags) const override {

        qreal totals[_CSTrait::channels_nb];

        qreal totalWeight = 0;
        qreal totalWeightTransparent = 0;

        memset(totals, 0, sizeof(qreal) * _CSTrait::channels_nb);

        for (; nPixels--; colors++, kernelValues++) {
            qreal weight = *kernelValues;
            const channels_type* color = _CSTrait::nativeArray(*colors);
            if (weight != 0) {
                if (_CSTrait::opacityU8(*colors) == 0) {
                    totalWeightTransparent += weight;
                } else {
                    for (uint i = 0; i < _CSTrait::channels_nb; i++) {
                        totals[i] += color[i] * weight;
                    }
                }
                totalWeight += weight;
            }
        }

        typename _CSTrait::channels_type* dstColor = _CSTrait::nativeArray(dst);

        bool allChannels = channelFlags.isEmpty();
        Q_ASSERT(allChannels || channelFlags.size() == (int)_CSTrait::channels_nb);
        if (totalWeightTransparent == 0) {
            for (uint i = 0; i < _CSTrait::channels_nb; i++) {
                if (allChannels || channelFlags.testBit(i)) {
                    compositetype v = totals[i] / factor + offset;
                    dstColor[ i ] = CLAMP(v, KoColorSpaceMathsTraits<channels_type>::min,
                                          KoColorSpaceMathsTraits<channels_type>::max);
                }
            }
        } else if (totalWeightTransparent != totalWeight) {
            if (totalWeight == factor) {
                qint64 a = (totalWeight - totalWeightTransparent);
                for (uint i = 0; i < _CSTrait::channels_nb; i++) {
                    if (allChannels || channelFlags.testBit(i)) {
                        if (i == (uint)_CSTrait::alpha_pos) {
                            compositetype v = totals[i] / totalWeight + offset;
                            dstColor[ i ] = CLAMP(v, KoColorSpaceMathsTraits<channels_type>::min,
                                                  KoColorSpaceMathsTraits<channels_type>::max);
                        } else {
                            compositetype v = totals[i] / a + offset;
                            dstColor[ i ] = CLAMP(v, KoColorSpaceMathsTraits<channels_type>::min,
                                                  KoColorSpaceMathsTraits<channels_type>::max);
                        }
                    }
                }
            } else {
                qreal a = qreal(totalWeight) / (factor * (totalWeight - totalWeightTransparent));
                for (uint i = 0; i < _CSTrait::channels_nb; i++) {
                    if (allChannels || channelFlags.testBit(i)) {
                        if (i == (uint)_CSTrait::alpha_pos) {
                            compositetype v = totals[i] / factor + offset;
                            dstColor[ i ] = CLAMP(v, KoColorSpaceMathsTraits<channels_type>::min,
                                                  KoColorSpaceMathsTraits<channels_type>::max);
                        } else {
                            compositetype v = (compositetype)(totals[i] * a + offset);
                            dstColor[ i ] = CLAMP(v, KoColorSpaceMathsTraits<channels_type>::min,
                                                  KoColorSpaceMathsTraits<channels_type>::max);
                        }
                    }
                }
            }
        }

    }
};

#endif
