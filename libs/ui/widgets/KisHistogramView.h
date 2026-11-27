/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_HISTOGRAM_VIEW_H
#define KIS_HISTOGRAM_VIEW_H

#include <QScopedPointer>
#include <QWidget>

#include "kritaui_export.h"

class KisHistogram;
class KoColorSpace;

class KRITAUI_EXPORT KisHistogramView : public QWidget
{
    Q_OBJECT

public:
    KisHistogramView(QWidget *parent);
    ~KisHistogramView();

    void setup(const QVector<KisHistogram*> &histograms,
               const QVector<const KoColorSpace*> &colorSpaces,
               const QVector<QVector<int>> &channels = {});
    const QVector<int>& channels() const;
    QColor defaultColor() const;
    qreal scale() const;
    bool isLogarithmic() const;

public Q_SLOTS:
    void setChannel(int channel, int histogramIndex = 0);
    void setChannels(const QVector<int> &channels, int histogramIndex = 0);
    void clearChannels();
    void setDefaultColor(const QColor &newDefaultColor);
    void setScale(qreal newScale);
    void setScaleToFit();
    void setScaleToCutLongPeaks();
    void setLogarithmic(bool logarithmic);

protected:
    void paintEvent(QPaintEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;

private:
    class Private;
    QScopedPointer<Private> m_d;
};

#endif
