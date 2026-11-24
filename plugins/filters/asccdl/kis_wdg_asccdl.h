/*
 * SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_WDG_ASCCDL_H
#define KIS_WDG_ASCCDL_H
#include <kis_config_widget.h>
#include <QWidget>
#include "ui_wdg_asccdl.h"
#include <KisVisualRectangleSelectorShape.h>
#include <KisVisualEllipticalSelectorShape.h>

class Ui_WdgASCCDL;

class KisASCCDLConfigWidget : public KisConfigWidget
{

    Q_OBJECT

public:
    KisASCCDLConfigWidget(QWidget * parent, const KoColorSpace *cs);
    ~KisASCCDLConfigWidget() override;

    KisPropertiesConfigurationSP  configuration() const override;
    void setConfiguration(const KisPropertiesConfigurationSP config) override;
    Ui_WdgASCCDL *m_page;
    const KoColorSpace *m_cs;
public Q_SLOTS:
    void slopeColorChanged(const KoColor &c);
    void offsetColorChanged(const KoColor &c);
    void powerColorChanged(const KoColor &c);
};

#endif
