/*
 * Krimble: Filter > Render > Lens Flare... (see KisFilterLensFlare.h, KrimbleLensFlare.h)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisFilterLensFlare.h"
#include "KrimbleLensFlare.h"

#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>

#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoColorModelStandardIds.h>
#include <KoColorConversionTransformation.h>
#include <KisGlobalResourcesInterface.h>
#include <KisSequentialIteratorProgress.h>
#include <filter/kis_filter_category_ids.h>
#include <filter/kis_filter_configuration.h>
#include <kis_paint_device.h>
#include <kis_default_bounds_base.h>
#include <kis_slider_spin_box.h>
#include <KoUpdater.h>
#include <QEvent>
#include <QMouseEvent>
#include <QTouchEvent>
#include <KisViewManager.h>
#include <canvas/kis_canvas2.h>
#include <canvas/kis_coordinates_converter.h>
#include <kis_image.h>

KisFilterLensFlare::KisFilterLensFlare()
    : KisFilter(id(), FiltersCategoryRenderId, i18n("&Lens Flare..."))
{
    setSupportsPainting(false);
    setSupportsAdjustmentLayers(true);
    setSupportsThreading(true);
    setColorSpaceIndependence(FULLY_INDEPENDENT);
}

QRect KisFilterLensFlare::neededRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const
{
    Q_UNUSED(config); Q_UNUSED(lod);
    return rect;
}

QRect KisFilterLensFlare::changedRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const
{
    Q_UNUSED(config); Q_UNUSED(lod);
    return rect;
}

namespace {
struct Pixel16 { quint16 b, g, r, a; };
}

void KisFilterLensFlare::processImpl(KisPaintDeviceSP device,
                                     const QRect &rect,
                                     const KisFilterConfigurationSP config,
                                     KoUpdater *progressUpdater) const
{
    const KoColorSpace *cs = device->colorSpace();
    if (cs->colorModelId() != RGBAColorModelID) return;
    const KoColorSpace *cs16 = KoColorSpaceRegistry::instance()->colorSpace(RGBAColorModelID.id(), Integer16BitsColorDepthID.id(), cs->profile());
    if (!cs16) return;

    QRect bounds = device->defaultBounds()->imageBorderRect();
    if (bounds.isEmpty()) bounds = device->exactBounds();
    if (bounds.isEmpty()) return;

    KrimbleLensFlare::Params params;
    params.kind = KrimbleLensFlare::Kind(qBound(0, config->getInt("type", 0), 3));
    params.sx = bounds.left() + bounds.width() * config->getDouble("x", 70.0) / 100.0;
    params.sy = bounds.top() + bounds.height() * config->getDouble("y", 25.0) / 100.0;
    params.cx = bounds.left() + bounds.width() / 2.0;
    params.cy = bounds.top() + bounds.height() / 2.0;
    params.diagonal = std::hypot(double(bounds.width()), double(bounds.height()));
    params.brightness = config->getInt("brightness", 100) / 100.0;
    params.rays = config->getInt("rays", 100) / 100.0;
    params.ghosts = config->getInt("ghosts", 100) / 100.0;
    params.ring = config->getInt("ring", 100) / 100.0;
    params.warmth = config->getInt("warmth", 0) / 100.0;
    const KrimbleLensFlare::Renderer renderer(params);

    QVector<Pixel16> buf;
    KisSequentialIteratorProgress it(device, rect, progressUpdater);
    int n = it.nConseqPixels();
    while (it.nextPixels(n)) {
        n = it.nConseqPixels();
        if (buf.size() < n) buf.resize(n);
        cs->convertPixelsTo(it.oldRawData(), reinterpret_cast<quint8*>(buf.data()), cs16, n,
                            KoColorConversionTransformation::internalRenderingIntent(),
                            KoColorConversionTransformation::internalConversionFlags());
        const int x0 = it.x(), y = it.y();
        for (int i = 0; i < n; ++i) {
            const KrimbleLensFlare::RGB light = renderer.light(x0 + i + 0.5, y + 0.5);
            Pixel16 &p = buf[i];
            auto screen = [](quint16 v, double l) {
                const double a = v / 65535.0;
                return quint16(qBound(0.0, (1.0 - (1.0 - a) * (1.0 - l)) * 65535.0 + 0.5, 65535.0));
            };
            p.r = screen(p.r, light.r);
            p.g = screen(p.g, light.g);
            p.b = screen(p.b, light.b);
        }
        cs16->convertPixelsTo(reinterpret_cast<const quint8*>(buf.data()), it.rawData(), cs, n,
                              KoColorConversionTransformation::internalRenderingIntent(),
                              KoColorConversionTransformation::internalConversionFlags());
    }
}

KisConfigWidget *KisFilterLensFlare::createConfigurationWidget(QWidget *parent, const KisPaintDeviceSP dev, bool useForMasks) const
{
    Q_UNUSED(dev);
    Q_UNUSED(useForMasks);
    return new KisWdgLensFlare(parent);
}

KisFilterConfigurationSP KisFilterLensFlare::defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const
{
    KisFilterConfigurationSP config = factoryConfiguration(resourcesInterface);
    config->setProperty("type", 0);
    config->setProperty("x", 70.0);
    config->setProperty("y", 25.0);
    config->setProperty("brightness", 100);
    config->setProperty("rays", 100);
    config->setProperty("ghosts", 100);
    config->setProperty("ring", 100);
    config->setProperty("warmth", 0);
    return config;
}

static KisSliderSpinBox *krimbleFlareSlider(QWidget *parent, int min, int max, const QString &suffix)
{
    KisSliderSpinBox *slider = new KisSliderSpinBox(parent);
    slider->setRange(min, max);
    slider->setSingleStep(1);
    slider->setPageStep(10);
    slider->setSuffix(suffix);
    return slider;
}

KisWdgLensFlare::KisWdgLensFlare(QWidget *parent)
    : KisConfigWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout();
    mainLayout->addLayout(form);
    mainLayout->addStretch();

    m_type = new QComboBox(this);
    m_type->addItem(i18n("Zoom lens"));
    m_type->addItem(i18n("Prime lens"));
    m_type->addItem(i18n("Cinematic"));
    m_type->addItem(i18n("Starburst"));
    form->addRow(i18n("Type:"), m_type);

    m_x = krimbleFlareSlider(this, 0, 100, i18n("%"));
    m_y = krimbleFlareSlider(this, 0, 100, i18n("%"));
    form->addRow(i18n("Light across:"), m_x);
    form->addRow(i18n("Light down:"), m_y);
    m_brightness = krimbleFlareSlider(this, 0, 200, i18n("%"));
    m_rays = krimbleFlareSlider(this, 0, 200, i18n("%"));
    m_ghosts = krimbleFlareSlider(this, 0, 200, i18n("%"));
    m_ring = krimbleFlareSlider(this, 0, 200, i18n("%"));
    m_warmth = krimbleFlareSlider(this, -100, 100, QString());
    form->addRow(i18n("Brightness:"), m_brightness);
    form->addRow(i18n("Rays:"), m_rays);
    form->addRow(i18n("Ghosts:"), m_ghosts);
    form->addRow(i18n("Ring:"), m_ring);
    form->addRow(i18n("Color (cool to warm):"), m_warmth);

    connect(m_type, SIGNAL(currentIndexChanged(int)), SIGNAL(sigConfigurationItemChanged()));
    Q_FOREACH (KisSliderSpinBox *s, QList<KisSliderSpinBox*>() << m_x << m_y << m_brightness << m_rays << m_ghosts << m_ring << m_warmth) {
        connect(s, SIGNAL(valueChanged(int)), SIGNAL(sigConfigurationItemChanged()));
    }
}

void KisWdgLensFlare::setConfiguration(const KisPropertiesConfigurationSP config)
{
    m_type->setCurrentIndex(qBound(0, config->getInt("type", 0), 3));
    m_x->setValue(qRound(config->getDouble("x", 70.0)));
    m_y->setValue(qRound(config->getDouble("y", 25.0)));
    m_brightness->setValue(config->getInt("brightness", 100));
    m_rays->setValue(config->getInt("rays", 100));
    m_ghosts->setValue(config->getInt("ghosts", 100));
    m_ring->setValue(config->getInt("ring", 100));
    m_warmth->setValue(config->getInt("warmth", 0));
}

KisPropertiesConfigurationSP KisWdgLensFlare::configuration() const
{
    KisFilterConfigurationSP config =
        new KisFilterConfiguration(KisFilterLensFlare::id().id(), 1, KisGlobalResourcesInterface::instance());
    config->setProperty("type", m_type->currentIndex());
    config->setProperty("x", double(m_x->value()));
    config->setProperty("y", double(m_y->value()));
    config->setProperty("brightness", m_brightness->value());
    config->setProperty("rays", m_rays->value());
    config->setProperty("ghosts", m_ghosts->value());
    config->setProperty("ring", m_ring->value());
    config->setProperty("warmth", m_warmth->value());
    return config;
}

void KisWdgLensFlare::setView(KisViewManager *view)
{
    m_view = view;
    if (m_canvasWidget) m_canvasWidget->removeEventFilter(this);
    m_canvasWidget = nullptr;
    if (view && view->canvasBase() && view->canvasBase()->canvasWidget()) {
        m_canvasWidget = view->canvasBase()->canvasWidget();
        m_canvasWidget->installEventFilter(this);
        m_canvasWidget->setAttribute(Qt::WA_AcceptTouchEvents, true);
    }
}

bool KisWdgLensFlare::moveLightTo(const QPointF &widgetPoint)
{
    if (!m_view || !m_view->canvasBase() || !m_view->image()) return false;
    const KisCoordinatesConverter *converter = m_view->canvasBase()->coordinatesConverter();
    if (!converter) return false;
    const QPointF imagePoint = converter->viewportToImage(converter->widgetToViewport(widgetPoint));
    const QRect bounds = m_view->image()->bounds();
    if (bounds.isEmpty()) return false;
    const int x = qBound(0, qRound((imagePoint.x() - bounds.left()) * 100.0 / bounds.width()), 100);
    const int y = qBound(0, qRound((imagePoint.y() - bounds.top()) * 100.0 / bounds.height()), 100);
    if (m_x->value() != x || m_y->value() != y) {
        m_x->blockSignals(true);
        m_y->blockSignals(true);
        m_x->setValue(x);
        m_y->setValue(y);
        m_x->blockSignals(false);
        m_y->blockSignals(false);
        emit sigConfigurationItemChanged();
    }
    return true;
}

bool KisWdgLensFlare::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != m_canvasWidget || !isVisible()) return KisConfigWidget::eventFilter(watched, event);
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove: {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        if (event->type() == QEvent::MouseMove && !(mouse->buttons() & Qt::LeftButton)) break;
        if (mouse->source() != Qt::MouseEventNotSynthesized) return true;
        return moveLightTo(mouse->localPos());
    }
    case QEvent::MouseButtonRelease:
        return true;
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate: {
        QTouchEvent *touch = static_cast<QTouchEvent*>(event);
        if (touch->touchPoints().size() != 1) break;
        event->accept();
        moveLightTo(touch->touchPoints().first().pos());
        return true;
    }
    case QEvent::TouchEnd:
        if (static_cast<QTouchEvent*>(event)->touchPoints().size() == 1) { event->accept(); return true; }
        break;
    default:
        break;
    }
    return KisConfigWidget::eventFilter(watched, event);
}
