/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_PAINTOP_REGISTRY_H_
#define KIS_PAINTOP_REGISTRY_H_

#include <QObject>

#include "KoGenericRegistry.h"

#include "kis_paintop.h"
#include <brushengine/kis_paintop_factory.h>
#include "kis_types.h"
#include <brushengine/kis_paintop_settings.h>
#include <brushengine/kis_paintop_preset.h>
#include <kis_threaded_text_rendering_workaround.h>

#include <kritaimage_export.h>

class KisPaintOp;
class KisPainter;

class KRITAIMAGE_EXPORT KisPaintOpRegistry : public QObject, public KoGenericRegistry<KisPaintOpFactory*>
{

    Q_OBJECT

public:
    KisPaintOpRegistry();
    ~KisPaintOpRegistry() override;

#ifdef HAVE_THREADED_TEXT_RENDERING_WORKAROUND
    void preinitializePaintOpIfNeeded(const KisPaintOpPresetSP preset);
#endif

    KisPaintOp* paintOp(const KisPaintOpPresetSP preset, KisPainter * painter, KisNodeSP node, KisImageSP image) const;

    KisInterstrokeDataFactory* createInterstrokeDataFactory(KisPaintOpPresetSP preset) const;

    KisPaintOpSettingsSP createSettings(const KoID& id, KisResourcesInterfaceSP resourcesInterface) const;

    KisPaintOpPresetSP defaultPreset(const KoID& id, KisResourcesInterfaceSP resourcesInterface) const;

    QIcon icon(const KoID & id) const;

    QList<KoID> listKeys() const;

public:

    static KisPaintOpRegistry* instance();

private:

    KisPaintOpRegistry(const KisPaintOpRegistry&);
    KisPaintOpRegistry operator=(const KisPaintOpRegistry&);
    void initRegistry();

    friend class KisPaintOpSettings;

    KisPaintOp * paintOp(const QString& id, const KisPaintOpSettingsSP settings, KisPainter * painter, KisNodeSP node, KisImageSP image) const;

};

#endif

