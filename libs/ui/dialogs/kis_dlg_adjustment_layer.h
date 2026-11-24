/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISDLGAdjustMENTLAYER_H
#define KISDLGAdjustMENTLAYER_H

#include <KoDialog.h>

class KisFilterConfiguration;
class KisNodeFilterInterface;
class KisViewManager;
#include "kis_types.h"
#include "ui_wdgfilternodecreation.h"

class KisDlgAdjustmentLayer : public KoDialog
{

    Q_OBJECT

public:

    KisDlgAdjustmentLayer(KisNodeSP node,
                          KisNodeFilterInterface* nfi,
                          KisPaintDeviceSP paintDevice,
                          const QString & layerName,
                          const QString & caption,
                          KisViewManager *view,
                          QWidget *parent = 0);
    ~KisDlgAdjustmentLayer() override;
    KisFilterConfigurationSP  filterConfiguration() const;
    QString layerName() const;

public Q_SLOTS:
    void adjustSize();

protected Q_SLOTS:
    void slotNameChanged(const QString &);
    void slotConfigChanged();
    void slotFilterWidgetSizeChanged();

private:
    KisNodeSP m_node;
    KisNodeFilterInterface *m_nodeFilterInterface;
    Ui::WdgFilterNodeCreation wdgFilterNodeCreation;
    KisFilterConfigurationSP m_currentFilter;
    bool m_customName;
    QString m_layerName;

};

#endif
