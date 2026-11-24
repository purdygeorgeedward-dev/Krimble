/*
 *  SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "kis_text_brush_factory.h"

#include <QFont>
#include <kis_dom_utils.h>
#include "kis_text_brush.h"
#include <KoResourceLoadResult.h>

KoResourceLoadResult KisTextBrushFactory::createBrush(const QDomElement& brushDefinition, KisResourcesInterfaceSP resourcesInterface)
{
    std::optional<KisBrushModel::BrushData> data =
        createBrushModel(brushDefinition, resourcesInterface);

    if (data) {
        return createBrush(*data, resourcesInterface);
    }

    return KoResourceSignature(ResourceType::Brushes, "", "", "");
}

KoResourceLoadResult KisTextBrushFactory::createBrush(const KisBrushModel::BrushData& data, KisResourcesInterfaceSP resourcesInterface)
{
    Q_UNUSED(resourcesInterface);

    KisTextBrushSP brush = KisTextBrushSP(new KisTextBrush());

    QFont font;
    font.fromString(data.textBrush.font);

    brush->setText(data.textBrush.text);
    brush->setFont(font);
    brush->setPipeMode(data.textBrush.usePipeMode);
    brush->setSpacing(data.common.spacing);
    brush->updateBrush();

    return brush;
}

std::optional<KisBrushModel::BrushData> KisTextBrushFactory::createBrushModel(const QDomElement &element, KisResourcesInterfaceSP resourcesInterface)
{
    Q_UNUSED(resourcesInterface);

    KisBrushModel::BrushData brush;

    brush.type = KisBrushModel::Text;

    brush.common.spacing = KisDomUtils::toDouble(element.attribute("spacing", "1.0"));

    brush.textBrush.text = element.attribute("text", "The quick brown fox ate your text");
    brush.textBrush.font = element.attribute("font");
    brush.textBrush.usePipeMode = element.attribute("pipe", "false") == "true";

    brush.textBrush.scale = 1.0;

    return {brush};
}

void KisTextBrushFactory::toXML(QDomDocument &doc, QDomElement &e, const KisBrushModel::BrushData &model)
{
    Q_UNUSED(doc);

    e.setAttribute("type", id());
    e.setAttribute("BrushVersion", "2");

    e.setAttribute("spacing", KisDomUtils::toString(model.common.spacing));
    e.setAttribute("text", model.textBrush.text);
    e.setAttribute("font", model.textBrush.font);
    e.setAttribute("pipe", model.textBrush.usePipeMode ? "true" : "false");
}
