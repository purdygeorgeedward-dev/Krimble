/*
 *  SPDX-FileCopyrightText: 2017 Laurent Valentin Jospin <laurent.valentin@famillejospin.ch>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISDOCUMENTAWARESPINBOXUNITMANAGER_H
#define KISDOCUMENTAWARESPINBOXUNITMANAGER_H

#include "kis_spin_box_unit_manager.h"
#include "kis_double_parse_unit_spin_box.h"

#include "kritaui_export.h"
class KisDocumentAwareSpinBoxUnitManagerBuilder : public KisSpinBoxUnitManagerBuilder
{

public:

    KisSpinBoxUnitManager* buildUnitManager(QObject* parent) override;
};

class KRITAUI_EXPORT KisDocumentAwareSpinBoxUnitManager : public KisSpinBoxUnitManager
{
    Q_OBJECT

public:

    enum PixDir {
        PIX_DIR_X,
        PIX_DIR_Y
    };

    static void setDocumentAwarenessToExistingUnitSpinBox(KisDoubleParseUnitSpinBox* spinBox, bool setUnitFromOutsideToggle = false);

    static KisDoubleParseUnitSpinBox* createUnitSpinBoxWithDocumentAwareness(QWidget* parent = 0);

    KisDocumentAwareSpinBoxUnitManager(QObject *parent = 0, int pPixDir = PIX_DIR_X);

    qreal getConversionFactor(int dim, QString psymbol) const override;
    qreal getConversionConstant(int dim, QString symbol) const override;

protected:

    virtual bool hasPercent(int unitDim) const override;

    PixDir pixDir;
};

#endif
