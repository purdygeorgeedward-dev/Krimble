/*
 *  SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_TEXT_BRUSH_FACTORY
#define KIS_TEXT_BRUSH_FACTORY

#include <QString>
#include <QDomElement>
#include "kis_brush_factory.h"
#include "kis_brush.h"

class BRUSH_EXPORT KisTextBrushFactory : public KisBrushFactory
{

public:

    KisTextBrushFactory() {}
    ~KisTextBrushFactory() override {}

    QString id() const override {
        return "kis_text_brush";
    }

    KoResourceLoadResult createBrush(const QDomElement& brushDefinition, KisResourcesInterfaceSP resourcesInterface) override;
    KoResourceLoadResult createBrush(const KisBrushModel::BrushData &data, KisResourcesInterfaceSP resourcesInterface) override;
    std::optional<KisBrushModel::BrushData> createBrushModel(const QDomElement &element, KisResourcesInterfaceSP resourcesInterface) override;
    void toXML(QDomDocument &doc, QDomElement &element, const KisBrushModel::BrushData &model) override;

};

#endif
