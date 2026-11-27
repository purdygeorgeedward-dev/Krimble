/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2017 Scott Petrovic <scottpetrovic@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "dialogs/KisDlgSavePreset.h"

#include <QDebug>
#include <QDate>
#include <QTime>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

#include <KoFileDialog.h>
#include "KisImportExportManager.h"
#include "QDesktopServices"
#include "KisResourceServerProvider.h"
#include <kis_paintop_preset_icon_library.h>
#include <KisResourceUserOperations.h>

#include <kstandardguiitem.h>

KisPresetSaveWidget::KisPresetSaveWidget(QWidget * parent)
    : KisPaintOpPresetSaveDialog(parent)
{
    brushPresetThumbnailWidget->setCutoutOverlayRect(QRect(0, 0, brushPresetThumbnailWidget->height(), brushPresetThumbnailWidget->width()));

    connect(clearBrushPresetThumbnailButton, SIGNAL(clicked(bool)), brushPresetThumbnailWidget, SLOT(fillDefault()));
    connect(loadImageIntoThumbnailButton, SIGNAL(clicked(bool)), this, SLOT(loadImageFromFile()));

    connect(loadScratchPadThumbnailButton, SIGNAL(clicked(bool)), this, SLOT(loadScratchpadThumbnail()));
    connect(loadExistingThumbnailButton, SIGNAL(clicked(bool)), this, SLOT(loadExistingThumbnail()));
    connect(loadIconLibraryThumbnailButton, SIGNAL(clicked(bool)), this, SLOT(loadImageFromLibrary()));

    KGuiItem::assign(buttons->button(QDialogButtonBox::Save), KStandardGuiItem::save());
    KGuiItem::assign(buttons->button(QDialogButtonBox::Cancel), KStandardGuiItem::cancel());
    connect(buttons, SIGNAL(accepted()), this, SLOT(savePreset()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
}

KisPresetSaveWidget::~KisPresetSaveWidget()
{

}

void KisPresetSaveWidget::scratchPadSetup(KisCanvasResourceProvider* resourceProvider)
{
    m_resourceProvider = resourceProvider;

    brushPresetThumbnailWidget->setupScratchPad(m_resourceProvider, Qt::white);
}

void KisPresetSaveWidget::showDialog()
{
    setModal(true);

    KisPaintOpPresetSP preset = m_resourceProvider->currentPreset();

    if (m_useNewBrushDialog) {
        setWindowTitle(i18n("Save New Brush Preset"));
        newBrushNameTexField->setVisible(true);
        clearBrushPresetThumbnailButton->setVisible(true);
        loadImageIntoThumbnailButton->setVisible(true);
        currentBrushNameLabel->setVisible(false);

        QString name = preset->name();
        if (preset && preset->resourceId() > -1) {
            name.append(" ").append(i18n("Copy"));
        }
        newBrushNameTexField->setText(name);

    } else {
        setWindowTitle(i18n("Save Brush Preset"));

        if (preset) {
            currentBrushNameLabel->setText(preset->name());
        }

        newBrushNameTexField->setVisible(false);
        currentBrushNameLabel->setVisible(true);
    }

    brushPresetThumbnailWidget->paintPresetImage();

    open();
}

void KisPresetSaveWidget::loadImageFromFile()
{
    KoFileDialog dialog(0, KoFileDialog::OpenFile, "OpenDocument");
    dialog.setMimeTypeFilters(KisImportExportManager::supportedMimeTypes(KisImportExportManager::Import));
    dialog.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    QString filename = dialog.filename();

    if (filename != "") {
        const QImage imageToLoad(filename);

        brushPresetThumbnailWidget->fillTransparent();
        brushPresetThumbnailWidget->paintCustomImage(imageToLoad);
    }

}

void KisPresetSaveWidget::loadScratchpadThumbnail()
{
    brushPresetThumbnailWidget->paintCustomImage(scratchPadThumbnailArea);
}

void KisPresetSaveWidget::loadExistingThumbnail()
{
    brushPresetThumbnailWidget->paintPresetImage();
}

void KisPresetSaveWidget::loadImageFromLibrary()
{
    QDialog dialog;
    dialog.setWindowTitle(i18n("Preset Icon Library"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    KisPaintopPresetIconLibrary *libWidget = new KisPaintopPresetIconLibrary(&dialog);
    layout->addWidget(libWidget);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    KGuiItem::assign(buttons->button(QDialogButtonBox::Ok), KStandardGuiItem::ok());
    KGuiItem::assign(buttons->button(QDialogButtonBox::Cancel), KStandardGuiItem::cancel());
    connect(buttons, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), &dialog, SLOT(reject()));
    layout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted) {

        QImage presetImage = libWidget->getImage();
        brushPresetThumbnailWidget->paintCustomImage(presetImage);
    }
}

void KisPresetSaveWidget::setFavoriteResourceManager(KisFavoriteResourceManager * favManager)
{
    m_favoriteResourceManager = favManager;
}

void KisPresetSaveWidget::savePreset()
{
    KisPaintOpPresetSP curPreset = m_resourceProvider->currentPreset();
    if (!curPreset) {
        return;
    }

    QString presetFileName = m_useNewBrushDialog ? newBrushNameTexField->text() : curPreset->name();
    presetFileName = presetFileName.replace(' ', '_').replace('.', '_');
    QString extension = curPreset->defaultFileExtension();

    if (!presetFileName.endsWith(extension)) {
        presetFileName.append(extension);
    }

    bool success = true;

    if (m_useNewBrushDialog) {
        KisPaintOpPresetSP newPreset = curPreset->clone().dynamicCast<KisPaintOpPreset>();
        newPreset->setResourceId(-1);
        newPreset->setFilename(presetFileName);
        newPreset->setName(m_useNewBrushDialog ? newBrushNameTexField->text() : curPreset->name());
        newPreset->setImage(brushPresetThumbnailWidget->cutoutOverlay());
        newPreset->setValid(true);
        newPreset->setStorageLocation("");
        if (!KisResourceUserOperations::addResourceWithUserInput(this, newPreset)) {
            success = false;
        }

        if (success) Q_EMIT resourceSelected(newPreset);

    }
    else {
        curPreset->setName(m_useNewBrushDialog ? newBrushNameTexField->text() : curPreset->name());
        curPreset->setImage(brushPresetThumbnailWidget->cutoutOverlay());
        curPreset->setFilename(presetFileName);

        if (!KisResourceUserOperations::updateResourceWithUserInput(this, curPreset)) {
            success = false;
        }

        if (success) Q_EMIT resourceSelected(curPreset);
    }

    if (success) {
        m_favoriteResourceManager->updateFavoritePresets();
        close();
    }

}

void KisPresetSaveWidget::saveScratchPadThumbnailArea(QImage image)
{
    scratchPadThumbnailArea = image;
}

void KisPresetSaveWidget::useNewBrushDialog(bool show)
{
    m_useNewBrushDialog = show;
}

