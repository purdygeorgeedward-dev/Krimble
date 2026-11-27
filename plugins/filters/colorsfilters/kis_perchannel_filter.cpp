/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_perchannel_filter.h"
#include "../levelfilter/KrimbleAutoAdjust.h"
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <cmath>

#include <Qt>
#include <QLayout>
#include <QPixmap>
#include <QPainter>
#include <QDomDocument>
#include <QHBoxLayout>

#include "KoChannelInfo.h"
#include "KoBasicHistogramProducers.h"
#include "KoColorModelStandardIds.h"
#include "KoColorSpace.h"
#include "KoColorTransformation.h"
#include "KoCompositeColorTransformation.h"
#include "KoCompositeOp.h"
#include "KoID.h"

#include "kis_signals_blocker.h"

#include "kis_bookmarked_configuration_manager.h"
#include "kis_config_widget.h"
#include <filter/kis_filter_configuration.h>
#include <kis_selection.h>
#include <kis_paint_device.h>
#include <kis_processing_information.h>

#include "kis_histogram.h"
#include "kis_painter.h"
#include "widgets/kis_curve_widget.h"
#include <KisGlobalResourcesInterface.h>

#include "kis_multichannel_utils.h"

KisPerChannelConfigWidget::KisPerChannelConfigWidget(QWidget * parent, KisPaintDeviceSP dev, Qt::WindowFlags f)
        : KisMultiChannelConfigWidget(parent, dev, f)
{
    init();

    // These are not used by this filter,
    // but the dialog is shared with KisCrossChannelFilter
    m_page->lblDriverChannel->hide();
    m_page->cmbDriverChannel->hide();

    // Krimble: an Auto button beside Reset. It works out a curve for red, green and blue from the picture
    // (the same as Image > Auto Color, without its saturation lift: KrimbleAutoAdjust.h) and puts the curves here,
    // so they can be adjusted by hand before OK.
    if (m_page->resetButton && m_page->resetButton->parentWidget()) {
        QPushButton *autoButton = new QPushButton(i18n("Auto"), m_page->resetButton->parentWidget());
        autoButton->setObjectName(QStringLiteral("krimbleCurvesAuto"));
        if (QLayout *layout = m_page->resetButton->parentWidget()->layout()) {
            if (QGridLayout *grid = qobject_cast<QGridLayout*>(layout)) {
                int index = grid->indexOf(m_page->resetButton), row = 0, column = 0, rowSpan = 1, columnSpan = 1;
                grid->getItemPosition(index, &row, &column, &rowSpan, &columnSpan);
                QHBoxLayout *pair = new QHBoxLayout();
                grid->removeWidget(m_page->resetButton);
                pair->addWidget(autoButton);
                pair->addWidget(m_page->resetButton);
                grid->addLayout(pair, row, column, rowSpan, columnSpan);
            } else if (QBoxLayout *box = qobject_cast<QBoxLayout*>(layout)) {
                box->insertWidget(box->indexOf(m_page->resetButton), autoButton);
            }
        }
        connect(autoButton, &QPushButton::clicked, this, [this]() {
            if (!m_dev) return;
            const KrimbleAutoAdjust::Curves curves = KrimbleAutoAdjust::computeCurves(m_dev, m_dev->exactBounds(), KrimbleAutoAdjust::Color);
            if (!curves.valid) return;
            for (int i = 0; i < m_virtualChannels.size() && i < m_curves.size(); ++i) {
                const VirtualChannelInfo &info = m_virtualChannels[i];
                if (info.type() != VirtualChannelInfo::REAL || !info.channelInfo()) continue;
                const QColor color = info.channelInfo()->color();
                int c = -1;
                if (color.red() > 200 && color.green() < 60 && color.blue() < 60) c = 0;
                else if (color.green() > 200 && color.red() < 60 && color.blue() < 60) c = 1;
                else if (color.blue() > 200 && color.red() < 60 && color.green() < 60) c = 2;
                if (c < 0) continue;
                const qreal b = curves.black[c], w = curves.white[c], p = curves.power[c];
                QList<QPointF> points;
                points << QPointF(b, 0.0);
                for (int k = 1; k <= 3; ++k) {
                    const qreal t = k / 4.0;
                    points << QPointF(b + (w - b) * t, std::pow(t, p));
                }
                points << QPointF(w, 1.0);
                m_curves[i] = KisCubicCurve(points);
            }
            syncActiveChannelUi();
            emit sigConfigurationItemChanged();
        });
    }
}

KisPerChannelConfigWidget::~KisPerChannelConfigWidget()
{}

