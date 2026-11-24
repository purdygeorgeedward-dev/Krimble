/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_MASK_
#define _KIS_MASK_

#include <QRect>

#include "kis_types.h"
#include "kis_global.h"
#include "kis_node.h"
#include "kis_indirect_painting_support.h"
#include "KisRenderPassFlags.h"

#include <kritaimage_export.h>

class KRITAIMAGE_EXPORT KisMask : public KisNode, public KisIndirectPaintingSupport
{

    Q_OBJECT

public:

    KisMask(KisImageWSP image, const QString &name);

    KisMask(const KisMask& rhs);

    ~KisMask() override;

    void setImage(KisImageWSP image) override;

    bool allowAsChild(KisNodeSP node) const override;

    void initSelection(KisSelectionSP copyFrom, KisLayerSP parentLayer);

    void initSelection(KisPaintDeviceSP copyFromDevice, KisLayerSP parentLayer);

    void initSelection(KisLayerSP parentLayer);

    const KoColorSpace * colorSpace() const override;
    const KoCompositeOp * compositeOp() const override;

    KisSelectionSP selection() const;

    KisPaintDeviceSP paintDevice() const override;

    KisPaintDeviceSP original() const override;

    KisPaintDeviceSP projection() const override;

    KisAbstractProjectionPlaneSP projectionPlane() const override;

    void setSelection(KisSelectionSP selection);

    void select(const QRect & rc, quint8 selectedness = MAX_SELECTED);

    QRect extent() const override;
    QRect exactBounds() const override;

    qint32 x() const override;

    void setX(qint32 x) override;

    qint32 y() const override;

    void setY(qint32 y) override;

    virtual QRect nonDependentExtent() const;

    QRect needRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const override;
    QRect changeRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const override;
    QImage createThumbnail(qint32 w, qint32 h, Qt::AspectRatioMode aspectRatioMode, KisThumbnailBoundsMode boundsMode) override;
    int thumbnailSeqNo() const override;

    void testingInitSelection(const QRect &rect, KisLayerSP parentLayer);

    bool supportsLodPainting() const override;

protected:
    void apply(KisPaintDeviceSP projection, const QRect & applyRect, const QRect & needRect, PositionToFilthy maskPos, KisRenderPassFlags flags) const;

    virtual void mergeInMaskInternal(KisPaintDeviceSP projection,
                                     KisSelectionSP effectiveSelection,
                                     const QRect &applyRect, const QRect &preparedNeedRect,
                                     PositionToFilthy maskPos,
                                     KisRenderPassFlags flags) const;

    virtual void flattenSelectionProjection(KisSelectionSP selection, const QRect &dirtyRect) const;

    virtual QRect decorateRect(KisPaintDeviceSP &src,
                               KisPaintDeviceSP &dst,
                               const QRect & rc,
                               PositionToFilthy maskPos,
                               KisRenderPassFlags flags) const;

    virtual bool paintsOutsideSelection() const;

    KisKeyframeChannel *requestKeyframeChannel(const QString &id) override;
    bool supportsKeyframeChannel(const QString &id) override;

    void baseNodeChangedCallback() override;

private:
    friend class KisMaskProjectionPlane;

private:

    struct Private;

    Private * const m_d;

};

#endif
