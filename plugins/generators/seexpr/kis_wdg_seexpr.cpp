/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2020 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QCheckBox>

#include <KSeExprUI/ErrorMessages.h>
#include <KisDialogStateSaver.h>
#include <KisGlobalResourcesInterface.h>
#include <KisResourceUserOperations.h>
#include <KoColor.h>
#include <KoResourceServer.h>
#include <KoResourceServerProvider.h>

#include <filter/kis_filter_configuration.h>
#include <kis_assert.h>
#include <kis_config.h>
#include <kis_debug.h>
#include <kis_icon.h>
#include <kis_signals_blocker.h>

#include "SeExprExpressionContext.h"
#include "generator.h"
#include "kis_wdg_seexpr.h"
#include "ui_wdgseexpr.h"

KisWdgSeExpr::KisWdgSeExpr(QWidget *parent)
    : KisConfigWidget(parent)
    , updateCompressor(1000, KisSignalCompressor::Mode::POSTPONE)
    , m_currentPreset(new KisSeExprScript(i18n("Untitled")))
    , m_saveDialog(new KisWdgSeExprPresetsSave(this))
    , m_isCreatingPresetFromScratch(true)
{
    m_widget = new Ui_WdgSeExpr();
    m_widget->setupUi(this);
    m_widget->txtEditor->setControlCollectionWidget(m_widget->wdgControls);

    m_widget->renameBrushPresetButton->setIcon(KisIconUtils::loadIcon("document-edit"));

    m_widget->reloadPresetButton->setIcon(KisIconUtils::loadIcon("reload-preset-16"));
    m_widget->reloadPresetButton->setToolTip(i18n("Reload the preset"));
    m_widget->dirtyPresetIndicatorButton->setIcon(KisIconUtils::loadIcon("warning"));
    m_widget->dirtyPresetIndicatorButton->setToolTip(i18n("The settings for this preset have changed from their default."));

    KisDialogStateSaver::restoreState(m_widget->txtEditor, "krita/generators/seexpr");
    m_widget->txtEditor->setExpr(m_widget->txtEditor->exprTe->toPlainText());

    m_widget->txtEditor->registerExtraVariable("$u", i18nc("SeExpr variable", "Normalized X axis coordinate of the image from its top-left corner"));
    m_widget->txtEditor->registerExtraVariable("$v", i18nc("SeExpr variable", "Normalized Y axis coordinate of the image from its top-left corner"));
    m_widget->txtEditor->registerExtraVariable("$w", i18nc("SeExpr variable", "Image width"));
    m_widget->txtEditor->registerExtraVariable("$h", i18nc("SeExpr variable", "Image height"));

    m_widget->txtEditor->updateCompleter();

    m_widget->txtEditor->exprTe->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    const QFontMetricsF fntTe(m_widget->txtEditor->exprTe->fontMetrics());
    m_widget->txtEditor->exprTe->setTabStopDistance(fntTe.horizontalAdvance("    "));

    connect(m_widget->scriptSelectorWidget, SIGNAL(resourceSelected(KoResourceSP)), this, SLOT(slotResourceSelected(KoResourceSP)));
    connect(m_saveDialog, SIGNAL(resourceSelected(KoResourceSP)), this, SLOT(slotResourceSaved(KoResourceSP)));

    connect(m_widget->renameBrushPresetButton, SIGNAL(clicked(bool)),
            this, SLOT(slotRenamePresetActivated()));
    connect(m_widget->cancelBrushNameUpdateButton, SIGNAL(clicked(bool)),
            this, SLOT(slotRenamePresetDeactivated()));
    connect(m_widget->updateBrushNameButton, SIGNAL(clicked(bool)),
            this, SLOT(slotSaveRenameCurrentPreset()));
    connect(m_widget->renameBrushNameTextField, SIGNAL(returnPressed()),
            this, SLOT(slotSaveRenameCurrentPreset()));

    connect(m_widget->saveBrushPresetButton, SIGNAL(clicked()),
        this, SLOT(slotSaveBrushPreset()));
    connect(m_widget->saveNewBrushPresetButton, SIGNAL(clicked()),
        this, SLOT(slotSaveNewBrushPreset()));

    connect(m_widget->reloadPresetButton, SIGNAL(clicked()),
        this, SLOT(slotReloadPresetClicked()));

    connect(m_widget->txtEditor, SIGNAL(apply()),
            &updateCompressor, SLOT(start()));
    connect(m_widget->txtEditor, SIGNAL(preview()),
            &updateCompressor, SLOT(start()));
    connect(m_widget->txtEditor, &ExprEditor::preview, this, &KisWdgSeExpr::slotHideCheckboxes);

    connect(&updateCompressor, SIGNAL(timeout()), this, SLOT(isValid()));

    togglePresetRenameUIActive(false);
    slotUpdatePresetSettings();

    m_widget->splitter->restoreState(KisConfig(true).readEntry("seExpr/splitLayoutState", QByteArray()));
    m_widget->tabWidget->setCurrentIndex(KisConfig(true).readEntry("seExpr/selectedTab",  -1));
}

