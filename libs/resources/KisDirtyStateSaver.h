/*
 * SPDX-FileCopyrightText: 2019 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KISDIRTYSTATESAVER_H
#define KISDIRTYSTATESAVER_H

#include <KoResource.h>

#include "kritaresources_export.h"
template <typename T>
class KisDirtyStateSaver
{
public:
    KisDirtyStateSaver(T resource)
        : m_resource(resource)
        , m_isDirty(resource->isDirty())

    {
    }

    ~KisDirtyStateSaver() {
        if (m_resource) {
            m_resource->setDirty(m_isDirty);
        }
    }

private:
    T m_resource;
    bool m_isDirty = false;
};

#endif
