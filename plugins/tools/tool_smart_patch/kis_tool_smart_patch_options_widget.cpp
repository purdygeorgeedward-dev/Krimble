/*
 *  SPDX-FileCopyrightText: 2017 Eugene Ingerman
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_smart_patch_options_widget.h"

#include "ui_kis_tool_smart_patch_options_widget.h"

#include <KoColorSpaceRegistry.h>
#include "KisPaletteModel.h"

#include "kis_config.h"
#include <resources/KoColorSet.h>
#include "kis_canvas_resource_provider.h"

#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <klocalizedstring.h>
#include "krimble_fill.h"   // Krimble 2026-10-19: the fill method


struct KisToolSmartPatchOptionsWidget::Private {
    Private()
    {
    }

    Ui_KisToolSmartPatchOptionsWidget *ui {nullptr};

    int getPatchRadius(void)
    {
        return ui->patchRadius->value();
    }
    int getAccuracy(void)
    {
        return ui->accuracySlider->value();
    }
};

KisToolSmartPatchOptionsWidget::KisToolSmartPatchOptionsWidget(KisCanvasResourceProvider */*provider*/, QWidget *parent)
    : QWidget(parent),
      m_d(new Private)
{
    m_d->ui = new Ui_KisToolSmartPatchOptionsWidget();
    m_d->ui->setupUi(this);

    // Krimble 2026-10-19: George: "Make the different types options in the tool options panel." The fill method, at the top of the healing brush's
    // Tool Options. It is saved, and Edit > Fill > Content-Aware... uses the same one. The two settings below it (accuracy, patch radius) are Krita's
    // and only change the Classic method.
    QLabel *methodLabel = new QLabel(i18n("Fill method:"), this);
    QComboBox *methodBox = new QComboBox(this);
    methodBox->addItem(i18n("Sharp: copies real pixels"), int(KrimbleFill::Sharp));
    methodBox->addItem(i18n("Smooth: blends (skin, sky)"), int(KrimbleFill::Smooth));
    methodBox->addItem(i18n("Classic: Krita's fill"), int(KrimbleFill::Classic));
    methodBox->setCurrentIndex(qMax(0, methodBox->findData(KrimbleFill::savedMethod())));
    connect(methodBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [methodBox](int) {
        KrimbleFill::saveMethod(methodBox->currentData().toInt());
    });
    if (QBoxLayout *box = qobject_cast<QBoxLayout*>(layout())) {
        box->insertWidget(0, methodBox);
        box->insertWidget(0, methodLabel);
    }
}

KisToolSmartPatchOptionsWidget::~KisToolSmartPatchOptionsWidget()
{
    delete m_d->ui;
    m_d->ui = nullptr;
}

int KisToolSmartPatchOptionsWidget::getPatchRadius()
{
    return m_d->getPatchRadius();
}

int KisToolSmartPatchOptionsWidget::getAccuracy()
{
    return m_d->getAccuracy();
}

