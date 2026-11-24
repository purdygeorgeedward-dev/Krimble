/*
 *  SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_DEFAULT_BOUNDS_BASE_H
#define KIS_DEFAULT_BOUNDS_BASE_H

#include <QRect>
#include "kis_shared.h"
#include "kis_shared_ptr.h"
#include "kritaimage_export.h"
#include "KisWraparoundAxis.h"
class KisDefaultBoundsBase;

typedef KisSharedPtr<KisDefaultBoundsBase> KisDefaultBoundsBaseSP;

class KRITAIMAGE_EXPORT KisDefaultBoundsBase : public KisShared
{
public:
    virtual ~KisDefaultBoundsBase();

    virtual QRect bounds() const = 0;

    virtual QRect imageBorderRect() const;

    virtual bool wrapAroundMode() const = 0;
    virtual WrapAroundAxis wrapAroundModeAxis() const = 0;
    virtual int currentLevelOfDetail() const = 0;
    virtual int currentTime() const = 0;
    virtual bool externalFrameActive() const = 0;

    virtual void* sourceCookie() const = 0;
};

#endif
