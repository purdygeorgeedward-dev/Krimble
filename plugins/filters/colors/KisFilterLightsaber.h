/*
 * Krimble: Filter > Render > Lightsaber... : a glowing energy blade drawn from scratch between two points.
 * Per-pixel: the distance from each pixel to the blade (a line with round ends) gives a white-hot core,
 * a colored inner glow and a soft wide outer glow; Screen blending, so it only brightens the picture.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISFILTERLIGHTSABER_H
#define KISFILTERLIGHTSABER_H

#include <KoID.h>
#include <klocalizedstring.h>
#include <filter/kis_filter.h>
#include <kis_config_widget.h>
#include <QPointer>

class KisSliderSpinBox;
class KisColorButton;
class QComboBox;

class KisFilterLightsaber : public KisFilter
{
public:
    KisFilterLightsaber();

    void processImpl(KisPaintDeviceSP device,
                     const QRect &rect,
                     const KisFilterConfigurationSP config,
                     KoUpdater *progressUpdater) const override;

    static inline KoID id()
    {
        return KoID("krimblelightsaber", i18n("Lightsaber"));
    }

    KisConfigWidget *createConfigurationWidget(QWidget *parent, const KisPaintDeviceSP dev, bool useForMasks) const override;
    KisFilterConfigurationSP defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const override;
    QRect neededRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const override { Q_UNUSED(config); Q_UNUSED(lod); return rect; }
    QRect changedRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const override { Q_UNUSED(config); Q_UNUSED(lod); return rect; }
};

class KisWdgLightsaber : public KisConfigWidget
{
    Q_OBJECT

public:
    explicit KisWdgLightsaber(QWidget *parent);

    void setConfiguration(const KisPropertiesConfigurationSP config) override;
    KisPropertiesConfigurationSP configuration() const override;
    void setView(KisViewManager *view) override;   // to follow a finger dragged on the picture

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    bool dragTo(const QPointF &widgetPoint, bool start);
    KisViewManager *m_view {nullptr};
    QPointer<QWidget> m_canvasWidget;
    bool m_movingStart {false};
    QComboBox *m_preset;
    KisColorButton *m_color;
    KisSliderSpinBox *m_x1, *m_y1, *m_x2, *m_y2;
    KisSliderSpinBox *m_width, *m_glow, *m_brightness;
};

#endif
