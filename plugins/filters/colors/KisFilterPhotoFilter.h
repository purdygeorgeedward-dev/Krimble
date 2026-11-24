/*
 * KRIMBLE 2026-10-02: Photo Filter (color filter with density).
 *
 * Built on the existing Fast Color Overlay blit (no new algorithm): the
 * chosen color is blended over the layer at the chosen density. With
 * "Preserve Luminosity" on, the "Color" blend mode is used (keeps the
 * layer's lightness); with it off, "Multiply" is used.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISFILTERPHOTOFILTER_H
#define KISFILTERPHOTOFILTER_H

#include <KoID.h>
#include <klocalizedstring.h>
#include <filter/kis_filter.h>
#include <kis_config_widget.h>

#include "KisFilterFastColorOverlay.h"

class KisColorButton;
class KisSliderSpinBox;
class QCheckBox;

class KisFilterPhotoFilter : public KisFilter
{
public:
    static QColor defaultColor();
    static int defaultDensity();
    static bool defaultPreserveLuminosity();

    KisFilterPhotoFilter();

    void processImpl(KisPaintDeviceSP device,
                     const QRect &rect,
                     const KisFilterConfigurationSP config,
                     KoUpdater *progressUpdater) const override;

    static inline KoID id()
    {
        return KoID("photofilter", i18n("Photo Filter"));
    }

    KisConfigWidget *createConfigurationWidget(QWidget *parent, const KisPaintDeviceSP dev, bool useForMasks) const override;
    KisFilterConfigurationSP defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const override;

private:
    KisFilterFastColorOverlay m_overlay;
};

class KisWdgPhotoFilter : public KisConfigWidget
{
    Q_OBJECT

public:
    explicit KisWdgPhotoFilter(QWidget *parent);

    void setConfiguration(const KisPropertiesConfigurationSP config) override;
    KisPropertiesConfigurationSP configuration() const override;

private:
    KisColorButton *m_colorButton;
    KisSliderSpinBox *m_densitySlider;
    QCheckBox *m_preserveLuminosity;
};

#endif
