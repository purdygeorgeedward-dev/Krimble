/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2018 Jouni Pentikainen <joupent@gmail.com>
 * SPDX-FileCopyrightText: 2020 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef _KIS_MULTICHANNEL_FILTER_BASE_H_
#define _KIS_MULTICHANNEL_FILTER_BASE_H_

#include <QList>

#include <filter/kis_color_transformation_filter.h>
#include <filter/kis_color_transformation_configuration.h>
#include <kis_config_widget.h>
#include <kis_paint_device.h>
#include "ui_wdg_perchannel.h"

#include "virtual_channel_info.h"

class KisMultiChannelFilter : public KisColorTransformationFilter
{
public:
    bool needsTransparentPixels(const KisFilterConfigurationSP config, const KoColorSpace *cs) const override;

    static QVector<VirtualChannelInfo> getVirtualChannels(const KoColorSpace *cs, int maxChannels = -1);
    static int findChannel(const QVector<VirtualChannelInfo> &virtualChannels, const VirtualChannelInfo::Type &channelType);

protected:
    KisMultiChannelFilter(const KoID &id, const QString &entry);
};

class KisMultiChannelFilterConfiguration : public KisColorTransformationConfiguration
{
public:
    KisMultiChannelFilterConfiguration(int channelCount, const QString & name, qint32 version, KisResourcesInterfaceSP resourcesInterface);
    KisMultiChannelFilterConfiguration(const KisMultiChannelFilterConfiguration &rhs);
    ~KisMultiChannelFilterConfiguration() override;

    using KisFilterConfiguration::fromXML;
    using KisFilterConfiguration::toXML;
    using KisFilterConfiguration::fromLegacyXML;

    void fromLegacyXML(const QDomElement& root) override;

    void fromXML(const QDomElement& e) override;
    void toXML(QDomDocument& doc, QDomElement& root) const override;

    void setCurves(QList<KisCubicCurve> &curves);
    bool isCompatible(const KisPaintDeviceSP) const override;

    const QVector<QVector<quint16> >& transfers() const;
    const QList<KisCubicCurve>& curves() const;

    virtual bool compareTo(const KisPropertiesConfiguration* rhs) const override;

    void setProperty(const QString& name, const QVariant& value) override;
    void setActiveCurve(int value);

protected:
    int m_channelCount {0};
    int m_activeCurve {-1};
    QList<KisCubicCurve> m_curves;
    QVector<QVector<quint16>> m_transfers;

    void init();
    void updateTransfer(int index);
    void updateTransfers();

    virtual KisCubicCurve getDefaultCurve() = 0;

    bool curveIndexFromCurvePropertyName(const QString& name, int& curveIndex) const;
};

class WdgPerChannel : public QWidget, public Ui::WdgPerChannel
{
    Q_OBJECT

public:
    WdgPerChannel(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }
};

class KisMultiChannelConfigWidget : public KisConfigWidget
{
    Q_OBJECT

public:
    KisMultiChannelConfigWidget(QWidget * parent, KisPaintDeviceSP dev, Qt::WindowFlags f = Qt::WindowFlags());
    ~KisMultiChannelConfigWidget() override;

    void setConfiguration(const KisPropertiesConfigurationSP config) override;

protected Q_SLOTS:
    void logHistView();
    void resetCurve();
    void slotCurveModified();
    void slotChannelSelected(int index);

protected:
    void init();
    void resetCurves();
    void setActiveChannel(int ch);
    void syncActiveChannelUi();
    virtual int findDefaultVirtualChannelSelection();

    virtual void updateChannelControls() = 0;
    virtual KisPropertiesConfigurationSP getDefaultConfiguration() = 0;

    inline QPixmap getHistogram();
    inline QPixmap createGradient(Qt::Orientation orient );

    QVector<VirtualChannelInfo> m_virtualChannels;
    int m_activeVChannel {0};
    mutable QList<KisCubicCurve> m_curves;

    KisPaintDeviceSP m_dev;
    WdgPerChannel * m_page {nullptr};
    KisHistogram *m_histogram {nullptr};
};

#endif
