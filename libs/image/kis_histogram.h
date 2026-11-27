/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt
 *  SPDX-FileCopyrightText: 2005 Bart Coppens <kde@bartcoppens.be>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_HISTOGRAM_
#define KIS_HISTOGRAM_

#include <QVector>
#include <QRect>

#include "KoHistogramProducer.h"

#include "kis_shared.h"
#include "kis_types.h"
#include "kritaimage_export.h"

enum enumHistogramType {
    LINEAR,
    LOGARITHMIC
};
class KRITAIMAGE_EXPORT KisHistogram : public KisShared
{

public:
    class Calculations
    {

        double m_max {0.0};
        double m_min {0.0};
        double m_mean {0.0};
        double m_total {0.0};

        quint32 m_high {0};
        quint32 m_low {0};
        quint32 m_count {0};

        friend class KisHistogram;

    public:

        Calculations()
        {
        }
        inline double getMax() {
            return m_max;
        }
        inline double getMin() {
            return m_min;
        }
        inline quint32 getHighest() {
            return m_high;
        }
        inline quint32 getLowest() {
            return m_low;
        }
        inline double getMean() {
            return m_mean;
        }
        inline quint32 getCount() {
            return m_count;
        }
        inline double getTotal() {
            return m_total;
        }
    };

    KisHistogram(KisPaintLayerSP layer,
                 KoHistogramProducer *producer,
                 const enumHistogramType type);

    KisHistogram(KisPaintDeviceSP paintdev,
                 const QRect &bounds,
                 KoHistogramProducer *producer,
                 const enumHistogramType type);

    virtual ~KisHistogram();

    void updateHistogram();

    void computeHistogram();

    Calculations calculations();
    Calculations selectionCalculations();

    inline quint32 getValue(quint8 i) {
        return m_producer->getBinAt(m_channel, i);
    }

    inline enumHistogramType getHistogramType() {
        return m_type;
    }
    inline void setHistogramType(enumHistogramType type) {
        m_type = type;
    }
    inline void setProducer(KoHistogramProducer *producer) {
        m_channel = 0;
        m_producer = producer;
    }
    inline void setChannel(qint32 channel) {
        Q_ASSERT(m_channel < m_completeCalculations.size());
        m_channel = channel;
    }
    inline KoHistogramProducer *producer() {
        return m_producer;
    }
    inline qint32 channel() {
        return m_channel;
    }

    inline bool hasSelection() {
        return m_selection;
    }
    inline double selectionFrom() {
        return m_selFrom;
    }
    inline double selectionTo() {
        return m_selTo;
    }
    inline void setNoSelection() {
        m_selection = false;
    }
    inline void setSelection(double from, double to) {
        m_selection = true; m_selFrom = from; m_selTo = to;
    }

private:
    void dump();
    QVector<Calculations> calculateForRange(double from, double to);
    Calculations calculateSingleRange(int channel, double from, double to);

    const KisPaintDeviceSP m_paintDevice;
    QRect m_bounds;
    KoHistogramProducer *m_producer {nullptr};
    enumHistogramType m_type {LINEAR};

    qint32 m_channel {0};
    double m_selFrom {0.0};
    double m_selTo {0.0};
    bool m_selection {false};

    QVector<Calculations> m_completeCalculations, m_selectionCalculations;
};

#endif
