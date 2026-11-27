/*
 *  SPDX-FileCopyrightText: 2019 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISCROPPEDORIGINALLAYERINTERFACE_H
#define KISCROPPEDORIGINALLAYERINTERFACE_H

#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisCroppedOriginalLayerInterface
{
public:
    virtual ~KisCroppedOriginalLayerInterface();

    virtual void forceUpdateHiddenAreaOnOriginal() = 0;
};

#endif
