/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "fastcolortransfer.h"

#include <math.h>

#include <kpluginfactory.h>

#include <kundo2command.h>

#include <KoColorSpaceRegistry.h>
#include <KoUpdater.h>

#include <filter/kis_filter_registry.h>
#include <kis_image.h>
#include <kis_paint_device.h>
#include <kis_selection.h>
#include <filter/kis_filter_category_ids.h>
#include <filter/kis_filter_configuration.h>
#include <kis_processing_information.h>

#include "kis_wdg_fastcolortransfer.h"
#include "ui_wdgfastcolortransfer.h"
#include <KisSequentialIteratorProgress.h>
#include <KoProgressUpdater.h>


K_PLUGIN_FACTORY_WITH_JSON(KritaFastColorTransferFactory, "kritafastcolortransfer.json", registerPlugin<FastColorTransferPlugin>();)


FastColorTransferPlugin::FastColorTransferPlugin(QObject *parent, const QVariantList &)
        : QObject(parent)
{
    KisFilterRegistry::instance()->add(new KisFilterFastColorTransfer());

}

FastColorTransferPlugin::~FastColorTransferPlugin()
{
}

KisFilterFastColorTransfer::KisFilterFastColorTransfer() : KisFilter(id(), FiltersCategoryAdjustId, i18n("&Match Color..."))
{
    setColorSpaceIndependence(FULLY_INDEPENDENT);
    setSupportsThreading(false);
    setSupportsPainting(false);
    setSupportsAdjustmentLayers(false);
}


KisConfigWidget * KisFilterFastColorTransfer::createConfigurationWidget(QWidget* parent, const KisPaintDeviceSP dev, bool) const
{
    Q_UNUSED(dev);
    return new KisWdgFastColorTransfer(parent);
}

KisFilterConfigurationSP KisFilterFastColorTransfer::defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const
{
    KisFilterConfigurationSP config = factoryConfiguration(resourcesInterface);
    config->setProperty("filename", "");
    return config;
}

#define CLAMP(x,l,u) ((x)<(l)?(l):((x)>(u)?(u):(x)))

