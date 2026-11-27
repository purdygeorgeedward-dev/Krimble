/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2004 Michael Thaler <michael.thaler@physik.tu-muenchen.de>
 *
 * ported from digikam, Copyrighted 2004 Gilles Caulier,
 * Original Emboss algorithm copyrighted 2004 by
 * Pieter Z. Voloshyn <pieter_voloshyn at ame.com.br>.
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_emboss_filter.h"

#include <stdlib.h>
#include <vector>

#include <QPoint>
#include <QSpinBox>

#include <klocalizedstring.h>
#include <kis_debug.h>
#include <kpluginfactory.h>

#include "KoIntegerMaths.h"
#include <KoUpdater.h>

#include <kis_random_accessor_ng.h>
#include <filter/kis_filter_registry.h>
#include <kis_global.h>
#include <kis_selection.h>
#include <kis_types.h>
#include <filter/kis_filter_category_ids.h>
#include <filter/kis_filter_configuration.h>
#include <kis_paint_device.h>
#include <kis_processing_information.h>

#include "widgets/kis_multi_integer_filter_widget.h"
#include <KisSequentialIteratorProgress.h>

KisEmbossFilter::KisEmbossFilter() : KisFilter(id(), FiltersCategoryStylizeId, i18n("&Emboss with Variable Depth..."))
{
    setSupportsPainting(false);
    setColorSpaceIndependence(TO_RGBA8);
    setSupportsThreading(false);
    setSupportsAdjustmentLayers(false);
}

KisFilterConfigurationSP KisEmbossFilter::defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const
{
    KisFilterConfigurationSP config = factoryConfiguration(resourcesInterface);
    config->setProperty("depth", 30);
    return config;
}

void KisEmbossFilter::processImpl(KisPaintDeviceSP device,
                                  const QRect& applyRect,
                                  const KisFilterConfigurationSP config,
                                  KoUpdater* progressUpdater
                                  ) const
{
    QPoint srcTopLeft = applyRect.topLeft();
    Q_ASSERT(device);

    quint32 embossdepth = config ?  config->getInt("depth", 30) : 30;

    float Depth = embossdepth / 10.0;
    int R = 0, G = 0, B = 0;
    uchar Gray = 0;
    int Width = applyRect.width();
    int Height = applyRect.height();

    KisSequentialIteratorProgress it(device, applyRect, progressUpdater);
    QColor color1;
    QColor color2;
    KisRandomConstAccessorSP acc = device->createRandomAccessorNG();
    while (it.nextPixel()) {

        device->colorSpace()->toQColor(it.oldRawData(), &color1);
        acc->moveTo(srcTopLeft.x() + it.x() + Lim_Max(it.x(), 1, Width), srcTopLeft.y() + it.y() + Lim_Max(it.y(), 1, Height));

        device->colorSpace()->toQColor(acc->oldRawData(), &color2);

        R = abs((int)((color1.red() - color2.red()) * (int)Depth + (quint8_MAX / 2)));
        G = abs((int)((color1.green() - color2.green()) * (int)Depth + (quint8_MAX / 2)));
        B = abs((int)((color1.blue() - color2.blue()) * (int)Depth + (quint8_MAX / 2)));

        Gray = CLAMP((R + G + B) / 3, 0, quint8_MAX);

        device->colorSpace()->fromQColor(QColor(Gray, Gray, Gray, color1.alpha()), it.rawData());
    }
}

int KisEmbossFilter::Lim_Max(int Now, int Up, int Max) const
{
    --Max;
    while (Now > Max - Up)
        --Up;
    return (Up);
}

KisConfigWidget * KisEmbossFilter::createConfigurationWidget(QWidget* parent, const KisPaintDeviceSP dev, bool) const
{
    Q_UNUSED(dev);

    vKisIntegerWidgetParam param;
    param.push_back(KisIntegerWidgetParam(10, 300, 30, i18nc("Emboss depth", "Depth"), "depth"));
    KisConfigWidget * w = new KisMultiIntegerFilterWidget(id().id(), parent, id().id(), param);
    Q_CHECK_PTR(w);
    return w;
}