KisWdgSeExpr::~KisWdgSeExpr()
{
    KisDialogStateSaver::saveState(m_widget->txtEditor, "krita/generators/seexpr");
    KisConfig(false).writeEntry("seExpr/splitLayoutState", m_widget->splitter->saveState());
    KisConfig(false).writeEntry("seExpr/selectedTab", m_widget->tabWidget->currentIndex());

    delete m_saveDialog;
    delete m_widget;
}

inline const Ui_WdgSeExpr *KisWdgSeExpr::widget() const
{
    return m_widget;
}

void KisWdgSeExpr::setConfiguration(const KisPropertiesConfigurationSP config)
{
    auto rserver = KoResourceServerProvider::instance()->seExprScriptServer();
    auto name = config->getString("seexpr", "Disney_noisecolor2");
    auto pattern = rserver->resource("", "", name);
    if (pattern) {
        m_widget->scriptSelectorWidget->setCurrentScript(pattern);
    }

    QString script = config->getString("script");

    if (!script.isNull()) {
        m_widget->txtEditor->setExpr(script, true);
    }
}

KisPropertiesConfigurationSP KisWdgSeExpr::configuration() const
{
    KisFilterConfigurationSP config = new KisFilterConfiguration("seexpr", 1, KisGlobalResourcesInterface::instance());

    if (m_widget->scriptSelectorWidget->currentResource()) {
        QVariant v;
        v.setValue(m_widget->scriptSelectorWidget->currentResource()->name());
        config->setProperty("pattern", v);
    }
    config->setProperty("script", QVariant(m_widget->txtEditor->getExpr()));

    return config;
}

void KisWdgSeExpr::slotResourceSaved(KoResourceSP resource)
{
    if (resource) {
        m_widget->scriptSelectorWidget->setCurrentScript(resource);
        slotResourceSelected(resource);
    }
}

