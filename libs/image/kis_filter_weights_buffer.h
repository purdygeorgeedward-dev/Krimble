/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_FILTER_WEIGHTS_BUFFER_H
#define __KIS_FILTER_WEIGHTS_BUFFER_H

#include "kis_fixed_point_maths.h"
#include "kis_filter_strategy.h"
#include "kis_debug.h"

#ifdef SANITY_CHECKS_ENABLED
static bool checkForAsymmetricZeros = false;

#define SANITY_CENTER_POSITION()                        \
    do {                                                \
        Q_ASSERT(scaledIter >= beginDst);               \
        Q_ASSERT(scaledIter <= endDst);                 \
                                                        \
        if (j == centerIndex) {                         \
            Q_ASSERT(scaledIter == centerSrc);          \
        }                                               \
    } while(0)

#define SANITY_ZEROS()                                                  \
    do {                                                                \
        if (checkForAsymmetricZeros) {                                  \
            for (int j = 0; j < span; j++) {                            \
                int idx2 = span - j - 1;                                \
                                                                        \
                if ((m_filterWeights[i].weight[j] && !m_filterWeights[i].weight[idx2]) || \
                    (!m_filterWeights[i].weight[j] && m_filterWeights[i].weight[idx2])) { \
                                                                        \
                    dbgKrita << "*******";                              \
                    dbgKrita << "Non-symmetric zero found:" << centerSrc; \
                    dbgKrita << "Weight" << j << ":" << m_filterWeights[i].weight[j]; \
                    dbgKrita << "Weight" << idx2 << ":" << m_filterWeights[i].weight[idx2]; \
                    qFatal("Non-symmetric zero -> fail");               \
                }                                                       \
            }                                                           \
        }                                                               \
    } while (0)

#define SANITY_CHECKSUM()                       \
    do {                                        \
        Q_ASSERT(sum == 255);                   \
    } while (0)

#else
#define SANITY_CENTER_POSITION()
#define SANITY_ZEROS()
#define SANITY_CHECKSUM()
#endif

#ifdef DEBUG_ENABLED
#define DEBUG_ALL()                                                     \
    do {                                                                \
        dbgKrita << "************** i =" << i;                          \
        dbgKrita << ppVar(centerSrc);                                   \
        dbgKrita << ppVar(centerIndex);                                 \
        dbgKrita << ppVar(beginSrc) << ppVar(endSrc);                   \
        dbgKrita << ppVar(beginDst) << ppVar(endDst);                   \
        dbgKrita << ppVar(scaledIter) << ppVar(scaledInc);              \
        dbgKrita << ppVar(span);                                        \
        dbgKrita << "===";                                              \
    } while (0)

#define DEBUG_SAMPLE()                                                  \
    do {                                                                \
        dbgKrita << ppVar(scaledIter) << ppVar(t);                      \
    } while (0)
#else
#define DEBUG_ALL() Q_UNUSED(beginDst); Q_UNUSED(endDst);
#define DEBUG_SAMPLE()
#endif

class KisFilterWeightsBuffer
{
public:
    struct FilterWeights {
        ~FilterWeights() {
            delete[] weight;
        }

        qint16 *weight;
        int span;
        int centerIndex;
    };

public:
    KisFilterWeightsBuffer(KisFilterStrategy *filterStrategy, qreal realScale) {
        Q_ASSERT(realScale > 0);

        m_filterWeights = new FilterWeights[256];
        m_maxSpan = 0;
        m_weightsPositionScale = 1;

        KisFixedPoint supportSrc;
        KisFixedPoint supportDst;

        if (realScale < 1.0 && realScale > (1.0 / (1 << 8))) {
            m_weightsPositionScale = KisFixedPoint(realScale);
            supportSrc.from256Frac(filterStrategy->intSupport(m_weightsPositionScale.toFloat()) / realScale);
            supportDst.from256Frac(filterStrategy->intSupport(m_weightsPositionScale.toFloat()));

        } else {
            supportSrc.from256Frac(filterStrategy->intSupport(m_weightsPositionScale.toFloat()));
            supportDst.from256Frac(filterStrategy->intSupport(m_weightsPositionScale.toFloat()));
        }

        for (int i = 0; i < 256; i++) {
            KisFixedPoint centerSrc;
            centerSrc.from256Frac(i);

            KisFixedPoint beginDst = -supportDst;
            KisFixedPoint endDst = supportDst;

            KisFixedPoint beginSrc = -supportSrc - centerSrc / m_weightsPositionScale;
            KisFixedPoint endSrc = supportSrc - centerSrc / m_weightsPositionScale;

            int span = (2 * supportSrc).toInt() +
                (beginSrc.isInteger() && endSrc.isInteger());

            int centerIndex = -beginSrc.toInt();

            m_filterWeights[i].centerIndex = centerIndex;
            m_filterWeights[i].span = span;
            m_filterWeights[i].weight = new qint16[span];
            m_maxSpan = qMax(m_maxSpan, span);

            KisFixedPoint scaledIter = centerSrc + beginSrc.toInt() * m_weightsPositionScale;
            KisFixedPoint scaledInc = m_weightsPositionScale;

            DEBUG_ALL();

            int sum = 0;
            for (int j = 0; j < span; j++) {
                int t = filterStrategy->intValueAt(scaledIter.to256Frac(), m_weightsPositionScale.toFloat());
                m_filterWeights[i].weight[j] = t;
                sum += t;

                DEBUG_SAMPLE();
                SANITY_CENTER_POSITION();

                scaledIter += scaledInc;
            }

            SANITY_ZEROS();

            if (sum != 255 && sum > 0) {
                qreal fixFactor = 255.0 / sum;
                sum = 0;

                for (int j = 0; j < span; j++) {
                    int t = qRound(m_filterWeights[i].weight[j] * fixFactor);

                    m_filterWeights[i].weight[j] = t;
                    sum += t;
                }
            }

            while (sum != 255) {
                int diff = sum < 255 ? 1 : -1;
                int index = findMaxIndex(m_filterWeights[i].weight, span);
                m_filterWeights[i].weight[index] += diff;
                sum += diff;
            }

            SANITY_CHECKSUM();
        }
    }

    ~KisFilterWeightsBuffer() {
        delete[] m_filterWeights;
    }

    FilterWeights* weights(KisFixedPoint pos) const {
        return m_filterWeights + pos.to256Frac();
    }

    int maxSpan() const {
        return m_maxSpan;
    }

    KisFixedPoint weightsPositionScale() const {
        return m_weightsPositionScale;
    }

private:
    int findMaxIndex(qint16 *buf, int size) {
        int maxValue = buf[0];
        int maxIndex = 0;

        for (int i = 1; i < size; i++) {
            if (buf[i] > maxValue) {
                maxValue = buf[i];
                maxIndex = i;
            }
        }

        return maxIndex;
    }

private:
    FilterWeights *m_filterWeights;
    int m_maxSpan;
    KisFixedPoint m_weightsPositionScale;
};

#endif
