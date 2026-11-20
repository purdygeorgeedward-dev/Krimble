/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2006 Frederic Coiffier <fcoiffie@gmail.com>
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <kpluginfactory.h>

#include <filter/kis_filter_registry.h>

#include "KisLevelsFilter.h"
#include "KisAutoLevelsFilters.h"
#include "KisLevelsFilterPlugin.h"

K_PLUGIN_FACTORY_WITH_JSON(LevelsFilterFactory, "kritalevelsfilter.json", registerPlugin<KisLevelsFilterPlugin>();)

KisLevelsFilterPlugin::KisLevelsFilterPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KisFilterRegistry::instance()->add(new KisLevelsFilter());
    KisFilterRegistry::instance()->add(new KisAutoToneFilter());
    KisFilterRegistry::instance()->add(new KisAutoColorFilter());
}

KisLevelsFilterPlugin::~KisLevelsFilterPlugin()
{}

#include "KisLevelsFilterPlugin.moc"
