/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_SELECTION_BASED_LAYER_H_
#define KIS_SELECTION_BASED_LAYER_H_

#include <QObject>
#include "kis_types.h"
#include "kis_layer.h"
#include "kis_indirect_painting_support.h"
#include <kritaimage_export.h>
#include "kis_node_filter_interface.h"

class KisFilterConfiguration;

class KRITAIMAGE_EXPORT KisSelectionBasedLayer : public KisLayer, public KisIndirectPaintingSupport, public KisNodeFilterInterface
{
    Q_OBJECT

public:
    KisSelectionBasedLayer(KisImageWSP image, const QString &name, KisSelectionSP selection, KisFilterConfigurationSP filterConfig);
    KisSelectionBasedLayer(const KisSelectionBasedLayer& rhs);
    ~KisSelectionBasedLayer() override;

    bool allowAsChild(KisNodeSP node) const override;

    void setImage(KisImageWSP image) override;

    KisPaintDeviceSP original() const override;
    KisPaintDeviceSP paintDevice() const override;

    bool needProjection() const override;

    virtual void resetCache(const KoColorSpace *colorSpace);

    using KisLayer::setDirty;

    void setDirty() override;

public:

    KisSelectionSP internalSelection() const;

    void setInternalSelection(KisSelectionSP selection);

    KisSelectionSP fetchComposedInternalSelection(const QRect &rect) const;

    qint32 x() const override;

    qint32 y() const override;

    void setX(qint32 x) override;

    void setY(qint32 y) override;

    bool supportsLodPainting() const override;

public:

    QRect extent() const override;

    QRect exactBounds() const override;

    QImage createThumbnail(qint32 w, qint32 h, Qt::AspectRatioMode aspectRatioMode, KisThumbnailBoundsMode boundsMode) override;

    int thumbnailSeqNo() const override;

protected:
    void copyOriginalToProjection(const KisPaintDeviceSP original,
                                  KisPaintDeviceSP projection,
                                  const QRect& rect) const override;
    QRect needRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const override;

protected:
    void initSelection();

    QRect cropChangeRectBySelection(const QRect &rect) const;

    void setUseSelectionInProjection(bool value) const;

    KisKeyframeChannel *requestKeyframeChannel(const QString &id) override;
    bool supportsKeyframeChannel(const QString &id) override;

public Q_SLOTS:
    void slotImageSizeChanged();

    KisLayer* layer() {
        return this;
    }

private:
    struct Private;
    Private * const m_d;
};

#endif

