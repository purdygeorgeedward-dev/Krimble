/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2010-2011 José Luis Vergara <pentalis@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_PHONG_BUMPMAP_FILTER_H
#define KIS_PHONG_BUMPMAP_FILTER_H

#include <kis_types.h>
#include <filter/kis_filter.h>

class KisFilterPhongBumpmap : public KisFilter
{
public:
    KisFilterPhongBumpmap();

public:

    void processImpl(KisPaintDeviceSP device,
                     const QRect& applyRect,
                     const KisFilterConfigurationSP config,
                     KoUpdater *progressUpdater
                     ) const override;

    QRect neededRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const override;
    QRect changedRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const override;

    KisConfigWidget *createConfigurationWidget(QWidget *parent, const KisPaintDeviceSP dev, bool useForMasks) const override;
    KisFilterConfigurationSP defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const override;
private:
};

#endif
