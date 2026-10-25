/*
 * default_tools.cc -- Part of Krita
 *
 * SPDX-FileCopyrightText: 2004 Boudewijn Rempt (boud@valdyas.org)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "default_tools.h"

#include <kis_debug.h>
#include <kpluginfactory.h>

#include <kis_tool.h>
#include <KoToolRegistry.h>

#include "kis_paint_device.h"
#include "kis_tool_fill.h"
#include "kis_tool_brush.h"
#include "kis_tool_smudge.h"
#include "kis_tool_soften.h"
#include "kis_tool_eraser.h"   // Krimble 2026-10-25
#include "kis_tool_dodge.h"
#include "kis_tool_burn.h"
#include "kis_tool_multihand.h"
#include "kis_tool_freehand.h"
#include "kis_tool_gradient.h"
#include "kis_tool_rectangle.h"
#include "kis_tool_colorsampler.h"
#include "kis_tool_line.h"
#include "kis_tool_ellipse.h"
#include "kis_tool_measure.h"
#include "kis_tool_path.h"
#include "kis_tool_move.h"
#include "kis_tool_pencil.h"
#include "kis_tool_pan.h"

K_PLUGIN_FACTORY_WITH_JSON(DefaultToolsFactory, "kritadefaulttools.json", registerPlugin<DefaultTools>();)


DefaultTools::DefaultTools(QObject *parent, const QVariantList &)
        : QObject(parent)
{
    KoToolRegistry::instance()->add(new KisToolFillFactory());
    KoToolRegistry::instance()->add(new KisToolGradientFactory());
    KoToolRegistry::instance()->add(new KisToolBrushFactory());
    KoToolRegistry::instance()->add(new KisToolSmudgeFactory()); // Krimble: dedicated Smudge tool (colorsmudge paintop)
    KoToolRegistry::instance()->add(new KisToolSoftenFactory()); // Krimble: dedicated Soften tool (filterop + Gaussian Blur)
    KoToolRegistry::instance()->add(new KisToolEraserFactory()); // Krimble 2026-10-25: dedicated Eraser tool (the brush with the eraser switch on)
    // Krimble 2026-10-20: George (1.0.40, test list, item 14): "Dodge and Burn suck. It's just black and white. Comment out these tools for now." and
    // "Dodge and Burn do nothing more than a simple brush could do, and they add clutter to an already cluttered toolbox." Not registered, so they are
    // not in the toolbox; their code (kis_tool_dodge.cc, kis_tool_burn.cc) stays and is still built, ready to come back as real exposure tools.
    // KoToolRegistry::instance()->add(new KisToolDodgeFactory()); // Krimble: dedicated Dodge tool (default brush + CompositeOp=dodge)
    // KoToolRegistry::instance()->add(new KisToolBurnFactory()); // Krimble: dedicated Burn tool (default brush + CompositeOp=burn)
    KoToolRegistry::instance()->add(new KisToolColorSamplerFactory());
    KoToolRegistry::instance()->add(new KisToolLineFactory());
    KoToolRegistry::instance()->add(new KisToolEllipseFactory());
    KoToolRegistry::instance()->add(new KisToolRectangleFactory());
    KoToolRegistry::instance()->add(new KisToolMeasureFactory());
    KoToolRegistry::instance()->add(new KisToolPathFactory());
    KoToolRegistry::instance()->add(new KisToolMoveFactory());
    // KRIMBLE 2026-10-03: removed from the toolbox at George's request so the first button is the Move tool (industry-standard order). Uncomment to bring it back. (Multibrush, Pencil)
    // KoToolRegistry::instance()->add(new KisToolMultiBrushFactory());
    // KoToolRegistry::instance()->add(new KisToolPencilFactory());
    KoToolRegistry::instance()->add(new KisToolPanFactory());
}

DefaultTools::~DefaultTools()
{
}

#include "default_tools.moc"
