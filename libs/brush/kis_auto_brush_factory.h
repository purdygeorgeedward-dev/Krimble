/*
 *  SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_AUTO_BRUSH_FACTORY
#define KIS_AUTO_BRUSH_FACTORY

#include <QString>
#include <QDomElement>

#include <KoID.h>

#include "kis_brush.h"
#include "kis_brush_factory.h"
#include "kis_fixed_paint_device.h"

class BRUSH_EXPORT KisAutoBrushFactory : public KisBrushFactory
{

public:

    KisAutoBrushFactory() {}
    ~KisAutoBrushFactory() override {}

    QString id() const override {
        return "auto_brush";
    }

    KoResourceLoadResult createBrush(const QDomElement& brushDefinition, KisResourcesInterfaceSP resourcesInterface) override;
    KoResourceLoadResult createBrush(const KisBrushModel::BrushData &data, KisResourcesInterfaceSP resourcesInterface) override;
    std::optional<KisBrushModel::BrushData> createBrushModel(const QDomElement& element, KisResourcesInterfaceSP resourcesInterface) override;
    void toXML(QDomDocument &doc, QDomElement &element, const KisBrushModel::BrushData &model) override;
    KoResourceLoadResult createBrush(const KisBrushModel::CommonData &commonData, const KisBrushModel::AutoBrushData &autoBrushData, KisResourcesInterfaceSP resourcesInterface);
};

#endif

