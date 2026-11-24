/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KIS_WDG_GENERATOR_H
#define KIS_WDG_GENERATOR_H

#include <QWidget>
#include <kis_types.h>

class KisFilterConfiguration;
class KisViewManager;
class KoColor;

class KisWdgGenerator : public QWidget
{

    Q_OBJECT

public:

    KisWdgGenerator(QWidget * parent);

    KisWdgGenerator(QWidget * parent, KisPaintDeviceSP dev);

    ~KisWdgGenerator() override;

    void initialize(KisViewManager *view);

    void setConfiguration(const KisFilterConfigurationSP  config);

    KisFilterConfigurationSP  configuration();

Q_SIGNALS:
    void previewConfiguration();

private Q_SLOTS:
    void slotGeneratorActivated(int);

public Q_SLOTS:
    void showFilterGallery(bool);

private:

    struct Private;
    Private * const d {nullptr};
};

#endif
