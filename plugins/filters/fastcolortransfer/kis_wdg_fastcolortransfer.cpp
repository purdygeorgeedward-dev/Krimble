/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_wdg_fastcolortransfer.h"

#include <QLayout>

#include <filter/kis_filter_configuration.h>
#include <KisDocument.h>
#include <KisPart.h>
#include <kis_image.h>
#include <kis_iterator_ng.h>
#include <kis_paint_device.h>
#include <kis_selection.h>
#include <kis_node.h>
#include <kis_group_layer.h>
#include <kundo2command.h>
#include <KoColorSpaceRegistry.h>
#include <kis_slider_spin_box.h>
#include <klocalizedstring.h>
#include "ui_wdgfastcolortransfer.h"
#include <KisGlobalResourcesInterface.h>

KisWdgFastColorTransfer::KisWdgFastColorTransfer(QWidget * parent) : KisConfigWidget(parent)
{
    m_widget = new Ui_WdgFastColorTransfer();
    m_widget->setupUi(this);

    m_widget->sldLuminance->setRange(1, 200);
    m_widget->sldLuminance->setValue(100);
    m_widget->sldLuminance->setSuffix(i18n("%"));

    m_widget->sldColorIntensity->setRange(1, 200);
    m_widget->sldColorIntensity->setValue(100);
    m_widget->sldColorIntensity->setSuffix(i18n("%"));

    m_widget->sldFade->setRange(0, 100);
    m_widget->sldFade->setValue(0);
    m_widget->sldFade->setSuffix(i18n("%"));

    populateSourceCombo();
    connect(m_widget->cmbSource, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSourceChanged(int)));
    slotSourceChanged(m_widget->cmbSource->currentIndex());

    connect(m_widget->cmbSource, SIGNAL(currentIndexChanged(int)), this, SIGNAL(sigConfigurationItemChanged()));
    connect(m_widget->cmbLayer, SIGNAL(currentIndexChanged(int)), this, SIGNAL(sigConfigurationItemChanged()));
    connect(m_widget->chkIgnoreSelection, SIGNAL(toggled(bool)), this, SIGNAL(sigConfigurationItemChanged()));
    connect(m_widget->chkUseSelectionSource, SIGNAL(toggled(bool)), this, SIGNAL(sigConfigurationItemChanged()));
    connect(m_widget->chkUseSelectionTarget, SIGNAL(toggled(bool)), this, SIGNAL(sigConfigurationItemChanged()));
    connect(m_widget->chkNeutralize, SIGNAL(toggled(bool)), this, SIGNAL(sigConfigurationItemChanged()));
    connect(m_widget->sldLuminance, SIGNAL(valueChanged(int)), this, SIGNAL(sigConfigurationItemChanged()));
    connect(m_widget->sldColorIntensity, SIGNAL(valueChanged(int)), this, SIGNAL(sigConfigurationItemChanged()));
    connect(m_widget->sldFade, SIGNAL(valueChanged(int)), this, SIGNAL(sigConfigurationItemChanged()));
}


KisWdgFastColorTransfer::~KisWdgFastColorTransfer()
{
    delete m_widget;
}

void KisWdgFastColorTransfer::populateSourceCombo()
{
    m_documents = KisPart::instance()->documents();
    m_widget->cmbSource->clear();
    m_widget->cmbSource->addItem(i18n("None"));
    Q_FOREACH (const QPointer<KisDocument> &doc, m_documents) {
        if (doc) {
            m_widget->cmbSource->addItem(doc->caption());
        }
    }
}

void KisWdgFastColorTransfer::populateLayerCombo(KisDocument *doc)
{
    m_widget->cmbLayer->clear();
    if (!doc || !doc->image()) {
        m_widget->cmbLayer->setEnabled(false);
        return;
    }
    m_widget->cmbLayer->setEnabled(true);
    m_widget->cmbLayer->addItem(i18n("Merged"));

    KisNodeSP node = doc->image()->rootLayer()->firstChild();
    while (node) {
        m_widget->cmbLayer->addItem(node->name());
        node = node->nextSibling();
    }
}

void KisWdgFastColorTransfer::slotSourceChanged(int index)
{
    if (index <= 0 || index > m_documents.size()) {
        m_widget->cmbLayer->clear();
        m_widget->cmbLayer->setEnabled(false);
        return;
    }
    populateLayerCombo(m_documents.at(index - 1));
}

void KisWdgFastColorTransfer::setConfiguration(const KisPropertiesConfigurationSP config)
{
    QVariant value;
    if (config->getProperty("ignoreSelection", value)) {
        m_widget->chkIgnoreSelection->setChecked(value.toBool());
    }
    if (config->getProperty("useSelectionSource", value)) {
        m_widget->chkUseSelectionSource->setChecked(value.toBool());
    }
    if (config->getProperty("useSelectionTarget", value)) {
        m_widget->chkUseSelectionTarget->setChecked(value.toBool());
    }
    if (config->getProperty("neutralize", value)) {
        m_widget->chkNeutralize->setChecked(value.toBool());
    }
    if (config->getProperty("luminance", value)) {
        m_widget->sldLuminance->setValue(value.toInt());
    }
    if (config->getProperty("colorIntensity", value)) {
        m_widget->sldColorIntensity->setValue(value.toInt());
    }
    if (config->getProperty("fade", value)) {
        m_widget->sldFade->setValue(value.toInt());
    }
}

