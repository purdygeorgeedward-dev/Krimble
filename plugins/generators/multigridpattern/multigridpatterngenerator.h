/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2020 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef MULTIGRID_PATTERN_GENERATOR_H
#define MULTIGRID_PATTERN_GENERATOR_H

#include <QObject>
#include <QVariant>
#include "generator/kis_generator.h"

class KisConfigWidget;

class KritaMultigridPatternGenerator : public QObject
{
    Q_OBJECT
public:
    KritaMultigridPatternGenerator(QObject *parent, const QVariantList &);
    ~KritaMultigridPatternGenerator() override;
};

struct KisMultiGridRhomb {
    QPolygonF shape;
    int parallel1;
    int parallel2;
    int line1;
    int line2;
};

class KisMultigridPatternGenerator : public KisGenerator
{
public:

    enum Connector{
        None,
        Acute,
        Obtuse,
        Cross,
        CenterDot,
        CornerDot
    };

    KisMultigridPatternGenerator();

    using KisGenerator::generate;

    void generate(KisProcessingInformation dst,
                  const QSize& size,
                  const KisFilterConfigurationSP config,
                  KoUpdater* progressUpdater
                 ) const override;

    static inline KoID id() {
        return KoID("multigrid", i18n("Multigrid"));
    }

    KisFilterConfigurationSP defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const override;
    KisConfigWidget * createConfigurationWidget(QWidget* parent, const KisPaintDeviceSP dev, bool useForMasks) const override;

    virtual bool allowsSplittingIntoPatches() const override { return false; }

private:
    QList<KisMultiGridRhomb> generateRhombs(int lines, int divisions, qreal offset) const;

    QList<int> getIndicesFromPoint(QPointF point, QList<qreal> angles, qreal offset) const;

    QPointF getVertice(QList<int> indices, QList<qreal> angles) const;
};

#endif
