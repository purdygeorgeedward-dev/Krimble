/*
 *  SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_GENERATOR_LAYER_H_
#define KIS_GENERATOR_LAYER_H_

#include <boost/none.hpp>

#include "kis_selection_based_layer.h"
#include <kritaimage_export.h>
#include <KisDelayedUpdateNodeInterface.h>

#include <QScopedPointer>

class KisFilterConfiguration;

class KRITAIMAGE_EXPORT KisGeneratorLayer
        : public KisSelectionBasedLayer,
          public KisDelayedUpdateNodeInterface
{
    Q_OBJECT

public:
    KisGeneratorLayer(KisImageWSP image, const QString &name, KisFilterConfigurationSP  kfc, KisSelectionSP selection);
    KisGeneratorLayer(const KisGeneratorLayer& rhs);
    ~KisGeneratorLayer() override;

    KisNodeSP clone() const override {
        return KisNodeSP(new KisGeneratorLayer(*this));
    }

    void setFilter(KisFilterConfigurationSP filterConfig, bool checkCompareConfig = true) override;
    void setFilterWithoutUpdate(KisFilterConfigurationSP filterConfig, bool checkCompareConfig = true);

    bool accept(KisNodeVisitor &) override;
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) override;

    QIcon icon() const override;
    KisBaseNode::PropertyList sectionModelProperties() const override;

    void update();
    QWeakPointer<boost::none_t> previewWithStroke(const KisStrokeId stroke);

    using KisSelectionBasedLayer::setDirty;
    void setDirty(const QVector<QRect> &rects) override;
    void setDirtyWithoutUpdate(const QVector<QRect> &rects);
    void setX(qint32 x) override;
    void setY(qint32 y) override;

    void resetCache(const KoColorSpace *colorSpace) override;

    void forceUpdateTimedNode() override;
    bool hasPendingTimedUpdates() const override;

private Q_SLOTS:
    void slotDelayedStaticUpdate();

private:
    void requestUpdateJobsWithStroke(const KisStrokeId stroke, const KisFilterConfigurationSP configuration);
    void resetCacheWithoutUpdate(const KoColorSpace *colorSpace);

public:
    KisLayer* layer() {
        return this;
    }

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif

