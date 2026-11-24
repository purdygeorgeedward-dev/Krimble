/*
 *  SPDX-FileCopyrightText: 2017 Laurent Valentin Jospin <laurent.valentin@famillejospin.ch>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_document_aware_spin_box_unit_manager.h"

#include "KisPart.h"
#include "KisMainWindow.h"
#include "KisView.h"
#include "KisDocument.h"
#include "kis_types.h"
#include "kis_image.h"
#include "kis_image_animation_interface.h"
#include "kis_time_span.h"

KisSpinBoxUnitManager* KisDocumentAwareSpinBoxUnitManagerBuilder::buildUnitManager(QObject* parent)
{
    return new KisDocumentAwareSpinBoxUnitManager(parent);
}

void KisDocumentAwareSpinBoxUnitManager::setDocumentAwarenessToExistingUnitSpinBox(KisDoubleParseUnitSpinBox* spinBox, bool setUnitFromOutsideToggle)
{
    KisDocumentAwareSpinBoxUnitManager* manager = new KisDocumentAwareSpinBoxUnitManager(spinBox);
    spinBox->setUnitManager(manager);
    spinBox->setUnitChangeFromOutsideBehavior(setUnitFromOutsideToggle);
}

KisDoubleParseUnitSpinBox* KisDocumentAwareSpinBoxUnitManager::createUnitSpinBoxWithDocumentAwareness(QWidget* parent)
{
    KisDoubleParseUnitSpinBox* spinBox = new KisDoubleParseUnitSpinBox(parent);
    setDocumentAwarenessToExistingUnitSpinBox(spinBox);

    return spinBox;
}

KisDocumentAwareSpinBoxUnitManager::KisDocumentAwareSpinBoxUnitManager(QObject *parent, int pPixDir):
    KisSpinBoxUnitManager(parent)
{
    if (pPixDir == PIX_DIR_Y) {
        pixDir = PIX_DIR_Y;
    } else {
        pixDir = PIX_DIR_X;
    }

    grantDocumentRelativeUnits();
}

qreal KisDocumentAwareSpinBoxUnitManager::getConversionFactor(int dim, QString psymbol) const
{
    QString symbol = psymbol;

    if (symbol == "%") {
        if (pixDir == PIX_DIR_X) {
            symbol = "vw";
        } else {
            symbol = "vh";
        }
    }

    qreal factor = KisSpinBoxUnitManager::getConversionFactor(dim, symbol);

    if (factor > 0) {
        return factor;
    }

    factor = 1;

    if (!KisPart::instance()->currentMainwindow()) {
        return factor;
    }

    KisView* view = KisPart::instance()->currentMainwindow()->activeView();

    if (view == nullptr) {
        return factor;
    }

    KisDocument* doc = view->document();

    if (doc == nullptr) {
        return factor;
    }

    KisImage* img = doc->image().data();

    if (img == nullptr) {
        return factor;
    }

    qreal resX = img->xRes();
    qreal resY = img->yRes();
    qreal sizeX = img->width();
    qreal sizeY = img->height();

    switch (dim) {

    case LENGTH:
        if (symbol == "px") {

            if (pixDir == PIX_DIR_X) {
                factor = resX;
            } else {
                factor = resY;
            }
        } else if (symbol == "vw") {
            qreal docWidth = sizeX/resX;

            factor = 100.0/docWidth;
        } else if (symbol == "vh") {
            qreal docHeight = sizeY/resY;

            factor = 100.0/docHeight;
        }
        break;

    case IMLENGTH:

        if (symbol == "vw") {
            factor = 100.0/sizeX;

        } else if (symbol == "vh") {
            factor = 100.0/sizeY;
        }
        break;

    case TIME:
    {
        if (symbol == "s") {
            qreal fps = img->animationInterface()->framerate();

            factor = 1/fps;
        } else if (symbol == "%") {
            const KisTimeSpan & time_range = img->animationInterface()->documentPlaybackRange();
            qreal n_frame = time_range.end() - time_range.start();

            factor = 100/n_frame;
        }
    }
        break;

    default:
        break;

    }

    return factor;
}

qreal KisDocumentAwareSpinBoxUnitManager::getConversionConstant(int dim, QString symbol) const
{
    if (dim == TIME && symbol == "%") {
        KisImage* img = KisPart::instance()->currentMainwindow()->activeView()->document()->image().data();
        const KisTimeSpan & time_range = img->animationInterface()->documentPlaybackRange();
        qreal n_frame = time_range.end() - time_range.start();

        return -time_range.start()*100.0/n_frame;
    }

    return KisSpinBoxUnitManager::getConversionConstant(dim, symbol);
}

bool KisDocumentAwareSpinBoxUnitManager::hasPercent(int unitDim) const {

    if (unitDim == IMLENGTH || unitDim == LENGTH) {
        return true;
    }

    return KisSpinBoxUnitManager::hasPercent(unitDim);
}
