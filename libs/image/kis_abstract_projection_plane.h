/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_ABSTRACT_PROJECTION_PLANE_H
#define __KIS_ABSTRACT_PROJECTION_PLANE_H

#include "kis_types.h"
#include "kis_layer.h"

class QRect;
class KisPainter;

class KRITAIMAGE_EXPORT  KisAbstractProjectionPlane
{
public:
    KisAbstractProjectionPlane();
    virtual ~KisAbstractProjectionPlane();

    virtual QRect recalculate(const QRect& rect, KisNodeSP filthyNode, KisRenderPassFlags flags) = 0;

    virtual void apply(KisPainter *painter, const QRect &rect) = 0;

    virtual QRect needRect(const QRect &rect, KisLayer::PositionToFilthy pos = KisLayer::N_FILTHY) const = 0;

    virtual QRect changeRect(const QRect &rect, KisLayer::PositionToFilthy pos = KisLayer::N_FILTHY) const = 0;

    virtual QRect accessRect(const QRect &rect, KisLayer::PositionToFilthy pos = KisLayer::N_FILTHY) const = 0;

    virtual QRect needRectForOriginal(const QRect &rect) const = 0;

    virtual QRect tightUserVisibleBounds() const = 0;

    virtual QRect looseUserVisibleBounds() const = 0;

    virtual KisPaintDeviceList getLodCapableDevices() const = 0;
};

class KisDumbProjectionPlane : public KisAbstractProjectionPlane
{
public:
    QRect recalculate(const QRect& rect, KisNodeSP filthyNode, KisRenderPassFlags flags) override;
    void apply(KisPainter *painter, const QRect &rect) override;

    QRect needRect(const QRect &rect, KisLayer::PositionToFilthy pos) const override;
    QRect changeRect(const QRect &rect, KisLayer::PositionToFilthy pos) const override;
    QRect accessRect(const QRect &rect, KisLayer::PositionToFilthy pos) const override;
    QRect needRectForOriginal(const QRect &rect) const override;
    QRect tightUserVisibleBounds() const override;
    QRect looseUserVisibleBounds() const override;

    KisPaintDeviceList getLodCapableDevices() const override;
};

#endif