namespace {

void computeLabStatistics(KisPaintDeviceSP device, const QRect &bounds, KisSelectionSP selection,
                           double &meanL, double &meanA, double &meanB,
                           double &sigmaL, double &sigmaA, double &sigmaB)
{
    const KoColorSpace* labCS = KoColorSpaceRegistry::instance()->lab16();

    KisPaintDeviceSP lab = new KisPaintDevice(*device.data());
    lab->convertTo(labCS, KoColorConversionTransformation::internalRenderingIntent(), KoColorConversionTransformation::internalConversionFlags());

    meanL = meanA = meanB = 0.;
    sigmaL = sigmaA = sigmaB = 0.;

    QRect rc = bounds;
    if (selection) {
        rc = rc.intersected(selection->selectedExactRect());
    }
    if (rc.isEmpty()) return;

    // Krimble 2026-10-26: each pixel counts as much as it is opaque (see fastcolortransfer.cpp): the transparent pixels of a cut-out are not part of its colors.
    double count = 0.;
    KisSequentialConstIterator it(lab, rc);
    while (it.nextPixel()) {
        // Krimble: sloppy first pass -- iterates the full rc rect rather
        // than skipping pixels outside an irregular selection's exact
        // shape when `selection` is set. Good enough for a rectangular or
        // roughly-rectangular selection; an irregular one will pull in a
        // few extra unselected pixels at the edges of its bounding rect.
        const quint16* data = reinterpret_cast<const quint16*>(it.oldRawData());
        const double w = double(data[3]) / 65535.0;
        if (w <= 0.) continue;
        meanL += w * data[0];
        meanA += w * data[1];
        meanB += w * data[2];
        sigmaL += w * double(data[0]) * data[0];
        sigmaA += w * double(data[1]) * data[1];
        sigmaB += w * double(data[2]) * data[2];
        count += w;
    }

    if (count <= 0.) return;
    const double totalSize = 1.0 / count;
    meanL *= totalSize; meanA *= totalSize; meanB *= totalSize;
    sigmaL *= totalSize; sigmaA *= totalSize; sigmaB *= totalSize;
}

}

KisPropertiesConfigurationSP KisWdgFastColorTransfer::configuration() const
{
    KisFilterConfigurationSP config = new KisFilterConfiguration("colortransfer", 1, KisGlobalResourcesInterface::instance());

    config->setProperty("ignoreSelection", m_widget->chkIgnoreSelection->isChecked());
    config->setProperty("useSelectionSource", m_widget->chkUseSelectionSource->isChecked());
    config->setProperty("useSelectionTarget", m_widget->chkUseSelectionTarget->isChecked());
    config->setProperty("neutralize", m_widget->chkNeutralize->isChecked());
    config->setProperty("luminance", m_widget->sldLuminance->value());
    config->setProperty("colorIntensity", m_widget->sldColorIntensity->value());
    config->setProperty("fade", m_widget->sldFade->value());

    const int sourceIndex = m_widget->cmbSource->currentIndex();
    const bool hasSource = sourceIndex > 0 && sourceIndex <= m_documents.size();
    config->setProperty("hasSource", hasSource);

    if (!hasSource) {
        // "None" source: PS uses the target image's own statistics here,
        // meaningful mainly combined with Neutralize (remove the target's
        // own color cast). processImpl() falls back to the target's own
        // stats as the reference when hasSource is false.
        return config;
    }

    KisDocument *sourceDoc = m_documents.at(sourceIndex - 1);
    if (!sourceDoc || !sourceDoc->image()) {
        config->setProperty("hasSource", false);
        return config;
    }

    KisImageSP sourceImage = sourceDoc->image();
    KisPaintDeviceSP ref;

    const int layerIndex = m_widget->cmbLayer->currentIndex();
    if (layerIndex <= 0) {
        // "Merged" (or nothing selected yet) -- use the flattened projection.
        ref = sourceImage->projection();
    } else {
        KisNodeSP node = sourceImage->rootLayer()->firstChild();
        int i = 1;
        while (node && i < layerIndex) {
            node = node->nextSibling();
            ++i;
        }
        ref = node ? node->paintDevice() : sourceImage->projection();
    }

    if (!ref) {
        config->setProperty("hasSource", false);
        return config;
    }

    KisSelectionSP sourceSelection;
    if (m_widget->chkUseSelectionSource->isChecked()) {
        sourceSelection = sourceImage->globalSelection();
    }

    double meanL_ref, meanA_ref, meanB_ref, sigmaL_ref, sigmaA_ref, sigmaB_ref;
    computeLabStatistics(ref, sourceImage->bounds(), sourceSelection,
                         meanL_ref, meanA_ref, meanB_ref, sigmaL_ref, sigmaA_ref, sigmaB_ref);

    config->setProperty("meanL", meanL_ref);
    config->setProperty("meanA", meanA_ref);
    config->setProperty("meanB", meanB_ref);
    config->setProperty("sigmaL", sigmaL_ref);
    config->setProperty("sigmaA", sigmaA_ref);
    config->setProperty("sigmaB", sigmaB_ref);

    return config;
}
