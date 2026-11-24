/*
 * KRIMBLE 2026-10-02: see KisFilterPhotoFilter.h
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisFilterPhotoFilter.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QVBoxLayout>

#include <KoColor.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoCompositeOpRegistry.h>
#include <KisGlobalResourcesInterface.h>
#include <filter/kis_filter_category_ids.h>
#include <filter/kis_filter_configuration.h>
#include <kis_paint_device.h>
#include <kis_color_button.h>
#include <kis_slider_spin_box.h>

KisFilterPhotoFilter::KisFilterPhotoFilter()
    : KisFilter(id(), FiltersCategoryAdjustId, i18n("&Photo Filter..."))
{
}

void KisFilterPhotoFilter::processImpl(KisPaintDeviceSP device,
                                       const QRect &rect,
                                       const KisFilterConfigurationSP config,
                                       KoUpdater *progressUpdater) const
{
    const KoColor color =
        config->getColor("color", KoColor(defaultColor(), KoColorSpaceRegistry::instance()->rgb8()));
    const int density = config->getPropertyLazy("density", defaultDensity());
    const bool preserveLuminosity = config->getPropertyLazy("preserveLuminosity", defaultPreserveLuminosity());

    KisFilterConfigurationSP overlayConfig =
        new KisFilterConfiguration(KisFilterFastColorOverlay::id().id(), 1, KisGlobalResourcesInterface::instance());
    overlayConfig->setProperty("color", color.toQColor());
    overlayConfig->setProperty("opacity", density);
    overlayConfig->setProperty("compositeop", preserveLuminosity ? COMPOSITE_COLOR : COMPOSITE_MULT);

    m_overlay.processImpl(device, rect, overlayConfig, progressUpdater);
}

KisConfigWidget *KisFilterPhotoFilter::createConfigurationWidget(QWidget *parent, const KisPaintDeviceSP dev, bool useForMasks) const
{
    Q_UNUSED(dev);
    Q_UNUSED(useForMasks);
    return new KisWdgPhotoFilter(parent);
}

KisFilterConfigurationSP KisFilterPhotoFilter::defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const
{
    KisFilterConfigurationSP config = factoryConfiguration(resourcesInterface);
    config->setProperty("color", defaultColor());
    config->setProperty("density", defaultDensity());
    config->setProperty("preserveLuminosity", defaultPreserveLuminosity());
    return config;
}

QColor KisFilterPhotoFilter::defaultColor()
{
    return QColor(236, 138, 0);
}

int KisFilterPhotoFilter::defaultDensity()
{
    return 25;
}

bool KisFilterPhotoFilter::defaultPreserveLuminosity()
{
    return true;
}

KisWdgPhotoFilter::KisWdgPhotoFilter(QWidget *parent)
    : KisConfigWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    m_colorButton = new KisColorButton(this);
    formLayout->addRow(i18n("Color:"), m_colorButton);

    m_densitySlider = new KisSliderSpinBox(this);
    m_densitySlider->setRange(0, 100);
    m_densitySlider->setSingleStep(1);
    m_densitySlider->setPageStep(10);
    m_densitySlider->setSuffix(i18n("%"));
    formLayout->addRow(i18n("Density:"), m_densitySlider);

    m_preserveLuminosity = new QCheckBox(i18n("Preserve Luminosity"), this);
    formLayout->addRow(QString(), m_preserveLuminosity);

    connect(m_colorButton, SIGNAL(changed(const KoColor&)), SIGNAL(sigConfigurationItemChanged()));
    connect(m_densitySlider, SIGNAL(valueChanged(int)), SIGNAL(sigConfigurationItemChanged()));
    connect(m_preserveLuminosity, SIGNAL(toggled(bool)), SIGNAL(sigConfigurationItemChanged()));
}

void KisWdgPhotoFilter::setConfiguration(const KisPropertiesConfigurationSP config)
{
    m_colorButton->setColor(
        config->getColor("color", KoColor(KisFilterPhotoFilter::defaultColor(), KoColorSpaceRegistry::instance()->rgb8())));
    m_densitySlider->setValue(config->getPropertyLazy("density", KisFilterPhotoFilter::defaultDensity()));
    m_preserveLuminosity->setChecked(
        config->getPropertyLazy("preserveLuminosity", KisFilterPhotoFilter::defaultPreserveLuminosity()));
}

KisPropertiesConfigurationSP KisWdgPhotoFilter::configuration() const
{
    KisFilterConfigurationSP config =
        new KisFilterConfiguration(KisFilterPhotoFilter::id().id(), 1, KisGlobalResourcesInterface::instance());

    config->setProperty("color", m_colorButton->color().toQColor());
    config->setProperty("density", m_densitySlider->value());
    config->setProperty("preserveLuminosity", m_preserveLuminosity->isChecked());

    return config;
}