void KisFilterFastColorTransfer::processImpl(KisPaintDeviceSP device,
                                             const QRect& applyRect,
                                             const KisFilterConfigurationSP config,
                                             KoUpdater* progressUpdater) const
{
    Q_ASSERT(device != 0);

    dbgPlugins << "Start transferring color";

    // Convert ref and src to LAB
    const KoColorSpace* labCS = KoColorSpaceRegistry::instance()->lab16();
    if (!labCS) {
        dbgPlugins << "The LAB colorspace is not available.";
        return;
    }
    
    dbgPlugins << "convert a copy of src to lab";
    const KoColorSpace* oldCS = device->colorSpace();
    KisPaintDeviceSP srcLAB = new KisPaintDevice(*device.data());
    dbgPlugins << "srcLab : " << srcLAB->extent();
    srcLAB->convertTo(labCS, KoColorConversionTransformation::internalRenderingIntent(), KoColorConversionTransformation::internalConversionFlags());

    KoProgressUpdater compositeUpdater(progressUpdater, KoProgressUpdater::Unthreaded);
    KoUpdater *updaterStats = compositeUpdater.startSubtask(1);
    KoUpdater *updaterMap = compositeUpdater.startSubtask(2);

    // Compute the means and sigmas of src
    dbgPlugins << "Compute the means and sigmas of src";
    double meanL_src = 0., meanA_src = 0., meanB_src = 0.;
    double sigmaL_src = 0., sigmaA_src = 0., sigmaB_src = 0.;

    // Krimble 2026-10-26: George, about a cut-out picture with a transparent background: the statistics counted every pixel of the rectangle, the transparent ones (black)
    // too, so they were pulled toward black and the match was wrong. Now each pixel counts as much as it is opaque (a weight from its alpha, so a fully transparent pixel
    // counts nothing and the soft edge of a cut-out counts a little). A picture with no transparent pixel gives exactly the numbers it gave before.
    double totalWeight = 0.;
    {
        KisSequentialConstIteratorProgress srcIt(srcLAB, applyRect, updaterStats);
        while (srcIt.nextPixel()) {
            const quint16* data = reinterpret_cast<const quint16*>(srcIt.oldRawData());
            const double w = double(data[3]) / 65535.0;
            if (w <= 0.) continue;
            const double L = data[0];
            const double A = data[1];
            const double B = data[2];
            meanL_src += w * L;
            meanA_src += w * A;
            meanB_src += w * B;
            sigmaL_src += w * L * L;
            sigmaA_src += w * A * A;
            sigmaB_src += w * B * B;
            totalWeight += w;
        }
    }
    
    // double totalSize = 1. / (applyRect.width() * applyRect.height());   // the old divisor: every pixel of the rectangle, transparent or not
    double totalSize = totalWeight > 0. ? 1. / totalWeight : 1. / (applyRect.width() * applyRect.height());
    meanL_src *= totalSize;
    meanA_src *= totalSize;
    meanB_src *= totalSize;
    sigmaL_src *= totalSize;
    sigmaA_src *= totalSize;
    sigmaB_src *= totalSize;
    
    dbgPlugins << totalSize << "" << meanL_src << "" << meanA_src << "" << meanB_src << "" << sigmaL_src << "" << sigmaA_src << "" << sigmaB_src;

    // Krimble: "None" source (matched PS's own documented behavior) uses
    // the target's own statistics as the reference -- a no-op on its own,
    // but meaningful combined with Neutralize below.
    const bool hasSource = config->getBool("hasSource", false);
    double meanL_ref = hasSource ? config->getDouble("meanL") : meanL_src;
    double meanA_ref = hasSource ? config->getDouble("meanA") : meanA_src;
    double meanB_ref = hasSource ? config->getDouble("meanB") : meanB_src;
    double sigmaL_ref = hasSource ? config->getDouble("sigmaL") : sigmaL_src;
    double sigmaA_ref = hasSource ? config->getDouble("sigmaA") : sigmaA_src;
    double sigmaB_ref = hasSource ? config->getDouble("sigmaB") : sigmaB_src;

    // Krimble: Neutralize forces the reference's a/b (chroma) means to the
    // Lab colorspace's neutral/gray point, removing whatever overall color
    // cast the match would otherwise have introduced -- independent of
    // whether an external source is in use, matching PS's own checkbox.
    if (config->getBool("neutralize", false)) {
        const double neutralAB = 32768.0; // Krita's lab16 encodes neutral a/b at the midpoint of the unsigned 16-bit range
        meanA_ref = neutralAB;
        meanB_ref = neutralAB;
    }

    const double luminancePct = config->getInt("luminance", 100) / 100.0;
    const double colorIntensityPct = config->getInt("colorIntensity", 100) / 100.0;
    const double fadePct = config->getInt("fade", 0) / 100.0;

    // Krimble: known simplification for this first pass -- useSelectionTarget
    // and ignoreSelection are read but not yet fully wired: target statistics
    // above are always computed over the whole applyRect (which is already
    // selection-bounded by Krita's normal filter-application behavior when a
    // selection is active, just not selection-*shaped* the way a precise
    // per-pixel mask would be), and the filter doesn't yet force whole-image
    // application when a selection exists and Ignore Selection is checked.

    // Transfer colors
    dbgPlugins << "Transfer colors";
    {
        // Krimble 2026-10-26: a picture of one flat color (no spread) divided by zero and gave NaN: then the factor is 1 (the pixel keeps its distance from the mean).
        auto spreadRatio = [](double refSigma, double refMean, double srcSigma, double srcMean) {
            const double num = refSigma - refMean * refMean;
            const double den = srcSigma - srcMean * srcMean;
            return (den > 1e-6 && num >= 0.) ? sqrt(num / den) : 1.0;
        };
        // double coefL = sqrt((sigmaL_ref - meanL_ref * meanL_ref) / (sigmaL_src - meanL_src * meanL_src));
        // double coefA = sqrt((sigmaA_ref - meanA_ref * meanA_ref) / (sigmaA_src - meanA_src * meanA_src));
        // double coefB = sqrt((sigmaB_ref - meanB_ref * meanB_ref) / (sigmaB_src - meanB_src * meanB_src));
        double coefL = spreadRatio(sigmaL_ref, meanL_ref, sigmaL_src, meanL_src);
        double coefA = spreadRatio(sigmaA_ref, meanA_ref, sigmaA_src, meanA_src);
        double coefB = spreadRatio(sigmaB_ref, meanB_ref, sigmaB_src, meanB_src);

        const double neutralAB = 32768.0;
        quint16 labPixel[4];

        KisSequentialConstIteratorProgress srcLabIt(srcLAB, applyRect, updaterMap);
        KisSequentialIterator dstIt(device, applyRect);
        while (srcLabIt.nextPixel() && dstIt.nextPixel()) {
            const quint16* data = reinterpret_cast<const quint16*>(srcLabIt.oldRawData());

            double matchedL = ((double)data[0] - meanL_src) * coefL + meanL_ref;
            double matchedA = ((double)data[1] - meanA_src) * coefA + meanA_ref;
            double matchedB = ((double)data[2] - meanB_src) * coefB + meanB_ref;

            // Luminance: direct brightness scale on the matched result.
            matchedL *= luminancePct;
            // Color Intensity: scales chroma (distance from neutral gray);
            // a value of 1% collapses a/b to neutral, i.e. grayscale.
            matchedA = neutralAB + (matchedA - neutralAB) * colorIntensityPct;
            matchedB = neutralAB + (matchedB - neutralAB) * colorIntensityPct;

            // Fade: blend back toward the original, unmatched pixel.
            matchedL = matchedL * (1.0 - fadePct) + (double)data[0] * fadePct;
            matchedA = matchedA * (1.0 - fadePct) + (double)data[1] * fadePct;
            matchedB = matchedB * (1.0 - fadePct) + (double)data[2] * fadePct;

            labPixel[0] = (quint16)CLAMP(matchedL, 0., 65535.);
            labPixel[1] = (quint16)CLAMP(matchedA, 0., 65535.);
            labPixel[2] = (quint16)CLAMP(matchedB, 0., 65535.);
            labPixel[3] = data[3];
            oldCS->fromLabA16(reinterpret_cast<const quint8*>(labPixel), dstIt.rawData(), 1);
        }
    }
}

#include "fastcolortransfer.moc"
