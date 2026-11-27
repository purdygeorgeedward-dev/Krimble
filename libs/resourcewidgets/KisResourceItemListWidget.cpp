/*
 * SPDX-FileCopyrightText: 2023 Srirupa Datta <srirupa.sps@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisResourceItemListWidget.h"

#include <QDebug>
#include <QEvent>
#include <QHeaderView>
#include <QHelpEvent>
#include <QScroller>
#include <QScrollBar>
#include <QScrollArea>

#include "KisIconToolTip.h"

struct  Q_DECL_HIDDEN KisResourceItemListWidget::Private
{
    ListViewMode viewMode = ListViewMode::IconGrid;
    bool strictSelectionMode {false};
    KisIconToolTip tip;

    QScroller* scroller {0};
    QString prev_scrollbar_style;
};

KisResourceItemListWidget::KisResourceItemListWidget(QWidget *parent)
    : QListWidget(parent)
    , m_d(new Private)
{
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setContextMenuPolicy(Qt::DefaultContextMenu);
    setViewMode(QListView::IconMode);
    setMovement(QListView::Static);
    setGridSize(QSize(56, 56));
    setIconSize(QSize(56, 56));
    setResizeMode(QListWidget::Adjust);
    setUniformItemSizes(true);

    m_d->scroller = KisKineticScroller::createPreconfiguredScroller(this);
    if (m_d->scroller) {
        connect(m_d->scroller, SIGNAL(stateChanged(QScroller::State)), this, SLOT(slotScrollerStateChange(QScroller::State)));
    }

    connect(this, SIGNAL(clicked(QModelIndex)), SIGNAL(currentResourceClicked(const QModelIndex &)));

     m_d->prev_scrollbar_style = horizontalScrollBar()->styleSheet();
}

KisResourceItemListWidget::~KisResourceItemListWidget()
{
}

void KisResourceItemListWidget::setListViewMode(ListViewMode viewMode)
{
    m_d->viewMode = viewMode;

    switch (viewMode) {
    case ListViewMode::IconGrid: {
        setViewMode(ViewMode::IconMode);
        setFlow(Flow::LeftToRight);
        setWrapping(true);
        horizontalScrollBar()->setStyleSheet(m_d->prev_scrollbar_style);
        setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
        break;
    }
    case ListViewMode::IconStripHorizontal: {
        setViewMode(ViewMode::IconMode);
        setFlow(Flow::LeftToRight);
        setWrapping(false);

        horizontalScrollBar()->setStyleSheet("QScrollBar::horizontal {height: 0px;}");
        setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        break;
    }
    case ListViewMode::Detail: {
        setViewMode(ViewMode::ListMode);
        setFlow(Flow::TopToBottom);
        setWrapping(false);
        setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
        setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
        break;
    }
    }

    setItemSize(iconSize());
}

void KisResourceItemListWidget::setItemSize(QSize size)
{
    switch (m_d->viewMode) {
    case ListViewMode::IconGrid: {
        setGridSize(size);
        setIconSize(size);
        break;
    }
    case ListViewMode::IconStripHorizontal: {
        break;
    }
    case ListViewMode::Detail: {
        const int w = width();
        setGridSize(QSize(w, size.height()));
        setIconSize(QSize(size));
        break;
    }
    }
}

void KisResourceItemListWidget::setStrictSelectionMode(bool enable)
{
    m_d->strictSelectionMode = enable;
}

void KisResourceItemListWidget::setFixedToolTipThumbnailSize(const QSize &size)
{
    m_d->tip.setFixedToolTipThumbnailSize(size);
}

void KisResourceItemListWidget::setToolTipShouldRenderCheckers(bool value)
{
    m_d->tip.setToolTipShouldRenderCheckers(value);
}

void KisResourceItemListWidget::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    QModelIndex current = currentIndex();
    if (selectionMode() == SingleSelection
            && m_d->strictSelectionMode
            && current.isValid()
            && current.row() >= start
            && current.row() <= end) {

        selectionModel()->clear();
    }
    QListWidget::rowsAboutToBeRemoved(parent, start, end);
}

void KisResourceItemListWidget::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QListWidget::selectionChanged(selected, deselected);

    if (selected.isEmpty()) {
        Q_EMIT currentResourceChanged(QModelIndex());
    }
    else {
        Q_EMIT currentResourceChanged(selected.indexes().first());
    }
}

QItemSelectionModel::SelectionFlags KisResourceItemListWidget::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    QItemSelectionModel::SelectionFlags cmd = QListWidget::selectionCommand(index, event);

    if (selectionMode() == SingleSelection
            && m_d->strictSelectionMode
            && cmd.testFlag(QItemSelectionModel::Deselect)) {

        cmd = QItemSelectionModel::NoUpdate;
    }
    return cmd;
}

void KisResourceItemListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QListWidget::contextMenuEvent(event);
    Q_EMIT contextMenuRequested(event->globalPos());
}

bool KisResourceItemListWidget::viewportEvent(QEvent *event)
{
    return QListWidget::viewportEvent(event);
}

void KisResourceItemListWidget::resizeEvent(QResizeEvent *event)
{
    if (m_d->viewMode == ListViewMode::IconStripHorizontal) {
        const int height = event->size().height();
        setGridSize(QSize(height, height));
        setIconSize(QSize(height, height));
    }
    else {
        setGridSize(gridSize());
    }
}
