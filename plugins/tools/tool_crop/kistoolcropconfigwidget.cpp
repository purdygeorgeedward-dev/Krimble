/*
    SPDX-FileCopyrightText: 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>

    SPDX-License-Identifier: GPL-2.0-or-later
*/


#include <QLabel>
#include <QBoxLayout>
#include "kistoolcropconfigwidget.h"
#include "kis_tool_crop.h"
#include <kis_icon.h>
#include <kis_acyclic_signal_connector.h>
#include <QStandardItemModel>


KisToolCropConfigWidget::KisToolCropConfigWidget(QWidget* parent, KisToolCrop* cropTool)
    : QWidget(parent)
    , m_cropTool(cropTool)
{
    setupUi(this);

#ifdef Q_OS_ANDROID
    // Krimble 2026-10-18: George (1.0.37, fc126d8): "6. Crop tool options don't line up well." / "Buttons, contents of panel." The two groups of fields (X and Y, then Width, Height and
    // Ratio with their lock buttons) stood side by side, wider than the panel, and each group had its own columns. They are stacked, and all the
    // labels get the same width and are aligned to the right, so the fields start at the same place.
    if (QBoxLayout *groups = findChild<QBoxLayout*>(QStringLiteral("horizontalLayout"))) {
        groups->setDirection(QBoxLayout::TopToBottom);
    }
    {
        const QList<QLabel*> labels = { label_horizPos, label_vertiPos, widthLabel, heightLabel, aspectLabel, label, label_2 };
        int labelWidth = 0;
        Q_FOREACH (QLabel *l, labels) labelWidth = qMax(labelWidth, l->sizeHint().width());
        Q_FOREACH (QLabel *l, labels) {
            l->setMinimumWidth(labelWidth);
            l->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
    }
#endif

    // update the UI based off data from crop tool
    cmbType->setCurrentIndex(m_cropTool->cropType());
    QStandardItemModel *cmbTypeModel = qobject_cast<QStandardItemModel *>(cmbType->model());
    // Disable "Layer"
    cmbTypeModel->item(2)->setEnabled(m_cropTool->cropTypeSelectable());
    // Disable "Frame"
    cmbTypeModel->item(3)->setEnabled(m_cropTool->cropTypeSelectable());

    intHeight->setValue(m_cropTool->cropHeight());
    intWidth->setValue(m_cropTool->cropWidth());
    intX->setValue(m_cropTool->cropX());
    intY->setValue(m_cropTool->cropY());
    doubleRatio->setValue(m_cropTool->ratio());
    cmbDecor->setCurrentIndex(m_cropTool->decoration());
    boolGrow->setChecked(m_cropTool->allowGrow());
    boolCenter->setChecked(m_cropTool->growCenter());

    lockRatioButton->setChecked(m_cropTool->lockRatio());
    lockHeightButton->setChecked(m_cropTool->lockHeight());
    lockWidthButton->setChecked(m_cropTool->lockWidth());

    QIcon lockedIcon = KisIconUtils::loadIcon("locked");
    QIcon unlockedIcon = KisIconUtils::loadIcon("unlocked");
    lockWidthButton->setIcon(lockWidthButton->isChecked() ? lockedIcon : unlockedIcon);
    lockHeightButton->setIcon(lockHeightButton->isChecked() ? lockedIcon : unlockedIcon);
    lockRatioButton->setIcon(lockRatioButton->isChecked() ? lockedIcon : unlockedIcon);

    KisAcyclicSignalConnector *connector;
    connector = new KisAcyclicSignalConnector(this);
    connector->connectForwardBool(lockRatioButton, SIGNAL(toggled(bool)), this, SIGNAL(lockRatioChanged(bool)));
    connector->connectBackwardBool(cropTool, SIGNAL(lockRatioChanged(bool)), lockRatioButton, SLOT(setChecked(bool)));

    connector = new KisAcyclicSignalConnector(this);
    connector->connectForwardBool(lockHeightButton, SIGNAL(toggled(bool)), this, SIGNAL(lockHeightChanged(bool)));
    connector->connectBackwardBool(cropTool, SIGNAL(lockHeightChanged(bool)), lockHeightButton, SLOT(setChecked(bool)));

    connector = new KisAcyclicSignalConnector(this);
    connector->connectForwardBool(lockWidthButton, SIGNAL(toggled(bool)), this, SIGNAL(lockWidthChanged(bool)));
    connector->connectBackwardBool(cropTool, SIGNAL(lockWidthChanged(bool)), lockWidthButton, SLOT(setChecked(bool)));

    connector = new KisAcyclicSignalConnector(this);
    connector->connectForwardDouble(doubleRatio, SIGNAL(valueChanged(double)), this, SIGNAL(ratioChanged(double)));
    connector->connectBackwardDouble(cropTool, SIGNAL(ratioChanged(double)), doubleRatio, SLOT(setValue(double)));

    connector = new KisAcyclicSignalConnector(this);
    connector->connectForwardInt(intHeight, SIGNAL(valueChanged(int)), this, SIGNAL(cropHeightChanged(int)));
    connector->connectBackwardInt(cropTool, SIGNAL(cropHeightChanged(int)), intHeight, SLOT(setValue(int)));

    connector = new KisAcyclicSignalConnector(this);
    connector->connectForwardInt(intWidth, SIGNAL(valueChanged(int)), this, SIGNAL(cropWidthChanged(int)));
    connector->connectBackwardInt(cropTool, SIGNAL(cropWidthChanged(int)), intWidth, SLOT(setValue(int)));

    connector = new KisAcyclicSignalConnector(this);
    connector->connectForwardInt(intX, SIGNAL(valueChanged(int)), this, SIGNAL(cropXChanged(int)));
    connector->connectBackwardInt(cropTool, SIGNAL(cropXChanged(int)), intX, SLOT(setValue(int)));

    connector = new KisAcyclicSignalConnector(this);
    connector->connectForwardInt(intY, SIGNAL(valueChanged(int)), this, SIGNAL(cropYChanged(int)));
    connector->connectBackwardInt(cropTool, SIGNAL(cropYChanged(int)), intY, SLOT(setValue(int)));

    connector = new KisAcyclicSignalConnector(this);
    connector->connectForwardInt(cmbType, SIGNAL(currentIndexChanged(int)), this, SIGNAL(cropTypeChanged(int)));
    connector->connectBackwardInt(cropTool, SIGNAL(cropTypeChanged(int)), cmbType, SLOT(setCurrentIndex(int)));

    connector = new KisAcyclicSignalConnector(this);
    connector->connectForwardInt(cmbDecor, SIGNAL(currentIndexChanged(int)), this, SIGNAL(decorationChanged(int)));
    connector->connectBackwardInt(cropTool, SIGNAL(decorationChanged(int)), cmbDecor, SLOT(setCurrentIndex(int)));

    connector = new KisAcyclicSignalConnector(this);
    connector->connectForwardBool(boolGrow, SIGNAL(toggled(bool)), this, SIGNAL(allowGrowChanged(bool)));
    connector->connectBackwardBool(cropTool, SIGNAL(canGrowChanged(bool)), boolGrow, SLOT(setChecked(bool)));

    connector = new KisAcyclicSignalConnector(this);
    connector->connectForwardBool(boolCenter, SIGNAL(toggled(bool)), this, SIGNAL(growCenterChanged(bool)));
    connector->connectBackwardBool(cropTool, SIGNAL(isCenteredChanged(bool)), boolCenter, SLOT(setChecked(bool)));

    connector = new KisAcyclicSignalConnector(this);
    //connector->connectForwardDouble();
    connector->connectBackwardVoid(cropTool, SIGNAL(cropTypeSelectableChanged()), this, SLOT(cropTypeSelectableChanged()));

    connect(lockWidthButton, SIGNAL(toggled(bool)), this, SLOT(updateLockWidthIcon()));
    connect(lockHeightButton, SIGNAL(toggled(bool)), this, SLOT(updateLockHeightIcon()));
    connect(lockRatioButton, SIGNAL(toggled(bool)), this, SLOT(updateLockRatioIcon()));

}

void KisToolCropConfigWidget::cropTypeSelectableChanged()
{
    QStandardItemModel *cmbTypeModel = qobject_cast<QStandardItemModel *>(cmbType->model());
    // Disable "Layer"
    cmbTypeModel->item(2)->setEnabled(m_cropTool->cropTypeSelectable());
    // Disable "Frame"
    cmbTypeModel->item(3)->setEnabled(m_cropTool->cropTypeSelectable());
}

void KisToolCropConfigWidget::updateLockRatioIcon()
{
    lockRatioButton->setIcon(lockRatioButton->isChecked() ? KisIconUtils::loadIcon("locked") : KisIconUtils::loadIcon("unlocked"));
}

void KisToolCropConfigWidget::updateLockWidthIcon()
{
    lockWidthButton->setIcon(lockWidthButton->isChecked() ? KisIconUtils::loadIcon("locked") : KisIconUtils::loadIcon("unlocked"));
}

void KisToolCropConfigWidget::updateLockHeightIcon()
{
    lockHeightButton->setIcon(lockHeightButton->isChecked() ? KisIconUtils::loadIcon("locked") : KisIconUtils::loadIcon("unlocked"));
}
