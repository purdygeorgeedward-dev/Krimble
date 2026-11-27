/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_FILTER_MASK_
#define _KIS_FILTER_MASK_

#include "kis_types.h"
#include "kis_effect_mask.h"

#include "kis_node_filter_interface.h"
#include "kis_filter_configuration.h"

class KRITAIMAGE_EXPORT KisFilterMask : public KisEffectMask, public KisNodeFilterInterface
{
    Q_OBJECT

public:
    KisFilterMask(KisImageWSP image, const QString &name = QString());

    KisFilterMask(const KisFilterMask& rhs);

    ~KisFilterMask() override;

    QIcon icon() const override;

    KisNodeSP clone() const override {
        return KisNodeSP(new KisFilterMask(*this));
    }

    bool accept(KisNodeVisitor &v) override;
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) override;

    void setFilter(KisFilterConfigurationSP filterConfig, bool checkCompareConfig = true) override;

    QRect decorateRect(KisPaintDeviceSP &src,
                       KisPaintDeviceSP &dst,
                       const QRect & rc,
                       PositionToFilthy maskPos,
                       KisRenderPassFlags flags) const override;

    QRect extent() const override;
    QRect exactBounds() const override;

    QRect changeRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const override;
    QRect needRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const override;

private:
    bool filterNeedsTransparentPixels() const;

private:
    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
