/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_DLG_ADJ_LAYER_PROPS_H
#define KIS_DLG_ADJ_LAYER_PROPS_H

#include <KoDialog.h>

class QLineEdit;

class KisFilter;
class KisFilterConfiguration;
class KisConfigWidget;
class KisNodeFilterInterface;
class KisViewManager;

#include "kis_types.h"

class KisDlgAdjLayerProps : public KoDialog
{
    Q_OBJECT

public:

    KisDlgAdjLayerProps(KisNodeSP node,
                        KisNodeFilterInterface *nfi,
                        KisPaintDeviceSP paintDevice,
                        KisViewManager *view,
                        KisFilterConfigurationSP configuration,
                        const QString & layerName,
                        const QString & caption,
                        QWidget *parent = 0,
                        const char *name = 0);

    KisFilterConfigurationSP  filterConfiguration() const;
    QString layerName() const;

private Q_SLOTS:

    void slotNameChanged(const QString &);
    void slotConfigChanged();

private:
    KisNodeSP m_node;
    KisPaintDeviceSP m_paintDevice;
    KisConfigWidget *m_currentConfigWidget;
    KisFilter *m_currentFilter;
    KisFilterConfigurationSP m_currentConfiguration;
    QLineEdit *m_layerName;
    KisNodeFilterInterface *m_nodeFilterInterface;
};

#endif
