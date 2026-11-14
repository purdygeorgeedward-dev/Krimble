/*
 *  SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChannelDockerWidget.h"

#include <QGridLayout>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QApplication>

#include "channelmodel.h"
#include <KisViewManager.h>
#include <kis_canvas2.h>
#include "KisChannelsThumbnailsStrokeStrategy.h"
#include <kis_display_color_converter.h>
#include <KisDisplayConfig.h>

ChannelDockerWidget::ChannelDockerWidget(QWidget *parent, const char *name)
    : KisWidgetWithIdleTask<QWidget>(parent)
{
    setObjectName(name);

    m_channelTable = new QTableView();
    m_model = new ChannelModel(this);
    m_channelTable->setModel(m_model);
    m_channelTable->setShowGrid(false);
    m_channelTable->horizontalHeader()->setStretchLastSection(true);
    m_channelTable->verticalHeader()->setVisible(false);
    m_channelTable->horizontalHeader()->setVisible(false);
    m_channelTable->setSelectionBehavior( QAbstractItemView::SelectRows );

    m_channelTable->setMinimumHeight(50);

    // Use a layout so the widget gets resized properly
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(m_channelTable);
    // Krimble 2026-11-13: George: "channels". A touch screen has no Ctrl key, so the two actions of a channel also have buttons under the list: "All" shows every channel again, "To selection" loads the
    // chosen row's channel as the selection. A tap on a channel's name or thumbnail shows only that channel (the checkboxes still switch single channels on and off).
    QHBoxLayout *buttons = new QHBoxLayout();
    QPushButton *allButton = new QPushButton(tr("All"));
    QPushButton *selectionButton = new QPushButton(tr("To selection"));
    allButton->setToolTip(tr("Show every channel again"));
    selectionButton->setToolTip(tr("Use the chosen channel as the selection (light = selected). Ctrl+click on a channel does the same."));
    buttons->addWidget(allButton);
    buttons->addWidget(selectionButton);
    layout->addLayout(buttons, 1, 0);
    connect(allButton, &QPushButton::clicked, this, [this]() { m_model->showAllChannels(); });
    connect(selectionButton, &QPushButton::clicked, this, &ChannelDockerWidget::slotLoadSelection);
    this->setLayout(layout);

    QScroller *scroller = KisKineticScroller::createPreconfiguredScroller(m_channelTable);
    if (scroller){
        connect(scroller, SIGNAL(stateChanged(QScroller::State)),
                this, SLOT(slotScrollerStateChanged(QScroller::State)));
    }

    connect(m_channelTable,&QTableView::activated, m_model, &ChannelModel::rowActivated);
    connect(m_channelTable, &QTableView::clicked, this, &ChannelDockerWidget::slotTableClicked);   // Krimble 2026-11-13
}

void ChannelDockerWidget::slotTableClicked(const QModelIndex &index)
{
    if (!index.isValid() || index.column() == 0) return;   // column 0 is the checkbox: the view toggles it itself
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) m_model->loadChannelAsSelection(index.row());
    else m_model->isolateChannel(index.row());
}

void ChannelDockerWidget::slotLoadSelection()
{
    const QModelIndex current = m_channelTable->currentIndex();
    if (current.isValid()) m_model->loadChannelAsSelection(current.row());
}

void ChannelDockerWidget::setCanvas(KisCanvas2 * canvas)
{
    if(m_canvas == canvas)
        return;
    m_canvas = canvas;

    m_model->setCanvas(m_canvas);

    KisWidgetWithIdleTask<QWidget>::setCanvas(canvas);

    if (m_canvas) {
        connect(m_canvas->displayColorConverter(), SIGNAL(displayConfigurationChanged()), SLOT(startUpdateCanvasProjection()));
        connect(m_model, SIGNAL(channelFlagsChanged()), m_canvas, SLOT(channelSelectionChanged()));
    }
}

void ChannelDockerWidget::startUpdateCanvasProjection()
{
    triggerCacheUpdate();
}

KisIdleTasksManager::TaskGuard ChannelDockerWidget::registerIdleTask(KisCanvas2 *canvas)
{
    return
        canvas->viewManager()->idleTasksManager()->
        addIdleTaskWithGuard([this](KisImageSP image) {
            const KisDisplayConfig config = m_canvas->displayColorConverter()->displayConfig();

            const QSize previewSize = m_model->thumbnailSizeLimit();

            KisChannelsThumbnailsStrokeStrategy *strategy =
                new KisChannelsThumbnailsStrokeStrategy(image->projection(), image->bounds(),
                                                        previewSize, false, config.profile,
                                                        config.intent, config.conversionFlags);

            connect(strategy, SIGNAL(thumbnailsUpdated(QVector<QImage>, const KoColorSpace*)), this, SLOT(updateImageThumbnails(QVector<QImage>, const KoColorSpace*)));

            return strategy;
        });
}

void ChannelDockerWidget::updateImageThumbnails(const QVector<QImage> &channels, const KoColorSpace *cs)
{
    m_model->setChannelThumbnails(channels, cs);
    m_channelTable->resizeRowsToContents();
    m_channelTable->resizeColumnsToContents();
}

void ChannelDockerWidget::clearCachedState()
{
    m_model->setChannelThumbnails({}, nullptr);
}
