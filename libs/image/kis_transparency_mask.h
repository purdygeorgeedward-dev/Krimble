/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_TRANSPARENCY_MASK_
#define _KIS_TRANSPARENCY_MASK_

#include "kis_types.h"
#include "kis_effect_mask.h"

class QRect;

class KRITAIMAGE_EXPORT KisTransparencyMask : public KisEffectMask
{
    Q_OBJECT

public:

    KisTransparencyMask(KisImageWSP image, const QString &name);
    KisTransparencyMask(const KisTransparencyMask& rhs);
    ~KisTransparencyMask() override;

    KisNodeSP clone() const override {
        return KisNodeSP(new KisTransparencyMask(*this));
    }

    QRect decorateRect(KisPaintDeviceSP &src, KisPaintDeviceSP &dst,
                       const QRect & rc,
                       PositionToFilthy maskPos,
                       KisRenderPassFlags flags) const override;
    QIcon icon() const override;
    bool accept(KisNodeVisitor &v) override;
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) override;

    QRect extent() const override;
    QRect exactBounds() const override;

    QRect changeRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const override;
    QRect needRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const override;

    bool paintsOutsideSelection() const override;
};

#endif
