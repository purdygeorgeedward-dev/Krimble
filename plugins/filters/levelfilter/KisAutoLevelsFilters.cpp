/*
 * KRIMBLE 2026-10-02: see KisAutoLevelsFilters.h
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisAutoLevelsFilters.h"

#include <QScopedPointer>
#include <QVector>

#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoColorTransformation.h>
#include <KoColorModelStandardIds.h>
#include <KoHistogramProducer.h>
#include <KoBasicHistogramProducers.h>
#include <KisGlobalResourcesInterface.h>
#include <KisSequentialIteratorProgress.h>
#include <filter/kis_filter_category_ids.h>
#include <filter/kis_filter_configuration.h>
#include <kis_paint_device.h>
#include <kis_processing_information.h>
#include <KoUpdater.h>
#include <kis_histogram.h>
#include <kis_types.h>

#include "../colorsfilters/kis_multichannel_utils.h"
#include "KisLevelsFilter.h"
#include "KisLevelsFilterConfiguration.h"
#include "KrimbleAutoAdjust.h"

KisAutoLevelsFilterBase::KisAutoLevelsFilterBase(const KoID &id,
                                                 const QString &menuEntry,
                                                 KisAutoLevels::MidtonesAdjustmentMethod midtonesMethod,
                                                 qreal midtonesAmount)
    : KisFilter(id, FiltersCategoryAdjustId, menuEntry)
    , m_midtonesMethod(midtonesMethod)
    , m_midtonesAmount(midtonesAmount)
{
    // The histogram is computed over the whole apply rect, so no threading.
    // No dialog: the filter applies immediately.
    setSupportsPainting(false);
    setSupportsThreading(false);
    setSupportsAdjustmentLayers(false);
    setColorSpaceIndependence(FULLY_INDEPENDENT);
    setShowConfigurationWidget(false);
}

void KisAutoLevelsFilterBase::processImpl(KisPaintDeviceSP device,
                                          const QRect &applyRect,
                                          const KisFilterConfigurationSP config,
                                          KoUpdater *progressUpdater) const
{
    Q_UNUSED(config);
    Q_ASSERT(device != 0);

    // Krimble: the curves are now worked out by KrimbleAutoAdjust (Auto Tone: each channel stretched on its own;
    // Auto Color: dark and light colors, neutral midtones, saturation lift). The old Levels-engine code is kept below, not used.
    KrimbleAutoAdjust::apply(device, applyRect,
                             id() == KisAutoColorFilter::id().id() ? KrimbleAutoAdjust::Color : KrimbleAutoAdjust::Tone,
                             progressUpdater);
#if 0
    const KoColorSpace *cs = device->colorSpace();

    // Same restriction as the "auto levels for all channels" button in the
    // Levels dialog (KisLevelsConfigWidget.cpp): RGB only.
    if (cs->colorModelId() != RGBAColorModelID) {
        return;
    }

    // Histogram of the real channels (built the same way as in
    // KisLevelsConfigWidget::updateHistograms()).
    const QList<QString> keys = KoHistogramProducerFactoryRegistry::instance()->keysCompatibleWith(cs);
    if (keys.isEmpty()) {
        return;
    }
    KoHistogramProducerFactory *hpf = KoHistogramProducerFactoryRegistry::instance()->get(keys.at(0));
    KisHistogram histogram(device, applyRect, hpf->generate(), LINEAR);

    // Same virtual channel list the Levels dialog uses, so that the
    // configuration below has the channel layout the Levels transformation
    // expects.
    const QVector<VirtualChannelInfo> virtualChannels = KisMultiChannelUtils::getVirtualChannels(cs);

    QVector<KisAutoLevels::ChannelHistogram> channelsHistograms;
    for (const VirtualChannelInfo &virtualChannelInfo : virtualChannels) {
        if (virtualChannelInfo.type() == VirtualChannelInfo::REAL && !virtualChannelInfo.isAlpha()) {
            channelsHistograms.append({&histogram, virtualChannelInfo.pixelIndex()});
        }
    }
    if (channelsHistograms.isEmpty()) {
        return;
    }

    // Output: black stays black, white stays white, target midtone is 50% gray.
    QVector<qreal> shadowsOutput(channelsHistograms.size(), 0.0);
    QVector<qreal> highlightsOutput(channelsHistograms.size(), 1.0);
    QVector<qreal> midtonesOutput(channelsHistograms.size(), 0.5);

    const QVector<KisLevelsCurve> autoCurves =
        KisAutoLevels::adjustPerChannelContrast(
            channelsHistograms,
            0.001, // shadows clipping: 0.1% (default of the Levels auto dialog)
            0.001, // highlights clipping: 0.1% (default of the Levels auto dialog)
            1.0,   // maximum input black/white offset: 100% (dialog default)
            m_midtonesMethod,
            m_midtonesAmount,
            shadowsOutput,
            highlightsOutput,
            midtonesOutput);

    // Put the computed curves into a Levels configuration, one per real
    // channel, leaving the other virtual channels at identity.
    KisLevelsFilterConfiguration *levelsConfig =
        new KisLevelsFilterConfiguration(virtualChannels.size(), KisGlobalResourcesInterface::instance());
    KisFilterConfigurationSP levelsConfigSP(levelsConfig);

    QVector<KisLevelsCurve> allCurves = levelsConfig->levelsCurves();
    for (int i = 0, j = 0; i < virtualChannels.size() && j < autoCurves.size(); ++i) {
        if (virtualChannels[i].type() == VirtualChannelInfo::REAL && !virtualChannels[i].isAlpha()) {
            allCurves[i] = autoCurves[j];
            ++j;
        }
    }
    levelsConfig->setLevelsCurves(allCurves);
    // Per-channel mode (not the single lightness curve).
    levelsConfig->setUseLightnessMode(false);

    // Apply through the existing Levels transformation.
    KisLevelsFilter levelsFilter;
    QScopedPointer<KoColorTransformation> adj(levelsFilter.createTransformation(cs, levelsConfigSP));
    if (!adj) {
        return;
    }

    KisSequentialIteratorProgress it(device, applyRect, progressUpdater);
    quint32 npix = it.nConseqPixels();
    while (it.nextPixels(npix)) {
        npix = it.nConseqPixels();
        adj->transform(it.oldRawData(), it.rawData(), npix);
    }
#endif
}

KisAutoToneFilter::KisAutoToneFilter()
    : KisAutoLevelsFilterBase(id(),
                              i18n("&Auto Tone"),
                              KisAutoLevels::MidtonesAdjustmentMethod_None,
                              0.0)
{
}

KisAutoColorFilter::KisAutoColorFilter()
    : KisAutoLevelsFilterBase(id(),
                              i18n("Auto Co&lor"),
                              KisAutoLevels::MidtonesAdjustmentMethod_UseMean,
                              1.0)
{
}
