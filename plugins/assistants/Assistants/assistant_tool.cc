/*
 * SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "assistant_tool.h"
#include "kis_assistant_tool.h"

#include <kpluginfactory.h>
#include <kis_canvas2.h>
#include <kis_coordinates_converter.h>
#include <kis_algebra_2d.h>
#include <KoToolRegistry.h>
#include "RulerAssistant.h"
#include "EllipseAssistant.h"
#include "SplineAssistant.h"
#include "PerspectiveAssistant.h"
#include "VanishingPointAssistant.h"
#include "InfiniteRulerAssistant.h"
#include "ParallelRulerAssistant.h"
#include "ConcentricEllipseAssistant.h"
#include "FisheyePointAssistant.h"
#include "TwoPointAssistant.h"
#include "PerspectiveEllipseAssistant.h"
#include "CurvilinearPerspectiveAssistant.h"
//#include "mesh_assistant.h"

K_PLUGIN_FACTORY_WITH_JSON(AssistantToolFactory, "kritaassistanttool.json", registerPlugin<AssistantToolPlugin>();)


AssistantToolPlugin::AssistantToolPlugin(QObject *parent, const QVariantList &)
        : QObject(parent)
{
    // Krimble 2026-10-30: the Assistant tool is NOT excluded: it is registered and is in the toolbox (George's final decision of 2026-10-28, "retain all
    // toolbox tools", and again on 2026-10-30). The comment that stood here, "Krimble 1.0.45-beta: Assistant Tool excluded; guide factories remain
    // registered.", was the plan before that decision.
    KoToolRegistry::instance()->add(new KisAssistantToolFactory()); // Retained: drawing guides

    KisPaintingAssistantFactoryRegistry::instance()->add(new RulerAssistantFactory);
    KisPaintingAssistantFactoryRegistry::instance()->add(new EllipseAssistantFactory);
    KisPaintingAssistantFactoryRegistry::instance()->add(new SplineAssistantFactory);
    KisPaintingAssistantFactoryRegistry::instance()->add(new PerspectiveAssistantFactory);
    KisPaintingAssistantFactoryRegistry::instance()->add(new VanishingPointAssistantFactory);
    KisPaintingAssistantFactoryRegistry::instance()->add(new InfiniteRulerAssistantFactory);
    KisPaintingAssistantFactoryRegistry::instance()->add(new ParallelRulerAssistantFactory);
    KisPaintingAssistantFactoryRegistry::instance()->add(new ConcentricEllipseAssistantFactory);
    KisPaintingAssistantFactoryRegistry::instance()->add(new FisheyePointAssistantFactory);
    KisPaintingAssistantFactoryRegistry::instance()->add(new TwoPointAssistantFactory);
    KisPaintingAssistantFactoryRegistry::instance()->add(new PerspectiveEllipseAssistantFactory);
    KisPaintingAssistantFactoryRegistry::instance()->add(new CurvilinearPerspectiveAssistantFactory);
//    KisPaintingAssistantFactoryRegistry::instance()->add(new MeshAssistantFactory);
}

AssistantToolPlugin::~AssistantToolPlugin()
{
}

#include "assistant_tool.moc"
