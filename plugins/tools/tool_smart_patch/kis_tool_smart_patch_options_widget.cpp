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
#include "krimble_fill.h"

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

KisToolSmartPatchOptionsWidget::KisToolSmartPatchOptionsWidget(KisCanvasResourceProvider * , QWidget *parent)
    : QWidget(parent),
      m_d(new Private)
{
    m_d->ui = new Ui_KisToolSmartPatchOptionsWidget();
    m_d->ui->setupUi(this);

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

