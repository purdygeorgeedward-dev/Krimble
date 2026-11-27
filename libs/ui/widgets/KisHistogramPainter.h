/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_HISTOGRAM_PAINTER_H
#define KIS_HISTOGRAM_PAINTER_H

#include <QScopedPointer>
#include <QPainter>
#include <QVector>
#include <QRect>

#include "kritaui_export.h"

class KisHistogram;
class KoColorSpace;

class KRITAUI_EXPORT KisHistogramPainter
{
public:
    KisHistogramPainter();
    ~KisHistogramPainter();

    void setup(KisHistogram *histogram, const KoColorSpace *colorSpace, QVector<int> channels = {});

    QImage paint(const QSize &imageSize);
    QImage paint(int w, int h);
    void paint(QPainter &painter, const QRect &rect);
    int totalNumberOfAvailableChannels() const;
    QList<int> availableChannels() const;
    const QVector<int>& channels() const;
    void setChannel(int channel);
    void setChannels(const QVector<int> &channels);
    QColor defaultColor() const;
    void setDefaultColor(const QColor &newDefaultColor);
    qreal scale() const;
    void setScale(qreal newScale);
    void setScaleToFit();
    void setScaleToCutLongPeaks();
    bool isLogarithmic() const;
    void setLogarithmic(bool logarithmic);

private:
    class Private;
    QScopedPointer<Private> m_d;
};

#endif
