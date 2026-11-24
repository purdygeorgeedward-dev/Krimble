/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISDELAYEDUPDATENODEINTERFACE_H
#define KISDELAYEDUPDATENODEINTERFACE_H

#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisDelayedUpdateNodeInterface
{
public:
    virtual ~KisDelayedUpdateNodeInterface();

    virtual void forceUpdateTimedNode() = 0;

    virtual bool hasPendingTimedUpdates() const = 0;
};

#endif
