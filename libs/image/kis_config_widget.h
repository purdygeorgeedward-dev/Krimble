/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt (boud@valdyas.org)
 *  SPDX-FileCopyrightText: 2004-2006 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_CONFIG_WIDGET_H_
#define _KIS_CONFIG_WIDGET_H_

#include <QWidget>
#include <kritaimage_export.h>

#include "kis_signal_compressor.h"
#include <kis_properties_configuration.h>

class KoCanvasResourcesInterface;
using KoCanvasResourcesInterfaceSP = QSharedPointer<KoCanvasResourcesInterface>;

class KisViewManager;

class KRITAIMAGE_EXPORT KisConfigWidget : public QWidget
{

    Q_OBJECT

protected:

    KisConfigWidget(QWidget * parent = 0, Qt::WindowFlags f = Qt::WindowFlags(), int delay = 200);

public:
    ~KisConfigWidget() override;

    virtual void setConfiguration(const KisPropertiesConfigurationSP  config) = 0;

    virtual KisPropertiesConfigurationSP configuration() const = 0;

    virtual void setView(KisViewManager *view);

    virtual void setCanvasResourcesInterface(KoCanvasResourcesInterfaceSP canvasResourcesInterface);
    virtual KoCanvasResourcesInterfaceSP canvasResourcesInterface() const;

Q_SIGNALS:

    void sigConfigurationUpdated();

    void sigConfigurationItemChanged();
    void sigSaveLockedConfig(KisPropertiesConfigurationSP p);
    void sigDropLockedConfig(KisPropertiesConfigurationSP p);

private Q_SLOTS:

    void slotConfigChanged();

private:
    KisSignalCompressor m_compressor;
    KoCanvasResourcesInterfaceSP m_canvasResourcesInterface;
};

#endif
