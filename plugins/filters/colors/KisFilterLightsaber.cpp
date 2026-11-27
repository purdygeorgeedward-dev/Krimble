/*
 * Krimble: Filter > Render > Lightsaber... (see KisFilterLightsaber.h)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisFilterLightsaber.h"

#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QTouchEvent>
#include <QtMath>
#include <cmath>

#include <KoColor.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoColorModelStandardIds.h>
#include <KoColorConversionTransformation.h>
#include <KoUpdater.h>
#include <KisGlobalResourcesInterface.h>
#include <KisSequentialIteratorProgress.h>
#include <filter/kis_filter_category_ids.h>
#include <filter/kis_filter_configuration.h>
#include <kis_paint_device.h>
#include <kis_default_bounds_base.h>
#include <kis_slider_spin_box.h>
#include <kis_color_button.h>
#include <KisViewManager.h>
#include <canvas/kis_canvas2.h>
#include <canvas/kis_coordinates_converter.h>
#include <kis_image.h>

namespace {
struct Pixel16 { quint16 b, g, r, a; };   // the 16-bit RGBA layout of Krita (blue first)

struct Preset { const char *name; QColor color; };
const Preset presets[] = {
    {I18N_NOOP("Blue"), QColor(40, 120, 255)},
    {I18N_NOOP("Green"), QColor(40, 230, 60)},
    {I18N_NOOP("Red"), QColor(255, 30, 30)},
    {I18N_NOOP("Purple"), QColor(170, 60, 255)},
    {I18N_NOOP("Yellow"), QColor(255, 210, 30)},
    {I18N_NOOP("White"), QColor(235, 240, 255)},
};
const int presetCount = 6;
}

KisFilterLightsaber::KisFilterLightsaber()
    : KisFilter(id(), FiltersCategoryRenderId, i18n("Lights&aber..."))
{
    setSupportsPainting(false);
    setSupportsAdjustmentLayers(true);
    setSupportsThreading(true);
    setColorSpaceIndependence(FULLY_INDEPENDENT);
}

void KisFilterLightsaber::processImpl(KisPaintDeviceSP device,
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

    const double ax = bounds.left() + bounds.width() * config->getDouble("x1", 25.0) / 100.0;
    const double ay = bounds.top() + bounds.height() * config->getDouble("y1", 75.0) / 100.0;
    const double bx = bounds.left() + bounds.width() * config->getDouble("x2", 75.0) / 100.0;
    const double by = bounds.top() + bounds.height() * config->getDouble("y2", 25.0) / 100.0;
    const double diag = std::hypot(double(bounds.width()), double(bounds.height()));
    const double w = qMax(0.5, diag * config->getInt("width", 10) / 1000.0);   // blade width, 1% of the diagonal at 10
    const double glow = config->getInt("glow", 100) / 100.0;
    const double bright = config->getInt("brightness", 100) / 100.0;
    const QColor c = config->getColor("color", KoColor(QColor(40, 120, 255), KoColorSpaceRegistry::instance()->rgb8())).toQColor();
    const double cr = c.redF(), cg = c.greenF(), cb = c.blueF();
    const double vx = bx - ax, vy = by - ay, len2 = qMax(1e-9, vx * vx + vy * vy);

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
            const double px = x0 + i + 0.5 - ax, py = y + 0.5 - ay;
            const double t = qBound(0.0, (px * vx + py * vy) / len2, 1.0);
            const double dx = px - t * vx, dy = py - t * vy;
            const double d = std::sqrt(dx * dx + dy * dy);
            // white-hot core with a soft edge, colored inner glow, wide soft outer glow
            const double core = qBound(0.0, (w * 0.5 - d) / qMax(1.0, w * 0.15) + 0.5, 1.0);
            const double q1 = d / (w * 1.3), q2 = d / (w * 4.5);
            const double inner = std::exp(-q1 * q1) * glow, outer = std::exp(-q2 * q2) * 0.55 * glow;
            const double lr = qBound(0.0, (core + (inner + outer) * cr) * bright, 1.0);
            const double lg = qBound(0.0, (core + (inner + outer) * cg) * bright, 1.0);
            const double lb = qBound(0.0, (core + (inner + outer) * cb) * bright, 1.0);
            Pixel16 &p = buf[i];
            auto screen = [](quint16 v, double l) {
                const double a = v / 65535.0;
                return quint16(qBound(0.0, (1.0 - (1.0 - a) * (1.0 - l)) * 65535.0 + 0.5, 65535.0));
            };
            p.r = screen(p.r, lr);
            p.g = screen(p.g, lg);
            p.b = screen(p.b, lb);
        }
        cs16->convertPixelsTo(reinterpret_cast<const quint8*>(buf.data()), it.rawData(), cs, n,
                              KoColorConversionTransformation::internalRenderingIntent(),
                              KoColorConversionTransformation::internalConversionFlags());
    }
}

KisConfigWidget *KisFilterLightsaber::createConfigurationWidget(QWidget *parent, const KisPaintDeviceSP dev, bool useForMasks) const
{
    Q_UNUSED(dev);
    Q_UNUSED(useForMasks);
    return new KisWdgLightsaber(parent);
}

KisFilterConfigurationSP KisFilterLightsaber::defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const
{
    KisFilterConfigurationSP config = factoryConfiguration(resourcesInterface);
    config->setProperty("color", QColor(40, 120, 255));
    config->setProperty("x1", 25.0);
    config->setProperty("y1", 75.0);
    config->setProperty("x2", 75.0);
    config->setProperty("y2", 25.0);
    config->setProperty("width", 10);
    config->setProperty("glow", 100);
    config->setProperty("brightness", 100);
    return config;
}

static KisSliderSpinBox *krimbleSaberSlider(QWidget *parent, int min, int max, const QString &suffix)
{
    KisSliderSpinBox *slider = new KisSliderSpinBox(parent);
    slider->setRange(min, max);
    slider->setSingleStep(1);
    slider->setPageStep(10);
    slider->setSuffix(suffix);
    return slider;
}

KisWdgLightsaber::KisWdgLightsaber(QWidget *parent)
    : KisConfigWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout();
    mainLayout->addLayout(form);
    mainLayout->addStretch();

    m_preset = new QComboBox(this);
    for (int i = 0; i < presetCount; ++i) m_preset->addItem(i18n(presets[i].name));
    m_preset->addItem(i18n("Other color"));
    form->addRow(i18n("Blade:"), m_preset);
    m_color = new KisColorButton(this);
    form->addRow(i18n("Color:"), m_color);

    m_x1 = krimbleSaberSlider(this, 0, 100, i18n("%"));
    m_y1 = krimbleSaberSlider(this, 0, 100, i18n("%"));
    m_x2 = krimbleSaberSlider(this, 0, 100, i18n("%"));
    m_y2 = krimbleSaberSlider(this, 0, 100, i18n("%"));
    form->addRow(i18n("Hilt end across:"), m_x1);
    form->addRow(i18n("Hilt end down:"), m_y1);
    form->addRow(i18n("Tip across:"), m_x2);
    form->addRow(i18n("Tip down:"), m_y2);
    m_width = krimbleSaberSlider(this, 1, 60, QString());
    m_glow = krimbleSaberSlider(this, 0, 200, i18n("%"));
    m_brightness = krimbleSaberSlider(this, 0, 200, i18n("%"));
    form->addRow(i18n("Blade width:"), m_width);
    form->addRow(i18n("Glow:"), m_glow);
    form->addRow(i18n("Brightness:"), m_brightness);

    connect(m_preset, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index >= 0 && index < presetCount) {
            m_color->setColor(KoColor(presets[index].color, KoColorSpaceRegistry::instance()->rgb8()));
        }
        emit sigConfigurationItemChanged();
    });
    connect(m_color, SIGNAL(changed(const KoColor&)), SIGNAL(sigConfigurationItemChanged()));
    Q_FOREACH (KisSliderSpinBox *s, QList<KisSliderSpinBox*>() << m_x1 << m_y1 << m_x2 << m_y2 << m_width << m_glow << m_brightness) {
        connect(s, SIGNAL(valueChanged(int)), SIGNAL(sigConfigurationItemChanged()));
    }
}

void KisWdgLightsaber::setConfiguration(const KisPropertiesConfigurationSP config)
{
    const KoColor color = config->getColor("color", KoColor(QColor(40, 120, 255), KoColorSpaceRegistry::instance()->rgb8()));
    m_color->setColor(color);
    int preset = presetCount;
    for (int i = 0; i < presetCount; ++i) if (presets[i].color == color.toQColor()) preset = i;
    m_preset->blockSignals(true);
    m_preset->setCurrentIndex(preset);
    m_preset->blockSignals(false);
    m_x1->setValue(qRound(config->getDouble("x1", 25.0)));
    m_y1->setValue(qRound(config->getDouble("y1", 75.0)));
    m_x2->setValue(qRound(config->getDouble("x2", 75.0)));
    m_y2->setValue(qRound(config->getDouble("y2", 25.0)));
    m_width->setValue(config->getInt("width", 10));
    m_glow->setValue(config->getInt("glow", 100));
    m_brightness->setValue(config->getInt("brightness", 100));
}

KisPropertiesConfigurationSP KisWdgLightsaber::configuration() const
{
    KisFilterConfigurationSP config =
        new KisFilterConfiguration(KisFilterLightsaber::id().id(), 1, KisGlobalResourcesInterface::instance());
    config->setProperty("color", m_color->color().toQColor());
    config->setProperty("x1", double(m_x1->value()));
    config->setProperty("y1", double(m_y1->value()));
    config->setProperty("x2", double(m_x2->value()));
    config->setProperty("y2", double(m_y2->value()));
    config->setProperty("width", m_width->value());
    config->setProperty("glow", m_glow->value());
    config->setProperty("brightness", m_brightness->value());
    return config;
}

void KisWdgLightsaber::setView(KisViewManager *view)
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

// a finger on the picture moves the nearer end of the blade (chosen when the finger goes down)
bool KisWdgLightsaber::dragTo(const QPointF &widgetPoint, bool start)
{
    if (!m_view || !m_view->canvasBase() || !m_view->image()) return false;
    const KisCoordinatesConverter *converter = m_view->canvasBase()->coordinatesConverter();
    if (!converter) return false;
    const QPointF imagePoint = converter->viewportToImage(converter->widgetToViewport(widgetPoint));
    const QRect bounds = m_view->image()->bounds();
    if (bounds.isEmpty()) return false;
    const double fx = (imagePoint.x() - bounds.left()) * 100.0 / bounds.width();
    const double fy = (imagePoint.y() - bounds.top()) * 100.0 / bounds.height();
    if (start) {
        const double d1 = std::hypot(fx - m_x1->value(), fy - m_y1->value());
        const double d2 = std::hypot(fx - m_x2->value(), fy - m_y2->value());
        m_movingStart = d1 <= d2;
    }
    KisSliderSpinBox *sx = m_movingStart ? m_x1 : m_x2;
    KisSliderSpinBox *sy = m_movingStart ? m_y1 : m_y2;
    const int x = qBound(0, qRound(fx), 100), y = qBound(0, qRound(fy), 100);
    if (sx->value() != x || sy->value() != y) {
        sx->blockSignals(true);
        sy->blockSignals(true);
        sx->setValue(x);
        sy->setValue(y);
        sx->blockSignals(false);
        sy->blockSignals(false);
        emit sigConfigurationItemChanged();   // one signal for both: one preview
    }
    return true;
}

bool KisWdgLightsaber::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != m_canvasWidget || !isVisible()) return KisConfigWidget::eventFilter(watched, event);
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove: {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        if (event->type() == QEvent::MouseMove && !(mouse->buttons() & Qt::LeftButton)) break;
        if (mouse->source() != Qt::MouseEventNotSynthesized) return true;
        return dragTo(mouse->localPos(), event->type() == QEvent::MouseButtonPress);
    }
    case QEvent::MouseButtonRelease:
        return true;
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate: {
        QTouchEvent *touch = static_cast<QTouchEvent*>(event);
        if (touch->touchPoints().size() != 1) break;   // two fingers still pan and zoom
        event->accept();
        dragTo(touch->touchPoints().first().pos(), event->type() == QEvent::TouchBegin);
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
