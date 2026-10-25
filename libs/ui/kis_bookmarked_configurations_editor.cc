/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_bookmarked_configurations_editor.h"
#include "ui_wdgbookmarkedconfigurationseditor.h"

#include "kis_bookmarked_configurations_model.h"

#include <QBoxLayout>   // Krimble 2026-10-24
#include <QGuiApplication>
#include <QScreen>

struct KisBookmarkedConfigurationsEditor::Private {
    Ui_WdgBookmarkedConfigurationsEditor editorUi;
    KisBookmarkedConfigurationsModel* model;
    KisSerializableConfigurationSP currentConfig;
};


KisBookmarkedConfigurationsEditor::KisBookmarkedConfigurationsEditor(QWidget* parent, KisBookmarkedConfigurationsModel* model, const KisSerializableConfigurationSP currentConfig) : QDialog(parent), d(new Private)
{
    d->editorUi.setupUi(this);
    // Krimble 2026-10-24: George (test list item 13): "Edit Presets is too wide". The list asked for 400 px (and the buttons stood beside it: about 570 px); a phone upright is about
    // 340. The list asks for 150 px now (the .ui file), and on a narrow or upright screen the buttons are in a row under the list.
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        if (screen->availableGeometry().width() < screen->availableGeometry().height() || screen->availableGeometry().width() < 600) {
            if (QBoxLayout *outer = qobject_cast<QBoxLayout*>(layout())) {
                outer->setDirection(QBoxLayout::TopToBottom);
                if (outer->count() > 1) {
                    if (QBoxLayout *buttons = qobject_cast<QBoxLayout*>(outer->itemAt(1)->layout())) {
                        buttons->setDirection(QBoxLayout::LeftToRight);
                    }
                }
            }
        }
    }
    d->model = model;
    d->currentConfig = currentConfig;
    d->editorUi.listConfigurations->setModel(d->model);
    connect(d->editorUi.pushButtonClose, SIGNAL(pressed()), SLOT(accept()));

    connect(d->editorUi.listConfigurations->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(currentConfigChanged(QItemSelection,QItemSelection)));
    currentConfigChanged(d->editorUi.listConfigurations->selectionModel()->selection(),
                         d->editorUi.listConfigurations->selectionModel()->selection());

    connect(d->editorUi.pushButtonDelete, SIGNAL(pressed()), SLOT(deleteConfiguration()));
    connect(d->editorUi.pushButtonBookmarkCurrent, SIGNAL(pressed()), SLOT(addCurrentConfiguration()));

    if (!d->currentConfig) {
        d->editorUi.pushButtonBookmarkCurrent->setEnabled(false);
    }
}

KisBookmarkedConfigurationsEditor::~KisBookmarkedConfigurationsEditor()
{
    delete d;
}

void KisBookmarkedConfigurationsEditor::currentConfigChanged(const QItemSelection& selected, const QItemSelection&)
{
    if (d->model) {
        d->editorUi.pushButtonDelete->setEnabled(!(selected.indexes().isEmpty()) ?
                d->model->isIndexDeletable(selected.indexes().first()) :
                false);
    }
}

void KisBookmarkedConfigurationsEditor::addCurrentConfiguration()
{
    if (d->model) {
        d->model->newConfiguration(ki18n("New configuration %1"), d->currentConfig);
    }
}

void KisBookmarkedConfigurationsEditor::deleteConfiguration()
{
    if (d->model) {
        d->model->deleteIndex(d->editorUi.listConfigurations->currentIndex());
    }
}

