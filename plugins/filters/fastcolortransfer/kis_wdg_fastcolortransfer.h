/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_WDG_FASTCOLORTRANSFER_H
#define KIS_WDG_FASTCOLORTRANSFER_H

#include <kis_config_widget.h>
#include <QPointer>

class Ui_WdgFastColorTransfer;
class KisDocument;

class KisWdgFastColorTransfer : public KisConfigWidget
{
    Q_OBJECT
public:
    KisWdgFastColorTransfer(QWidget * parent);
    ~KisWdgFastColorTransfer() override;
    void setConfiguration(const KisPropertiesConfigurationSP) override;
    inline const Ui_WdgFastColorTransfer* widget() const {
        return m_widget;
    }
    KisPropertiesConfigurationSP configuration() const override;

private Q_SLOTS:
    void slotSourceChanged(int index);

private:
    void populateSourceCombo();
    void populateLayerCombo(KisDocument *doc);

    Ui_WdgFastColorTransfer* m_widget;
    QList<QPointer<KisDocument> > m_documents;
};

#endif
