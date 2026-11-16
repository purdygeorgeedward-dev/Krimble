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

/**
 * Krimble: reworked into the industry standard's Match Color dialog shape -- pick a
 * source from currently open documents (not a file loaded from disk, which
 * is what this used to require), a layer within it (or Merged), selection-
 * scoping checkboxes, Neutralize, and Luminance/Color Intensity/Fade
 * sliders. The underlying statistical color-transfer math is unchanged
 * (see fastcolortransfer.cpp) -- this only changes how the reference
 * statistics get gathered and how strongly the result gets applied.
 */
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
