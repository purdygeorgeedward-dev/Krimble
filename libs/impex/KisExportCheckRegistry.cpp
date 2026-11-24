/*
 * SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KisExportCheckRegistry.h"
#include <KoID.h>
#include <klocalizedstring.h>
#include <kis_image.h>
#include <KoColorSpace.h>
#include <KoColorModelStandardIds.h>
#include <KoColorSpaceRegistry.h>
#include <kis_generator_registry.h>

#include <AnimationCheck.h>
#include <ColorModelCheck.h>
#include <ColorModelHomogenousCheck.h>
#include <ColorModelPerLayerCheck.h>
#include <CompositionsCheck.h>
#include <ExifCheck.h>
#include <FillLayerTypeCheck.h>
#include <ImageSizeCheck.h>
#include <IntegralFrameDuration.h>
#include <LayerOpacityCheck.h>
#include <MultiLayerCheck.h>
#include <MultiTransparencyMaskCheck.h>
#include <NodeTypeCheck.h>
#include <PSDLayerStylesCheck.h>
#include <sRGBProfileCheck.h>
#include <ShapeLayerTypeCheck.h>

#include <QGlobalStatic>

Q_GLOBAL_STATIC(KisExportCheckRegistry, s_instance)

KisExportCheckRegistry::KisExportCheckRegistry ()
{
    KisExportCheckFactory *chkFactory = 0;

    chkFactory = new MultiLayerCheckFactory();
    add(chkFactory->id(), chkFactory);

    chkFactory = new MultiTransparencyMaskCheckFactory();
    add(chkFactory->id(), chkFactory);

    chkFactory = new AnimationCheckFactory();
    add(chkFactory->id(), chkFactory);

    chkFactory = new CompositionsCheckFactory();
    add(chkFactory->id(), chkFactory);

    chkFactory = new PSDLayerStyleCheckFactory();
    add(chkFactory->id(), chkFactory);

    chkFactory = new ExifCheckFactory();
    add(chkFactory->id(), chkFactory);

    chkFactory = new TiffExifCheckFactory();
    add(chkFactory->id(), chkFactory);

    chkFactory = new sRGBProfileCheckFactory();
    add(chkFactory->id(), chkFactory);

    chkFactory = new ImageSizeCheckFactory();
    add(chkFactory->id(), chkFactory);

    chkFactory = new ColorModelHomogenousCheckFactory();
    add(chkFactory->id(), chkFactory);

    chkFactory = new IntegralFrameDurationCheckFactory();
    add(chkFactory->id(), chkFactory);

    chkFactory = new LayerOpacityCheckFactory();
    add(chkFactory->id(), chkFactory);

    QList<KoID> allColorModels = KoColorSpaceRegistry::instance()->colorModelsList(KoColorSpaceRegistry::AllColorSpaces);
    Q_FOREACH(const KoID &colorModelID, allColorModels) {
        QList<KoID> allColorDepths = KoColorSpaceRegistry::instance()->colorDepthList(colorModelID.id(), KoColorSpaceRegistry::AllColorSpaces);
        Q_FOREACH(const KoID &colorDepthID, allColorDepths) {

            Q_ASSERT(!colorModelID.name().isEmpty());
            Q_ASSERT(!colorDepthID.name().isEmpty());

            chkFactory = new ColorModelPerLayerCheckFactory(colorModelID, colorDepthID);
            add(chkFactory->id(), chkFactory);

            chkFactory = new ColorModelCheckFactory(colorModelID, colorDepthID);
            add(chkFactory->id(), chkFactory);
        }
    }

    chkFactory = new NodeTypeCheckFactory("KisCloneLayer", i18n("Clone Layer"));
    add(chkFactory->id(), chkFactory);
    chkFactory = new NodeTypeCheckFactory("KisGroupLayer", i18nc("A group of layers", "Group"));
    add(chkFactory->id(), chkFactory);
    chkFactory = new NodeTypeCheckFactory("KisFileLayer", i18n("File Layer"));
    add(chkFactory->id(), chkFactory);
    chkFactory = new NodeTypeCheckFactory("KisShapeLayer", i18n("Vector Layer"));
    add(chkFactory->id(), chkFactory);
    chkFactory = new NodeTypeCheckFactory("KisAdjustmentLayer", i18n("Filter Layer"));
    add(chkFactory->id(), chkFactory);
    chkFactory = new NodeTypeCheckFactory("KisGeneratorLayer", i18n("Generator Layer"));
    add(chkFactory->id(), chkFactory);
    chkFactory = new NodeTypeCheckFactory("KisColorizeMask", i18n("Colorize Mask"));
    add(chkFactory->id(), chkFactory);
    chkFactory = new NodeTypeCheckFactory("KisFilterMask", i18n("Filter Mask"));
    add(chkFactory->id(), chkFactory);
    chkFactory = new NodeTypeCheckFactory("KisTransformMask", i18n("Transform Mask"));
    add(chkFactory->id(), chkFactory);
    chkFactory = new NodeTypeCheckFactory("KisTransparencyMask", i18n("Transparency Mask"));
    add(chkFactory->id(), chkFactory);
    chkFactory = new NodeTypeCheckFactory("KisSelectionMask", i18n("Selection Mask"));
    add(chkFactory->id(), chkFactory);

    for (QString generatorId : KisGeneratorRegistry::instance()->keys()) {
        chkFactory = new FillLayerTypeCheckFactory(generatorId);
        add(chkFactory->id(), chkFactory);
    }

    chkFactory = new ShapeLayerTypeCheckFactory("KoPathShape");
    add(chkFactory->id(), chkFactory);
    chkFactory = new ShapeLayerTypeCheckFactory("KoPathShape", "RectangleShape");
    add(chkFactory->id(), chkFactory);
    chkFactory = new ShapeLayerTypeCheckFactory("KoPathShape", "EllipseShape");
    add(chkFactory->id(), chkFactory);
    chkFactory = new ShapeLayerTypeCheckFactory("KoPathShape", "StarShape");
    add(chkFactory->id(), chkFactory);
    chkFactory = new ShapeLayerTypeCheckFactory("KoPathShape", "SpiralShape");
    add(chkFactory->id(), chkFactory);
    chkFactory = new ShapeLayerTypeCheckFactory("ImageShape");
    add(chkFactory->id(), chkFactory);
    chkFactory = new ShapeLayerTypeCheckFactory("KoShapeGroup");
    add(chkFactory->id(), chkFactory);
    chkFactory = new ShapeLayerTypeCheckFactory("KoSvgTextShapeID");
    add(chkFactory->id(), chkFactory);
}

KisExportCheckRegistry::~KisExportCheckRegistry ()
{
    qDeleteAll(values());
}

KisExportCheckRegistry *KisExportCheckRegistry ::instance()
{
    return s_instance;
}

