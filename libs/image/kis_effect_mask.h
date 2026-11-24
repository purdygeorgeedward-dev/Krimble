/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_EFFECT_MASK_
#define _KIS_EFFECT_MASK_

#include "kis_mask.h"
class KRITAIMAGE_EXPORT KisEffectMask : public KisMask
{

    Q_OBJECT

public:

    KisEffectMask(KisImageWSP image, const QString &name);

    ~KisEffectMask() override;
    KisEffectMask(const KisEffectMask& rhs);

    QIcon icon() const override;

    using KisMask::apply;
};

#endif
