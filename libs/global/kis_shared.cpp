/*
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_shared.h"
#include "kis_debug.h"

KisShared::KisShared()
{
    _sharedWeakReference = 0;
}

KisShared::~KisShared()
{
    Q_ASSERT(_ref == 0);

    if(_sharedWeakReference && !_sharedWeakReference->deref())
        delete _sharedWeakReference;
}
