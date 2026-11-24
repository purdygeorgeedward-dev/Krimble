/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KODOCKREGISTRY_
#define KODOCKREGISTRY_

#include "KoGenericRegistry.h"
#include <KoDockFactoryBase.h>

#include "kritaflake_export.h"

class KRITAFLAKE_EXPORT KoDockRegistry : public KoGenericRegistry<KoDockFactoryBase*>
{
public:
    KoDockRegistry();
    ~KoDockRegistry() override;

    static KoDockRegistry *instance();

private:

    KoDockRegistry(const KoDockRegistry&);
    KoDockRegistry operator=(const KoDockRegistry&);
    void init();

    class Private;
    Private * const d;
};

#endif
