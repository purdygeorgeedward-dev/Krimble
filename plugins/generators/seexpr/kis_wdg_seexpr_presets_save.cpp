/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2020 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <KStandardGuiItem>
#include <QDate>
#include <QMessageBox>
#include <QTime>

#include <KisImportExportManager.h>
#include <KisResourceUserOperations.h>
#include <KoColorSpaceRegistry.h>
#include <KoFileDialog.h>
#include <KoResourceServerProvider.h>

#include <kis_fill_painter.h>
#include <kis_paint_device.h>
#include <kis_default_bounds.h>

#include "KisResourceTypes.h"
#include "kis_wdg_seexpr_presets_save.h"
#include "resources/KisSeExprScript.h"

KisWdgSeExprPresetsSave::KisWdgSeExprPresetsSave(QWidget *parent)
    : KisWdgSeExprSavePreset(parent)
    , m_currentPreset(nullptr)
{
    connect(loadExistingThumbnailButton, SIGNAL(clicked(bool)), this, SLOT(loadExistingThumbnail()));
    connect(loadImageIntoThumbnailButton, SIGNAL(clicked(bool)), this, SLOT(loadImageFromFile()));
    connect(loadRenderFromScriptButton, SIGNAL(clicked(bool)), this, SLOT(renderScriptToThumbnail()));
    connect(clearPresetThumbnailButton, SIGNAL(clicked(bool)), presetThumbnailWidget, SLOT(clear()));

    KGuiItem::assign(buttons->button(QDialogButtonBox::Save), KStandardGuiItem::save());
    KGuiItem::assign(buttons->button(QDialogButtonBox::Cancel), KStandardGuiItem::cancel());

    connect(buttons, SIGNAL(accepted()), this, SLOT(savePreset()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(close()));
}

KisWdgSeExprPresetsSave::~KisWdgSeExprPresetsSave()
{
}

void KisWdgSeExprPresetsSave::setCurrentPreset(KisSeExprScriptSP resource)
{
    m_currentPreset = resource;
}

void KisWdgSeExprPresetsSave::setCurrentRenderConfiguration(KisFilterConfigurationSP config)
{
    m_currentConfiguration = config;
}

void KisWdgSeExprPresetsSave::showDialog()
{
    setModal(true);

    KisSeExprScriptSP preset = m_currentPreset;

    if (m_useNewPresetDialog) {
        setWindowTitle(i18n("Save New SeExpr Preset"));

        if (preset) {
            QString name = preset->name().replace("_", " ");
            if (preset->resourceId() > -1) {
                name = QString("%1 %2").arg(name, i18n("Copy"));
            }
            newPresetNameTextField->setText(name);
            newPresetNameTextField->setReadOnly(false);
            newPresetNameTextField->setEnabled(true);
        }
    } else {
        setWindowTitle(i18n("Save SeExpr Preset"));

        if (preset) {
            const QString name(preset->name().replace("_", " "));
            newPresetNameTextField->setText(name);
            newPresetNameTextField->setReadOnly(true);
            newPresetNameTextField->setEnabled(false);
        }
    }

    if (preset) {
        presetThumbnailWidget->setPixmap(QPixmap::fromImage(preset->image()));
    }

    open();
}

void KisWdgSeExprPresetsSave::loadImageFromFile()
{
    KoFileDialog dialog(0, KoFileDialog::OpenFile, "OpenDocument");
    dialog.setMimeTypeFilters(KisImportExportManager::supportedMimeTypes(KisImportExportManager::Import));
    dialog.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    QString filename = dialog.filename();

    if (!filename.isEmpty()) {
        const QImage imageToLoad(filename);

        presetThumbnailWidget->clear();
        presetThumbnailWidget->setPixmap(QPixmap::fromImage(imageToLoad));
    }
}

void KisWdgSeExprPresetsSave::loadExistingThumbnail()
{
    presetThumbnailWidget->setPixmap(QPixmap::fromImage(m_currentPreset->image()));
}

void KisWdgSeExprPresetsSave::renderScriptToThumbnail()
{
    if (m_currentConfiguration) {
        KisDefaultBoundsBaseSP bounds(new KisWrapAroundBoundsWrapper(new KisDefaultBounds(), QRect(0, 0, 256, 256)));
        KisPaintDeviceSP src = new KisPaintDevice(KoColorSpaceRegistry::instance()->rgb8());
        src->setDefaultBounds(bounds);
        src->setSupportsWraparoundMode(true);
        KisFillPainter fillPainter(src);
        fillPainter.fillRect(0, 0, 256, 256, m_currentConfiguration);

        QImage thumbnail = src->convertToQImage(nullptr, 0, 0, 256, 256);
        presetThumbnailWidget->setPixmap(QPixmap::fromImage(thumbnail));
    }
}

void KisWdgSeExprPresetsSave::savePreset()
{
    KIS_ASSERT_RECOVER_RETURN(m_currentPreset);

    KisResourceModel model(ResourceType::SeExprScripts);
    QModelIndex idx = model.indexForResourceId(m_currentPreset->resourceId());
    bool r = true;

    if (idx.isValid() && !m_useNewPresetDialog) {
        const QString presetFileName = m_currentPreset->name().split(" ").join("_");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        if (!presetThumbnailWidget->pixmap()->isNull()) {
            m_currentPreset->setImage(presetThumbnailWidget->pixmap()->toImage());
        }
#else
        if (!presetThumbnailWidget->pixmap().isNull()) {
            m_currentPreset->setImage(presetThumbnailWidget->pixmap().toImage());
        }
#endif
        m_currentPreset->setScript(m_currentConfiguration->getString("script"));
        m_currentPreset->setFilename(presetFileName + m_currentPreset->defaultFileExtension());
        m_currentPreset->setValid(true);
        r = KisResourceUserOperations::updateResourceWithUserInput(this, m_currentPreset);
        if (r) {
            Q_EMIT resourceSelected(m_currentPreset);
        }
    } else {
        KisSeExprScriptSP newPreset = m_currentPreset->clone().dynamicCast<KisSeExprScript>();

        if (newPreset) {
            const QString presetFileName = newPresetNameTextField->text().split(" ").join("_");
            newPreset->setName(newPresetNameTextField->text());
            newPreset->setFilename(presetFileName + newPreset->defaultFileExtension());
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            if (!presetThumbnailWidget->pixmap()->isNull()) {
                newPreset->setImage(presetThumbnailWidget->pixmap()->toImage());
            }
#else
            if (!presetThumbnailWidget->pixmap().isNull()) {
                newPreset->setImage(presetThumbnailWidget->pixmap().toImage());
            }
#endif
            newPreset->setScript(m_currentConfiguration->getString("script"));
            newPreset->setValid(true);

            r = KisResourceUserOperations::addResourceWithUserInput(this, newPreset);

            if (r) {
                m_currentPreset = newPreset;
                Q_EMIT resourceSelected(m_currentPreset);
            }
        }
    }

    close();
}

void KisWdgSeExprPresetsSave::useNewPresetDialog(bool show)
{
    m_useNewPresetDialog = show;
}

#include "moc_kis_wdg_seexpr_presets_save.cpp"
