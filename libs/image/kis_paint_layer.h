/*
 *  SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_PAINT_LAYER_H_
#define KIS_PAINT_LAYER_H_

#include "kis_types.h"
#include "kis_layer.h"
#include "kis_indirect_painting_support.h"
#include "KisDecoratedNodeInterface.h"

#include <QBitArray>

class KoColorSpace;

class KRITAIMAGE_EXPORT KisPaintLayer : public KisLayer, public KisIndirectPaintingSupport, public KisDecoratedNodeInterface
{

    Q_OBJECT

public:
    KisPaintLayer(KisImageWSP image, const QString& name, quint8 opacity, KisPaintDeviceSP dev);

    KisPaintLayer(KisImageWSP image, const QString& name, quint8 opacity);

    KisPaintLayer(KisImageWSP image, const QString& name, quint8 opacity, const KoColorSpace * colorSpace);
    KisPaintLayer(const KisPaintLayer& rhs);
    ~KisPaintLayer() override;

    KisNodeSP clone() const override {
        return KisNodeSP(new KisPaintLayer(*this));
    }

    bool allowAsChild(KisNodeSP) const override;

    bool needProjection() const override;

    QIcon icon() const override;
    void setImage(KisImageWSP image) override;

    KisBaseNode::PropertyList sectionModelProperties() const override;
    void setSectionModelProperties(const KisBaseNode::PropertyList &properties) override;

public:

    QRect extent() const override;
    QRect exactBounds() const override;

    bool accept(KisNodeVisitor &v) override;
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) override;

    void setChannelLockFlags(const QBitArray& channelFlags);

    const QBitArray& channelLockFlags() const;

    KisPaintDeviceSP paintDevice() const override;

    KisPaintDeviceSP original() const override;

    bool alphaLocked() const;

    void setAlphaLocked(bool lock);

    bool onionSkinEnabled() const;

    void setOnionSkinEnabled(bool state);

    void flushOnionSkinCache();

    KisPaintDeviceList getLodCapableDevices() const override;

    bool decorationsVisible() const override;
    void setDecorationsVisible(bool value, bool update) override;
    using KisDecoratedNodeInterface::setDecorationsVisible;

    void handleKeyframeChannelFrameChange(const KisKeyframeChannel *channel, int time) override;
    void handleKeyframeChannelFrameAdded(const KisKeyframeChannel *channel, int time) override;

public Q_SLOTS:
    void slotExternalUpdateOnionSkins();

public:

    KisLayer* layer() {
        return this;
    }

protected:
    void copyOriginalToProjection(const KisPaintDeviceSP original,
                                  KisPaintDeviceSP projection,
                                  const QRect& rect) const override;

    KisKeyframeChannel *requestKeyframeChannel(const QString &id) override;
    bool supportsKeyframeChannel(const QString &id) override;
    KisFrameChangeUpdateRecipe handleKeyframeChannelFrameAboutToBeRemovedImpl(const KisKeyframeChannel *channel, int time) override;

private:
    struct Private;
    Private * const m_d;
};

typedef KisSharedPtr<KisPaintLayer> KisPaintLayerSP;

#endif

