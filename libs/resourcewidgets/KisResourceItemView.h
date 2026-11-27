/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISRESOURCEITEMVIEW_H
#define KISRESOURCEITEMVIEW_H

#include <QTableView>
#include <QScroller>

#include <KisKineticScroller.h>

#include "KisIconToolTip.h"

class QEvent;
class QModelIndex;

#include "kritaresourcewidgets_export.h"

class KRITARESOURCEWIDGETS_EXPORT KisResourceItemView : public QTableView
{
    Q_OBJECT

public:

    enum ViewMode {
        FIXED_COLUMNS,
        FIXED_ROWS
    };

    explicit KisResourceItemView(QWidget *parent = 0);
    ~KisResourceItemView() override { disconnect(); }

public Q_SLOTS:
    void slotScrollerStateChange(QScroller::State state){ KisKineticScroller::updateCursor(this, state); }

Q_SIGNALS:

    void sigSizeChanged();

Q_SIGNALS:

    void currentResourceChanged(const QModelIndex &);
    void currentResourceClicked(const QModelIndex &);

    void contextMenuRequested(const QPoint &);

protected:

    friend class KisResourceItemChooser;

    void contextMenuEvent(QContextMenuEvent *event) override;
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

    void mousePressEvent(QMouseEvent *event) override;
    bool viewportEvent(QEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void setViewMode(ViewMode mode);

    void updateView();

private Q_SLOTS:
    void slotItemClicked(const QModelIndex &index);

private:
    KisIconToolTip m_tip;
    QModelIndex m_beforeClickIndex;
    ViewMode m_viewMode;
};

#endif
