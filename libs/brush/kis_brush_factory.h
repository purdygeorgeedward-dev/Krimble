/*
 *  SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_BRUSH_FACTORY
#define KIS_BRUSH_FACTORY

#include "kis_brush.h"
#include "KisBrushModel.h"

class QDomElement;

class BRUSH_EXPORT KisBrushFactory
{

public:

    KisBrushFactory() {}
    virtual ~KisBrushFactory() {}

    virtual QString id() const = 0;

    virtual QString name() const {
        return QString();
    }

    virtual KoResourceLoadResult createBrush(const QDomElement& element, KisResourcesInterfaceSP resourcesInterface) = 0;
    virtual KoResourceLoadResult createBrush(const KisBrushModel::BrushData &data, KisResourcesInterfaceSP resourcesInterface) = 0;
    virtual std::optional<KisBrushModel::BrushData> createBrushModel(const QDomElement& element, KisResourcesInterfaceSP resourcesInterface) = 0;
    virtual void toXML(QDomDocument &doc, QDomElement &element, const KisBrushModel::BrushData &model) = 0;
};

#endif
