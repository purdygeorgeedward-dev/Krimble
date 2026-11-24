/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_TOOL_REGISTRY_H_
#define KO_TOOL_REGISTRY_H_

#include "KoGenericRegistry.h"
#include "kritaflake_export.h"

class KoToolFactoryBase;

class KRITAFLAKE_EXPORT KoToolRegistry : public KoGenericRegistry<KoToolFactoryBase*>
{
public:
    KoToolRegistry();
    ~KoToolRegistry() override;

    static KoToolRegistry *instance();

private:
    KoToolRegistry(const KoToolRegistry&);
    KoToolRegistry operator=(const KoToolRegistry&);
    void init();

    class Private;
    Private * const d;
};

#endif
