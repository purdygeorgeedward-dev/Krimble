/*
 * Krimble: Filter > Render > Lens Flare... (the math is in KrimbleLensFlare.h).
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISFILTERLENSFLARE_H
#define KISFILTERLENSFLARE_H

#include <KoID.h>
#include <klocalizedstring.h>
#include <filter/kis_filter.h>
#include <kis_config_widget.h>
#include <QPointer>

class KisSliderSpinBox;
class QComboBox;

class KisFilterLensFlare : public KisFilter
{
public:
    KisFilterLensFlare();

    void processImpl(KisPaintDeviceSP device,
                     const QRect &rect,
                     const KisFilterConfigurationSP config,
                     KoUpdater *progressUpdater) const override;

    static inline KoID id()
    {
        return KoID("krimblelensflare", i18n("Lens Flare"));
    }

    KisConfigWidget *createConfigurationWidget(QWidget *parent, const KisPaintDeviceSP dev, bool useForMasks) const override;
    KisFilterConfigurationSP defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const override;
    QRect neededRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const override;
    QRect changedRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const override;
};

class KisWdgLensFlare : public KisConfigWidget
{
    Q_OBJECT

public:
    explicit KisWdgLensFlare(QWidget *parent);

    void setConfiguration(const KisPropertiesConfigurationSP config) override;
    KisPropertiesConfigurationSP configuration() const override;
    void setView(KisViewManager *view) override;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    bool moveLightTo(const QPointF &widgetPoint);
    KisViewManager *m_view {nullptr};
    QPointer<QWidget> m_canvasWidget;
    QComboBox *m_type;
    KisSliderSpinBox *m_x;
    KisSliderSpinBox *m_y;
    KisSliderSpinBox *m_brightness;
    KisSliderSpinBox *m_rays;
    KisSliderSpinBox *m_ghosts;
    KisSliderSpinBox *m_ring;
    KisSliderSpinBox *m_warmth;
};

#endif
