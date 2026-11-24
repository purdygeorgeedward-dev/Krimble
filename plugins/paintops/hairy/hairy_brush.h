/*
 *  SPDX-FileCopyrightText: 2008-2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _HAIRY_BRUSH_H_
#define _HAIRY_BRUSH_H_

#include <QVector>
#include <QTransform>

#include <KoColor.h>

#include "trajectory.h"
#include "bristle.h"

#include <kis_paint_device.h>
#include <brushengine/kis_paint_information.h>
#include <kis_random_accessor_ng.h>

class KoCompositeOp;

class KisHairyProperties
{
public:
    quint16 radius;
    quint16 inkAmount;
    qreal sigma;
    QVector<qreal> inkDepletionCurve;
    bool inkDepletionEnabled;
    bool isbrushDimension1D;
    bool useMousePressure;
    bool useSaturation;
    bool useOpacity;
    bool useWeights;

    bool useSoakInk;
    bool connectedPath;
    bool antialias;
    bool useCompositing;

    quint8 pressureWeight;
    quint8 bristleLengthWeight;
    quint8 bristleInkAmountWeight;
    quint8 inkDepletionWeight;

    qreal shearFactor;
    qreal randomFactor;
    qreal scaleFactor;
    qreal threshold;

};

class HairyBrush
{

public:
    HairyBrush();
    ~HairyBrush();

    void paintLine(KisPaintDeviceSP dab, KisPaintDeviceSP layer, const KisPaintInformation &pi1, const KisPaintInformation &pi2, qreal scale, qreal rotation);
    void setInkColor(const KoColor &color) {
        m_color = color;
    }
    void setProperties(KisHairyProperties * properties) {
        m_properties = properties;
    }
    void fromDabWithDensity(KisFixedPaintDeviceSP dab, qreal density);

private:
    void addBristleInk(Bristle *bristle,const QPointF &pos, const KoColor &color);
    void plotPixel(int wx, int wy, const KoColor &color);
    void darkenPixel(int wx, int wy, const KoColor &color);
    void paintParticle(QPointF pos, const KoColor& color, qreal weight);
    void paintParticle(QPointF pos, const KoColor& color);
    void colorifyBristles(KisPaintDeviceSP source, QPointF point);

    void repositionBristles(double angle, double slope);
    double computeMousePressure(double distance);

    void saturationDepletion(Bristle * bristle, KoColor &bristleColor, qreal pressure, qreal inkDepletion);
    void opacityDepletion(Bristle * bristle, KoColor &bristleColor, qreal pressure, qreal inkDepletion);
    qreal fetchInkDepletion(Bristle * bristle, int inkDepletionSize);

    void initAndCache();

private:
    const KisHairyProperties * m_properties {nullptr};

    QVector<Bristle*> m_bristles;
    QTransform m_transform;

    Trajectory m_trajectory;
    QHash<QString, QVariant> m_params;
    KisPaintDeviceSP m_dab;
    KisRandomAccessorSP m_dabAccessor;
    const KoCompositeOp * m_compositeOp {nullptr};
    quint32 m_pixelSize {0};

    int m_counter {0};

    double m_lastAngle {0.0};
    double m_oldPressure {1.0};
    KoColor m_color;

    int m_saturationId {-1};
    KoColorTransformation * m_transfo {nullptr};

    inline bool firstStroke() const {
        return (m_counter == 1);
    }
};

#endif
