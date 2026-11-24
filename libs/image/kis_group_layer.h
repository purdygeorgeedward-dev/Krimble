/*
 *  SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_GROUP_LAYER_H_
#define KIS_GROUP_LAYER_H_

#include "kis_layer.h"
#include "kis_types.h"

class KoColorSpace;

class KRITAIMAGE_EXPORT KisGroupLayer : public KisLayer
{
    Q_OBJECT

public:
    KisGroupLayer(KisImageWSP image, const QString &name, quint8 opacity, const KoColorSpace * colorSpace = 0);
    KisGroupLayer(const KisGroupLayer& rhs);
    ~KisGroupLayer() override;

    KisNodeSP clone() const override {
        return KisNodeSP(new KisGroupLayer(*this));
    }

    bool allowAsChild(KisNodeSP) const override;

    QIcon icon() const override;

    KisBaseNode::PropertyList sectionModelProperties() const override;
    void setSectionModelProperties(const KisBaseNode::PropertyList &properties) override;

    void setImage(KisImageWSP image) override;

    KisLayerSP createMergedLayerTemplate(KisLayerSP prevLayer) override;
    void fillMergedLayerTemplate(KisLayerSP dstLayer, KisLayerSP prevLayer, bool skipPaintingThisLayer) override;

    void resetCache(const KoColorSpace *colorSpace);

    const KoColorSpace * colorSpace() const override;

    KisPaintDeviceSP original() const override;

    KisPaintDeviceSP lazyDestinationForSubtreeComposition() const;

    qint32 x() const override;
    qint32 y() const override;
    void setX(qint32 x) override;
    void setY(qint32 y) override;

    KisPaintDeviceSP paintDevice() const override;

    bool accept(KisNodeVisitor &v) override;
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) override;

    void setDefaultProjectionColor(KoColor color);

    KoColor defaultProjectionColor() const;

    bool passThroughMode() const;
    void setPassThroughMode(bool value);

    QRect extent() const override;
    QRect exactBounds() const override;

    bool projectionIsValid() const;

    QRect calculateChildrenTightUserVisibleBounds() const;
    QRect calculateChildrenLooseUserVisibleBounds() const;

protected:
    KisLayer* onlyMeaningfulChild() const;
    KisPaintDeviceSP tryObligeChild() const;
    std::tuple<KisPaintDeviceSP, bool> originalImpl() const;

    QRect amortizedProjectionRectForCleanupInChangePass() const override;
private:
    bool checkCloneLayer(KisCloneLayerSP clone) const;
    bool checkNodeRecursively(KisNodeSP node) const;

private:
    struct Private;
    Private * const m_d;
};

#endif

