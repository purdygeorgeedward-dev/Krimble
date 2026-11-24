/*
 *  SPDX-FileCopyrightText: 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "particle_brush.h"

#include "kis_paint_device.h"
#include "kis_random_accessor_ng.h"

#include <KoColorSpace.h>
#include <KoColor.h>

#include <kis_global.h>

#include <math.h>

const qreal TIME = 0.000030;

ParticleBrush::ParticleBrush()
{
    m_properties = 0;
}

ParticleBrush::~ParticleBrush()
{
}

void ParticleBrush::initParticles()
{
    m_particlePos.resize(m_properties->particleCount);
    m_particleNextPos.resize(m_properties->particleCount);
    m_acceleration.resize(m_properties->particleCount);
}

void ParticleBrush::setInitialPosition(const QPointF &pos)
{
    for (int i = 0; i < m_properties->particleCount; i++) {
        m_particlePos[i] = pos;
        m_particleNextPos[i] = pos;
        m_acceleration[i] = (i + m_properties->particleIterations) * 0.5;
    }
}

void ParticleBrush::paintParticle(KisRandomAccessorSP accWrite, const KoColorSpace * cs, const QPointF &pos, const KoColor& color, qreal weight, bool respectOpacity)
{
    KoColor myColor(color);
    quint8 opacity = respectOpacity ? myColor.opacityU8() : OPACITY_OPAQUE_U8;

    int ipx = floor(pos.x());
    int ipy = floor(pos.y());
    qreal fx = pos.x() - ipx;
    qreal fy = pos.y() - ipy;

    quint8 btl = qRound((1.0 - fx) * (1.0 - fy) * opacity * weight);
    quint8 btr = qRound((fx)  * (1.0 - fy) * opacity * weight);
    quint8 bbl = qRound((1.0 - fx) * (fy)  * opacity * weight);
    quint8 bbr = qRound((fx)  * (fy)  * opacity * weight);

    accWrite->moveTo(ipx  , ipy);
    myColor.setOpacity(quint8(kisBoundFast<quint16>(OPACITY_TRANSPARENT_U8, btl + cs->opacityU8(accWrite->rawData()), OPACITY_OPAQUE_U8)));
    memcpy(accWrite->rawData(), myColor.data(), cs->pixelSize());

    accWrite->moveTo(ipx + 1, ipy);
    myColor.setOpacity(quint8(kisBoundFast<quint16>(OPACITY_TRANSPARENT_U8, btr + cs->opacityU8(accWrite->rawData()), OPACITY_OPAQUE_U8)));
    memcpy(accWrite->rawData(), myColor.data(), cs->pixelSize());

    accWrite->moveTo(ipx, ipy + 1);
    myColor.setOpacity(quint8(kisBoundFast<quint16>(OPACITY_TRANSPARENT_U8, bbl + cs->opacityU8(accWrite->rawData()), OPACITY_OPAQUE_U8)));
    memcpy(accWrite->rawData(), myColor.data(), cs->pixelSize());

    accWrite->moveTo(ipx + 1, ipy + 1);
    myColor.setOpacity(quint8(kisBoundFast<quint16>(OPACITY_TRANSPARENT_U8, bbr + cs->opacityU8(accWrite->rawData()), OPACITY_OPAQUE_U8)));
    memcpy(accWrite->rawData(), myColor.data(), cs->pixelSize());
}

void ParticleBrush::draw(KisPaintDeviceSP dab, const KoColor& color, const QPointF &pos)
{
    KisRandomAccessorSP accessor = dab->createRandomAccessorNG();
    const KoColorSpace * cs = dab->colorSpace();

    QRect boundingRect;

    if (m_properties->particleScaleX < 0 || m_properties->particleScaleY < 0 || m_properties->particleGravity < 0) {
        boundingRect = dab->defaultBounds()->bounds();
    }

    for (int i = 0; i < m_properties->particleIterations; i++) {
        for (int j = 0; j < m_properties->particleCount; j++) {

            QPointF dist = pos - m_particlePos[j];
            dist.setX(dist.x() * m_properties->particleScaleX);
            dist.setY(dist.y() * m_properties->particleScaleY);
            dist = dist * m_acceleration[j];
            m_particleNextPos[j] = m_particleNextPos[j] + dist;
            m_particleNextPos[j] *= m_properties->particleGravity;
            m_particlePos[j] = m_particlePos[j] + (m_particleNextPos[j] * TIME);

            QPointF pointF = m_particlePos[j];

            const qint32 max = 2147483600;
            const qint32 min = -max;
            bool nearInfinity = pointF.x() < min || pointF.x () > max || pointF.y() < min || pointF.y() > max;
            bool inside = boundingRect.contains(m_particlePos[j].toPoint());

            if (boundingRect.isEmpty() || (inside && !nearInfinity)) {
                paintParticle(accessor, cs, m_particlePos[j], color, m_properties->particleWeight, true);
            }

        }
    }
}

