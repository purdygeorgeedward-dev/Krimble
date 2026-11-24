/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_SELECTION_MASK_
#define _KIS_SELECTION_MASK_

#include <QRect>

#include "kis_base_node.h"

#include "kis_types.h"
#include "kis_effect_mask.h"
#include "KisDecoratedNodeInterface.h"

class KRITAIMAGE_EXPORT KisSelectionMask : public KisEffectMask, public KisDecoratedNodeInterface
{
    Q_OBJECT
public:

    KisSelectionMask(KisImageWSP image, const QString &name = QString());

    ~KisSelectionMask() override;
    KisSelectionMask(const KisSelectionMask& rhs);

    QIcon icon() const override;

    KisNodeSP clone() const override {
        return KisNodeSP(new KisSelectionMask(*this));
    }

    void setSelection(KisSelectionSP selection);

    bool accept(KisNodeVisitor &v) override;
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) override;

    KisBaseNode::PropertyList sectionModelProperties() const override;
    void setSectionModelProperties(const KisBaseNode::PropertyList &properties) override;

    void setVisible(bool visible, bool isLoading = false) override;
    bool active() const;
    void setActive(bool active);

    QRect needRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const override;
    QRect changeRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const override;

    QRect extent() const override;
    QRect exactBounds() const override;

    void notifySelectionChangedCompressed();

    bool decorationsVisible() const override;
    void setDecorationsVisible(bool value, bool update) override;
    using KisDecoratedNodeInterface::setDecorationsVisible;

    void setDirty(const QVector<QRect> &rects) override;
    using KisEffectMask::setDirty;

protected:
    void flattenSelectionProjection(KisSelectionSP selection, const QRect &dirtyRect) const override;

    void mergeInMaskInternal(KisPaintDeviceSP projection,
                             KisSelectionSP effectiveSelection,
                             const QRect &applyRect, const QRect &preparedNeedRect,
                             KisNode::PositionToFilthy maskPos, KisRenderPassFlags flags) const override;

    bool paintsOutsideSelection() const override;

private:
    Q_PRIVATE_SLOT(m_d, void slotSelectionChangedCompressed())
    Q_PRIVATE_SLOT(m_d, void slotConfigChanged())

    struct Private;
    Private * const m_d;
};

#endif
