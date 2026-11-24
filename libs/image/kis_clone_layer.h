/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_CLONE_LAYER_H_
#define KIS_CLONE_LAYER_H_

#include <QObject>
#include "kis_types.h"
#include "kis_layer.h"

#include <kritaimage_export.h>
#include "kis_node_uuid_info.h"

class KisNodeVisitor;

enum CopyLayerType {
    COPY_PROJECTION,
    COPY_ORIGINAL
};

class KRITAIMAGE_EXPORT KisCloneLayer : public KisLayer
{

    Q_OBJECT

public:

    KisCloneLayer(KisLayerSP from, KisImageWSP image, const QString &name, quint8 opacity);
    KisCloneLayer(const KisCloneLayer& rhs);
    ~KisCloneLayer() override;

    KisNodeSP clone() const override {
        return KisNodeSP(new KisCloneLayer(*this));
    }

    KisLayerSP reincarnateAsPaintLayer() const;

    void setImage(KisImageWSP image) override;
    bool allowAsChild(KisNodeSP) const override;

    KisPaintDeviceSP original() const override;
    KisPaintDeviceSP paintDevice() const override;
    bool needProjection() const override;

    const KoColorSpace* colorSpace() const override;

    QIcon icon() const override;
    KisBaseNode::PropertyList sectionModelProperties() const override;

    qint32 x() const override;
    qint32 y() const override;

    void setX(qint32) override;
    void setY(qint32) override;

    QRect extent() const override;

    QRect exactBounds() const override;

    bool accept(KisNodeVisitor &) override;
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) override;

    void setCopyFromInfo(KisNodeUuidInfo info);
    KisNodeUuidInfo copyFromInfo() const;

    void setCopyFrom(KisLayerSP layer);
    KisLayerSP copyFrom() const;

    void setCopyType(CopyLayerType type);
    CopyLayerType copyType() const;

    void setDirtyOriginal(const QRect &rect, bool dontInvalidateFrames);

    QRect needRectOnSourceForMasks(const QRect &rc) const;

    void syncLodCache() override;

protected:
    QRect accessRect(const QRect &rect, PositionToFilthy pos) const override;

    void copyOriginalToProjection(const KisPaintDeviceSP original,
                                  KisPaintDeviceSP projection,
                                  const QRect& rect) const override;

    void notifyParentVisibilityChanged(bool value) override;
    QRect outgoingChangeRect(const QRect &rect) const override;
private:

    struct Private;
    Private * const m_d {nullptr};

};

#endif

