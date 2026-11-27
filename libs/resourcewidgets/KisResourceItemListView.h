/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2019 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISRESOURCEITEMLISTVIEW_H
#define KISRESOURCEITEMLISTVIEW_H

#include <QListView>
#include <QScopedPointer>

#include <KisKineticScroller.h>

#include "kritaresourcewidgets_export.h"
#include "ResourceListViewModes.h"

class KRITARESOURCEWIDGETS_EXPORT KisResourceItemListView : public QListView
{
    Q_OBJECT

public:
    KisResourceItemListView(QWidget *parent = nullptr);
    ~KisResourceItemListView() override;

    void setListViewMode(ListViewMode layout);

    void setItemSize(QSize size);

    void setStrictSelectionMode(bool enable);

    void setFixedToolTipThumbnailSize(const QSize &size);
    void setToolTipShouldRenderCheckers(bool value);

public Q_SLOTS:
    void slotScrollerStateChange(QScroller::State state){ KisKineticScroller::updateCursor(this, state); }

Q_SIGNALS:

    void sigSizeChanged();

    void currentResourceChanged(const QModelIndex &);
    void currentResourceClicked(const QModelIndex &);

    void contextMenuRequested(const QPoint &);

protected Q_SLOTS:
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) override;
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

protected:
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *event = nullptr) const override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    bool viewportEvent(QEvent *event) override;

private:
    void resizeEvent(QResizeEvent *event) override;

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