#define BITS_PER_BYTE 8
#define pwr2(p) (1<<p)
void KisPerChannelConfigWidget::updateChannelControls()
{
    // Getting range accepted by channel
    VirtualChannelInfo &currentVChannel = m_virtualChannels[m_activeVChannel];

    KoChannelInfo::enumChannelValueType valueType = currentVChannel.valueType();

    int order = BITS_PER_BYTE * currentVChannel.channelSize();
    int maxValue = pwr2(order);
    int min;
    int max;

    m_curveControlsManager.reset();

    switch (valueType) {
    case KoChannelInfo::UINT8:
    case KoChannelInfo::UINT16:
    case KoChannelInfo::UINT32:
        min = 0;
        max = maxValue - 1;
        break;
    case KoChannelInfo::INT8:
    case KoChannelInfo::INT16:
        min = -maxValue / 2;
        max = maxValue / 2 - 1;
        break;
    case KoChannelInfo::FLOAT16:
    case KoChannelInfo::FLOAT32:
    case KoChannelInfo::FLOAT64:
    default:
        //Hack Alert: should be changed to float
        if (m_dev->colorSpace()->colorModelId() == LABAColorModelID || m_dev->colorSpace()->colorModelId() == CMYKAColorModelID) {
            if (m_dev->colorSpace()->channelCount() > m_activeVChannel) {
                const QList<KoChannelInfo*> channels = m_dev->colorSpace()->channels();
                min = channels[m_activeVChannel]->getUIMin();
                max = channels[m_activeVChannel]->getUIMax();
            } else {
                // it must be "Hue", "Saturation" or other "channel" that isn't actually accessible in the color space
                min = 0;
                // specific number apparently doesn't matter,
                // if there is 255, it will work just fine, too
                max = 100;
            }
        } else {
            min = 0;
            max = 100;
        }
        break;
    }

    m_curveControlsManager.reset(new KisCurveWidgetControlsManagerInt(m_page->curveWidget,
                                                                      m_page->intIn, m_page->intOut, min, max, min, max));
}


KisPropertiesConfigurationSP KisPerChannelConfigWidget::configuration() const
{
    const int numChannels = m_virtualChannels.size();

    KisPinnedSharedPtr<KisPerChannelFilterConfiguration> cfg =
        new KisPerChannelFilterConfiguration(numChannels, KisGlobalResourcesInterface::instance());

    KIS_ASSERT_RECOVER(m_activeVChannel < m_curves.size()) { return cfg; }

    cfg->setCurves(m_curves);
    cfg->setActiveCurve(m_activeVChannel);

    return cfg;
}

KisPropertiesConfigurationSP KisPerChannelConfigWidget::getDefaultConfiguration()
{
    return new KisPerChannelFilterConfiguration(m_virtualChannels.size(), KisGlobalResourcesInterface::instance());
}

KisPerChannelFilterConfiguration::KisPerChannelFilterConfiguration(int channelCount, KisResourcesInterfaceSP resourcesInterface)
        : KisMultiChannelFilterConfiguration(channelCount, "perchannel", 1, resourcesInterface)
{
    init();
}

KisPerChannelFilterConfiguration::KisPerChannelFilterConfiguration(const KisPerChannelFilterConfiguration &rhs)
    : KisMultiChannelFilterConfiguration(rhs)
{
}

KisPerChannelFilterConfiguration::~KisPerChannelFilterConfiguration()
{
}

KisFilterConfigurationSP KisPerChannelFilterConfiguration::clone() const
{
    return new KisPerChannelFilterConfiguration(*this);
}

KisCubicCurve KisPerChannelFilterConfiguration::getDefaultCurve()
{
    return KisCubicCurve();
}

// KisPerChannelFilter

KisPerChannelFilter::KisPerChannelFilter() : KisMultiChannelFilter(id(), i18n("&Color Adjustment curves..."))
{
    setShortcut(QKeySequence(Qt::CTRL | Qt::Key_M));
}

KisConfigWidget * KisPerChannelFilter::createConfigurationWidget(QWidget *parent, const KisPaintDeviceSP dev, bool) const
{
    return new KisPerChannelConfigWidget(parent, dev);
}

KisFilterConfigurationSP  KisPerChannelFilter::factoryConfiguration(KisResourcesInterfaceSP resourcesInterface) const
{
    return new KisPerChannelFilterConfiguration(0, resourcesInterface);
}

KoColorTransformation* KisPerChannelFilter::createTransformation(const KoColorSpace* cs, const KisFilterConfigurationSP config) const
{
    const KisPerChannelFilterConfiguration* configBC =
        dynamic_cast<const KisPerChannelFilterConfiguration*>(config.data()); // Somehow, this shouldn't happen
    Q_ASSERT(configBC);

    QList<bool> isIdentityList;
    for (const KisCubicCurve &curve : configBC->curves()) {
        isIdentityList.append(curve.isIdentity());
    }

    return KisMultiChannelUtils::createPerChannelTransformationFromTransfers(cs, configBC->transfers(), isIdentityList);
}
