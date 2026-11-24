/*
 * KDE. Krita Project.
 *
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISSCREENTONEGENERATORFUNCTIONSAMPLER_H
#define KISSCREENTONEGENERATORFUNCTIONSAMPLER_H

#include <QtGlobal>
#include <QTransform>

#include "KisScreentoneGeneratorConfiguration.h"

template <typename Function>
class KisScreentoneGeneratorFunctionSampler
{
public:
    KisScreentoneGeneratorFunctionSampler(const KisScreentoneGeneratorConfigurationSP config)
        : KisScreentoneGeneratorFunctionSampler(config, Function())
    {}

    KisScreentoneGeneratorFunctionSampler(const KisScreentoneGeneratorConfigurationSP config,
                                          const Function &the_function)
        : m_function(the_function)
    {
        qreal sizeX, sizeY;
        if (config->sizeMode() == KisScreentoneSizeMode_PixelBased) {
            const bool constrainSize = config->constrainSize();
            sizeX = config->sizeX();
            sizeY = constrainSize ? sizeX : config->sizeY();
        } else {
            const qreal resolution = config->resolution();
            const bool constrainFrequency = config->constrainFrequency();
            const qreal frequencyX = config->frequencyX();
            const qreal frequencyY = constrainFrequency ? frequencyX : config->frequencyY();
            sizeX = qMax(1.0, resolution / frequencyX);
            sizeY = qMax(1.0, resolution / frequencyY);
        }
        const qreal positionX = config->positionX();
        const qreal positionY = config->positionY();
        const qreal shearX = config->shearX();
        const qreal shearY = config->shearY();
        const qreal rotation = config->rotation();

        QTransform t;
        if (config->alignToPixelGrid()) {
            t.rotate(-rotation);
            t.scale(sizeX, sizeY);
            t.shear(-shearX, -shearY);
            const QSizeF macrocellSize(
                static_cast<qreal>(config->alignToPixelGridX()),
                static_cast<qreal>(config->alignToPixelGridY())
            );
            const QPointF u1 = t.map(QPointF(macrocellSize.width(), 0.0));
            const QPointF u2 = t.map(QPointF(0.0, macrocellSize.height()));
            QPointF v1(qRound(u1.x()), qRound(u1.y()));
            QPointF v2(qRound(u2.x()), qRound(u2.y()));
            if (qFuzzyCompare(v1.y() * v2.x(), v2.y() * v1.x()) &&
                !qFuzzyIsNull(v1.x() * v2.x() + v1.y() * v2.y())) {
                const qreal dist1 = kisSquareDistance(u1, v1);
                const qreal dist2 = kisSquareDistance(u2, v2);
                const QPointF *p_u = dist1 > dist2 ? &u1 : &u2;
                QPointF *p_v = dist1 > dist2 ? &v1 : &v2;
                QPair<int, qreal> dists[4]{
                    {1, kisSquareDistance(*p_u, *p_v + QPointF(0.0, -1.0))},
                    {2, kisSquareDistance(*p_u, *p_v + QPointF(1.0, 0.0))},
                    {3, kisSquareDistance(*p_u, *p_v + QPointF(0.0, 1.0))},
                    {4, kisSquareDistance(*p_u, *p_v + QPointF(-1.0, 0.0))}
                };
                std::sort(
                    std::begin(dists), std::end(dists),
                    [](const QPair<int, qreal> &a, const QPair<int, qreal> &b)
                    {
                        return a.second < b.second;
                    }
                );
                if (dists[0].first == 1) {
                    p_v->setY(p_v->y() - 1.0);
                } else if (dists[0].first == 2) {
                    p_v->setX(p_v->x() + 1.0);
                } else if (dists[0].first == 3) {
                    p_v->setY(p_v->y() + 1.0);
                } else {
                    p_v->setX(p_v->x() - 1.0);
                }
            }
            QPolygonF quad;
            quad.append(QPointF(0, 0));
            quad.append(v1 / macrocellSize.width());
            quad.append(v1 / macrocellSize.width() + v2 / macrocellSize.height());
            quad.append(v2 / macrocellSize.height());
            QTransform::quadToSquare(quad, t);
            t.translate(qRound(positionX), qRound(positionY));
        } else {
            t.shear(shearX, shearY);
            t.scale(qFuzzyIsNull(sizeX) ? 0.0 : 1.0 / sizeX, qFuzzyIsNull(sizeY) ? 0.0 : 1.0 / sizeY);
            t.rotate(rotation);
            t.translate(positionX, positionY);
        }

        m_imageToScreenTransform = t;
    }

    qreal operator()(qreal x, qreal y) const
    {
        qreal xx, yy;
        m_imageToScreenTransform.map(x, y, &xx, &yy);
        return m_function(xx, yy);
    }

private:
    Function m_function;
    QTransform m_imageToScreenTransform;
};

#endif