void KisWdgSeExpr::slotResourceSelected(KoResourceSP resource)
{
    KisSeExprScriptSP preset = resource.dynamicCast<KisSeExprScript>();
    if (preset) {
        m_currentPreset = preset;

        m_isCreatingPresetFromScratch = false;

        m_widget->txtEditor->setExpr(m_currentPreset->script(), true);

        QString formattedBrushName = m_currentPreset->name().replace("_", " ");
        m_widget->currentBrushNameLabel->setText(formattedBrushName);
        m_widget->renameBrushNameTextField->setText(formattedBrushName);
        QSize thumbSize = QSize(55, 55)*devicePixelRatioF();
        QPixmap thumbnail = QPixmap::fromImage(m_currentPreset->image().scaled(thumbSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        thumbnail.setDevicePixelRatio(devicePixelRatioF());
        m_widget->presetThumbnailicon->setPixmap(thumbnail);

        togglePresetRenameUIActive(false);
        slotUpdatePresetSettings();

        updateCompressor.start();
    }
}

void KisWdgSeExpr::slotRenamePresetActivated()
{
    togglePresetRenameUIActive(true);
}

void KisWdgSeExpr::slotRenamePresetDeactivated()
{
    togglePresetRenameUIActive(false);
}

void KisWdgSeExpr::togglePresetRenameUIActive(bool isRenaming)
{
    m_widget->renameBrushNameTextField->setVisible(isRenaming);
    m_widget->updateBrushNameButton->setVisible(isRenaming);
    m_widget->cancelBrushNameUpdateButton->setVisible(isRenaming);

    m_widget->currentBrushNameLabel->setVisible(!isRenaming);
    m_widget->renameBrushPresetButton->setVisible(!isRenaming);
    m_widget->saveBrushPresetButton->setEnabled(!isRenaming);
    m_widget->saveBrushPresetButton->setVisible(!isRenaming);
    m_widget->saveNewBrushPresetButton->setEnabled(!isRenaming);
    m_widget->saveNewBrushPresetButton->setVisible(!isRenaming);
}

void KisWdgSeExpr::slotSaveRenameCurrentPreset()
{
    KIS_ASSERT_RECOVER_RETURN(m_currentPreset);

    const auto prevScript = m_currentPreset->script();
    bool isDirty = m_currentPreset->isDirty();

    togglePresetRenameUIActive(false);
    slotUpdatePresetSettings();

    int currentPresetResourceId = m_currentPreset->resourceId();

    QString renamedPresetName = m_widget->renameBrushNameTextField->text();

    if (currentPresetResourceId < 0) {
        m_currentPreset->setName(renamedPresetName);
        slotUpdatePresetSettings();
        return;
    }

    slotReloadPresetClicked();

    KisResourceModel model(ResourceType::SeExprScripts);
    KoResourceSP properCleanResource = model.resourceForId(currentPresetResourceId);
    const bool success = KisResourceUserOperations::renameResourceWithUserInput(this, properCleanResource, renamedPresetName);

    if (isDirty) {
        properCleanResource.dynamicCast<KisSeExprScript>()->setScript(prevScript);
        properCleanResource.dynamicCast<KisSeExprScript>()->setDirty(isDirty);
    }

    if (success) slotResourceSelected(properCleanResource);

    slotUpdatePresetSettings();
}

void KisWdgSeExpr::slotUpdatePresetSettings()
{
    if (m_isCreatingPresetFromScratch) {
        m_widget->presetThumbnailicon->setVisible(false);
        m_widget->dirtyPresetIndicatorButton->setVisible(false);
        m_widget->reloadPresetButton->setVisible(false);
        m_widget->saveBrushPresetButton->setVisible(false);
        m_widget->saveNewBrushPresetButton->setEnabled(false);
        m_widget->renameBrushPresetButton->setVisible(false);
    } else {
        if (!m_currentPreset) {
            return;
        }

        bool isPresetDirty = m_currentPreset->isDirty();

        m_widget->presetThumbnailicon->setVisible(true);
        m_widget->dirtyPresetIndicatorButton->setVisible(isPresetDirty);
        m_widget->reloadPresetButton->setVisible(isPresetDirty);
        m_widget->saveBrushPresetButton->setEnabled(isPresetDirty);
        m_widget->saveNewBrushPresetButton->setEnabled(true);
        m_widget->renameBrushPresetButton->setVisible(true);
    }
}

void KisWdgSeExpr::slotSaveBrushPreset()
{
    KisFilterConfigurationSP currentConfiguration = static_cast<KisFilterConfiguration *>(configuration().data());

    m_saveDialog->useNewPresetDialog(false);
    m_saveDialog->setCurrentPreset(m_currentPreset);
    m_saveDialog->setCurrentRenderConfiguration(currentConfiguration);
    m_saveDialog->loadExistingThumbnail();
    m_saveDialog->showDialog();
}

void KisWdgSeExpr::slotSaveNewBrushPreset()
{
    KisFilterConfigurationSP currentConfiguration = static_cast<KisFilterConfiguration *>(configuration().data());

    m_saveDialog->useNewPresetDialog(true);
    m_saveDialog->setCurrentPreset(m_currentPreset);
    m_saveDialog->setCurrentRenderConfiguration(currentConfiguration);
    m_saveDialog->loadExistingThumbnail();
    m_saveDialog->showDialog();
}

void KisWdgSeExpr::slotReloadPresetClicked()
{
    KisSignalsBlocker blocker(this);

    KisResourceModel model(ResourceType::SeExprScripts);
    const bool success = model.reloadResource(m_currentPreset);

    KIS_SAFE_ASSERT_RECOVER_NOOP(success && "couldn't reload preset");

    warnPlugins << "resourceSelected: preset" << m_currentPreset
                << (m_currentPreset ? QString("%1").arg(m_currentPreset->valid()) : "");

    KIS_ASSERT(!m_currentPreset->isDirty());

    if (success) slotResourceSelected(m_currentPreset);
}

void KisWdgSeExpr::slotHideCheckboxes()
{
    for (auto controls : m_widget->wdgControls->findChildren<ExprControl *>()) {
        for (auto wdg : controls->findChildren<QCheckBox *>(QString(), Qt::FindDirectChildrenOnly)) {
            wdg->setCheckState(Qt::Unchecked);
            wdg->setVisible(false);
        }
    }
}

void KisWdgSeExpr::isValid()
{
    QString script = m_widget->txtEditor->getExpr();
    SeExprExpressionContext expression(script);

    expression.setDesiredReturnType(KSeExpr::ExprType().FP(3));

    expression.m_vars["u"] = new SeExprVariable();
    expression.m_vars["v"] = new SeExprVariable();
    expression.m_vars["w"] = new SeExprVariable();
    expression.m_vars["h"] = new SeExprVariable();

    m_widget->txtEditor->clearErrors();

    if (!expression.isValid()) {
        const auto &errors = expression.getErrors();

        for (const auto &occurrence : errors) {
            QString message = ErrorMessages::message(occurrence.error);
            for (const auto &arg : occurrence.ids) {
                message = message.arg(QString::fromStdString(arg));
            }
            m_widget->txtEditor->addError(occurrence.startPos, occurrence.endPos, message);
        }

        m_widget->saveBrushPresetButton->setEnabled(false);
        m_widget->saveNewBrushPresetButton->setEnabled(false);
    }
    else if (!expression.returnType().isFP(3)) {
        QString type = QString::fromStdString(expression.returnType().toString());
        m_widget->txtEditor->addError(1, 1, tr2i18n("Expected this script to output color, got '%1'").arg(type));

        m_widget->saveBrushPresetButton->setEnabled(false);
        m_widget->saveNewBrushPresetButton->setEnabled(false);
    } else {
        m_widget->txtEditor->clearErrors();
        Q_EMIT sigConfigurationItemChanged();

        if (m_currentPreset->script() != m_widget->txtEditor->getExpr()) {
            m_currentPreset->setScript(m_widget->txtEditor->getExpr());
            m_currentPreset->setDirty(true);
        }

        slotUpdatePresetSettings();

        if (m_isCreatingPresetFromScratch) {
            m_widget->saveNewBrushPresetButton->setEnabled(true);
        }
    }
}
