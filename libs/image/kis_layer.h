/*
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_LAYER_H_
#define KIS_LAYER_H_

#include <QRect>
#include <QMetaType>
#include <QObject>

#include "kritaimage_export.h"

#include "kis_base_node.h"

#include "kis_types.h"
#include "kis_node.h"
#include "kis_psd_layer_style.h"

#include "KisRenderPassFlags.h"

template <class T>
class QStack;

class QBitArray;
class KisCloneLayer;
class KisPSDLayerStyle;
class KisAbstractProjectionPlane;
class KisLayerProjectionPlane;
typedef QSharedPointer<KisLayerProjectionPlane> KisLayerProjectionPlaneSP;

namespace KisMetaData
{
class Store;
}

class KRITAIMAGE_EXPORT KisLayer : public KisNode
{

    Q_OBJECT

public:

    KisLayer(KisImageWSP image, const QString &name, quint8 opacity);
    KisLayer(const KisLayer& rhs);
    ~KisLayer() override;

    const KoColorSpace * colorSpace() const override;

    const KoCompositeOp * compositeOp() const override;

    KisPSDLayerStyleSP layerStyle() const;
    void setLayerStyle(KisPSDLayerStyleSP layerStyle);

    KisAbstractProjectionPlaneSP projectionPlane() const override;

    virtual KisLayerProjectionPlaneSP internalProjectionPlane() const;

    QRect partialChangeRect(KisNodeSP lastNode, const QRect& rect);
    void buildProjectionUpToNode(KisPaintDeviceSP projection, KisNodeSP lastNode, const QRect& rect);

    virtual bool needProjection() const;

    KisPaintDeviceSP projection() const override;

    KisPaintDeviceSP original() const override = 0;

    virtual KisSelectionMaskSP selectionMask() const;

    virtual KisSelectionSP selection() const;

    KisBaseNode::PropertyList sectionModelProperties() const override;
    void setSectionModelProperties(const KisBaseNode::PropertyList &properties) override;

    void disableAlphaChannel(bool disable);

    bool alphaChannelDisabled() const;

    virtual void setChannelFlags(const QBitArray & channelFlags);

    QBitArray & channelFlags() const;

    bool temporary() const;

    void setTemporary(bool t);

    void setImage(KisImageWSP image) override;

    virtual KisLayerSP tryCreateInternallyMergedLayerFromMutipleLayers(QList<KisLayerSP> layers);

    virtual KisLayerSP createMergedLayerTemplate(KisLayerSP prevLayer);
    virtual void fillMergedLayerTemplate(KisLayerSP dstLayer, KisLayerSP prevLayer, bool skipPaintingThisLayer = false);

    void registerClone(KisCloneLayerWSP clone);

    void unregisterClone(KisCloneLayerWSP clone);

    const QList<KisCloneLayerWSP> registeredClones() const;

    bool hasClones() const;

    void updateClones(const QRect &rect, bool dontInvalidateFrames);

    void notifyChildMaskChanged();

public:
    qint32 x() const override;
    qint32 y() const override;

    void setX(qint32 x) override;
    void setY(qint32 y) override;

    QRect extent() const override;

    QRect exactBounds() const override;

    QImage createThumbnail(qint32 w, qint32 h, Qt::AspectRatioMode aspectRatioMode, KisThumbnailBoundsMode boundsMode) override;

    int thumbnailSeqNo() const override;

    QImage createThumbnailForFrame(qint32 w, qint32 h, int time, Qt::AspectRatioMode aspectRatioMode, KisThumbnailBoundsMode boundsMode) override;

    QRect tightUserVisibleBounds() const;

    QRect looseUserVisibleBounds() const;

public:
    bool hasEffectMasks() const;

    QList<KisEffectMaskSP> effectMasks() const;

    QList<KisEffectMaskSP> effectMasks(KisNodeSP lastNode) const;

    KisFilterMaskSP colorOverlayMask() const;

    KisLayerSP parentLayer() const;

    KisMetaData::Store* metaData();

protected:
    virtual QRect amortizedProjectionRectForCleanupInChangePass() const;

    QRect changeRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const override;

    void childNodeChanged(KisNodeSP changedChildNode) override;

protected:

    QRect updateProjection(const QRect& rect, KisNodeSP filthyNode, KisRenderPassFlags flags);

    virtual void copyOriginalToProjection(const KisPaintDeviceSP original,
                                          KisPaintDeviceSP projection,
                                          const QRect& rect) const;
    virtual QRect incomingChangeRect(const QRect &rect) const;

    virtual QRect outgoingChangeRect(const QRect &rect) const;

    QRect needRectForOriginal(const QRect &rect) const;

    QRect masksChangeRect(const QList<KisEffectMaskSP> &masks,
                          const QRect &requestedRect,
                          bool &rectVariesFlag) const;

    QRect masksNeedRect(const QList<KisEffectMaskSP> &masks,
                        const QRect &changeRect,
                        QStack<QRect> &applyRects,
                        bool &rectVariesFlag) const;

    QRect applyMasks(const KisPaintDeviceSP source,
                     KisPaintDeviceSP destination,
                     const QRect &requestedRect,
                     KisNodeSP filthyNode,
                     KisNodeSP lastNode,
                     KisRenderPassFlags flags) const;

    bool canMergeAndKeepBlendOptions(KisLayerSP otherLayer);

    QList<KisEffectMaskSP> searchEffectMasks(KisNodeSP lastNode) const;

private:
    friend class KisLayerMasksCache;
    friend class KisLayerProjectionPlane;
    friend class KisTransformMask;
    friend class KisLayerTest;

private:
    QRect layerExtentImpl(bool exactBounds) const;
    QRect userVisibleBoundsImpl(bool exactBounds) const;

private:
    struct Private;
    Private * const m_d;
};

Q_DECLARE_METATYPE(KisLayerSP)

#endif
